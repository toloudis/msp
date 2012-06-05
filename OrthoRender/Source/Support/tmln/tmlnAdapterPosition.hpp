/*****************************************************************************
**	tmlnAdapterPosition.hpp
**
**	 Adapter for altering position of something
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef TMLN_ADAPTERPOSITION_HPP
#error tmlnAdapterPosition.hpp multiply included
#endif
#define TMLN_ADAPTERPOSITION_HPP


#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


class tmlnAdapterPosition
{
public:
	//--------------------------------------------------------------------
	//  Set new position for object, needs to be implemented in
	// derived class
	//--------------------------------------------------------------------
	virtual void  SetPosition(const maPoint3d &i_Pos) = 0;
};

class tmlnAdapterGetPosition
{
public:
	//--------------------------------------------------------------------
	//  Get position of object, for localizing sounds
	//--------------------------------------------------------------------
	virtual maPoint3d  GetPosition() const = 0;

};
