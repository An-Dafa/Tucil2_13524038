#include "octree.hpp"
#include <cmath>
#include <future>
using namespace std;

Octree::Octree(int maxDepth) : maxDepth(maxDepth) {
    nodesFormedCount.assign(maxDepth + 1, 0);
    nodesSkippedCount.assign(maxDepth + 1, 0);
}

const vector<BoundingBox>& Octree::getVoxels() const {
    return voxels;
}

vector<BoundingBox> Octree::divideBox(const BoundingBox& box) {
    vector<BoundingBox> children(8);
    Vector3 center = box.getCenter();
    Vector3 min = box.min;
    Vector3 max = box.max;

    children[0].min = Vector3(min.x, min.y, min.z);       
    children[0].max = Vector3(center.x, center.y, center.z);

    children[1].min = Vector3(center.x, min.y, min.z);    
    children[1].max = Vector3(max.x, center.y, center.z);

    children[2].min = Vector3(min.x, center.y, min.z);    
    children[2].max = Vector3(center.x, max.y, center.z);

    children[3].min = Vector3(center.x, center.y, min.z); 
    children[3].max = Vector3(max.x, max.y, center.z);

    children[4].min = Vector3(min.x, min.y, center.z);    
    children[4].max = Vector3(center.x, center.y, max.z);

    children[5].min = Vector3(center.x, min.y, center.z); 
    children[5].max = Vector3(max.x, center.y, max.z);

    children[6].min = Vector3(min.x, center.y, center.z); 
    children[6].max = Vector3(center.x, max.y, max.z);

    children[7].min = Vector3(center.x, center.y, center.z); 
    children[7].max = Vector3(max.x, max.y, max.z);

    return children;
}

bool Octree::checkIntersection(const BoundingBox& box, const Triangle& tri) {
    Vector3 boxCenter = box.getCenter();
    Vector3 boxExtents = (box.max - box.min) * 0.5f;

    Vector3 v0 = tri.v0 - boxCenter;
    Vector3 v1 = tri.v1 - boxCenter;
    Vector3 v2 = tri.v2 - boxCenter;

    Vector3 e0 = v1 - v0;
    Vector3 e1 = v2 - v1;
    Vector3 e2 = v0 - v2;

    Vector3 triMin(
        min({v0.x, v1.x, v2.x}),
        min({v0.y, v1.y, v2.y}),
        min({v0.z, v1.z, v2.z})
    );

    Vector3 triMax(
        max({v0.x, v1.x, v2.x}),
        max({v0.y, v1.y, v2.y}),
        max({v0.z, v1.z, v2.z})
    );

    if (triMin.x > boxExtents.x || triMax.x < -boxExtents.x ||triMin.y > boxExtents.y || triMax.y < -boxExtents.y || triMin.z > boxExtents.z || triMax.z < -boxExtents.z) {
        return false;
    }

    Vector3 normal = e0.cross(e1);
    float d = normal.dot(v0);
    float r = boxExtents.x * abs(normal.x) + boxExtents.y * abs(normal.y) + boxExtents.z * abs(normal.z);
    
    if (abs(normal.dot(v0)) > r) return false;

    float p0, p1, p2, rad;
    
    #define AXISTEST(axis_v, vA, vB, vC, proj_extents) \
        p0 = (vA).dot(axis_v); \
        p1 = (vB).dot(axis_v); \
        p2 = (vC).dot(axis_v); \
        rad = proj_extents; \
        if (max({p0, p1, p2}) < -rad || min({p0, p1, p2}) > rad) return false;

    Vector3 a00(0, -e0.z, e0.y); 
    AXISTEST(a00, v0, v1, v2, boxExtents.y * abs(a00.y) + boxExtents.z * abs(a00.z));
    Vector3 a01(0, -e1.z, e1.y); 
    AXISTEST(a01, v0, v1, v2, boxExtents.y * abs(a01.y) + boxExtents.z * abs(a01.z));
    Vector3 a02(0, -e2.z, e2.y); 
    AXISTEST(a02, v0, v1, v2, boxExtents.y * abs(a02.y) + boxExtents.z * abs(a02.z));

    Vector3 a10(e0.z, 0, -e0.x); 
    AXISTEST(a10, v0, v1, v2, boxExtents.x * abs(a10.x) + boxExtents.z * abs(a10.z));
    Vector3 a11(e1.z, 0, -e1.x); 
    AXISTEST(a11, v0, v1, v2, boxExtents.x * abs(a11.x) + boxExtents.z * abs(a11.z));
    Vector3 a12(e2.z, 0, -e2.x); 
    AXISTEST(a12, v0, v1, v2, boxExtents.x * abs(a12.x) + boxExtents.z * abs(a12.z));

    Vector3 a20(-e0.y, e0.x, 0); 
    AXISTEST(a20, v0, v1, v2, boxExtents.x * abs(a20.x) + boxExtents.y * abs(a20.y));
    Vector3 a21(-e1.y, e1.x, 0); 
    AXISTEST(a21, v0, v1, v2, boxExtents.x * abs(a21.x) + boxExtents.y * abs(a21.y));
    Vector3 a22(-e2.y, e2.x, 0); AXISTEST
    (a22, v0, v1, v2, boxExtents.x * abs(a22.x) + boxExtents.y * abs(a22.y));

    #undef AXISTEST

    return true;
}

void Octree::buildRecursive(const BoundingBox& box, const vector<Triangle>& triangles, int currentDepth) {
    if (currentDepth <= maxDepth) {
        lock_guard<mutex> lock(voxelMutex);
        nodesFormedCount[currentDepth]++;
    }

    vector<Triangle> intersectingTriangles;
    for (const auto& tri : triangles) {
        if (checkIntersection(box, tri)) {
            intersectingTriangles.push_back(tri);
        }
    }

    if (intersectingTriangles.empty()) {
        if (currentDepth <= maxDepth) {
            lock_guard<mutex> lock(voxelMutex);
            nodesSkippedCount[currentDepth]++;
        }
        return;
    }

    if (currentDepth == maxDepth) {
        lock_guard<mutex> lock(voxelMutex);
        voxels.push_back(box);
        return;
    }

    vector<BoundingBox> children = divideBox(box);
    for (int i = 0; i < 8; ++i) {
        buildRecursive(children[i], intersectingTriangles, currentDepth + 1);
    }
}

void Octree::voxelize(const BoundingBox& rootBox, const vector<Triangle>& triangles) {
    voxels.clear();
    
    nodesFormedCount.assign(maxDepth + 1, 0);
    nodesSkippedCount.assign(maxDepth + 1, 0);
    
    Vector3 size = rootBox.max - rootBox.min;
    float maxDim = max({size.x, size.y, size.z});
    Vector3 center = rootBox.getCenter();
    
    BoundingBox cubeRoot;
    cubeRoot.min = center - Vector3(maxDim/2, maxDim/2, maxDim/2);
    cubeRoot.max = center + Vector3(maxDim/2, maxDim/2, maxDim/2);

    vector<BoundingBox> children = divideBox(cubeRoot);
    vector<future<void>> futures;

    for (int i = 0; i < 8; ++i) {
        futures.push_back(async(launch::async, [this, childBox = children[i], &triangles]() {
            this->buildRecursive(childBox, triangles, 1);
        }));
    }

    for (auto& f : futures) {
        f.get();
    }
}