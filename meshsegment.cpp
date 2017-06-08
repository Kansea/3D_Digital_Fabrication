#include "meshsegment.h"


MeshSegment::MeshSegment()
{
    winCoords.clear();
    indicesForFindFace.clear();
    faceIndices.clear();
}

void MeshSegment::DrawSegmentedMesh(uint listName, MeshLoader theMesh, ClientPos theRect, int whichButton, bool isMySearchMethod)
{
    if (!theMesh.isEmpty) {
        winCoords.clear();
        indicesForFindFace.clear();
        faceIndices.clear();

        glNewList(listName, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < (int)theMesh.faceArray.size(); ++i) {
            aiFace f = theMesh.faceArray[i];
            for (int j = 0; j < (int)f.mNumIndices; ++j) {
                glColor3f(1.0f, 1.5f, 0.3f);
                glNormal3f(theMesh.normArray[f.mIndices[j]].x,
                           theMesh.normArray[f.mIndices[j]].y,
                           theMesh.normArray[f.mIndices[j]].z);
                glVertex3f(theMesh.vertArray[f.mIndices[j]].x,
                           theMesh.vertArray[f.mIndices[j]].y,
                           theMesh.vertArray[f.mIndices[j]].z);
                MappingToWindow(theMesh.vertArray[f.mIndices[j]]);
            }
        }
        glEnd();
        glEndList();
        // Pass the data to pointcloud
        mCloud.points = winCoords;

        if (isMySearchMethod)
            MeshSegmentation(theMesh, winCoords, theRect, whichButton);
        else
            MeshSegmentationKDtree(theMesh, winCoords, theRect, whichButton);

        PassToGL(listName, theMesh, faceIndices);
    }
}

void MeshSegment::MappingToWindow(aiVector3D v)
{
    int       viewport[4];
    double    mvMat[16], projectMat[16];
    double    posX, posY, posZ;
    aiVector3D  tmp;

    glPushMatrix();
    {
        // The order must be same as rendering order
        glTranslatef(0, 0, 0);
        glRotatef(AppUnit::gRotScene[0] / 16.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(AppUnit::gRotScene[1] / 16.0f, 0.0f, 1.0f, 0.0f);
        glScalef(AppUnit::gScaleScene, AppUnit::gScaleScene, AppUnit::gScaleScene);

        glTranslatef(AppUnit::gTransObjects[0], AppUnit::gTransObjects[1], AppUnit::gTransObjects[2]);
        glRotatef(AppUnit::gRotObjects[0], 1.0f, 0.0f, 0.0f);
        glRotatef(AppUnit::gRotObjects[2], 0.0f, 0.0f, 1.0f);
        glScalef(AppUnit::gScaleObjects, AppUnit::gScaleObjects, AppUnit::gScaleObjects);

        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, mvMat);
        glGetDoublev(GL_PROJECTION_MATRIX, projectMat);
    }
    glPopMatrix();

    gluProject((double)v.x, (double)v.y, (double)v.z, mvMat, projectMat, viewport, &posX, &posY, &posZ);
    posY = (double)AppUnit::winHeight - posY;
    tmp.Set((float)posX, (float)posY, (float)posZ);
    winCoords.push_back(tmp);
}

