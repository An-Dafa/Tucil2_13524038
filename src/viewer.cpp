#include "raylib.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

Model LoadObj(const string& filename) {
    Model model = { 0 };
    ifstream file(filename);
    if (!file.is_open()) return model;

    vector<Vector3> temp_vertices;
    vector<float> mesh_vertices;
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vector3 v;
            iss >> v.x >> v.y >> v.z;
            temp_vertices.push_back(v);
        } 
        else if (prefix == "f") {
            string v1, v2, v3;
            iss >> v1 >> v2 >> v3;

            auto getIdx = [](const string& token) {
                size_t pos = token.find('/');
                string num = (pos == string::npos) ? token : token.substr(0, pos);
                return stoi(num) - 1; 
            };

            int i1 = getIdx(v1), i2 = getIdx(v2), i3 = getIdx(v3);
            mesh_vertices.push_back(temp_vertices[i1].x); mesh_vertices.push_back(temp_vertices[i1].y); mesh_vertices.push_back(temp_vertices[i1].z);
            mesh_vertices.push_back(temp_vertices[i2].x); mesh_vertices.push_back(temp_vertices[i2].y); mesh_vertices.push_back(temp_vertices[i2].z);
            mesh_vertices.push_back(temp_vertices[i3].x); mesh_vertices.push_back(temp_vertices[i3].y); mesh_vertices.push_back(temp_vertices[i3].z);
        }
    }

    if (mesh_vertices.empty()) return model;

    float minX = 1e9, minY = 1e9, minZ = 1e9;
    float maxX = -1e9, maxY = -1e9, maxZ = -1e9;
    for (size_t i = 0; i < mesh_vertices.size(); i += 3) {
        float x = mesh_vertices[i], y = mesh_vertices[i+1], z = mesh_vertices[i+2];
        if (x < minX) minX = x; if (x > maxX) maxX = x;
        if (y < minY) minY = y; if (y > maxY) maxY = y;
        if (z < minZ) minZ = z; if (z > maxZ) maxZ = z;
    }

    float centerX = (minX + maxX) / 2.0f;
    float centerY = (minY + maxY) / 2.0f;
    float centerZ = (minZ + maxZ) / 2.0f;
    
    float sizeX = maxX - minX;
    float sizeY = maxY - minY;
    float sizeZ = maxZ - minZ;
    float maxExtent = sizeX;
    if (sizeY > maxExtent) maxExtent = sizeY;
    if (sizeZ > maxExtent) maxExtent = sizeZ;

    float targetSize = 25.0f; 
    float scale = targetSize / (maxExtent == 0 ? 1.0f : maxExtent);

    for (size_t i = 0; i < mesh_vertices.size(); i += 3) {
        mesh_vertices[i]   = (mesh_vertices[i] - centerX) * scale;
        mesh_vertices[i+1] = (mesh_vertices[i+1] - centerY) * scale;
        mesh_vertices[i+2] = (mesh_vertices[i+2] - centerZ) * scale;
    }

    Mesh mesh = { 0 };
    mesh.vertexCount = mesh_vertices.size() / 3;
    mesh.triangleCount = mesh.vertexCount / 3;
    
    mesh.vertices = (float *)MemAlloc(mesh_vertices.size() * sizeof(float));
    mesh.normals = (float *)MemAlloc(mesh_vertices.size() * sizeof(float));
    mesh.colors = (unsigned char *)MemAlloc(mesh.vertexCount * 4 * sizeof(unsigned char));

    Vector3 lightDir = { 0.5f, 0.8f, 0.3f };
    float len = sqrt(lightDir.x*lightDir.x + lightDir.y*lightDir.y + lightDir.z*lightDir.z);
    lightDir.x /= len; lightDir.y /= len; lightDir.z /= len;

    for (size_t i = 0; i < mesh_vertices.size(); i++) {
        mesh.vertices[i] = mesh_vertices[i];
    }

    for (size_t i = 0; i < mesh.vertexCount; i += 3) {
        float v0x = mesh.vertices[i*3],     v0y = mesh.vertices[i*3+1],   v0z = mesh.vertices[i*3+2];
        float v1x = mesh.vertices[(i+1)*3], v1y = mesh.vertices[(i+1)*3+1], v1z = mesh.vertices[(i+1)*3+2];
        float v2x = mesh.vertices[(i+2)*3], v2y = mesh.vertices[(i+2)*3+1], v2z = mesh.vertices[(i+2)*3+2];

        float e1x = v1x - v0x, e1y = v1y - v0y, e1z = v1z - v0z;
        float e2x = v2x - v0x, e2y = v2y - v0y, e2z = v2z - v0z;

        float nx = e1y * e2z - e1z * e2y;
        float ny = e1z * e2x - e1x * e2z;
        float nz = e1x * e2y - e1y * e2x;

        float nlen = sqrt(nx*nx + ny*ny + nz*nz);
        if (nlen > 0.0f) { nx /= nlen; ny /= nlen; nz /= nlen; }

        float intensity = (nx * lightDir.x + ny * lightDir.y + nz * lightDir.z);
        intensity = 0.4f + (intensity + 1.0f) * 0.3f; 
        if (intensity > 1.0f) intensity = 1.0f;

        unsigned char colorValue = (unsigned char)(220 * intensity); 

        for (int j = 0; j < 3; j++) {
            mesh.normals[(i+j)*3] = nx;     mesh.normals[(i+j)*3+1] = ny;   mesh.normals[(i+j)*3+2] = nz;
            mesh.colors[(i+j)*4] = colorValue;   
            mesh.colors[(i+j)*4+1] = colorValue; 
            mesh.colors[(i+j)*4+2] = colorValue; 
            mesh.colors[(i+j)*4+3] = 255;        
        }
    }

    UploadMesh(&mesh, false);
    model = LoadModelFromMesh(mesh);
    return model;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Cara penggunaan: ./bin/viewer.exe <path_ke_file.obj>\n";
        return 1;
    }

    string filePath = argv[1];
    InitWindow(800, 600, "Obj Viewer");
    SetTargetFPS(60);

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 15.0f, 15.0f, 15.0f }; 
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Model model = LoadObj(filePath);
    bool isModelLoaded = (model.meshCount > 0);
    float camRadius = 30.0f;
    float camAngleX = 0.78f; 
    float camAngleY = 0.78f;

    while (!WindowShouldClose()) {
        camRadius -= GetMouseWheelMove() * 10.0f; 
        if (camRadius < 3.0f) camRadius = 3.0f;  
        if (camRadius > 1000.0f) camRadius = 1000.0f; 

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            camAngleX -= GetMouseDelta().x * 0.01f;
            camAngleY += GetMouseDelta().y * 0.01f;
            if (camAngleY > 1.5f) camAngleY = 1.5f;
            if (camAngleY < -1.5f) camAngleY = -1.5f;
        }

        camera.position.x = camRadius * cos(camAngleY) * cos(camAngleX);
        camera.position.y = camRadius * sin(camAngleY);
        camera.position.z = camRadius * cos(camAngleY) * sin(camAngleX);
        camera.target = (Vector3){0.0f, 0.0f, 0.0f}; 

        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                DrawGrid(20, 1.0f);
                if (isModelLoaded) {
                    DrawModel(model, (Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, WHITE); 
                }
            EndMode3D();

            DrawText("Object Viewer", 10, 10, 20, DARKBLUE);
            DrawText(TextFormat("File: %s", filePath.c_str()), 10, 40, 10, DARKGRAY);
            
            if (!isModelLoaded) {
                DrawText("ERROR: FILE TIDAK DITEMUKAN ATAU KOSONG!", 10, 70, 20, RED);
                DrawText("Pastikan path file benar dan file sudah di-voxelize.", 10, 100, 15, GRAY);
            } else {
                DrawText("Status: SUKSES", 10, 70, 15, DARKGREEN);
            }

            DrawFPS(800 - 90, 10);
        EndDrawing();
    }

    if (isModelLoaded) UnloadModel(model);
    CloseWindow();
    return 0;
}