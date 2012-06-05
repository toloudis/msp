/****************************************************************************\
**	rasterizerUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/private/rasterizerUtil.hpp"

#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
//#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#include "GraphicsDX11/g2d/g2dWindowDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/eff/effShaderArray.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maVector2d.hpp"
//#include "Core/ma/maVector3d.hpp"
//#include "Core/ma/maVector4d.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"

#include <vector>

namespace 
{
	bool g_bRastCreated = false;
	ID3D11RasterizerState* l_DefaultRasterizerState = NULL;

	void copyMat(const maMatrix4x4& m, float* mf)
	{
		for (int i = 0; i < 16; i++)
			mf[i] = m(i / 4, i % 4);
	}

	struct cbTransforms
	{
		float g_worldIT[16];
		float g_wvp[16];
		float g_wv[16];
		float g_world[16];
		maVector4d g_eyePos;
	};
	// Constant buffer for passing parameters
	ID3D11Buffer* g_pcbTransforms = NULL;


	struct cbMaterial
	{
		maFloatRGBA g_ambient;// : MaterialAmbient = {0.5f, 0.5f, 0.5f, 1.0f};
		maFloatRGBA g_diffuse;// : MaterialDiffuse = {1.0f, 1.0f, 1.0f, 1.0f};
		maFloatRGBA g_specular;// : MaterialSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
		// x=shininess, y=uscale, z=vscale, w=dbl sided
		maVector4d g_params; 
//		float g_shininess;// : MaterialPower = 1.0f;
//		float g_uScale;// : UScale = 1.0f;
//		float g_vScale;// : VScale = 1.0f;
//		bool g_bDoubleSided;// : DoubleSided = false;
	};
	// Constant buffer for passing parameters
	ID3D11Buffer* g_pcbMaterial = NULL;

	struct cbLightInfo
	{
		maVector4d Pos;
		maFloatRGBA Diffuse;
		maFloatRGBA Specular;
		maVector4d Falloff;
		maVector4d ConeInfo;	/* x,y,z are normalized direction, w is cos(ConeAngle) */
	};
	ID3D11Buffer* g_pcbLight = NULL;

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void get_light_info(const g3dLight* i_pLight,
					cbLightInfo &o_Info)
{
	// dmt - why aren't these virtual functions? we don't really need these casts... do we?

	const g3dPointLight* point_light = dynamic_cast<const g3dPointLight*>(i_pLight);
	if (point_light)
	{
		maPoint3d pos = point_light->GetPosition();
		o_Info.Pos.Set(pos.m_X, pos.m_Y, pos.m_Z, 1.0f);

		o_Info.Falloff.Set(point_light->GetFalloff0(), point_light->GetFalloff1(), point_light->GetFalloff2());
	}
	const g3dDirectionalLight* dir_light = dynamic_cast<const g3dDirectionalLight*>(i_pLight);
	if (dir_light)
	{
		maVector3d dir = dir_light->GetDirection(); 
		// handy to reverse the direction
		o_Info.Pos.Set(-dir.m_X, -dir.m_Y, -dir.m_Z, 0.0f);
		o_Info.Falloff.Set(1,0,0);
	}
	const g3dProjectedLight* proj_light = dynamic_cast<const g3dProjectedLight*>(i_pLight);
	if (proj_light)
	{
		maPoint3d pos = proj_light->GetPosition();
		maVector3d dir = proj_light->GetDirection(); 

		o_Info.Pos.Set(pos.m_X, pos.m_Y, pos.m_Z, 1.0f);
		o_Info.ConeInfo.Set(-dir.m_X, -dir.m_Y, -dir.m_Z, 
			cosf(0.5f * proj_light->GetAngle() * maConstants::c_fAngleToRad));

		o_Info.Falloff.Set(proj_light->GetFalloff0(), proj_light->GetFalloff1(), proj_light->GetFalloff2());
	}

	maFloatRGBA light_color = i_pLight->GetIntensity();
//		if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererCreate::e_HDR)
	light_color *= i_pLight->GetIntensityFactor();
	light_color.SetAlpha(1.0f); // make sure light doesn't change alpha of color

	if (i_pLight->IsDiffuseEnabled() && g3dPrefs::CurrentPrefs().m_bEnableDiffuseLighting)
	{
		o_Info.Diffuse = light_color;//maVector4d(light_color.GetRed(), light_color.GetGreen(), light_color.GetBlue(), light_color.GetAlpha());
	}
	else 
	{
		o_Info.Diffuse.Set(0,0,0,1);
	}

	// when baking, turn off specular lighting contribution.
	if (i_pLight->IsSpecularEnabled() 
		&& (g3dPrefs::CurrentPrefs().m_bEnableSpecularLighting || g3dSingleLightRendering::GetDoGlowPass())
		&& !g3dSingleLightRendering::GetDoBaking())
	{
		o_Info.Specular = light_color;//maVector4d(light_color.GetRed(), light_color.GetGreen(), light_color.GetBlue(), light_color.GetAlpha());
		o_Info.Specular.SetAlpha(0.0f);  // need to keep alpha
	}
	else 
	{
		o_Info.Specular.Set(0,0,0,0);
	}
}

