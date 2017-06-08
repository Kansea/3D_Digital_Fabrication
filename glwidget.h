#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "appunit.h"
#include "meshloader.h"
#include "meshsegment.h"
#include "meshdeform.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();

private:	// Private variables and functions
    /* @The instance pointer for Mesh class */
    MeshLoader  mesh;
    MeshSegment segment;
    MeshDeform  meshDef;
    double mTimer;

    /* @Define a variable for handling 3D mesh data list */
    uint		mObjList;

    /* @Define a variable for handling ground data list */
    uint		mGroundList;

    /* @Define a variables for saving rotation data */
    int			xRot, yRot, zRot;

    /* @Define a variables for saving translate and scale */
    float		xObjTranslate, yObjTranslate, zObjTranslate, mZoom, xZoom, yZoom, zZoom;

    /* @Define a variable for saving the old position of cursor*/
    QPoint		mOldPos;

    /* @Define x, y postions and width, height for defined rect */
    QRect       qRect;
    ClientPos   mRect;

    /* @Define a variable for passing slot data */
    bool    isDrawGround;
    bool    isMyMethod, isKdtree;

    /* @Inherits initializedGL function from QT QGLWidget */
    void	initializeGL();

    /* @Inherits paintEventGL function from QT QWidget */
    void    paintEvent(QPaintEvent*);

    /* @Render Opengl function */
    void    RenderGL();

    /* @Inherits resizeGL function from QT QGLWidget */
    void	resizeGL(int w, int h);

    /* @Get last position of cursor when mouse button was pressed*/
    void	mousePressEvent(QMouseEvent* e);

    /* @Set the rotation values when mouse was moved */
    void	mouseMoveEvent(QMouseEvent* e);

    /* @Set the scale value when mouse wheel was rolling */
    void	wheelEvent(QWheelEvent* e);

    /* @This function for keys event when some key was pressed */
    void    keyPressEvent(QKeyEvent* e);

    void    SentMeshInfo(double timer);

public slots:	// Pubilc receivers
    /* @Implement DrawMesh function from Mesh class */
    void    DrawMesh(string FileName);

    /* @Implement WriteMesh function form Mesh class */
    void    NewMesh(string namePath);

    /* @Implement DrawGround function from Mesh class */
    void    DrawGround();

    /* @Implement DrawSegMesh function from Mesh class */
    void    DrawSegMesh(int whichButton);

    /* @Implement DeleteSelectMesh function again from Mesh class */
    void    DrawDeleteMesh(int whichButton);

    /* @Implement mesh deformation */
    void    DeformMeshX(int value);
    void    DeformMeshY(int value);
    void    DeformMeshZ(int value);

    /* @Set the x-axis rotation value */
    void    SetXRotation(int angle);

    /* @Set the y-axis rotation value */
    void    SetYRotation(int angle);

    /* @Set the z-axis rotation value */
    void    SetZRotation(int angle);

    /* @Set the x-axis translate value */
    void    SetXTranslate(int x);

    /* @Set the y-axis translate value */
    void    SetYTranslate(int y);

    /* @Set the z-axis translate value */
    void    SetZTranslate(int z);

    /* @Set render mode if it is wireframe */
    void    SetWireframe(bool isWireframe);

    /* @Set render mode if it is drawground */
    void    SetGround(bool theGround);

    /* @Set searching method by checkbox */
    void    SetDefaultSearch(bool MyMethod);
    void    SetKdtreeSearch(bool KdMethod);

    /* @Implement clear mouse rect function */
    void    ClearMouseRect();

signals:	// Sender for QT object
    void    SentLoadTime(QString time);
    void    SentSegTime(QString time);
    void    SentNumVert(QString v);
    void    SentNumNorm(QString n);
    void    SentNumFace(QString f);
};

#endif // GLWIDGET_H
