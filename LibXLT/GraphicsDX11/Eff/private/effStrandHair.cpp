/*****************************************************************************
**  effStrandHair.cpp
**
**      effStrandHair is the DX11 implementation of a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/eff/effStrandHair.hpp"

#include "Graphics/eff/effStrandHairData.hpp"

#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/G3d/g3dPrefs.hpp"

namespace
{
	ID3D11ShaderResourceView* l_pVolumeTexture = NULL;
}

effStrandHair::effStrandHair(const fsLocator& i_Directory, ID3DX11Effect* i_pEffect, std::string i_name)
:	effShaderBaseDX11(i_Directory, i_pEffect, i_name)
{
	MapParameter( "hasHairSupport", m_hHasHairSupport );
	MapParameter( "g_bHasOSM", m_hHasOSM );
	MapParameter( "g_bHasOSMMulti", m_hHasOSMMulti );
	MapParameter( "g_bHasOSMMulti32", m_hHasOSMMulti32 );
	MapParameter( "g_HasDepthMap", m_hHasDepthMap );
	MapParameter( "projOSM", m_hOSM[0] );
	MapParameter( "projOSM2", m_hOSM[1] );
	MapParameter( "projOSM3", m_hOSM[2] );
	MapParameter( "projOSM4", m_hOSM[3] );
	MapParameter( "projOSM5", m_hOSM[4] );
	MapParameter( "projOSM6", m_hOSM[5] );
	MapParameter( "projOSM7", m_hOSM[6] );
	MapParameter( "projOSM8", m_hOSM[7] );
	MapParameter( "depthMap", m_hDepthMap );
	MapParameter( "g_ZNear", m_hZNear );
	MapParameter( "g_ZFar", m_hZFar );
	MapParameter( "g_InvScreenSize", m_hInvScreenSize );
	MapParameter( "g_LightViewPlane", m_hLightViewPlane );
//	MapParameter( "g_Aspect", m_hAspect );
	MapParameter( "g_SubPixelPower", m_hSubPixelPower );
	MapParameter( "hairDataTexture", m_hDataTexture );
	MapParameter( "g_HasOpacityShadowTexture3D", m_hHasOpacityVolume );
	MapParameter( "g_HairTessellationValue", m_hTessellation );
	MapParameter( "OpacityShadowTexture3D", m_hOpacityVolume );
	MapParameter( "g_ClumpRadius", m_hClumpRadius );
}

void effStrandHair::SetupMaterial(const matMaterial* i_Material,
							int i_MaterialLayerIndex) const
{
	effShaderBaseDX11::SetupMaterial( i_Material, i_MaterialLayerIndex );	//setup base material first

	const effStrandHairData* pData = &(i_Material->GetStrandHairData());
	DBG_ASSERT(pData != NULL, "Bad effect data type matched with effStrandHair");

	//set near and far to object bounds
	m_hZNear->SetFloat(pData->m_ZNear);
	m_hZFar->SetFloat(pData->m_ZFar);

	m_hInvScreenSize->SetFloatVector( pData->m_InvScreenSize.Ptr());
	//set the light plane
	m_hLightViewPlane->SetFloatVector( pData->m_LightViewPlane.Ptr() );

	//set the subpixel power value
	m_hSubPixelPower->SetFloat(pData->m_SubPixelPower);

	//set opacity maps
	bool bOSM = (pData->m_pOSM != NULL);	//it has an OSM if the first is valid
	bool bOSMMulti = (pData->m_pOSM[3] != NULL);	//it's multi if the fourth is valid
	bool bOSMMulti32 = (pData->m_pOSM[7] != NULL);	//it's multi32 if the seventh is valid

	m_hHasOSM->SetBool( (bOSM && !bOSMMulti) ? TRUE : FALSE );
	m_hHasOSMMulti->SetBool( bOSMMulti ? TRUE : FALSE );
	m_hHasOSMMulti32->SetBool( bOSMMulti32 ? TRUE : FALSE );

	for( int i = 0; i < 8; i++ )
	{
		m_hOSM[i]->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_pOSM[i]));
	}

	//set depth map
	m_hHasDepthMap->SetBool( (pData->m_pDepthTexture != NULL) ? TRUE : FALSE );
	m_hDepthMap->SetResource(g3dDX11TextureUtil::GetD3DTexture(pData->m_pDepthTexture));

	//set Opacity Volume
	m_hHasOpacityVolume->SetBool( (l_pVolumeTexture != NULL) ? TRUE : FALSE );
	m_hOpacityVolume->SetResource( l_pVolumeTexture );

	//set tessellation
	maVector2d Tess;
	Tess.SetX( g3dPrefs::CurrentPrefs().m_HairTessellation );
	Tess.SetY( (float)g3dPrefs::CurrentPrefs().m_HairInterpolationCount );
	m_hTessellation->SetFloatVector( Tess.Ptr() );

	m_hClumpRadius->SetFloat( g3dPrefs::CurrentPrefs().m_HairClumpRadius );
}

effShaderData* effStrandHair::CreateData(const matMaterial* i_Mat)
{
	return new effStrandHairData;
}

void effStrandHair::SetVolumeTexture( ID3D11ShaderResourceView* i_pVolume )
{
	l_pVolumeTexture = i_pVolume;
}

