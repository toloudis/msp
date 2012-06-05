/********************************************************************************************\
**  ptltData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#ifdef PTLT_DATA_HPP
#error ptltData.hpp multiply included
#endif
#define PTLT_DATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
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


//============================================================================
//============================================================================
class ptltData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	ptltData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	ptltData(const ptltData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~ptltData();

	//-------------------------------------------------
	bool operator == (const ptltData& i_Item);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	ptltData& operator=(const ptltData& i_Data);

public:
	prtyBoolean				m_Enabled;
	prtyBoolean				m_bEditorVisible;
	prtyColor				m_Color;
	prtyBoolean				m_ShadowSource;
	prtyPoint3d				m_Falloff;
	prtyDistance			m_Range;
	prtyFloat				m_Intensity;
	prtyName				m_Name;
	prtyPoint3dTransform	m_Position;
	prtyBoolean				m_bDiffuseEnabled;
	prtyBoolean				m_bSpecularEnabled;
	prtyBoolean				m_bAffectsFur;
	prtyBoolean				m_bAffectsGlow;
};
