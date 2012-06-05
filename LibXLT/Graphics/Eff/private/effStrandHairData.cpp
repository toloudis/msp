#include "Graphics/eff/effStrandHairData.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Core/it/itString.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include <vector>
#include <algorithm>
#include <functional>

effStrandHairData::effStrandHairData()
{
	Default();
}

effStrandHairData::~effStrandHairData()
{
}

effStrandHairData::effStrandHairData(const effStrandHairData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

effStrandHairData& effStrandHairData::operator = (const effStrandHairData& i_CopyFrom)
{
	if (&i_CopyFrom == this) return *this;

	effShaderData::operator = (i_CopyFrom);

	m_ZNear = i_CopyFrom.m_ZNear;
	m_ZFar = i_CopyFrom.m_ZFar;
	m_InvScreenSize = i_CopyFrom.m_InvScreenSize;
	m_LightViewPlane = i_CopyFrom.m_LightViewPlane;
	m_SubPixelPower = i_CopyFrom.m_SubPixelPower;

	for( int i = 0; i < 8; i++ )
	{
		m_pOSM[i] = i_CopyFrom.m_pOSM[i];
	}
	m_pDepthTexture = i_CopyFrom.m_pDepthTexture;
	m_pDataTexture = i_CopyFrom.m_pDataTexture;
	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effStrandHairData::operator == (const effStrandHairData& i_EffStrandHairData) const
{

	return (m_ZNear == i_EffStrandHairData.m_ZNear) &&
	(m_ZFar == i_EffStrandHairData.m_ZFar) &&
	(m_InvScreenSize == i_EffStrandHairData.m_InvScreenSize) &&
	(m_SubPixelPower == i_EffStrandHairData.m_SubPixelPower) &&
	(m_LightViewPlane == i_EffStrandHairData.m_LightViewPlane) &&
	(m_pOSM[0] == i_EffStrandHairData.m_pOSM[0]) &&
	(m_pOSM[1] == i_EffStrandHairData.m_pOSM[1]) &&
	(m_pOSM[2] == i_EffStrandHairData.m_pOSM[2]) &&
	(m_pOSM[3] == i_EffStrandHairData.m_pOSM[3]) &&
	(m_pOSM[4] == i_EffStrandHairData.m_pOSM[4]) &&
	(m_pOSM[5] == i_EffStrandHairData.m_pOSM[5]) &&
	(m_pOSM[6] == i_EffStrandHairData.m_pOSM[6]) &&
	(m_pOSM[7] == i_EffStrandHairData.m_pOSM[7]) &&
	(m_pDepthTexture == i_EffStrandHairData.m_pDepthTexture) &&
	(m_pDataTexture == i_EffStrandHairData.m_pDataTexture);
}

void effStrandHairData::Default()
{
	m_SubPixelPower = 1.0f,
	m_LightViewPlane = maVector4d( 0, 0, 1, 0 );
	m_ZNear = 1.0f;
	m_ZFar = 1000.0f;
	m_pDepthTexture = NULL;
	m_pDataTexture = NULL;

	for( int i = 0; i < 8; i++ )
	{
		m_pOSM[i] = NULL;
	}
}
