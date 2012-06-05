/*****************************************************************************
**	rtIntersections.cpp
**
**		see .hpp
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/rt/rtIntersections.hpp"


//--------------------------------------------------------------------
// IntersectTriangle()
//--------------------------------------------------------------------
int rtIntersections::IntersectTriangle( IntersectionData &id, Ray ray, Triangle triangle, float &tmin) 
{
	float a = triangle.v0.GetX() - triangle.v1.GetX();
	float b = triangle.v0.GetX() - triangle.v2.GetX();
	float c = ray.d.GetX();
	float d = triangle.v0.GetX() - ray.o.GetX();
	float e = triangle.v0.GetY() - triangle.v1.GetY();
	float f = triangle.v0.GetY() - triangle.v2.GetY();
	float g = ray.d.GetY();
	float h = triangle.v0.GetY() - ray.o.GetY();
	float i = triangle.v0.GetZ() - triangle.v1.GetZ();
	float j = triangle.v0.GetZ() - triangle.v2.GetZ();
	float k = ray.d.GetZ();
	float l = triangle.v0.GetZ() - ray.o.GetZ();

	float m = f*k - g*j, n = h*k - g*l, p = f*l - h*j;
	float q = g*i - e*k, s = e*j - f*i;

	float inv_denom = 1.0 / (a*m + b*q + c*s);

	float e1 = d*m - b*n - c*p;
	float beta = e1*inv_denom;

	if ( beta < 0.0 ) 
	{
		return 0;
	}

	float r = e*l - h*i;
	float e2 = a*n +d*q + c*r;
	float gamma = e2*inv_denom;

	if ( gamma < 0.0 ) 
	{
		return 0;
	}
	if ( beta + gamma > 1.0 ) 
	{
		return 0;
	}

	float e3 = a*p - b*r + d*s;
	float t = e3*inv_denom;

	if ( t < K_EPSILON ) 
	{
		return 0;
	}

	tmin = t;
	id.N = triangle.normal;
	id.P = ray.o + ray.d*t;

	return 1;
}

//--------------------------------------------------------------------
// IntersectSphere()
//--------------------------------------------------------------------
int rtIntersections::IntersectSphere( IntersectionData &id , Ray ray , Sphere sphere , float &tmin ) {

	float t;	
	maVector3d temp = ray.o - sphere.center;
	float a = ray.d.Dot(ray.d);
	float b = 2.0 * temp.Dot(ray.d);
	float c = temp.Dot(temp) - sphere.radius * sphere.radius;
	float disc = b*b - 4.0*a*c;

	if ( disc < 0.0 ) 
	{
		id.hit = 0;
		return 0;
	} 
	else 
	{
		float e = sqrt(disc);
		float denom = 2.0 * a;
		t = (-b - e) / denom;

		if (t > K_EPSILON) 
		{
			tmin = t;
			id.hit = 1;
			id.N = (temp + ray.d*t) / sphere.radius;
			id.P = ray.o + ray.d*t;
			return 1;
		}
		t = (-b + e) / denom;

		if (t > K_EPSILON) 
		{
			tmin = t;
			id.hit = 1;
			id.N = (temp + ray.d*t) / sphere.radius;
			id.P = ray.o + ray.d*t;
			return 1;
		}
	}
	return 0;
}

//--------------------------------------------------------------------
// IntersectRectangle()
//--------------------------------------------------------------------
int rtIntersections::IntersectRectangle( IntersectionData &id , Ray ray , Rectangle rectangle , float &tmin ) 
{
	float t = rectangle.normal.Dot(rectangle.p0 - ray.o) / rectangle.normal.Dot(ray.d);

	if ( t <= K_EPSILON ) 
	{
		id.hit = 0;
		return 0;
	}

	maPoint3d p = (ray.d*t) + ray.o;
	maPoint3d d = p - rectangle.p0;

	float ddota = d.Dot(rectangle.a);

	if ( ddota < 0.0 || ddota > rectangle.a_len_squared ) 
	{
		id.hit = 0;
		return 0;
	}

	float ddotb = d.Dot(rectangle.b);

	if ( ddotb < 0.0 || ddotb > rectangle.b_len_squared ) 
	{
		id.hit = 0;
		return 0;
	}

	tmin = t;
	id.N = rectangle.normal;
	id.P = p;
	id.hit = 1;

	return 1;
}