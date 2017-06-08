#include "meshloader.h"

MeshLoader::MeshLoader()
{
    mScene = NULL;
    isEmpty = true;
    mImporter = new Assimp::Importer;
    vertArray.clear();
    normArray.clear();
    faceArray.clear();
    vfTable.clear();
}

void MeshLoader::LoadandDraw(const string fileName, uint listName, double& time)
{
    clock_t start = clock();
    vertArray.clear();
    normArray.clear();
    faceArray.clear();
    vfTable.clear();
    mScene = mImporter->ReadFile(fileName, aiProcess_OptimizeMeshes | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_ImproveCacheLocality);
    if (!fileName.empty())
    {
        if (mScene) {
            aiVector3D tmp;
            for (int i = 0; i < (int)mScene->mNumMeshes; ++i)
            {
                aiMesh* aiMesh = mScene->mMeshes[i];
                glNewList(listName, GL_COMPILE);
                // int colors = (int)aiMesh->mNumFaces / 5;
                for (int j = 0; j < (int)aiMesh->mNumFaces; ++j)
                {
                    aiFace f = aiMesh->mFaces[j];
                    faceArray.push_back(f);
                    glBegin(GL_TRIANGLES);
                    glColor3f(1.0f, 0.5f, 0.3f);  // The color is orange {1.0f, 0.5f, 0.3f}

                    // Color test for searching method
                    /* if (j < colors)
                        glColor3f(1.0f, 0.0f, 0.0f);
                    if (j > colors && j < colors * 2)
                        glColor3f(0.0f, 1.0f, 0.0f);
                    if (j > colors * 2 && j < colors * 3)
                        glColor3f(0.0f, 0.0f, 1.0f);
                    if (j > colors * 3 && j < colors * 4)
                        glColor3f(1.0f, 1.0f, 0.0f);
                    if (j > colors * 4)
                        glColor3f(0.0f, 1.0f, 1.0f); */

                    for (int k = 0; k < (int)f.mNumIndices; ++k)
                    {
                        tmp.x = aiMesh->mVertices[f.mIndices[k]].x;
                        tmp.y = aiMesh->mVertices[f.mIndices[k]].y;
                        tmp.z = aiMesh->mVertices[f.mIndices[k]].z;
                        glVertex3f(tmp.x, tmp.y, tmp.z);
                        vertArray.push_back(tmp);

                        tmp.x = aiMesh->mNormals[f.mIndices[k]].x;
                        tmp.y = aiMesh->mNormals[f.mIndices[k]].y;
                        tmp.z = aiMesh->mNormals[f.mIndices[k]].z;
                        glNormal3f(tmp.x, tmp.y, tmp.z);
                        normArray.push_back(tmp);
                        // Crate a V-F table for finding face index when getting targeted vextices
                        vfTable.insert(map<uint, uint>::value_type(f.mIndices[k], j));
                    }
                    glEnd();
                }
                glEndList();
            }
        }
        isEmpty = false;
        time = ((double)clock() - (double)start) / (double)CLOCKS_PER_SEC;
    }
    else {
        cout << "No mesh data to load" << endl;
        isEmpty = true;
    }
}

void MeshLoader::CreateNewMesh(MeshLoader theMesh, const string filePath)
{
    ofstream outData;
    outData.open(filePath, ofstream::out);
    outData << "# Create OBJ File by Lin." << endl;

    if (!theMesh.faceArray.empty()) {
        // Write vectices to mesh file
        for (int i = 0; i < (int)theMesh.faceArray.size(); ++i) {
            aiFace f = theMesh.faceArray[i];
            for (int j = 0; j < (int)f.mNumIndices; ++j) {
                aiVector3D tmp = theMesh.vertArray[f.mIndices[j]];
                outData << "v " << tmp.x << " " << tmp.y << " " << tmp.z << endl;
            }
        }
        // Write normals to mesh file
        for (int i = 0; i < (int)theMesh.faceArray.size(); ++i) {
            aiFace f = theMesh.faceArray[i];
                aiVector3D tmp = theMesh.normArray[f.mIndices[0]];
                outData << "vn " << tmp.x << " " << tmp.y << " " << tmp.z << endl;
        }
        // Write faces to mesh file
        for (int i = 0; i < (int)theMesh.faceArray.size(); ++i) {
            int f = (i * 3) + 1;
            outData << "f " << f << "//" << i+1 << " " << f+1 << "//" << i+1 << " " << f+2 << "//" << i+1 << endl;
        }
        outData.close();
    }else {
        cout << "No mesh data to create" << endl;
    }
}
