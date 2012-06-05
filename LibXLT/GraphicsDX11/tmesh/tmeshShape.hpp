/****************************************************************************\
**	tmeshShape.hpp
**
**  Virtual class that con be be used to implement primitives or
**  acceleration structures.         
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef TMESH_SHAPE_HPP
#error tmeshShape.hpp multiply included
#endif
#define TMESH_SHAPE_HPP

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif

/// The virtual base class for all objects that can be intersected with a ray.
class Shape
{
public:
    virtual ~Shape() {}

    /// Calculate the bounding box of the shape.
    /// \param[in] time0 Minimum of time interval being sampled.
    /// \param[in] time1 Maximum of time interval being sampled.
    /// \return The bounding box of the object in the interval [time0, time1).
    ///
    virtual maAxisBox boundingBox(double time0, double time1) const = 0;

	maVector3d v1;
	maVector3d v2;
	maVector3d v3;
	maVector3d n1;
	maVector3d n2;
	maVector3d n3;
	maVector3d uv1;
	maVector3d uv2;
	maVector3d uv3;
	maVector3d tan1;
	maVector3d tan2;
	maVector3d tan3;
	maVector3d bn1;
	maVector3d bn2;
	maVector3d bn3;
	maVector4d data;

};

