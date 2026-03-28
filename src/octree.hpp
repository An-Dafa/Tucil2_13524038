#ifndef OCTREE_HPP
#define OCTREE_HPP

#include "geometry.hpp"
#include <vector>
#include <mutex>

class Octree {
public:
    Octree(int maxDepth);
    void voxelize(const BoundingBox& rootBox, const std::vector<Triangle>& triangles);
    const std::vector<BoundingBox>& getVoxels() const;
    std::vector<int> getNodesFormed() const { 
        return nodesFormedCount; 
    }
    std::vector<int> getNodesSkipped() const { 
        return nodesSkippedCount; 
    }

private:
    int maxDepth;
    std::vector<BoundingBox> voxels;
    std::mutex voxelMutex;
    std::vector<int> nodesFormedCount;
    std::vector<int> nodesSkippedCount;

    void buildRecursive(const BoundingBox& box, const std::vector<Triangle>& triangles, int currentDepth);
    std::vector<BoundingBox> divideBox(const BoundingBox& box);
    bool checkIntersection(const BoundingBox& box, const Triangle& tri);
};

#endif