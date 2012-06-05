/*****************************************************************************
**	mtrOperations.cpp
**
**	Interface for dialogs to change material info
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "StdAfx.h"
#include "mtrOperations.hpp"

#include "mtrLevel.hpp"

#include "dbgLog.hpp"

namespace mtrOperations
{

	namespace
	{
		int l_SelMatIndex = -1;


	}	// end of namespace

	//--------------------------------------------------------------------
	// SetSelectedMaterialIndex - notify this utility when the selected 
	//	material index has changed. Use "-1" for no selected material.
	//--------------------------------------------------------------------
	void  SetSelectedMaterialIndex(int i_Index)
	{
		l_SelMatIndex = i_Index;
	}

	//--------------------------------------------------------------------
	// Pass in changed data structure to alter material data for 
	//	selected material
	//--------------------------------------------------------------------
	void ChangeMaterialData(const mtrMaterialTemplate &i_Data)
	{
		if (l_SelMatIndex >= 0)
		{
			mtrLevel::SetMaterialData(l_SelMatIndex, i_Data);
		}
	}

	//--------------------------------------------------------------------
	// Use data structure from template manager to set material for 
	//	selected material
	//--------------------------------------------------------------------
	void ApplyMaterialTemplate(const mtrMaterialTemplate &i_Data)
	{
		if (l_SelMatIndex >= 0)
		{
			mtrLevel::ApplyMaterialTemplate(l_SelMatIndex, i_Data);
		}
	}

}	// end of namespace