void MeshSegment::MeshSegmentation(MeshLoader theMesh, vector<aiVector3D> pos, ClientPos rect, int whichButton)
{
    ofstream outData;
    outData.open("mesh.obj", ofstream::out);
    outData << "# Create OBJ File by Lin." << endl;
    // Start from topleft
    if (rect.width > 0 && rect.height > 0) {
        for (int i = 0; i < (int)pos.size(); ++i) {
            aiVector3D tmp = pos[i];
            if (tmp.x > (float)rect.x && tmp.x < (float)rect.lastX && tmp.y > (float)rect.y && tmp.y < (float)rect.lastY)
                indicesForFindFace.push_back(i);
        }
    }
    // Start from topright
    if (rect.width < 0 && rect.height > 0) {
        for (int i = 0; i < (int)pos.size(); ++i) {
            aiVector3D tmp = pos[i];
            if (tmp.x > (float)rect.lastX && tmp.x < (float)rect.x && tmp.y > (float)rect.y && tmp.y < (float)rect.lastY)
                indicesForFindFace.push_back(i);
        }
    }
    // Start from bottomleft
    if (rect.width > 0 && rect.height < 0) {
        for (int i = 0; i < (int)pos.size(); ++i) {
            aiVector3D tmp = pos[i];
            if (tmp.x > (float)rect.x && tmp.x < (float)rect.lastX && tmp.y > (float)rect.lastY && tmp.y < (float)rect.y)
                indicesForFindFace.push_back(i);
        }
    }
    // Start from bottomright
    if (rect.width < 0 && rect.height < 0) {
        for (int i = 0; i < (int)pos.size(); ++i) {
            aiVector3D tmp = pos[i];
            if (tmp.x > (float)rect.lastX && tmp.x < (float)rect.x && tmp.y > (float)rect.lastY && tmp.y < (float)rect.y)
                indicesForFindFace.push_back(i);
        }
    }
    // Value 0 means do segment operation
    if (whichButton == 0) {
        if (!indicesForFindFace.empty() && !theMesh.vfTable.empty()) {
            for (int i = 0; i < (int)indicesForFindFace.size(); ++i) {
                map<uint, uint>::iterator it = theMesh.vfTable.find(indicesForFindFace[i]);
                faceIndices.push_back(it->second);
            }
            int i = 0;
            do {
                if (faceIndices.at(i) == faceIndices.at(i + 1))
                    faceIndices.erase(faceIndices.begin() + (i + 1));
                else
                    ++i;
            } while (i + 1 != (int)faceIndices.size());
        }
        if (!faceIndices.empty()) {
            // Write vectices to mesh file
            for (int i = 0; i < (int)faceIndices.size(); ++i) {
                aiFace f = theMesh.faceArray[faceIndices[i]];
                for (int j = 0; j < (int)f.mNumIndices; ++j) {
                    aiVector3D tmp = theMesh.vertArray[f.mIndices[j]];
                    outData << "v " << tmp.x << " " << tmp.y << " " << tmp.z << endl;
                }
            }
            // Write normals to mesh file
            /*for (int i = 0; i < (int)faceIndices.size(); ++i) {
                aiFace f = theMesh.faceArray[faceIndices[i]];
                    aiVector3D tmp = theMesh.normArray[f.mIndices[0]];
                    outData << "vn " << tmp.x << " " << tmp.y << " " << tmp.z << endl;
            }*/
            // Write faces to mesh file
            for (int i = 0; i < (int)faceIndices.size(); ++i) {
                int f = (i * 3) + 1;
                outData << "f " << f << " " << f+1 << " " << f+2 << endl;
            }
        }
    }
    // Value 1 means do delete operation
    if (whichButton == 1) {
        if (!indicesForFindFace.empty() && !theMesh.vfTable.empty()) {
            for (int i = 0; i < (int)indicesForFindFace.size(); ++i) {

                map<uint, uint>::iterator it = theMesh.vfTable.find(indicesForFindFace[i]);
                theMesh.vfTable.erase(it);
            }

            for (auto& x: theMesh.vfTable) {
                faceIndices.push_back(x.second);
            }
            int i = 0;
            do {
                if (faceIndices.at(i) == faceIndices.at(i + 1))
                    faceIndices.erase(faceIndices.begin() + (i + 1));
                else
                    ++i;
            } while (i + 1 != (int)faceIndices.size());
        }
        if (!faceIndices.empty()) {
            // Write vectices to mesh file
            for (int i = 0; i < (int)faceIndices.size(); ++i) {
                aiFace f = theMesh.faceArray[faceIndices[i]];
                for (int j = 0; j < (int)f.mNumIndices; ++j) {
                    aiVector3D tmp = theMesh.vertArray[f.mIndices[j]];
                    outData << "v " << tmp.x << " " << tmp.y << " " << tmp.z << endl;
                }
            }
            // Write normals to mesh file
            for (int i = 0; i < (int)faceIndices.size(); ++i) {
                aiFace f = theMesh.faceArray[faceIndices[i]];
                    aiVector3D tmp = theMesh.normArray[f.mIndices[0]];
                    outData << "vn " << tmp.x << " " << tmp.y << " " << tmp.z << endl;
            }
            // Write faces to mesh file
            for (int i = 0; i < (int)faceIndices.size(); ++i) {
                int f = (i * 3) + 1;
                outData << "f " << f << "//" << i+1 << " " << f+1 << "//" << i+1 << " " << f+2 << "//" << i+1 << endl;
            }
        }
    }
    outData.close();
}

