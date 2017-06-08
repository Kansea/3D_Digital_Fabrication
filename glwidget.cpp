#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    setFormat(QGLFormat(QGL::SampleBuffers));
    setAutoFillBackground(false);
    mObjList    = 0;
    mGroundList = 0;
    xRot        = 0;
    yRot        = 0;
    zRot        = 0;
    mZoom       = 1.0f;
    xZoom       = 1.0f;
    yZoom       = 1.0f;
    zZoom       = 1.0f;
    xObjTranslate  = 0;
    yObjTranslate  = 0;
    zObjTranslate  = 0;
    isDrawGround   = false;
    isMyMethod     = true;
    mRect.clear();
}

GLWidget::~GLWidget()
{
    makeCurrent();
    glDeleteLists(mObjList, 1);
    glDeleteLists(mGroundList, 1);
}

void GLWidget::initializeGL()
{
    qglClearColor(Qt::gray);
    glClearDepth(1.0f);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_FLAT);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    float diff[] = {0.5f, 0.5f, 0.5f, 1.0f};
    float amb[] = {0.2f, 0.2f, 0.2f, 1.0f};
    float lpos0[] = {0.9f, 0.9f, 0.9f};
    float lpos1[] = {-0.9f, 0.9f, -0.9f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_POSITION, lpos0);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT1, GL_POSITION, lpos1);
}

void GLWidget::paintEvent(QPaintEvent*)
{

    makeCurrent();
    RenderGL();

    // 2D graphices draw function
    qRect.setX(mRect.x);
    qRect.setY(mRect.y);
    qRect.setWidth(mRect.width);
    qRect.setHeight(mRect.height);
    QPainter painter(this);
    painter.setPen(QPen(Qt::red, 1));
    painter.drawRect(qRect);
    painter.end();
}

void GLWidget::RenderGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Setup a camera for scene
    gluLookAt(0.0, 1.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    {
        // Define a scene
        glTranslatef(0, 0, 0);
        glRotatef(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
        glScalef(mZoom, mZoom, mZoom);
        if (isDrawGround)
            glCallList(mGroundList);
        // Define an object exsited in scene
        {
            glTranslatef(xObjTranslate, yObjTranslate, zObjTranslate);
            glRotatef(AppUnit::gRotObjects[0], 1.0f, 0.0f, 0.0f);
            glRotatef(AppUnit::gRotObjects[2], 0.0f, 0.0f, 1.0f);
            //glScalef(AppUnit::gScaleObjects, AppUnit::gScaleObjects, AppUnit::gScaleObjects);
            glScalef(xZoom, yZoom, zZoom);
            glCallList(mObjList);
        }
    }
}

void GLWidget::resizeGL(int w, int h)
{
    AppUnit::winWidth = w;
    AppUnit::winHeight = h;
    qreal pixelRatio = devicePixelRatio();
    glViewport(0, 0, (int)w * pixelRatio, (int)h * pixelRatio);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (double)((w * pixelRatio) / (h * pixelRatio)), 0.1f, 1000.0f);
}

void GLWidget::DrawMesh(string FileName)
{
    // Generate list buffer for OpenGL
    mObjList = glGenLists(1);
    // Load mMesh file and pass the mesh data to buffer
    mesh.LoadandDraw(FileName, mObjList, mTimer);
    SentMeshInfo(mTimer);
    update();;
}

void GLWidget::DrawSegMesh(int whichButton)
{
    emit SentSegTime(QString::number(0));
    clock_t start = clock();
    segment.DrawSegmentedMesh(mObjList, mesh, mRect, whichButton, isMyMethod);
    // Clear rect position for mouse
    mRect.clear();
    mesh.LoadandDraw("mesh.obj", mObjList, mTimer);
    SentMeshInfo(mTimer);
    emit SentSegTime(QString::number((double)(clock() - start) / CLOCKS_PER_SEC));
    update();
}

void GLWidget::DrawDeleteMesh(int whichButton)
{
    emit SentSegTime(QString::number(0));
    clock_t start = clock();
    segment.DrawSegmentedMesh(mObjList, mesh, mRect, whichButton, isMyMethod);
    // Clear rect position for mouse
    mRect.clear();
    mesh.LoadandDraw("mesh.obj", mObjList, mTimer);
    SentMeshInfo(mTimer);
    emit SentSegTime(QString::number((double)(clock() - start) / CLOCKS_PER_SEC));
    update();
}

void GLWidget::DrawGround()
{
    // Generate list buffer for OpenGL
    mGroundList = glGenLists(1);
    // Pass the mMesh data to buffer
    glNewList(mGroundList, GL_COMPILE);
    glBegin(GL_LINES);
    for (int i = 0; i < 10; ++i) {
        if (i == 0) {
            glColor3f(0.0f, 0.0f, 1.0f);
        } else {
            glColor3f(0.0f, 0.0f, 0.0f);
        }
        glVertex3f(-0.1f * i, 0.0f, 0.9f);
        glVertex3f(-0.1f * i, 0.0f, -0.9f);
        glVertex3f(0.1f * i, 0.0f, 0.9f);
        glVertex3f(0.1f * i, 0.0f, -0.9f);
    }
    for (int j = 0; j < 10; ++j) {
        if (j == 0) {
            glColor3f(1.0f, 0.0f, 0.0f);
        } else {
            glColor3f(0.0f, 0.0f, 0.0f);
        }
        glVertex3f(0.9f, 0.0f, 0.1f * j);
        glVertex3f(-0.9f, 0.0f, 0.1f * j);
        glVertex3f(-0.9f, 0.0f, -0.1f * j);
        glVertex3f(0.9f, 0.0f, -0.1f * j);
    }
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.9f, 0.0f);
    glEnd();
    glEndList();
}

