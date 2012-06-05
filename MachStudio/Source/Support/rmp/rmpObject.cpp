/*****************************************************************************
**	rmpObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/rmp/rmpObject.hpp"

#include "Support/rmp/rmpData.hpp"
#include "Support/rmp/rmpDialogMgr.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyGradientEditUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
rmpObject::rmpObject()
 : m_Callback(NULL),
   m_pPasteButton(NULL)
{
	RegisterProperties();		
}
rmpObject::~rmpObject()
{		
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rmpObject::SetRampChangedCallback(const RampChangedFunction& i_FuncPtr)
{
	m_Callback = i_FuncPtr;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rmpObject::RegisterProperties()
{
	prtyPropertyUIInfo* pPUII;
	prtyComboBoxUIInfo* pCBUII;
	prtyRangedFloatUIInfo* pRFUII;

	pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_Shape), "Asset", "Ramp shape");
	AddProperty( pCBUII );
	pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_Interpolation), "Asset", "Ramp interpolation method");
	AddProperty( pCBUII );

	pCBUII  = new prtyComboBoxUIInfo(&(m_Data.m_TexSize), "Asset", "Ramp texture size");
	pCBUII->AddItem(std::string("256"), 0);
	pCBUII->AddItem(std::string("512"), 1);
	pCBUII->AddItem(std::string("1024"), 2);
	pCBUII->AddItem(std::string("2048"), 3);
	pCBUII->AddItem(std::string("4096"), 4);
	pCBUII->AddItem(std::string("8192"), 5);
	AddProperty( pCBUII );

	pPUII  = new prtyGradientEditUIInfo(&(m_Data.m_Gradient), "Asset", "Ramp Editor");
	AddProperty( pPUII );

	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_UWave), "Asset", "U Wave");
	pRFUII->SetMaximum(1.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty( pRFUII );
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_UWaveFreq), "Asset", "U Wave Frequency");
	pRFUII->SetMaximum(1.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty( pRFUII );
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_VWave), "Asset", "V Wave");
	pRFUII->SetMaximum(1.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty( pRFUII );
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_VWaveFreq), "Asset", "V Wave Frequency");
	pRFUII->SetMaximum(1.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty( pRFUII );
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_Noise), "Asset", "Noise");
	pRFUII->SetMaximum(1.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty( pRFUII );
	pRFUII = new prtyRangedFloatUIInfo(&(m_Data.m_NoiseFreq), "Asset", "Noise Frequency");
	pRFUII->SetMaximum(1.0);
	pRFUII->SetDecimalPlaces(3);
	AddProperty( pRFUII );

	prtyButtonUIInfo* pBUII;
	pBUII = new prtyButtonUIInfo(&(m_Data.m_Copy), "Asset", "Copy");
	pBUII->SetText("Copy");
	AddProperty( pBUII );

	m_pPasteButton = new prtyButtonUIInfo(&(m_Data.m_Paste), "Asset", "Paste");
	m_pPasteButton->SetText("Paste");
	m_pPasteButton->SetReadOnly(true);
	AddProperty( m_pPasteButton );

	//	Register callbacks for items that need to be updated immediately.
	m_Data.m_Shape.AddCallback(new prtyCallbackWrapper<rmpObject>(this, &rmpObject::RampPropertyChanged));
	m_Data.m_Interpolation.AddCallback(new prtyCallbackWrapper<rmpObject>(this, &rmpObject::RampPropertyChanged));
	m_Data.m_TexSize.AddCallback(new prtyCallbackWrapper<rmpObject>(this, &rmpObject::RampPropertyChanged));
	m_Data.m_Gradient.AddCallback(new prtyCallbackWrapper<rmpObject>(this, &rmpObject::RampPropertyChanged));
	m_Data.m_UWave.AddCallback(new prtyCallbackWrapper<rmpObject>(this, &rmpObject::RampPropertyChanged));
	m_Data.m_UWaveFreq.AddCallback(new prtyCallbackWrapper<rmpObject>(this, &rmpObject::RampPropertyChanged));
	m_Data.m_VWave.AddCallback(new prtyCallbackWrapper<rmpObject>(this, &rmpObject::RampPropertyChanged));
	m_Data.m_VWaveFreq.AddCallback(new prtyCallbackWrapper<rmpObject>(this, &rmpObject::RampPropertyChanged));
	m_Data.m_Noise.AddCallback(new prtyCallbackWrapper<rmpObject>(this, &rmpObject::RampPropertyChanged));
	m_Data.m_NoiseFreq.AddCallback(new prtyCallbackWrapper<rmpObject>(this, &rmpObject::RampPropertyChanged));
	m_Data.m_Copy.AddCallback(new prtyCallbackWrapper<rmpObject>(this, &rmpObject::RampCopyRequest));
	m_Data.m_Paste.AddCallback(new prtyCallbackWrapper<rmpObject>(this, &rmpObject::RampPasteRequest));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rmpObject::RampPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (m_Callback)
		m_Callback(i_bDirty);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rmpObject::RampCopyRequest(prtyProperty *i_pProperty, bool i_bDirty)
{
	if( m_pPasteButton != NULL )
	{
		m_pPasteButton->SetReadOnly(false);
		m_pPasteButton->UpdateControl();
	}

	rmpDialogMgr::CopyData();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rmpObject::RampPasteRequest(prtyProperty *i_pProperty, bool i_bDirty)
{
	if( m_pPasteButton != NULL )
	{
		m_pPasteButton->SetReadOnly(true);
		m_pPasteButton->UpdateControl();
	}

	rmpDialogMgr::PasteData();
}