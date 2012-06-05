/********************************************************************************************\
**	ControlsTestData.hpp
**
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#ifdef CONTROLSTESTDATA_HPP
#error ControlsTestData.hpp multiply included
#endif
#define CONTROLSTESTDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
//prtyColor
//prtyDirectory
//prtyEnum
//prtyFileName
//prtyFilePath
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
//prtyGradient
//prtyHotKey
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_INT8_HPP
#include "Core/prty/prtyInt8.hpp"
#endif
//prtyListChecked
//prtyNamed
//prtyPoint3d
//prtyPoint3dTransform
//prtyRotation
//prtyText
//prtyTextureFileName
//prtyTime
#ifndef PRTY_TRIGGER_HPP
#include "Core/prty/prtyTrigger.hpp"
#endif
//prtyVector3d
//prtyVideo


//============================================================================
//============================================================================
class ControlsTestData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	ControlsTestData();

	//---------------------------------------------------------------------------
	//	Properties
	//---------------------------------------------------------------------------
	prtyBoolean	m_bBooleanTest;
	prtyTrigger	m_bTriggerTest;
	prtyInt8	m_Int8Test;
	prtyInt32	m_Int32Test;
	prtyFloat	m_FloatTest;
};

