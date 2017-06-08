#ifndef MeshLoader_H
#define MeshLoader_H

#include "appunit.h"


class MeshLoader
{
public:
    MeshLoader();
    /* @Load and Draw the slected mesh file */
    void LoadandDraw(const string fileName, uint listName, double& time);

    /* @Create a new mesh file */
    void CreateNewMesh(MeshLoader theMesh, const string filePath);

    /* @Define arrays for saving vertices and normals */
    vector<aiVector3D> vertArray, normArray;

    /* @Define array for saving faces */
    vector<aiFace> faceArray;

    /* @Define a boolen to note mesh data */
    bool isEmpty;

    /* @Table of Vertex-Face */
    map<uint, uint> vfTable;

private:
    /* @Define a importer by Assimp Labs */
    Assimp::Importer* mImporter;

    /* @Define a pointer for scene */
    const aiScene* mScene;
};

#endif // MeshLoader_H
