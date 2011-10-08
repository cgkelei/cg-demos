#pragma once
#ifndef _ConvertBridge__H
#define _ConvertBridge__H

#include <gtest/gtest.h>
#include "MathUtil.h"
#include "D3DX10Math.h"
#include "Frustum.h"

using namespace Engine;

bool operator == (const D3DXMATRIX& d3dMat, const Matrix4f& myMat);
bool operator == (const Matrix4f& myMat, const D3DXMATRIX& d3dMat);
bool operator == (const D3DXPLANE& d3dPlane, const Plane3f& myPlane);
bool operator != (const D3DXPLANE& d3dPlane, const Plane3f& myPlane);


#endif