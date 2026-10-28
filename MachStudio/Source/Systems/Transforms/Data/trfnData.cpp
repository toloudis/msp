/********************************************************************************************\
**  trfnData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Transforms/Data/trfnData.hpp"
#include "Support/xfrm/xfrmApplyTransformUtil.hpp"
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif

#include "Core/env/envSTLHelpers.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
trfnData::trfnData()
:	m_bVisible("Visible",true),
	m_Name("Name"),
	m_Position("Position", maPoint3d( 0.0f, 0.0f, 0.0f )),
	m_Orientation("Orientation", maRotation( 0.0f, 0.0f, 0.0f )),
	m_Scale("Scale", 1),
	m_PivotPoint("Local Pivot"),
	m_bPickable("Pickable", true),
	m_bWireframe("Wireframe", false),
	m_bInheritsTransform("Inherits Transform", true),
	m_SourceFile("Source File")
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
trfnData::~trfnData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool trfnData::operator == (const trfnData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
trfnData& trfnData::operator=(const trfnData& i_Data)
{
	if (this == &i_Data) return *this;

	this->m_Name				= i_Data.m_Name;

	this->m_bVisible			= i_Data.m_bVisible;
	this->m_Name				= i_Data.m_Name;

	this->m_Orientation			= i_Data.m_Orientation;
	this->m_Position			= i_Data.m_Position;
	this->m_Scale				= i_Data.m_Scale;
	this->m_PivotPoint			= i_Data.m_PivotPoint;
	this->m_PivotCompensation	= i_Data.m_PivotCompensation;
	this->m_bPickable			= i_Data.m_bPickable;
	this->m_bWireframe			= i_Data.m_bWireframe;
	this->m_bInheritsTransform	= i_Data.m_bInheritsTransform;

	this->m_Objects				= i_Data.m_Objects;
	this->m_SourceFile			= i_Data.m_SourceFile;
	this->m_SourceIdentity		= i_Data.m_SourceIdentity;

	return *this;
}


//---------------------------------------------------------------------------
// Apply i_Matrix to its placement, without undo
//---------------------------------------------------------------------------
void trfnData::ApplyTransformation(const maMatrix4x4& i_Matrix)
{
	xfrmApplyTransformUtil::ApplyTransformation(i_Matrix,
												m_Position,
												m_Orientation,
												m_Scale,
												m_PivotPoint,
												m_PivotCompensation);
}
