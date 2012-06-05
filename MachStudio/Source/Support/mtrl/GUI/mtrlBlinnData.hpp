/*****************************************************************************
**  mtrlBlinnData.hpp
**
**      mtrlBlinnData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_BLINNDATA_HPP
#error mtrlBlinnData.hpp multiply included
#endif
#define MTRL_BLINNDATA_HPP

#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif


//============================================================================
//============================================================================
class mtrlBlinnData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlBlinnData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlBlinnData(const mtrlBlinnData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlBlinnData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlBlinnData& operator=(const mtrlBlinnData& i_Data);

	prtyColor m_ColorAmbient;
	prtyColor m_ColorDiffuse;
	prtyColor m_ColorSpecular;
	prtyColor m_ColorEmissive;

	prtyFloat m_SpecularPower;
	prtyFloat m_BumpMapScale;
	prtyFloat m_Reflectivity;
	prtyFloat m_IOR;
	prtyFloat m_DiffuseRoughness;
	prtyFloat m_Transparency;

	prtyFloat m_UScale, m_VScale;
	prtyFloat m_UTrans, m_VTrans;
	prtyFloat m_UVAngle;

	prtyFileName m_TextureDiffuse;
	prtyFileName m_TextureSpecular;
	prtyFileName m_TextureGloss;
	prtyFileName m_TextureEnvironment;
	prtyFileName m_TextureNormalMap;
	prtyFileName m_TextureReflectFactorMap;
	prtyFileName m_TextureIORMap;
	prtyFileName m_TextureTransparencyMap;
};