#define MAX_LIGHTS				8
int LightingLoop(const g3dSceneNode* i_pNode)
{
	DBG_ASSERT(g_pcbLight , "bad CB");

	// prep shader for light passes:
	ID3D11VertexShader* pVS = NULL;
	ID3D11PixelShader* pPS = NULL;
	effShaderArray::GetDefaultLightingEffect(&pVS, &pPS);

	g2dDX11Global::g_pDeviceContext->VSSetShader(pVS, NULL, 0);
	g2dDX11Global::g_pDeviceContext->PSSetShader(pPS, NULL, 0);

	// Setup some lights:
	// get all the enabled lights
	static std::vector<g3dLight*> enabledLights;
	enabledLights.clear();
	const std::vector<g3dLight*>& sceneLights = g3dLightMgrDX11::Implementation()->GetLights( );
	for (int i = 0; i < sceneLights.size(); i++)
	{
		g3dLight* pLight = sceneLights[i];
		if ( pLight->IsEnabled() ) 
			enabledLights.push_back(pLight);
	}

	int nLights = min(MAX_LIGHTS, enabledLights.size());

	// copy light data into world buffer.
	cbLightInfo curLight;
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	int nTriangles = 0;
	for ( int i = 0 ; i < nLights; i++ ) 
	{
		g3dLight* light = enabledLights[i];
		get_light_info(light, curLight);
		
//		DBG_ASSERT(&curLight == &(curLight.light), "light struct aliignment is off");
//		DBG_ASSERT(sizeof(curLight) == sizeof(curLight.light), "light struct aliignment is off");

		g2dDX11Global::g_pDeviceContext->Map( g_pcbLight, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
		memcpy( MappedResource.pData, &curLight, sizeof(curLight) );
		g2dDX11Global::g_pDeviceContext->Unmap( g_pcbLight, 0 );

		ID3D11Buffer* ppCB[3] = { g_pcbTransforms, g_pcbMaterial, g_pcbLight };
		g2dDX11Global::g_pDeviceContext->VSSetConstantBuffers( 0, 3, ppCB );
		g2dDX11Global::g_pDeviceContext->PSSetConstantBuffers( 0, 3, ppCB );

		nTriangles += g3dRendererMgr::Render( i_pNode, NULL, NULL );
	}
	return nTriangles;
}

int DrawNode(g3dSceneNode* i_pNode)
{
	// resolve material/effect
//	const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;
//	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	// use default technique
//	pEffect->SetTechnique(matShaderEffect::e_Default);

	// set shader globals
//	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
//	g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

	// shading data
//	pEffect->SetupMaterial(pMaterial);

//	pEffect->SetupAmbientLighting(i_pNode->GetWorldBox());

	// I think we actually always want to set up the ambient pass,
	// even if that just means we set it up with somethinng with
	// NULL pointers and booleans set to false
//	static const g3dAmbientEnvState l_NoAmbientEnvState;
//	pEffect->SetupAmbientPass(l_NoAmbientEnvState);

//	int nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );

	void* pShaderBytecode = NULL;
	unsigned long bytecodeLength = 0;
	ID3D11VertexShader* pVS = NULL;
	ID3D11PixelShader* pPS = NULL;
	effShaderArray::GetDefaultEffect(&pShaderBytecode, &bytecodeLength, &pVS, &pPS);

	g2dDX11Global::g_pDeviceContext->VSSetShader(pVS, NULL, 0);
	g2dDX11Global::g_pDeviceContext->PSSetShader(pPS, NULL, 0);

	cbTransforms tf;

	maMatrix4x4 matrix;

	matrix = i_pNode->GetFragment()->IsModelSpaceVertices() ? i_pNode->GetTotalTransform() : maMatrix4x4();
//	matrix.Transpose();

	copyMat(matrix, tf.g_world);
	copyMat(matrix, tf.g_worldIT);

	matrix  = (i_pNode->GetFragment()->IsModelSpaceVertices() ? i_pNode->GetTotalTransform() : maMatrix4x4()) * g3dSceneGlobal::GetCameraTransform();
//	matrix.Transpose();
	copyMat(matrix, tf.g_wv);

	matrix = (i_pNode->GetFragment()->IsModelSpaceVertices() ? i_pNode->GetTotalTransform() : maMatrix4x4()) *
		g3dSceneGlobal::GetCameraTransform() * 
		g3dSceneGlobal::GetProjectionTransform();
//	matrix.Transpose();
	copyMat(matrix, tf.g_wvp);

	tf.g_eyePos = maVector4d(g3dSceneGlobal::GetCameraPos());

    // Setup constant buffers
	DBG_ASSERT(g_pcbTransforms , "bad CB");
	DBG_ASSERT(g_pcbMaterial , "bad CB");

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	g2dDX11Global::g_pDeviceContext->Map( g_pcbTransforms, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	memcpy( MappedResource.pData, &tf, sizeof(tf) );
	g2dDX11Global::g_pDeviceContext->Unmap( g_pcbTransforms, 0 );

	cbMaterial mat;
	matMaterial* pMaterial = i_pNode->GetMaterial();
	if (pMaterial == NULL)
		pMaterial = i_pNode->GetFragment()->GetMaterial();
	const effPhongData* pData = pMaterial->GetTypedData<effPhongData>();
	mat.g_ambient = maFloatRGBA(0,0,0,1);//pData->GetEmissive() + pData->GetAmbient();
	mat.g_diffuse = maFloatRGBA(0.3f,0.3f,0.3f,1);//pData->GetDiffuse();
	mat.g_params.m_X = 100;//pData->m_SpecularPower;
	mat.g_specular = maFloatRGBA(0,0,0,1);//pData->GetSpecular();
	effUVTransform uvx = pMaterial->GetUVTransform();
	mat.g_params.m_Y = uvx.m_UScale;
	mat.g_params.m_Z = uvx.m_VScale;
	mat.g_params.m_W = 0;

	g2dDX11Global::g_pDeviceContext->Map( g_pcbMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
	memcpy( MappedResource.pData, &mat, sizeof(mat) );
	g2dDX11Global::g_pDeviceContext->Unmap( g_pcbMaterial, 0 );

	ID3D11Buffer* ppCB[2] = { g_pcbTransforms, g_pcbMaterial };
//	g2dDX11Global::g_pDeviceContext->VSSetConstantBuffers( 0, 2, ppCB );
//	g2dDX11Global::g_pDeviceContext->PSSetConstantBuffers( 0, 2, ppCB );

	int nTriangles = 0;
//	nTriangles += g3dRendererMgr::Render( i_pNode, NULL, NULL );

	nTriangles += LightingLoop(i_pNode);

	return nTriangles;
}


int RecurseDrawNodes(g3dSceneNode* i_Node)
{
	if (!i_Node->GetRenderable())
		return 0;

	int npoly = 0;

	g3dFragment* frag = i_Node->GetFragment();
	if (frag)
	{
		npoly += DrawNode( i_Node );
	}

	for (int i = 0; i < i_Node->GetNumChildren(); i++)
	{
		npoly += RecurseDrawNodes(i_Node->GetChild(i));
	}
	return npoly;
}

void CreateShaders()
{
}
void DestroyShaders()
{
}
void CreateBuffers(ID3D11Device* pd3dDevice)
{
	D3D11_BUFFER_DESC Desc;
    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc.MiscFlags = 0;    
    Desc.ByteWidth = sizeof( cbTransforms );
	pd3dDevice->CreateBuffer( &Desc, NULL, &g_pcbTransforms );

    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc.MiscFlags = 0;    
    Desc.ByteWidth = sizeof( cbMaterial );
	pd3dDevice->CreateBuffer( &Desc, NULL, &g_pcbMaterial );

    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc.MiscFlags = 0;    
    Desc.ByteWidth = sizeof( cbLightInfo );
	pd3dDevice->CreateBuffer( &Desc, NULL, &g_pcbLight );

}
void DestroyBuffers()
{
	g_pcbMaterial->Release();
	g_pcbTransforms->Release();
	g_pcbLight->Release();
}
void CreateFullscreenQuad(ID3D11Device* pd3dDevice)
{
	// RS state
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory( &rsDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FillMode = D3D11_FILL_SOLID;//D3D11_FILL_WIREFRAME;
/////////
// ALERT!!!
	// this flag might have to change depending on the fullscreen quad or mesh rendering.
	// to be settled later.
	// leaving it like this for compatibility with code grabbed from sdk samples.
	rsDesc.FrontCounterClockwise = TRUE;
/////////
	rsDesc.DepthClipEnable = TRUE;
    g2dDX11Global::g_pDevice->CreateRasterizerState(&rsDesc, &l_DefaultRasterizerState);

}
void DestroyFullscreenQuad()
{
	l_DefaultRasterizerState->Release();
	l_DefaultRasterizerState=NULL;
}

} // namespace

void RasterPass::Init()
{
	if (g_bRastCreated)
		return;
	g_bRastCreated = true;

	CreateShaders();
	CreateBuffers(g2dDX11Global::g_pDevice);
	CreateFullscreenQuad(g2dDX11Global::g_pDevice);
}

void RasterPass::Render(g2dRenderTarget* i_pWindow, const camCamera* i_pCamera, const g3dScene* i_pScene)
{
	g2dDX11Global::g_pDeviceContext->RSSetState( l_DefaultRasterizerState );

	i_pWindow->MakeCurrent();

	g3dLightMgr::EnableHeadlight(true);
	g3dLightMgr::SetHeadlightDirection( i_pCamera->GetDirection() );

	int npoly = 0;
	int n = i_pScene->GetNumLayers();
	for (int i = 0; i < n; i++)
	{
		npoly += RecurseDrawNodes(i_pScene->GetLayer(i)->GetRootNode());
	}

	// copy output into window backbuffer:
//	g2dWindowDX11* pWnd = dynamic_cast<g2dWindowDX11*>(i_pWindow);
//	g2dD3D11RenderTargetPtr pTarget = pWnd->GetBackBuffer();
//	hr = DumpToTexture(g2dDX11Global::g_pDeviceContext, RES_X, RES_Y,
//		g_pRayTraceRV, pTarget );
//	pTarget->Release();

	g3dLightMgr::EnableHeadlight(g3dPrefs::CurrentPrefs().m_bHeadlightOn);
	g3dLightMgr::SetHeadlightDirection( i_pCamera->GetDirection() );

}

void RasterPass::CleanUp()
{
	if (!g_bRastCreated)
		return;
	g_bRastCreated = false;

	DestroyFullscreenQuad();
	DestroyBuffers();
	DestroyShaders();
}

