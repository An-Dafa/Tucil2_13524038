#ifndef OBJ_PARSER_HPP
#define OBJ_PARSER_HPP

#include "geometry.hpp"
#include <vector>
#include <string>

// Fungsi untuk membaca input obj
bool readObj(const std::string& filename, std::vector<Vector3>& out_vertices, std::vector<Triangle>& out_triangles, BoundingBox& out_bbox);

// Fungsi untuk menulis output kubus-kubus voxel menjadi obj
bool writeObj(const std::string& filename, const std::vector<Triangle>& voxelTriangles);

#endif