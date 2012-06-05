/********************************************************************************************\
**  mtrlWater.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlWater.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Graphics/eff/effWaterData.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"

mtrlWater::mtrlWater(mdlMaterialInfo& i_Data, effWaterData* i_pShaderData)
:	m_Material(i_Data), 
	m_pShaderData(i_pShaderData)
{
	DBG_ASSERT0(m_pShaderData != NULL, "mtrlWater expected effWaterData");

	m_Data.m_FadeBias.SetValue(m_pShaderData->m_FadeBias);
	m_Data.m_FadeExp.SetValue(m_pShaderData->m_FadeExp);
	m_Data.m_NoiseBumpFactor.SetValue(m_pShaderData->m_NoiseBumpFactor);
	m_Data.m_NoiseSpeed.SetValue(m_pShaderData->m_NoiseSpeed);
	m_Data.m_RingBumpFactor.SetValue(m_pShaderData->m_RingBumpFactor);
	m_Data.m_RingFreq.SetValue(m_pShaderData->m_RingFreq);
	m_Data.m_RingSpeed.SetValue(m_pShaderData->m_RingSpeed);
	m_Data.m_TimeOffset.SetValue(m_pShaderData->m_TimeOffset);
	m_Data.m_WaveSpeed.SetValue(m_pShaderData->m_WaveSpeed);
	m_Data.m_RingCenter.SetValue(maPoint3d(m_pShaderData->m_RingCenter.GetX(),
		m_pShaderData->m_RingCenter.GetY(),
		m_pShaderData->m_RingCenter.GetZ()));
	m_Data.m_WaterColor.SetValue(m_pShaderData->m_WaterColor);

	// Create channels for the properties (this will set the "original value"
	// for the channels to the value in the material data).
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_FadeBias);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_FadeExp);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_NoiseBumpFactor);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_NoiseSpeed);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_RingBumpFactor);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_RingFreq);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_RingSpeed);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_TimeOffset);
	this->AddFloatChannel(m_Material.GetMaterialName(), m_Data.m_WaveSpeed);
	//this->AddVectorChannel(m_Material.GetMaterialName(), m_Data.m_RingCenter);
	this->AddColorChannel(m_Material.GetMaterialName(), m_Data.m_WaterColor);

	RegisterData();
}


void mtrlWater::RegisterData()
{
	prtyPropertyUIInfo* pPUII;
	pPUII  = new prtyColorRGBAEditUIInfo(&(m_Data.m_WaterColor), "Color", "Color of the water");
	AddProperty( pPUII );

	prtyRangedFloatUIInfo* pRFUII = NULL;
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_FadeBias), "Reflection", "fade btw reflection and water color");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_FadeExp), "Reflection", "fade curve exponent");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(20.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_NoiseBumpFactor), "Noise", "noise amount");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(2.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_NoiseSpeed), "Noise", "noise speed");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_RingBumpFactor), "Wave", "ripple amount");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(2.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_RingFreq), "Wave", "ripple frequency");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(5.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_RingSpeed), "Wave", "ripple speed");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(50.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_TimeOffset), "Wave", "time offset for ripples");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(10000.0f);
	AddProperty( pRFUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_WaveSpeed), "Wave", "Wave speed");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(2.0f);
	AddProperty( pRFUII );
	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_RingCenter), "Wave", "Ripple center");
	AddProperty( pPUII );


	m_Data.m_FadeBias.AddCallback(new prtyCallbackWrapper<mtrlWater>(this, &mtrlWater::Update));
	m_Data.m_FadeExp.AddCallback(new prtyCallbackWrapper<mtrlWater>(this, &mtrlWater::Update));
	m_Data.m_NoiseBumpFactor.AddCallback(new prtyCallbackWrapper<mtrlWater>(this, &mtrlWater::Update));
	m_Data.m_NoiseSpeed.AddCallback(new prtyCallbackWrapper<mtrlWater>(this, &mtrlWater::Update));
	m_Data.m_RingBumpFactor.AddCallback(new prtyCallbackWrapper<mtrlWater>(this, &mtrlWater::Update));
	m_Data.m_RingFreq.AddCallback(new prtyCallbackWrapper<mtrlWater>(this, &mtrlWater::Update));
	m_Data.m_RingSpeed.AddCallback(new prtyCallbackWrapper<mtrlWater>(this, &mtrlWater::Update));
	m_Data.m_TimeOffset.AddCallback(new prtyCallbackWrapper<mtrlWater>(this, &mtrlWater::Update));
	m_Data.m_WaveSpeed.AddCallback(new prtyCallbackWrapper<mtrlWater>(this, &mtrlWater::Update));
	m_Data.m_RingCenter.AddCallback(new prtyCallbackWrapper<mtrlWater>(this, &mtrlWater::Update));
	m_Data.m_WaterColor.AddCallback(new prtyCallbackWrapper<mtrlWater>(this, &mtrlWater::Update));
}

void mtrlWater::Update(prtyProperty *i_pProperty, bool i_bDirty)
{
	if (i_bDirty)
	{
		effWaterData* pData = dynamic_cast<effWaterData*>(m_Material.ShaderData());
		DBG_ASSERT0(pData != NULL, "mtrlWater expected effWaterData");
		set_shader_data(pData);	// set into material template
	}
	set_shader_data(m_pShaderData);	// set into material's shader data directly
}

//----------------------------------------------------------------------------
// Set shader data from material into shader data
//----------------------------------------------------------------------------
void mtrlWater::set_shader_data(effWaterData* i_pData)
{
	i_pData->m_FadeBias = m_Data.m_FadeBias.GetValue();
	i_pData->m_FadeExp = m_Data.m_FadeExp.GetValue();
	i_pData->m_NoiseBumpFactor = m_Data.m_NoiseBumpFactor.GetValue();
	i_pData->m_NoiseSpeed = m_Data.m_NoiseSpeed.GetValue();
	i_pData->m_RingBumpFactor = m_Data.m_RingBumpFactor.GetValue();
	i_pData->m_RingFreq = m_Data.m_RingFreq.GetValue();
	i_pData->m_RingSpeed = m_Data.m_RingSpeed.GetValue();
	i_pData->m_TimeOffset = m_Data.m_TimeOffset.GetValue();
	i_pData->m_WaveSpeed = m_Data.m_WaveSpeed.GetValue();
	i_pData->m_RingCenter = maVector4d(m_Data.m_RingCenter.GetValue().GetX(), 
		m_Data.m_RingCenter.GetValue().GetY(),
		m_Data.m_RingCenter.GetValue().GetZ(), 1);
	i_pData->m_WaterColor = m_Data.m_WaterColor.GetValue();
}
