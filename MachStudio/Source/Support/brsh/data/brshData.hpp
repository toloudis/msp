/********************************************************************************************\
**  brshData.hpp
**
**		property data for the paint brush
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef BRSH_DATA_HPP
#error brshData.hpp multiply included
#endif
#define BRSH_DATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif

#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif

#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif

#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif

#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif

#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

#ifndef PRTY_TRIGGER_HPP
#include "Core/prty/prtyTrigger.hpp"
#endif

//============================================================================
//============================================================================
class brshData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	brshData();

	//---------------------------------------------------------------------------
	//	brush property
	//---------------------------------------------------------------------------	
	prtyBoolean		m_bEnabled;
	prtyText		m_CurrentCanvas;
	prtyColor		m_Color;
	prtyFloat		m_BrushSize;
	prtyFloat		m_Opacity;
	prtyFilePath	m_BrushShape;	
	prtyTrigger		m_SaveButton; 

};  // end brshData class