#include "meshdeform.h"

MeshDeform::MeshDeform()
{

}

void MeshDeform::meshDef(MeshLoader mesh, int value)
{
    cout << "silde value: " << value << endl;
    cout << "Deformation for mesh: " << mesh.vertArray.size() << endl;
}

