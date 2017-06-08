#ifndef MESHDEFORM_H
#define MESHDEFORM_H

#include <meshloader.h>
#include <iostream>


using namespace std;

class MeshDeform
{
public:
    MeshDeform();

    void meshDef(MeshLoader mesh, int value);
};

#endif // MESHDEFORM_H
