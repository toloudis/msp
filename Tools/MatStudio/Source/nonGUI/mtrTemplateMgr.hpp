/*****************************************************************************
**  mtrTemplateMgr.hpp
**
**		Manages templates loaded from special directory that define
**	common configurations of material settings and texture layers.
**	This makes it easier to apply a common material to new mesh.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef MTR_TEMPLATEMGR_HPP
#error mtrTemplateMgr.hpp multiply included
#endif
#define MTR_TEMPLATEMGR_HPP

#ifndef MTR_MATERIALTEMPLATE_HPP
#include "mtrMaterialTemplate.hpp"
#endif
#ifndef IT_STRING_HPP
#include "itString.hpp"
#endif

//============================================================================
//============================================================================
class fsLocator;

namespace mtrTemplateMgr
{

	//============================================================================
	//	Initialize()
	//============================================================================
	void Initialize(const fsLocator &i_TemplateDir);

	//============================================================================
	//	DeInitialize()
	//============================================================================
	void DeInitialize();

	//============================================================================
	//	GetNumTemplates()
	//============================================================================
	int GetNumTemplates();

	//============================================================================
	//	GetTemplateName()
	//============================================================================
	itString GetTemplateName(int i_Index);

	//============================================================================
	//	GetTemplateMaterial()
	//============================================================================
	mtrMaterialTemplate GetTemplateMaterial(int i_Index);

}