void GLWidget::NewMesh(string namePath)
{
    mesh.CreateNewMesh(mesh, namePath);
}

void GLWidget::DeformMeshX(int value)
{
    float _value = (float)value;
    if (value < _value)
        xZoom -= _value / 10;
    else
        xZoom += _value / 10;
    xZoom += _value / 10;
    meshDef.meshDef(mesh, _value);
    update();
}

void GLWidget::DeformMeshY(int value)
{
    float _value = (float)value;
    if (value < _value)
        yZoom -= _value / 10;
    else
        yZoom += _value / 10;
    yZoom += _value / 10;
    meshDef.meshDef(mesh, _value);
    update();
}

void GLWidget::DeformMeshZ(int value)
{
    float _value = (float)value;
    if (value < _value)
        zZoom -= _value / 10;
    else
        zZoom += _value / 10;
    meshDef.meshDef(mesh, _value);
    update();
}

void GLWidget::SetWireframe(bool isWireframe)
{
    if (isWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    update();
}

void GLWidget::SetGround(bool theGround)
{
    isDrawGround = theGround;
    update();
}

void GLWidget::SetDefaultSearch(bool MyMethod)
{
    isMyMethod = MyMethod;
    if (isMyMethod)
        isKdtree = false;

}

void GLWidget::SetKdtreeSearch(bool KdMethod)
{
    isKdtree = KdMethod;
    if (isKdtree)
        isMyMethod = false;
}

void GLWidget::ClearMouseRect()
{
    mRect.clear();
    update();
}

void GLWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->buttons() == Qt::LeftButton)
    {
        mRect.x = e->pos().x();
        mRect.y = e->pos().y();
    }
    if (e->buttons() == Qt::RightButton)
    {
        ClearMouseRect();
    }
    if (e->buttons() == Qt::MiddleButton)
    {
        mOldPos = e->pos();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent* e)
{

    if (e->buttons() == Qt::MiddleButton)
    {
        int deltaX = e->x() - mOldPos.x();
        int deltaY = e->y() - mOldPos.y();
        SetXRotation(xRot + 4 * deltaY);
        SetYRotation(yRot + 4 * deltaX);
    }
    if (e->buttons() == Qt::LeftButton) {
        mRect.lastX = e->x();
        mRect.lastY = e->y();
        mRect.width = e->x() - mRect.x;
        mRect.height = e->y() - mRect.y;
        update();
    }
    mOldPos = e->pos();
}

void GLWidget::wheelEvent(QWheelEvent* e)
{
    int numDegrees = e->delta() / 120;
    //int numSteps = numDegrees / 15;
    mZoom += (float)(numDegrees * 0.08f);
    AppUnit::gScaleScene = mZoom;
    update();

}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_A) {
        AppUnit::gRotObjects[2] += 1.0f;
        update();
    }
    if (e->key() == Qt::Key_D) {
        AppUnit::gRotObjects[2] -= 1.0f;
        update();
    }
    if (e->key() == Qt::Key_W) {
        AppUnit::gRotObjects[0] -= 1.0f;
        update();
    }
    if (e->key() == Qt::Key_S) {
        AppUnit::gRotObjects[0] += 1.0f;
        update();
    }
    if (e->key() == Qt::Key_Z) {
        AppUnit::gScaleObjects += 0.1f;
        update();
    }
    if (e->key() == Qt::Key_X) {
        AppUnit::gScaleObjects -= 0.1f;
        update();
    }
}

void GLWidget::SetXRotation(int angle)
{
    AppUnit::NormalizeAngleForRotation(angle);
    if (angle != xRot) {
        xRot = angle;
        AppUnit::gRotScene[0] = xRot;
        update();
    }
}

void GLWidget::SetYRotation(int angle)
{
    AppUnit::NormalizeAngleForRotation(angle);
    if (angle != yRot) {
        yRot = angle;
        AppUnit::gRotScene[1] = yRot;
        update();
    }
}

void GLWidget::SetZRotation(int angle)
{
    AppUnit::NormalizeAngleForRotation(angle);
    if (angle != zRot) {
        zRot = angle;
        AppUnit::gRotScene[2] = zRot;
        update();
    }
}

void GLWidget::SetXTranslate(int x)
{
    xObjTranslate = (float)x * 0.1f;
    AppUnit::gTransObjects[0] = xObjTranslate;
    update();
}

void GLWidget::SetYTranslate(int y)
{
    yObjTranslate = (float)y * 0.1f;
    AppUnit::gTransObjects[1] = yObjTranslate;
    update();
}

void GLWidget::SetZTranslate(int z)
{
    zObjTranslate = (float)z * 0.1f;
    AppUnit::gTransObjects[2] = zObjTranslate;
    update();
}

void GLWidget::SentMeshInfo(double timer)
{
    emit SentLoadTime(QString::number(timer));
    emit SentNumVert(QString::number(mesh.vertArray.size()));
    emit SentNumNorm(QString::number(mesh.normArray.size()));
    emit SentNumFace(QString::number(mesh.faceArray.size()));
}