void MeshSegment::MeshSegmentationKDtree(MeshLoader theMesh, vector<aiVector3D> pos, ClientPos rect, int whichButton)
{
    ofstream outData;
    outData.open("mesh.obj", ofstream::out);
    outData << "# Create OBJ File by Lin." << endl;

    // Construct a kd-tree index
    typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<float, PointCloud<float> >,PointCloud<float>,SIZE_OF_COORDS /* dim */> MyKdtree;
    MyKdtree mKdtree(SIZE_OF_COORDS /* dim */, mCloud, KDTreeSingleIndexAdaptorParams(10 /* max leaf */));
    mKdtree.buildIndex();
#if 0
    // Test resize of dataset and rebuild of index
    mCloud.points.resize(mCloud.points.size()*0.5);
    kIndex.buildIndex();
#endif
    // Kd-tree building end;

    float* targetPoint = new float[SIZE_OF_COORDS];
    targetPoint[0] = abs((rect.x + rect.lastX) / 2);
    targetPoint[1] = abs((rect.y + rect.lastY) / 2);
    float searchRadius = (pow((rect.lastX - rect.x), 2) + pow((rect.lastY - rect.y), 2)) / 2;
    vector<pair<size_t, float>> matches;
    SearchParams params;
    const size_t nMatches = mKdtree.radiusSearch(&targetPoint[0], searchRadius, matches, params);

    // Start from topleft
    if (rect.width > 0 && rect.height > 0) {
        for (int i = 0; i < nMatches; ++i) {
            aiVector3D tmp = mCloud.points[matches[i].first];
            if (tmp.x > (float)rect.x && tmp.x < (float)rect.lastX && tmp.y > (float)rect.y && tmp.y < (float)rect.lastY)
                indicesForFindFace.push_back((int)matches[i].first);
        }
    }
    // Start from topright
    if (rect.width < 0 && rect.height > 0) {
        for (int i = 0; i < nMatches; ++i) {
            aiVector3D tmp = mCloud.points[matches[i].first];
            if (tmp.x > (float)rect.lastX && tmp.x < (float)rect.x && tmp.y > (float)rect.y && tmp.y < (float)rect.lastY)
                indicesForFindFace.push_back((int)matches[i].first);
        }
    }
    // Start from bottomleft
    if (rect.width > 0 && rect.height < 0) {
        for (int i = 0; i < nMatches; ++i) {
            aiVector3D tmp = mCloud.points[matches[i].first];
            if (tmp.x > (float)rect.x && tmp.x < (float)rect.lastX && tmp.y > (float)rect.lastY && tmp.y < (float)rect.y)
                indicesForFindFace.push_back((int)matches[i].first);
        }
    }
    // Start from bottomright
    if (rect.width < 0 && rect.height < 0) {
        for (int i = 0; i < nMatches; ++i) {
            aiVector3D tmp = mCloud.points[matches[i].first];
            if (tmp.x > (float)rect.lastX && tmp.x < (float)rect.x && tmp.y > (float)rect.lastY && tmp.y < (float)rect.y)
                indicesForFindFace.push_back((int)matches[i].first);
        }
    }
    // Value 0 means do segment operation
    if (whichButton == 0) {
        if (!indicesForFindFace.empty() && !theMesh.vfTable.empty()) {
            for (int i = 0; i < (int)indicesForFindFace.size(); ++i) {
                map<uint, uint>::iterator it = theMesh.vfTable.find(indicesForFindFace[i]);
                faceIndices.push_back(it->second);
            }
            int i = 0;
            do {
                if (faceIndices.at(i) == faceIndices.at(i + 1))
                    faceIndices.erase(faceIndices.begin() + (i + 1));
                else
                    ++i;
            } while (i + 1 != (int)faceIndices.size());
        }
        if (!faceIndices.empty()) {
            // Write vectices to mesh file
            for (int i = 0; i < (int)faceIndices.size(); ++i) {
                aiFace f = theMesh.faceArray[faceIndices[i]];
                for (int j = 0; j < (int)f.mNumIndices; ++j) {
                    aiVector3D tmp = theMesh.vertArray[f.mIndices[j]];
                    outData << "v " << tmp.x << " " << tmp.y << " " << tmp.z << endl;
                }
            }
            // Write normals to mesh file
            /*for (int i = 0; i < (int)faceIndices.size(); ++i) {
                aiFace f = theMesh.faceArray[faceIndices[i]];
                    aiVector3D tmp = theMesh.normArray[f.mIndices[0]];
                    outData << "vn " << tmp.x << " " << tmp.y << " " << tmp.z << endl;
            }*/
            // Write faces to mesh file
            for (int i = 0; i < (int)faceIndices.size(); ++i) {
                int f = (i * 3) + 1;
                outData << "f " << f << " " << f+1 << " " << f+2 << endl;
            }
        }
    }
    // Value 1 means do delete operation
    if (whichButton == 1) {
        if (!indicesForFindFace.empty() && !theMesh.vfTable.empty()) {
            for (int i = 0; i < (int)indicesForFindFace.size(); ++i) {

                map<uint, uint>::iterator it = theMesh.vfTable.find(indicesForFindFace[i]);
                theMesh.vfTable.erase(it);
            }
            for (auto& x: theMesh.vfTable) {
                faceIndices.push_back(x.second);
            }
            int i = 0;
            do {
                if (faceIndices.at(i) == faceIndices.at(i + 1))
                    faceIndices.erase(faceIndices.begin() + (i + 1));
                else
                    ++i;
            } while (i + 1 != (int)faceIndices.size());
        }
        if (!faceIndices.empty()) {
            // Write vectices to mesh file
            for (int i = 0; i < (int)faceIndices.size(); ++i) {
                aiFace f = theMesh.faceArray[faceIndices[i]];
                for (int j = 0; j < (int)f.mNumIndices; ++j) {
                    aiVector3D tmp = theMesh.vertArray[f.mIndices[j]];
                    outData << "v " << tmp.x << " " << tmp.y << " " << tmp.z << endl;
                }
            }
            // Write normals to mesh file
            for (int i = 0; i < (int)faceIndices.size(); ++i) {
                aiFace f = theMesh.faceArray[faceIndices[i]];
                    aiVector3D tmp = theMesh.normArray[f.mIndices[0]];
                    outData << "vn " << tmp.x << " " << tmp.y << " " << tmp.z << endl;
            }
            // Write faces to mesh file
            for (int i = 0; i < (int)faceIndices.size(); ++i) {
                int f = (i * 3) + 1;
                outData << "f " << f << "//" << i+1 << " " << f+1 << "//" << i+1 << " " << f+2 << "//" << i+1 << endl;
            }
        }
    }
    outData.close();
}

void MeshSegment::PassToGL(uint listName, MeshLoader theMesh, vector<int> index)
{
    if (!index.empty()) {
        glNewList(listName, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < (int)index.size(); ++i) {
            aiFace f = theMesh.faceArray[index[i]];
            for (int j = 0; j < (int)f.mNumIndices; ++j) {
                glColor3f(0.0f, 0.5f, 0.3f);
                glNormal3f(theMesh.normArray[f.mIndices[j]].x,
                           theMesh.normArray[f.mIndices[j]].y,
                           theMesh.normArray[f.mIndices[j]].z);
                glVertex3f(theMesh.vertArray[f.mIndices[j]].x,
                           theMesh.vertArray[f.mIndices[j]].y,
                           theMesh.vertArray[f.mIndices[j]].z);
            }
        }
        glEnd();
        glEndList();
        index.clear();
    }
}


















