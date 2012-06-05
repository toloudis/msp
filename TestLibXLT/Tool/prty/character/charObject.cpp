//
//	char system object
//
#include "charObject.hpp"

#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyFolderChooserUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"


//------------------------------------------------------------------------
//------------------------------------------------------------------------
charObject::charObject()
{
	RegisterProperties();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void charObject::RegisterProperties()
{
	prtyPropertyUIInfo* pPUII;

	pPUII = new prtyCheckBoxUIInfo(&(m_basedata.m_bEditorVisible), "category2", "Is this object visible in the editor");
	AddProperty( pPUII );
	pPUII = new prtyFloatEditUIInfo(&(m_basedata.m_FOV), "category2", "Field of View");
	AddProperty( pPUII );
	pPUII = new prtyVector3dEditUIInfo(&(m_basedata.m_Position), "category1", "position of the object");
	AddProperty( pPUII );
	pPUII = new prtyTextBoxUIInfo(&(m_basedata.m_FileName), "category3", "File Name");
	AddProperty( pPUII );
	pPUII = new prtyVector3dEditUIInfo(&(m_basedata.m_Orientation), "category1", "far clipping plane");
	AddProperty( pPUII );
	pPUII = new prtyFolderChooserUIInfo(&(m_basedata.m_Dir), "category3", "Special Texture Directory");
	AddProperty( pPUII );
	pPUII = new prtyFileChooserUIInfo(&(m_basedata.m_File), "category3", "The Character File");
	AddProperty( pPUII );
	pPUII = new prtyComboBoxUIInfo(&(m_basedata.m_State), "category4", "The emotional state of the character");
	AddProperty( pPUII );
}

