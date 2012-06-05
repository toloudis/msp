/********************************************************************************************\
**  dynControlData.hpp
**
**  This data is for both types of control nodes:
**		- a single axis rotation control with min/max
**		- a more general rotate and translate node
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef DYN_CONTROLDATA_HPP
#error dynControlData.hpp multiply included
#endif
#define DYN_CONTROLDATA_HPP

#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif 
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif 
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif 

class dynControlData
{
public:
	dynControlData();

	prtyText m_Name;
	prtyText m_Node;

	// Euler angles (X,Y,Z) in degrees
	prtyFloat m_RotateX;
	prtyFloat m_RotateY;
	prtyFloat m_RotateZ;

	prtyVector3d m_Translation;
	prtyVector3d m_Scale;
};

