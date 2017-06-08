//
//  AppUnit.h
//  TutorialGLSL
//
//  Created by Lin Jiaqing on 4/4/14.
//  Copyright (c) 2014 Lin Jiaqing. All rights reserved.
//
#ifndef APPUNIT_H
#define APPUNIT_H

#define SIZE_OF_COORDS 2
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (b > a ? b : a)

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <map>
#include <algorithm>
#include <vector>
#include <ctime>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDrag>
#include <QMimeData>
#include <QListWidgetItem>
#include <QDesktopServices>
#include <QGLWidget>
#include <QImage>
#include <QImageWriter>
#include <QPaintEvent>



using namespace std;

class ClientPos {
public:
    /* @Variables are used to store data from mouse rect */
    int    x, y, width, height, lastX, lastY;

    /* @Clear x, y, lastX, lastY, width and height */
    void   clear();
};

class AppUnit
{
public:
	AppUnit();
	~AppUnit();

    /* @Define window width and height for storing data */
    static int          winWidth, winHeight;

	/* @Define mouse cursor position for storing data */
	static double       xPos, yPos;

	/* @Define rotation for UI */
    static float        gRotObjects[], gRotScene[];

	/* @Define rotation for UI */
    static float        gTransObjects[], gTransScene[];

	/* @Define scale for mesh */
    static float        gScaleObjects, gScaleScene;

	/* @The information of loaded mesh */
	static int          numVertex, numNormal, numFace;
    static double       ls, ss;

	/* @Normalize rotation angles for mouse moving */
	static void NormalizeAngleForRotation(int& angle);
};

#endif	// APPUNIT_H
