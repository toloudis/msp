/*****************************************************************************
**	brshPropertyObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/brsh/data/brshPropertyObject.hpp"

#include "Support/brsh/brshPaintBrushMgr.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/prty/prtyButtonUIInfo.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
brshPropertyObject::brshPropertyObject()
:	m_pSaveButton(NULL)
{
	Init();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
brshPropertyObject::~brshPropertyObject()
{
}

//----------------------------------------------------------------------------
// Return the texture location of this object
//----------------------------------------------------------------------------
fsLocator& brshPropertyObject::GetTextureLocator()
{
	return m_ShapeName;
}

//----------------------------------------------------------------------------
// Set the read only state for the save button
//----------------------------------------------------------------------------
void brshPropertyObject::SetSaveEnabled(bool i_bEnabled)
{
	m_pSaveButton->SetReadOnly(!i_bEnabled);
	m_pSaveButton->UpdateControl();
}

//----------------------------------------------------------------------------
// Set the read only state for the save button
//----------------------------------------------------------------------------
bool brshPropertyObject::GetSaveEnabled()
{
	return !m_pSaveButton->GetReadOnly();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void brshPropertyObject::Init()
{
	m_ShapeName = fsLocator();
	RegisterProperties();
	AddCallbacks();
}

//------------------------------------------------------------------------
// Assign data objects to their UI components
//------------------------------------------------------------------------
void brshPropertyObject::RegisterProperties()
{
	prtyPropertyUIInfo* pPUII;
	prtyRangedFloatUIInfo* pRFUII;
	prtyColorRGBAEditUIInfo* pRGBUII;
	prtyFileChooserUIInfo* pFCUII;
//	prtyTextureFileChooserUIInfo* pTFCUII;

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bEnabled), "Paint Brush", "Enable Paint Mode");
	this->AddProperty( pPUII );
	
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_CurrentCanvas), "Paint Brush", "File path of current paint object");
	pPUII->SetReadOnly(true);
	this->AddProperty( pPUII );

	pRGBUII = new prtyColorRGBAEditUIInfo(&(m_Data.m_Color), "Paint Brush", "Paint Color");
	this->AddProperty(pRGBUII);

	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_BrushSize), "Paint Brush", "Size of paint brush");
	pRFUII->SetMinimum(0.1f);
	pRFUII->SetMaximum(2.0f);
	pRFUII->SetDecimalPlaces(2);
	pRFUII->SetNumTicks(20);
	pRFUII->SetRestrictFlag(true);
	this->AddProperty(pRFUII);
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_Opacity), "Paint Brush", "Level of paint Opacity");
	pRFUII->SetMinimum(0);
	pRFUII->SetMaximum(100);
	pRFUII->SetDecimalPlaces(0);
	pRFUII->SetNumTicks(20);
	pRFUII->SetRestrictFlag(true);
	this->AddProperty(pRFUII);
	
	pFCUII = new prtyFileChooserUIInfo(&(m_Data.m_BrushShape), "Paint Brush", "Texture to define the brush shape");
	this->AddProperty(pFCUII);

	m_pSaveButton = new prtyButtonUIInfo(&(m_Data.m_SaveButton), "Paint Brush", "Save the painted texture to file");
	m_pSaveButton->SetText("Save Painting");
	this->AddProperty(m_pSaveButton);
	m_pSaveButton->SetReadOnly(true);
	//pTFCUII = new prtyTextureFileChooserUIInfo(&(m_Data.m_Brush), "Paint Brush", "Other Texture File");
	//pTFCUII->AddItem(pTFCUII->e_Paint);
	//pTFCUII->AddItem(pTFCUII->e_Ramp);
	//this->AddProperty(pTFCUII);
}

//------------------------------------------------------------------------
// Assign the function callbacks for each data object
//------------------------------------------------------------------------
void brshPropertyObject::AddCallbacks()
{
	m_Data.m_bEnabled.AddCallback(new prtyCallbackWrapper<brshPropertyObject>(this, &brshPropertyObject::UpdatePaintMode));
	m_Data.m_BrushShape.AddCallback(new prtyCallbackWrapper<brshPropertyObject>(this, &brshPropertyObject::UpdateBrushShape));
	m_Data.m_Color.AddCallback(new prtyCallbackWrapper<brshPropertyObject>(this, &brshPropertyObject::UpdateBrushColor));
	m_Data.m_Opacity.AddCallback(new prtyCallbackWrapper<brshPropertyObject>(this, &brshPropertyObject::UpdateBrushColor));
	m_Data.m_SaveButton.AddCallback(new prtyCallbackWrapper<brshPropertyObject>(this, &brshPropertyObject::SaveTexture));
}

//------------------------------------------------------------------------
// Property change callback
//------------------------------------------------------------------------
//virtual
void brshPropertyObject::UpdatePaintMode(prtyProperty *i_pProperty, bool i_bDirty)
{
	bool bEnabled = m_Data.m_bEnabled.GetValue();
	brshPaintBrushMgr::EnablePaint(bEnabled);
}
//virtual
void brshPropertyObject::UpdateBrushColor(prtyProperty *i_pProperty, bool i_bDirty)
{
	brshPaintBrushMgr::UpdateBrushStroke();
}
//virtual
void brshPropertyObject::UpdateBrushShape(prtyProperty *i_pProperty, bool i_bDirty)
{
	//ConfirmShapeTexture();
	m_ShapeName = m_Data.m_BrushShape.GetValue();
	brshPaintBrushMgr::UpdateBrushStroke();
}
//virtual
void brshPropertyObject::SaveTexture(prtyProperty *i_pProperty, bool i_bDirty)
{
	fsLocator saveLocation = m_ShapeName;
	if (guiFileDialogUtils::GetSaveFileName(std::string("All files (*.*)|*.*"), saveLocation))
	{
		brshPaintBrushMgr::SaveImage(saveLocation);
	}
}




