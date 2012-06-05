/*****************************************************************************
**	rtStructures.cpp
**
**		Definitions
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef RT_STRUCTURES_HPP
#error rtStructures.hpp multiply included
#endif
#define RT_STRUCTURES_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

//============================================================================
//============================================================================
#define K_EPSILON		0.005
#define VIEW_DIST		450
#define NUM_TRIANGLES	10


//============================================================================
// Definition of a ray
//============================================================================
typedef struct {
	maVector3d o;
	maVector3d d;
} Ray;

//============================================================================
// Definition of a structure containing intersection point data
//============================================================================
typedef struct {
	bool hit;
	maPoint3d P;
	maVector3d N;
	maVector3d S;
	maVector3d R;
	maVector3d V;
	maVector3d H;
	maFloatRGBA color;
} IntersectionData;

//============================================================================
// Definition of a triangle
//============================================================================
typedef struct {
	maVector3d v0;
	maVector3d v1;
	maVector3d v2;
	maVector3d normal;
	maFloatRGBA color;
} Triangle;

//============================================================================
// Definition of a sphere
//============================================================================
typedef struct {
	float radius;	
	maVector3d center;
	maFloatRGBA color;
} Sphere;

//============================================================================
// Definition of a rectangle
//============================================================================
typedef struct {
	maPoint3d p0;
	maVector3d a;
	maVector3d b;
	maVector3d normal;
	float a_len_squared;
	float b_len_squared;
	maFloatRGBA color;
} Rectangle;

//============================================================================
//============================================================================
typedef struct {
	Triangle triangles [NUM_TRIANGLES];
} ObjectList;

//============================================================================
//============================================================================
typedef struct {
	maPoint3d eye;
	maPoint3d lookat;
	maVector3d up;
	maVector3d u,v,w;
} Camera;

//============================================================================
// Definition of a light source
//============================================================================
typedef struct {
	maPoint3d pos;
	maFloatRGBA color;
} LightSource;

//============================================================================
// Definition of a view plane
//============================================================================
typedef struct {
	int hres;
	int vres;
} ViewPlane;

//============================================================================
// Definition of a world
//============================================================================
typedef struct {
	maFloatRGBA backColor;
	ObjectList objectList;
	Camera camera;
	LightSource lightSource;
	ViewPlane viewplane;
} World;
