/****************************************************************************\
**	TemplateManager.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "stdafx.h"
#include "TemplateManager.hpp"

#include "mtrDialogUtil.hpp"
#include "mtrLevel.hpp"
#include "mtrOperations.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void TemplateManager::menuItem_Click(System::Object *  sender, System::EventArgs *  e)
{
	System::Windows::Forms::MenuItem *item = __try_cast<System::Windows::Forms::MenuItem*>(sender);

	// Set template material into selected material
	mtrOperations::ApplyMaterialTemplate( mtrTemplateMgr::GetTemplateMaterial(item->Index) );
	mtrDialogUtil::UpdateMaterialDialog();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void TemplateManager::menu_popup(System::Object *  sender, System::EventArgs *  e)
{
	bool have_Material = (mtrLevel::GetNumMaterials() > 0);
	
	int num_items = this->m_pMenu->MenuItems->Count;
	for (int i=0; i<num_items; i++)
	{
		this->m_pMenu->MenuItems->get_Item(i)->Enabled = have_Material;
	}
}