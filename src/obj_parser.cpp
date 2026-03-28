#include "obj_parser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

bool readObj(const string& filename, vector<Vector3>& out_vertices, vector<Triangle>& out_triangles, BoundingBox& out_bbox) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Tidak dapat membuka file input " << filename << "\n";
        return false;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;

        istringstream iss(line);
        string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vector3 vec;
            iss >> vec.x >> vec.y >> vec.z;
            out_vertices.push_back(vec);

            out_bbox.min.x = min(out_bbox.min.x, vec.x);
            out_bbox.min.y = min(out_bbox.min.y, vec.y);
            out_bbox.min.z = min(out_bbox.min.z, vec.z);
            out_bbox.max.x = max(out_bbox.max.x, vec.x);
            out_bbox.max.y = max(out_bbox.max.y, vec.y);
            out_bbox.max.z = max(out_bbox.max.z, vec.z);
        } 
        else if (prefix == "f") {
            string v1, v2, v3;
            iss >> v1 >> v2 >> v3;

            auto getVertexIndex = [](const string& token) {
                size_t pos = token.find('/');
                string idxStr = (pos == string::npos) ? token : token.substr(0, pos);
                try {
                    int idx = stoi(idxStr);
                    if (idx < 0) return -1;
                    return idx - 1;
                } catch (...) {
                    return -1;
                }
            };

            int i1 = getVertexIndex(v1);
            int i2 = getVertexIndex(v2);
            int i3 = getVertexIndex(v3);

            if (i1 >= 0 && i2 >= 0 && i3 >= 0 && i1 < out_vertices.size() && i2 < out_vertices.size() && i3 < out_vertices.size()) {
                out_triangles.push_back({out_vertices[i1], out_vertices[i2], out_vertices[i3]});
            }
        }
    }

    file.close();
    return true;
}

bool writeObj(const string& filename, const vector<Triangle>& voxelTriangles) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Tidak dapat membuat file output " << filename << "\n";
        return false;
    }

    int vertexCount = 1;
    for (const auto& tri : voxelTriangles) {
        file << "v " << tri.v0.x << " " << tri.v0.y << " " << tri.v0.z << "\n";
        file << "v " << tri.v1.x << " " << tri.v1.y << " " << tri.v1.z << "\n";
        file << "v " << tri.v2.x << " " << tri.v2.y << " " << tri.v2.z << "\n";
        
        file << "f " << vertexCount << " " << vertexCount+1 << " " << vertexCount+2 << "\n";
        vertexCount += 3;
    }

    file.close();
    return true;
}