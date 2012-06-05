/****************************************************************************\
**  cptrRenderBakeData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderBakeData.hpp"
#include "Graphics/g3d/g3dBake.hpp"

cptrRenderBakeData::cptrRenderBakeData()
	: m_BakeMode("Bake Mode"),
	  m_OutputFormat("Texture Format"),
	  m_OutputResolution("Texture Resolution"),
	  m_OutputDir("Output Directory"),
	  m_bIsLit("Bake Light", true),
	  m_bIsEnvironment("Bake Enviornment", true),
	  m_bIsShadows("Bake Shadow", true),
	  m_bHDRAA("Hardware Antialiasing", true),
	  m_bIsBakeNormal("Bake Normal", true),
	  m_bIsBakeColor("Bake Color", true),
	  m_bIsAOVolume("Bake AO Volume", true),
	  m_bIsLPVGI("Bake LPV GI", true),
	  m_bIsSaveAndReplace("Replace Material and Save", true)
{
	m_BakeMode.SetEnumTag(g3dBake::bk_Color, "Color");
	m_BakeMode.SetEnumTag(g3dBake::bk_Normals, "Normals");

	m_OutputFormat.SetEnumTag(bk_DDS, "DDS");
	m_OutputFormat.SetEnumTag(bk_BMP, "BMP");
	m_OutputFormat.SetEnumTag(bk_JPG, "JPG");
	m_OutputFormat.SetEnumTag(bk_PNG, "PNG");
	//m_OutputFormat.SetEnumTag(bk_TGA, "TGA");
	m_OutputFormat.SetEnumTag(bk_TIF, "TIF");

	m_OutputResolution.SetEnumTag(bk_512, "512x512");
	m_OutputResolution.SetEnumTag(bk_1024, "1024x1024");
	m_OutputResolution.SetEnumTag(bk_2048, "2048x2048");
	m_OutputResolution.SetEnumTag(bk_4096, "4096x4096");
}

