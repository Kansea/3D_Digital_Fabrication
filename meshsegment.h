#ifndef MESHSEGMENT_H
#define MESHSEGMENT_H

#include "appunit.h"
#include "meshloader.h"
#include "nanoflann.hpp"

using namespace nanoflann;

class MeshSegment
{
public:
    MeshSegment();
    /* @Segment the inputed mesh data and pass it to buffer */
    void DrawSegmentedMesh(uint listName, MeshLoader theMesh, ClientPos theRect, int whichButton, bool isMySearchMethod);

private:
    /* @Define variables and struct for KD-tree search */
    template <typename T>
    struct PointCloud {
        vector<aiVector3D> points;

        // Must return the number of data points
        inline size_t kdtree_get_point_count() const { return points.size(); }

        // Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
        inline T kdtree_distance(const T *p1, const size_t idx_p2, size_t /*size*/) const
        {
            const T d0 = p1[0] - points[idx_p2].x;
            const T d1 = p1[1] - points[idx_p2].y;
            return d0*d0 + d1*d1;
        }

        // Returns the dim'th component of the idx'th point in the class:
        // Since this is inlined and the "dim" argument is typically an immediate value, the
        //  "if/else's" are actually solved at compile time.
        inline T kdtree_get_pt(const size_t idx, int dim) const
        {
            if (dim == 0) return points[idx].x;
            else return points[idx].y;
        }

        // Optional bounding-box computation: return false to default to a standard bbox computation loop.
        // Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
        // Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
        template <class BBOX>
        bool kdtree_get_bbox(BBOX& /*bb*/) const { return false; }
    };

    MeshSegment::PointCloud<float> mCloud;


    /* @Define variables for mesh segment processing */
    vector<int> faceIndices;
    vector<int> indicesForFindFace;

    /* @Define a variable for storing mapped cursor coordinates */
    vector<aiVector3D> winCoords;

    /* @Mapping object coordinates to window view port */
    inline void MappingToWindow(aiVector3D v);

    /* @Calculate which part will be segment or delete, 0 means Segment Button was pressed, 1 means Delete Button was pressed */
    inline void MeshSegmentation(MeshLoader theMesh, vector<aiVector3D> pos, ClientPos theRect, int whichButton);
    inline void MeshSegmentationKDtree(MeshLoader theMesh, vector<aiVector3D> pos, ClientPos theRect, int whichButton);

    /* @Pass the segmented or deleted data to openGL buffer */
    inline void PassToGL(uint listName, MeshLoader theMesh, vector<int> index);
};

#endif // MESHSEGMENT_H
