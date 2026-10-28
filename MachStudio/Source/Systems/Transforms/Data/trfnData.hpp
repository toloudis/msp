/********************************************************************************************\
**  trfnData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#ifdef TRFN_DATA_HPP
#error trfnData.hpp multiply included
#endif
#define TRFN_DATA_HPP

#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif 
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif
#ifndef PRTY_POINT3DTRANSFORM_HPP
#include "Core/prty/prtyPoint3dTransform.hpp"
#endif 
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif


//============================================================================
//============================================================================
class trfnData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	trfnData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~trfnData();

	//-------------------------------------------------
	bool operator == (const trfnData& i_Item);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	trfnData& operator=(const trfnData& i_Data);

	//------------------------------------------------------------------------
	// Apply i_Matrix to its placement, without undo (what parenting does to
	// keep the world pose)
	//------------------------------------------------------------------------
	void ApplyTransformation(const maMatrix4x4& i_Matrix);

public:
	prtyName				m_Name;
	prtyBoolean				m_bVisible;
	prtyPoint3dTransform	m_Position;
	prtyRotation			m_Orientation;
	prtyFloat				m_Scale;
	prtyPoint3d				m_PivotPoint;
	prtyVector3d			m_PivotCompensation;
	prtyBoolean				m_bPickable;
	prtyBoolean				m_bWireframe;
	prtyBoolean				m_bInheritsTransform;

	std::vector<nameString> m_Objects;
	prtyFilePath			m_SourceFile;		// the scene file this came from; empty: not from a file
	std::string				m_SourceIdentity;	// its identity in that file (iscnObjectDesc::m_Identity)
};
