//
//  AppUnit.cpp
//  TutorialGLSL
//
//  Created by Lin Jiaqing on 4/4/14.
//  Copyright (c) 2014 Lin Jiaqing. All rights reserved.
//

#include "appunit.h"

int     AppUnit::winWidth     = 0;
int     AppUnit::winHeight    = 0;
double  AppUnit::xPos         = 0;
double  AppUnit::yPos         = 0;
float   AppUnit::gScaleObjects= 1.0f;
float   AppUnit::gScaleScene  = 1.0f;
float   AppUnit::gRotObjects[]  = {0.0f, 0.0f, 0.0f};
float   AppUnit::gRotScene[]  = {0.0f, 0.0f, 0.0f};
float   AppUnit::gTransObjects[] = {0.0f, 0.0f, 0.0f};
float   AppUnit::gTransScene[] = {0.0f, 0.0f, 0.0f};
int     AppUnit::numVertex	  = 0;
int     AppUnit::numNormal    = 0;
int     AppUnit::numFace      = 0;
double  AppUnit::ls           = 0;
double  AppUnit::ss           = 0;


void ClientPos::clear()
{
    x = y = width = height = lastX = lastY = 0;
}

AppUnit::AppUnit() {}
AppUnit::~AppUnit() {}

void AppUnit::NormalizeAngleForRotation(int& angle)
{
    while (angle < 360) {
        angle += 360 * 16;
    }
    while (angle > 360) {
        angle -= 360 * 16;
    }
}
