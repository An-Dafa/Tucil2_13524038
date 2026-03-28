#include <iostream>
#include <chrono>
#include <vector>
#include "obj_parser.hpp"
#include "octree.hpp"

using namespace std;

void convertVoxelsToTriangles(const vector<BoundingBox>& voxels, vector<Triangle>& out_triangles) {
    for (const auto& voxel : voxels) {
        Vector3 min = voxel.min;
        Vector3 max = voxel.max;
        
        Vector3 v0(min.x, min.y, min.z);
        Vector3 v1(max.x, min.y, min.z);
        Vector3 v2(max.x, max.y, min.z);
        Vector3 v3(min.x, max.y, min.z);
        Vector3 v4(min.x, min.y, max.z);
        Vector3 v5(max.x, min.y, max.z);
        Vector3 v6(max.x, max.y, max.z);
        Vector3 v7(min.x, max.y, max.z);

        out_triangles.push_back({v0, v3, v1}); out_triangles.push_back({v1, v3, v2});
        out_triangles.push_back({v5, v6, v4}); out_triangles.push_back({v4, v6, v7}); 
        out_triangles.push_back({v4, v7, v0}); out_triangles.push_back({v0, v7, v3}); 
        out_triangles.push_back({v1, v2, v5}); out_triangles.push_back({v5, v2, v6}); 
        out_triangles.push_back({v3, v7, v2}); out_triangles.push_back({v2, v7, v6}); 
        out_triangles.push_back({v4, v0, v5}); out_triangles.push_back({v5, v0, v1});
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Penggunaan: " << argv[0] << " <input.obj> <kedalaman_octree> <output.obj>\n";
        return 1;
    }

    string inputFile = argv[1];
    int depth = stoi(argv[2]);
    string outputFile = argv[3];

    vector<Vector3> vertices;
    vector<Triangle> triangles;
    BoundingBox rootBox;

    cout << "Memuat file input...\n";
    if (!readObj(inputFile, vertices, triangles, rootBox)) {
        return 1;
    }

    auto start_time = chrono::high_resolution_clock::now();

    Octree octree(depth);
    octree.voxelize(rootBox, triangles);

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = end_time - start_time;

    const auto& voxels = octree.getVoxels();
    
    vector<Triangle> voxelTriangles;
    convertVoxelsToTriangles(voxels, voxelTriangles);

    size_t totalVertices = voxels.size() * 8;
    size_t totalFaces = voxelTriangles.size(); 

    writeObj(outputFile, voxelTriangles);

    cout << "\nHASIL VOXELIZATION\n";
    cout << "Banyaknya voxel yang terbentuk: " << voxels.size() << "\n";
    cout << "Banyaknya vertex yang terbentuk: " << totalVertices << "\n";
    cout << "Banyaknya faces yang terbentuk: " << totalFaces << "\n";
    
    cout << "Statistik node octree yang terbentuk:\n";
    vector<int> nodesFormed = octree.getNodesFormed(); 
    for(int i = 1; i <= depth; i++) {
        cout << i << " : " << (i < nodesFormed.size() ? nodesFormed[i] : 0) << "\n";
    }

    cout << "Statistik node yang tidak perlu ditelusuri:\n";
    vector<int> nodesSkipped = octree.getNodesSkipped();
    for(int i = 1; i <= depth; i++) {
        cout << i << " : " << (i < nodesSkipped.size() ? nodesSkipped[i] : 0) << "\n";
    }

    cout << "Kedalaman octree: " << depth << "\n";
    cout << "Lama waktu program berjalan: " << elapsed.count() << " ms\n";
    cout << "Path dimana file .obj disimpan: " << outputFile << "\n";

    return 0;
}