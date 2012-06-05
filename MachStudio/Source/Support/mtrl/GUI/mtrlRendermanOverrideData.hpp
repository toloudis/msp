/*****************************************************************************
**  mtrlRendermanOverrideData.hpp
**
**      mtrlRendermanOverrideData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_RENDERMANOVERRIDEDATA_HPP
#error mtrlRendermanOverrideData.hpp multiply included
#endif
#define MTRL_RENDERMANOVERRIDEDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif


//============================================================================
//============================================================================
class mtrlRendermanOverrideData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlRendermanOverrideData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlRendermanOverrideData(const mtrlRendermanOverrideData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlRendermanOverrideData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlRendermanOverrideData& operator=(const mtrlRendermanOverrideData& i_Data);

public:
	prtyFilePath m_ShaderLocation;
	prtyText m_ParamList;
	prtyText m_AttributeList;
	prtyBoolean m_bOverrideShader;
	prtyBoolean m_bOverrideAttributes;
};

