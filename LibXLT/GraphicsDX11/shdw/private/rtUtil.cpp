/****************************************************************************\
**	rtUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/private/rtUtil.hpp"

#include "GraphicsDX11/bump/bumpTriMeshBumpFrag.hpp"
#include "GraphicsDX11/eff/effShaderUtilWin.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/g2dFullscreenQuad.hpp"
#include "GraphicsDX11/g2d/g2dWindowDX11.hpp"
#include "GraphicsDX11/g3d/g3dDX11TextureUtil.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/shdw/shdwPassToneMap.hpp"

#include "Core/app/appTime.hpp"
#include "Core/ma/maConstants.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dPointLight.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

//#include "Tool/api3d/api3dObjectSimple.hpp"
//#include "Tool/api3d/api3dScene.hpp"
//#include "Tool/api3d/api3dShape.hpp"

//--------------------------------------------------------------------------------------
// Global Defines - SHOULD ALWAYS MATCH CPP & HLSL
//--------------------------------------------------------------------------------------
#define RES_X					640
#define RES_Y					360
#define THREAD_REDUCE_FACTOR	4
#define MAX_LIGHTS				4
#define MAX_MESHES				16
#define NUM_RAY_ELEMENTS		4

//--------------------------------------------------------------------------------------
// Local Defines
//--------------------------------------------------------------------------------------
#define RAY_BUFFER_SIZE			RES_X*RES_Y*NUM_RAY_ELEMENTS

#define K_EPSILON				1.0f
#define K_LARGEVAL				9999.0f
#define NUM_BOUNCES				2

#define AA_SWITCH				1
#define NUM_SAMPLES				16
#define LIGHT_MULTIPLIER		1.5f

#define LOAD_SHADERS_FROM_FILE

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
ID3D11RasterizerState* l_DefaultRasterizerState = NULL;

//api3dObject*	g_pObject = NULL;		// 3D icon/geometry info
//api3dObject*	g_pObject2 = NULL;		// 3D icon/geometry info
//api3dObject*	g_pObject3 = NULL;		// 3D icon/geometry info
//api3dObject*	g_pObject4 = NULL;		// 3D icon/geometry info
//api3dObject*	g_pObject5 = NULL;		// 3D icon/geometry info

bool g_bRTCreated = false;

//--------------------------------------------------------------------------------------
// Debugging
//--------------------------------------------------------------------------------------
ID3D11Buffer* pDebugBuf;
D3D11_MAPPED_SUBRESOURCE MappedResource; 
HRESULT hr;
maVector4d* ptr;
maVector4d o1;
maVector4d d1;
maVector4d o2;
maVector4d d2;


//--------------------------------------------------------------------------------------
// DX11 shaders, buffers, views
//--------------------------------------------------------------------------------------
ID3D11PixelShader*          g_p_DumpBufferPS = NULL;
ID3D11ComputeShader*        g_p_ComputeRaysCS = NULL;
ID3D11ComputeShader*        g_p_PostProcessCS = NULL;
ID3D11ComputeShader*        g_p_RenderPrimaryCS = NULL;
ID3D11ComputeShader*        g_p_RenderReflectCS = NULL;
ID3D11ComputeShader*        g_p_RenderRefractCS = NULL;

ID3D11Buffer*               g_p_CB = NULL;
ID3D11Buffer*               g_p_RT_World_CB = NULL;
ID3D11Buffer*               g_p_RT_Meshes_CB = NULL;

ID3D11Buffer*               g_p_Canvas = NULL;
ID3D11ShaderResourceView*   g_p_CanvasSRV = NULL;
ID3D11UnorderedAccessView*  g_p_CanvasUAV = NULL;

ID3D11Buffer*               g_p_ReflInputRays = NULL;
ID3D11ShaderResourceView*   g_p_ReflInputRaysSRV = NULL;
ID3D11UnorderedAccessView*  g_p_ReflInputRaysUAV = NULL;

ID3D11Buffer*               g_p_RefrInputRays = NULL;
ID3D11ShaderResourceView*   g_p_RefrInputRaysSRV = NULL;
ID3D11UnorderedAccessView*  g_p_RefrInputRaysUAV = NULL;

ID3D11Buffer*               g_p_OutputRays = NULL;
ID3D11ShaderResourceView*   g_p_OutputRaysSRV = NULL;
ID3D11UnorderedAccessView*  g_p_OutputRaysUAV = NULL;

ID3D11Buffer*				g_p_BVH = NULL;
ID3D11ShaderResourceView*	g_p_BVHSRV = NULL;

ID3D11ShaderResourceView*	g_VBSRVs[MAX_MESHES]; 
ID3D11ShaderResourceView*	g_IBSRVs[MAX_MESHES]; 
ID3D11ShaderResourceView*	g_BVHSRVs[MAX_MESHES]; 
ID3D11ShaderResourceView*	g_TexSRVs[MAX_MESHES]; 
ID3D11ShaderResourceView*	g_EnvTexSRVs[MAX_MESHES]; 

//--------------------------------------------------------------------------------------
// Structs!
//--------------------------------------------------------------------------------------
struct LightSource {
	maVector4d pos; // .W component = whether it casts a shadow or not
	maVector4d diffuseColor;
	maVector4d specularColor;
	maVector4d fallOff;
	maVector4d coneInfo;
};
struct CB_RT_Meshes
{
	maVector4d boxMin[MAX_MESHES];
	maVector4d boxMax[MAX_MESHES];
	maMatrix4x4 worldToObject[MAX_MESHES];
	maMatrix4x4 objectToWorld[MAX_MESHES];
	maFloatRGBA diffuse[MAX_MESHES];
	maFloatRGBA specular[MAX_MESHES];
	maFloatRGBA ambient[MAX_MESHES];	
	maVector4d shininess[MAX_MESHES]; // x = shininess, y = reflectivity, z = diffuse map, w = env map	
	maVector4d nTriangles[MAX_MESHES]; // x = ntriangles, y = fresnelBias, z = frenelPower,	w = IOR
	maVector4d textureRes[MAX_MESHES]; // xy is main texture, zw is env texture

	void Clear(int i)
	{
		boxMin[i] = maVector3d(0,0,0);
		boxMax[i] = maVector3d(0,0,0);
		worldToObject[i].Identity();
		objectToWorld[i].Identity();
		diffuse[i] = maFloatRGBA(1,1,1,1);
		specular[i] = maFloatRGBA(0,0,0,1);
		ambient[i] = maFloatRGBA(0,0,0,1);
		shininess[i] = maVector3d(0,0,0);
		nTriangles[i] = maVector4d(0,0,0,0);
		textureRes[i] = maVector4d(0,0,0,0);	
	}
};
struct CB_RT_World
{
    struct
    {
        maVector4d pos[MAX_LIGHTS]; // .W component = whether it casts a shadow or not
		maVector4d diffuseColor[MAX_LIGHTS];
		maVector4d specularColor[MAX_LIGHTS];
		maVector4d fallOff[MAX_LIGHTS];
		maVector4d coneInfo[MAX_LIGHTS];
    } LightSource;

    struct
    {
		maMatrix4x4 CameraToScreen[1];
		maMatrix4x4 WorldToScreen[1];
		maMatrix4x4 RasterToCamera[1];
		maMatrix4x4 ScreenToRaster[1];
		maMatrix4x4 RasterToScreen[1];
		maMatrix4x4 CameraToWorld[1];
		maMatrix4x4 RasterToWorld[1];

		maVector4d eye[1];
		maVector4d lookat[1];
		maVector4d up[1];
		maVector4d u[1];
		maVector4d v[1];
		maVector4d w[1];
    } Camera;

	maVector4d backColor[1];

	maVector4d rtParams_1[1];		// < K_EPSILON , K_LARGEVAL , BounceNum , enable shadows >
	maVector4d rtParams_2[1];	// < NUM_BOUNCES , JUST_SECONDARIES , KR , enable reflections >
	maVector4d rtParams_3[1];	// << refract_toggle , refraction attenuation, 0, 0 >>
	
    int resolution[2];

	int numLights[1];
	int numMeshes[1];

	//int padding_one[1];
	//int padding_two[1];
	//int padding_three[1];
};

//--------------------------------------------------------------------
// SWAP()
//--------------------------------------------------------------------
template <class T>
void SWAP( T* &x, T* &y )
{
    T* temp = x;
    x = y;
    y = temp;
}

//--------------------------------------------------------------------
// SetCamTransforms()
//--------------------------------------------------------------------
void SetCamTransforms(CB_RT_World& cbRTWorld, const camCamera* i_pCamera) {

	float Screen[4];
	// top, bottom, left, right
	i_pCamera->GetSubViewport(Screen[0], Screen[1], Screen[2], Screen[3]);

	//[-1..1]x[-1..1]
	maMatrix4x4 translate0;
	translate0.MakeTranslate(1,1,0);
	//[0..2]x[0..2]
	maMatrix4x4 scale0;
	scale0.MakeScale(0.5f, 0.5f, 1);
	//[0..1]x[0..1]
	maMatrix4x4 scale1;
	scale1.MakeScale(RES_X, RES_Y, 1);
	maMatrix4x4 ScreenToRaster = translate0 * scale0 * scale1;

	maMatrix4x4 RasterToScreen = ScreenToRaster;
	RasterToScreen.Invert();
	maMatrix4x4 CameraToScreen;
	i_pCamera->GetProjectionMatrix(CameraToScreen);
	maMatrix4x4 ScreenToCamera = CameraToScreen;
	ScreenToCamera.Invert();
	maMatrix4x4 WorldToCamera;
	i_pCamera->GetCameraMatrix(WorldToCamera);
	maMatrix4x4 WorldToScreen  = WorldToCamera * CameraToScreen;
	maMatrix4x4 RasterToCamera = RasterToScreen * ScreenToCamera;
	maMatrix4x4 CameraToWorld = WorldToCamera;
	CameraToWorld.Invert();
	maMatrix4x4 RasterToWorld = RasterToCamera * CameraToWorld;

	cbRTWorld.Camera.ScreenToRaster[0] = ScreenToRaster;
	cbRTWorld.Camera.RasterToScreen[0] = RasterToScreen;
	cbRTWorld.Camera.CameraToScreen[0] = CameraToScreen;
	cbRTWorld.Camera.WorldToScreen[0]  = WorldToScreen;
	cbRTWorld.Camera.RasterToCamera[0] = RasterToCamera;
	cbRTWorld.Camera.CameraToWorld[0] = CameraToWorld;
	cbRTWorld.Camera.RasterToWorld[0] = RasterToWorld;
}

//--------------------------------------------------------------------------------------------
// CreateAndCopyToDebugBuf() Debug function which copies a GPU buffer to a CPU readable buffer
//--------------------------------------------------------------------------------------------
ID3D11Buffer* CreateAndCopyToDebugBuf( ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer )
{
    ID3D11Buffer* debugbuf = NULL;

    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof(desc) );
    pBuffer->GetDesc( &desc );
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    pDevice->CreateBuffer(&desc, NULL, &debugbuf);

    pd3dImmediateContext->CopyResource( debugbuf, pBuffer );

    return debugbuf;
}
        // Turn on this and set a breakpoint right after the call to Map to see what's written to pBuffer
#if 0
        ID3D11Buffer* pDebugBuf = CreateAndCopyToDebugBuf( DXUTGetD3D11Device(), pd3dImmediateContext, pBuffer );
        D3D11_MAPPED_SUBRESOURCE MappedResource;     
        V( pd3dImmediateContext->Map( pDebugBuf, 0, D3D11_MAP_READ, 0, &MappedResource ) );
        pd3dImmediateContext->Unmap( pDebugBuf, 0 );
        SAFE_RELEASE( pDebugBuf );        
#endif


//--------------------------------------------------------------------
// GetLightInfo()
//--------------------------------------------------------------------
void GetLightInfo(const g3dLight* i_pLight, LightSource &o_Info)
{
	// dmt - why aren't these virtual functions? we don't really need these casts... do we?

	const g3dPointLight* point_light = dynamic_cast<const g3dPointLight*>(i_pLight);
	if (point_light)
	{
		maPoint3d pos = point_light->GetPosition();
		o_Info.pos.Set(pos.m_X, pos.m_Y, pos.m_Z, point_light->GetCastsShadow() ? 1.0f : 0.0f);

		o_Info.fallOff.Set(point_light->GetFalloff0(), point_light->GetFalloff1(), point_light->GetFalloff2());
	}
	const g3dDirectionalLight* dir_light = dynamic_cast<const g3dDirectionalLight*>(i_pLight);
	if (dir_light)
	{
		maVector3d dir = dir_light->GetDirection(); 
		// handy to reverse the direction
		o_Info.pos.Set(-dir.m_X, -dir.m_Y, -dir.m_Z, 0.0f);
	}
	const g3dProjectedLight* proj_light = dynamic_cast<const g3dProjectedLight*>(i_pLight);
	if (proj_light)
	{
		maPoint3d pos = proj_light->GetPosition();
		maVector3d dir = proj_light->GetDirection(); 

		o_Info.pos.Set(pos.m_X, pos.m_Y, pos.m_Z, 1.0f);
		o_Info.coneInfo.Set(-dir.m_X, -dir.m_Y, -dir.m_Z, 
			cosf(0.5f * proj_light->GetAngle() * maConstants::c_fAngleToRad));

		o_Info.fallOff.Set(proj_light->GetFalloff0(), proj_light->GetFalloff1(), proj_light->GetFalloff2());
	}

	maFloatRGBA light_color = i_pLight->GetIntensity();
//		if (g3dPrefs::CurrentPrefs().m_RendererType == g3dSceneRendererCreate::e_HDR)
	light_color *= i_pLight->GetIntensityFactor();
	light_color.SetAlpha(1.0f); // make sure light doesn't change alpha of color

	if (i_pLight->IsDiffuseEnabled() && g3dPrefs::CurrentPrefs().m_bEnableDiffuseLighting)
	{
		o_Info.diffuseColor = maVector4d(light_color.GetRed(), light_color.GetGreen(), light_color.GetBlue(), light_color.GetAlpha());
	}
	else 
	{
		o_Info.diffuseColor.Set(0,0,0,1);
	}

	// when baking, turn off specular lighting contribution.
	if (i_pLight->IsSpecularEnabled() 
		&& (g3dPrefs::CurrentPrefs().m_bEnableSpecularLighting || g3dSingleLightRendering::GetDoGlowPass())
		&& !g3dSingleLightRendering::GetDoBaking())
	{
		o_Info.specularColor = maVector4d(light_color.GetRed(), light_color.GetGreen(), light_color.GetBlue(), light_color.GetAlpha());
		o_Info.specularColor.SetW(0.0f);  // need to keep alpha
	}
	else 
	{
		o_Info.specularColor.Set(0,0,0,0);
	}
}

//--------------------------------------------------------------------
// RecurseGatherMeshes()
//--------------------------------------------------------------------
int RecurseGatherMeshes(g3dSceneNode* i_Node, CB_RT_Meshes& cbRTMeshes, CB_RT_World& cbRTWorld, int& io_NumMeshes,
						g3dAmbientEnvState* i_Env )
{
	if (!i_Node->GetRenderable())
		return 0;
	if (io_NumMeshes == MAX_MESHES)
		return 0;

	int npoly = 0;

	g3dFragment* frag = i_Node->GetFragment();
	if (frag)
	{
		bumpTriMeshBumpFrag* bFrag = dynamic_cast<bumpTriMeshBumpFrag*>(frag);
		if (bFrag)
		{
			g_VBSRVs[io_NumMeshes] = bFrag->m_pVBSRV; 
			g_IBSRVs[io_NumMeshes] = bFrag->m_pIBSRV; 
			g_BVHSRVs[io_NumMeshes] = bFrag->m_pBVHSRV; 

			cbRTMeshes.boxMax[io_NumMeshes] = maVector3d(i_Node->GetWorldBox().GetMaxX(), 
														 i_Node->GetWorldBox().GetMaxY(), 
														 i_Node->GetWorldBox().GetMaxZ()) + maVector3d(AABB_INCREASE,AABB_INCREASE,AABB_INCREASE);
			cbRTMeshes.boxMin[io_NumMeshes] = maVector3d(i_Node->GetWorldBox().GetMinX(), 
														 i_Node->GetWorldBox().GetMinY(), 
														 i_Node->GetWorldBox().GetMinZ()) - maVector3d(AABB_INCREASE,AABB_INCREASE,AABB_INCREASE);

			cbRTMeshes.nTriangles[io_NumMeshes].m_X = (float)(bFrag->GetNumIndices()/3);
			maMatrix4x4 w2o = i_Node->GetTotalTransform();
			cbRTMeshes.objectToWorld[io_NumMeshes] = w2o;
			w2o.Invert();			
			cbRTMeshes.worldToObject[io_NumMeshes] = w2o;

			// material
			matMaterial* pMaterial = i_Node->GetMaterial();
			if (pMaterial == NULL)
				pMaterial = i_Node->GetFragment()->GetMaterial();
			shared_ptr<effShaderParams> shaderParams = pMaterial->GetShaderParams();
			if (shaderParams)
			{

				effParamColor* cParam = shaderParams->FindColorParam("g_diffuse");
				if (cParam)
					cbRTMeshes.diffuse[io_NumMeshes] = cParam->GetProperty().GetValue();
				else
					cbRTMeshes.diffuse[io_NumMeshes] = maFloatRGBA(0.3f,0.3f,0.3f,1);

				cParam = shaderParams->FindColorParam("g_specular");
				if (cParam)
					cbRTMeshes.specular[io_NumMeshes] = cParam->GetProperty().GetValue();
				else
					cbRTMeshes.specular[io_NumMeshes] = maFloatRGBA(0.3f,0.3f,0.3f,1);

				effParamFloat* fParam = shaderParams->FindFloatParam("g_shininess");
				if (fParam)
					cbRTMeshes.shininess[io_NumMeshes].m_X = fParam->GetProperty().GetValue();
				else
					cbRTMeshes.shininess[io_NumMeshes].m_X = 25;

				fParam = shaderParams->FindFloatParam("reflFactor");
				if (fParam)
					cbRTMeshes.shininess[io_NumMeshes].m_Y = fParam->GetProperty().GetValue();
				else
					cbRTMeshes.shininess[io_NumMeshes].m_Y = 1.0f;

				fParam = shaderParams->FindFloatParam("fresnelBias");
				if (fParam)
					cbRTMeshes.nTriangles[io_NumMeshes].m_Y = fParam->GetProperty().GetValue();
				else
					cbRTMeshes.nTriangles[io_NumMeshes].m_Y = 1.0f;
				fParam = shaderParams->FindFloatParam("fresnelPower");
				if (fParam)
					cbRTMeshes.nTriangles[io_NumMeshes].m_Z = fParam->GetProperty().GetValue();
				else
					cbRTMeshes.nTriangles[io_NumMeshes].m_Z = 1.0f;

				fParam = shaderParams->FindFloatParam("IOR");
				if (fParam)
					cbRTMeshes.nTriangles[io_NumMeshes].m_W = fParam->GetProperty().GetValue();
				else
					cbRTMeshes.nTriangles[io_NumMeshes].m_W = 1.0f;

				effParamTexture* tParam = shaderParams->FindTextureParam("diffuseMap");
				if (tParam)
				{
					ID3D11ShaderResourceView* pTexSRV = g3dDX11TextureUtil::GetD3DTexture(tParam->GetTexture());
					g_TexSRVs[io_NumMeshes] = pTexSRV; 
					cbRTMeshes.shininess[io_NumMeshes].m_Z = (pTexSRV == NULL) ? 0.0f : 1.0f;
					if (tParam->GetTexture())
					{
						cbRTMeshes.textureRes[io_NumMeshes].SetX((float)tParam->GetTexture()->GetWidth());
						cbRTMeshes.textureRes[io_NumMeshes].SetY((float)tParam->GetTexture()->GetHeight());
					}
				}
				else
				{
					g_TexSRVs[io_NumMeshes] = NULL; 
					cbRTMeshes.shininess[io_NumMeshes].m_Z = 0.0f;
				}

				if (i_Env != NULL)
				{
					cbRTMeshes.ambient[io_NumMeshes] = i_Env->m_DiffuseColor * i_Env->m_DiffuseFactor;

					matTexture* pTex = i_Env->m_DiffuseMap;
					ID3D11ShaderResourceView* pTexSRV = g3dDX11TextureUtil::GetD3DTexture(pTex);
					g_EnvTexSRVs[io_NumMeshes] = pTexSRV; 
					cbRTMeshes.shininess[io_NumMeshes].m_W = (pTexSRV == NULL) ? 0.0f : 1.0f;
					if (pTexSRV != NULL)
					{
						cbRTMeshes.textureRes[io_NumMeshes].SetZ((float)pTex->GetWidth());
						cbRTMeshes.textureRes[io_NumMeshes].SetW((float)pTex->GetHeight());
					}
				}
				else
				{
					cbRTMeshes.shininess[io_NumMeshes].m_W = 0.0f;
					cbRTMeshes.ambient[io_NumMeshes] = maFloatRGBA(1,1,1,1);
				}

			}
			else
			{
				const effPhongData* pData = pMaterial->GetTypedData<effPhongData>();
				cbRTMeshes.ambient[io_NumMeshes] = pData->GetEmissive() + pData->GetAmbient();
				//cbRTMeshes.diffuse[io_NumMeshes] = pData->GetDiffuse();
				cbRTMeshes.diffuse[io_NumMeshes] = cbRTMeshes.ambient[io_NumMeshes];
				cbRTMeshes.shininess[io_NumMeshes].m_X = pData->m_SpecularPower;
				cbRTMeshes.shininess[io_NumMeshes].m_Y = pData->m_Reflectivity;
				cbRTMeshes.specular[io_NumMeshes] = pData->GetSpecular();

				g_TexSRVs[io_NumMeshes] = NULL; 
				cbRTMeshes.shininess[io_NumMeshes].m_Z = 0.0f;
				g_EnvTexSRVs[io_NumMeshes] = NULL; 
				cbRTMeshes.shininess[io_NumMeshes].m_W = 0.0f;
			}

			io_NumMeshes++;
			npoly = 1;
		}
	}

	for (int i = 0; i < i_Node->GetNumChildren(); i++)
	{
		npoly += RecurseGatherMeshes(i_Node->GetChild(i), cbRTMeshes, cbRTWorld, io_NumMeshes,
			i_Node->GetChild(i)->GetEnvironment() ? i_Node->GetChild(i)->GetEnvironment() : i_Env );
	}
	return npoly;
}

//--------------------------------------------------------------------
// GatherMeshes()
//--------------------------------------------------------------------
int GatherMeshes(CB_RT_Meshes& cbRTMeshes, CB_RT_World& cbRTWorld, const g3dScene* i_pScene)
{
	int nMeshes = 0;

	RecurseGatherMeshes(i_pScene->GetLayer(0)->GetRootNode(), cbRTMeshes, cbRTWorld, nMeshes, 
		i_pScene->GetLayer(0)->GetRootNode()->GetEnvironment() );

	static int polycount = 0;
	int newpolycount = 0;
	for (int  i = 0; i < nMeshes; i++)
	{
		newpolycount += (int)cbRTMeshes.nTriangles[i].m_X;
	}
	if (newpolycount != polycount)
	{
		polycount = newpolycount;
		DBG_LOG("Poly count: " << polycount);
	}

	return nMeshes;
}

//--------------------------------------------------------------------
// RunComputeShader()
//--------------------------------------------------------------------
void RunComputeShader( ID3D11DeviceContext* pd3dImmediateContext,
                       ID3D11ComputeShader* pComputeShader,
                       UINT nNumSRVs, ID3D11ShaderResourceView** pShaderResourceViews, 
                       UINT nNumCBuffers, ID3D11Buffer** pCBCS, void** pCSData, DWORD* dwNumDataBytes,
                       UINT nNumUAVs, ID3D11UnorderedAccessView** pUnorderedAccessViews,
                       UINT X, UINT Y, UINT Z )
{ 
    HRESULT hr = S_OK;
    
    pd3dImmediateContext->CSSetShader( pComputeShader, NULL, 0 );
    pd3dImmediateContext->CSSetShaderResources( 0, nNumSRVs, pShaderResourceViews );
    pd3dImmediateContext->CSSetUnorderedAccessViews( 0, nNumUAVs, pUnorderedAccessViews, (UINT*)pUnorderedAccessViews );
	for (int i = 0; i < nNumCBuffers; i++)
	{
		if ( pCBCS[i] )
		{
			D3D11_MAPPED_SUBRESOURCE MappedResource;
			( pd3dImmediateContext->Map( pCBCS[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
			memcpy( MappedResource.pData, pCSData[i], dwNumDataBytes[i] );
			pd3dImmediateContext->Unmap( pCBCS[i], 0 );
		}
	}
	pd3dImmediateContext->CSSetConstantBuffers( 0, nNumCBuffers, pCBCS );

    pd3dImmediateContext->Dispatch( X, Y, Z );

    ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
    pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, ppUAViewNULL, (UINT*)(&ppUAViewNULL) );

	static ID3D11ShaderResourceView* ppSRVNULL[128] = {
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	};
	DBG_ASSERT(nNumSRVs < 128, "too many views");
    pd3dImmediateContext->CSSetShaderResources( 0, nNumSRVs, ppSRVNULL );
	static ID3D11Buffer* ppBufferNULL[128] = {
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	};
	DBG_ASSERT(nNumCBuffers < 128, "too many cbuffers");
    pd3dImmediateContext->CSSetConstantBuffers( 0, nNumCBuffers, ppBufferNULL );
}

void SetupBuffersAndDispatchSinglepass(ID3D11DeviceContext* g_pd3dContext, CB_RT_World cbRTWorld, CB_RT_Meshes cbRTMeshes,
						     int NumThreadGroups_X, int NumThreadGroups_Y, int NumThreadGroups_Z ) 
{
	// compute the camera rays into the g_p_GPU_Rays buffer.
	ID3D11Buffer* cbs[1] = {g_p_RT_World_CB};//g_p_Camera_CB};
	void* cbdata[1] = {&cbRTWorld};//cbCamera};
	DWORD cbsize[1] = {sizeof(cbRTWorld)};//cbCamera)};
	ID3D11UnorderedAccessView* aUAViews[2];
	aUAViews[0] = g_p_ReflInputRaysUAV;
	aUAViews[1] = g_p_CanvasUAV;

    RunComputeShader( g_pd3dContext, 
                      g_p_ComputeRaysCS,
                      0, NULL,
                      1, cbs, cbdata, cbsize,
                      2,aUAViews,
                      NumThreadGroups_X, NumThreadGroups_Y, NumThreadGroups_Z );

	// send the world data to the ray tracer.
	// 1(rays) + vtx buffers + index buffers + bvh's + textures + envtextures
	const int NUM_SINGLE_SRVs = 1;
	const int NUM_SRVS = NUM_SINGLE_SRVs + MAX_MESHES + MAX_MESHES + MAX_MESHES + MAX_MESHES + MAX_MESHES;
	DBG_ASSERT(NUM_SRVS <= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, "too many resources for shader");
    ID3D11ShaderResourceView* aRViews[ NUM_SRVS ];
	for (int i = 0; i < MAX_MESHES; i++)
	{
		aRViews[i] = g_VBSRVs[i];
		aRViews[i + MAX_MESHES] = g_IBSRVs[i];
		aRViews[i + MAX_MESHES+MAX_MESHES] = g_BVHSRVs[i];
		aRViews[i + MAX_MESHES+MAX_MESHES+MAX_MESHES] = g_TexSRVs[i];
		aRViews[i + MAX_MESHES+MAX_MESHES+MAX_MESHES+MAX_MESHES] = g_EnvTexSRVs[i];
	}
	aRViews[NUM_SRVS-1] = g_p_ReflInputRaysSRV;

	ID3D11Buffer* rtcbs[2] = {g_p_RT_Meshes_CB, g_p_RT_World_CB};
	void* rtcbdata[2] = {&cbRTMeshes, &cbRTWorld};
	DWORD rtcbsize[2] = {sizeof(cbRTMeshes), sizeof(cbRTWorld)};

	ID3D11UnorderedAccessView* aUAViews_2[2];
	aUAViews_2[0] = g_p_CanvasUAV;
	aUAViews_2[1] = g_p_OutputRaysUAV;
	RunComputeShader( g_pd3dContext, 
                      g_p_RenderPrimaryCS,
                      NUM_SRVS, aRViews,
                      2, rtcbs, rtcbdata, rtcbsize,
                      2,aUAViews_2,
                      NumThreadGroups_X, NumThreadGroups_Y, NumThreadGroups_Z );
}

void DispatchPrimaryGen(ID3D11DeviceContext* g_pd3dContext, CB_RT_World cbRTWorld, CB_RT_Meshes cbRTMeshes,
						ID3D11UnorderedAccessView* outputRays, ID3D11UnorderedAccessView* canvas,						
						int NumThreadGroups_X, int NumThreadGroups_Y, int NumThreadGroups_Z) 
{
	// CB's
	const int NUM_CBs_Gen = 1;
	ID3D11Buffer* cbs_Gen[NUM_CBs_Gen] = {g_p_RT_World_CB};
	void* cbdata_Gen[NUM_CBs_Gen] = {&cbRTWorld};
	DWORD cbsize_Gen[NUM_CBs_Gen] = {sizeof(cbRTWorld)};

	// UAV's
	const int NUM_UAVs_Gen = 2;
	ID3D11UnorderedAccessView* aUAViews_Gen[NUM_UAVs_Gen];
	aUAViews_Gen[0] = outputRays;
	aUAViews_Gen[1] = canvas;
	
	// Generate primary rays (GenRays.hlsl)
    RunComputeShader( g_pd3dContext, 
                      g_p_ComputeRaysCS,			// Compute Shader
                      0, NULL,						// SRV's
                      NUM_CBs_Gen, cbs_Gen, cbdata_Gen, cbsize_Gen,	// CB's
                      NUM_UAVs_Gen, aUAViews_Gen,			// UAV's
                      NumThreadGroups_X, NumThreadGroups_Y, NumThreadGroups_Z );
}

void DispatchRay(ID3D11DeviceContext* g_pd3dContext, CB_RT_World cbRTWorld, CB_RT_Meshes cbRTMeshes,
				 ID3D11ShaderResourceView* inputRays, ID3D11UnorderedAccessView* outputRays,
				 ID3D11UnorderedAccessView* canvas, ID3D11ComputeShader* computeShader, 
				 int bounceNum, int NumThreadGroups_X, int NumThreadGroups_Y, int NumThreadGroups_Z) 
{

	// SRV's
	// send the world data to the ray tracer
	// 1(rays) + vtx buffers + index buffers + bvh's + textures + envtextures
	const int NUM_SINGLE_SRVs = 1;
	const int NUM_SRVs = NUM_SINGLE_SRVs + MAX_MESHES + MAX_MESHES + MAX_MESHES + MAX_MESHES + MAX_MESHES;
	DBG_ASSERT(NUM_SINGLE_SRVs <= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, "too many resources for shader");
	ID3D11ShaderResourceView* aSRViews[ NUM_SRVs ];
	for (int i = 0; i < MAX_MESHES; i++)
	{
		aSRViews[i] = g_VBSRVs[i];
		aSRViews[i + MAX_MESHES] = g_IBSRVs[i];
		aSRViews[i + MAX_MESHES+MAX_MESHES] = g_BVHSRVs[i];
		aSRViews[i + MAX_MESHES+MAX_MESHES+MAX_MESHES] = g_TexSRVs[i];
		aSRViews[i + MAX_MESHES+MAX_MESHES+MAX_MESHES+MAX_MESHES] = g_EnvTexSRVs[i];
	}
	aSRViews[NUM_SRVs-1] = inputRays;

	// CB's
	const int NUM_CBs = 2;
	cbRTWorld.rtParams_1[0].m_Z = (float)bounceNum;
	ID3D11Buffer* cbs[NUM_CBs] = {g_p_RT_Meshes_CB, g_p_RT_World_CB};
	void* cbdata[NUM_CBs] = {&cbRTMeshes, &cbRTWorld};
	DWORD cbsize[NUM_CBs] = {sizeof(cbRTMeshes), sizeof(cbRTWorld)};

	// UAV's
	const int NUM_UAVs = 2;
	ID3D11UnorderedAccessView* aUAViews[NUM_UAVs];
	aUAViews[0] = canvas;
	aUAViews[1] = outputRays;

	// Ray trace (RayTraceCS.hlsl)
	RunComputeShader( g_pd3dContext, 
					  computeShader,					// Compute Shader
					  NUM_SRVs, aSRViews,			// SRV's
					  NUM_CBs, cbs, cbdata, cbsize,	// CB's
					  NUM_UAVs, aUAViews,			// UAV's
					  NumThreadGroups_X, NumThreadGroups_Y, NumThreadGroups_Z );
}

//--------------------------------------------------------------------
// DoMultiPass()
//--------------------------------------------------------------------
void DoMultiPass(ID3D11DeviceContext* g_pd3dContext, CB_RT_World cbRTWorld, CB_RT_Meshes cbRTMeshes,
				 int NumThreadGroups_X, int NumThreadGroups_Y, int NumThreadGroups_Z ) 
{
	// Generate primary directions
	DispatchPrimaryGen( g_pd3dContext, cbRTWorld, cbRTMeshes,
						g_p_ReflInputRaysUAV, g_p_CanvasUAV, 
						NumThreadGroups_X, NumThreadGroups_Y, NumThreadGroups_Z);
	
	// Primary rays
	int bounceNum = 0;
	DispatchRay( g_pd3dContext, cbRTWorld, cbRTMeshes, 	            
				 g_p_ReflInputRaysSRV, g_p_OutputRaysUAV, g_p_CanvasUAV, g_p_RenderPrimaryCS,
				 bounceNum, NumThreadGroups_X, NumThreadGroups_Y, NumThreadGroups_Z);

	// Copy primary directions to refraction & reflection buffers
	g_pd3dContext->CopyResource(g_p_RefrInputRays, g_p_OutputRays);
	g_pd3dContext->CopyResource(g_p_ReflInputRays, g_p_OutputRays);
	
	// Reflection rays start!
	if ( g3dPrefs::CurrentPrefs().m_bRTReflection ) 
	{
		for ( bounceNum = 1 ; bounceNum <= g3dPrefs::CurrentPrefs().m_RTNumReflBounces ; bounceNum++ )
		{
			DispatchRay( g_pd3dContext, cbRTWorld, cbRTMeshes, 
						 g_p_ReflInputRaysSRV, g_p_OutputRaysUAV, g_p_CanvasUAV, g_p_RenderReflectCS,			 
						 bounceNum, NumThreadGroups_X, NumThreadGroups_Y, NumThreadGroups_Z);

			g_pd3dContext->CopyResource(g_p_ReflInputRays, g_p_OutputRays) ;

		} 
	}

	// Refractive rays start!	
	if ( g3dPrefs::CurrentPrefs().m_bRTRefraction )
	{
		for ( bounceNum = 1 ; bounceNum <= g3dPrefs::CurrentPrefs().m_RTNumRefrBounces ; bounceNum++ )
		{
			DispatchRay( g_pd3dContext, cbRTWorld, cbRTMeshes, 
						 g_p_RefrInputRaysSRV, g_p_OutputRaysUAV, g_p_CanvasUAV, g_p_RenderRefractCS,					 
						 bounceNum, NumThreadGroups_X, NumThreadGroups_Y, NumThreadGroups_Z);

			g_pd3dContext->CopyResource(g_p_RefrInputRays, g_p_OutputRays) ;

		} 
	}	
		
}

//--------------------------------------------------------------------
// RayTraceCS11()
//--------------------------------------------------------------------
void RayTraceCS11(ID3D11DeviceContext* g_pd3dContext, const camCamera* i_pCamera,
			   const g3dScene* i_pScene, int W, int H, 
			   int NumThreadGroups_X, int NumThreadGroups_Y, int NumThreadGroups_Z ) 
{
	// Utility colors in here
	maVector4d black	= maVector4d(0,0,0,1);
	maVector4d red		= maVector4d(1,0,0,1);
	maVector4d green	= maVector4d(0,1,0,1);
	maVector4d blue		= maVector4d(0,0,1,1);
	maVector4d white	= maVector4d(1,1,1,1);
	maVector4d yellow	= maVector4d(1,1,0,1);
	maVector4d aqua		= maVector4d(0,1,1,1);
	maVector4d pink		= maVector4d(1,0,1,1);
	maVector4d purple	= maVector4d(0.5f,0,0.5f,1);
	maVector4d gray		= maVector4d(0.2f,0.2f,0.2f,1);
    
	// Send everything to GPU
    CB_RT_World cbRTWorld;

    cbRTWorld.resolution[0] = W;
    cbRTWorld.resolution[1] = H;

	// Setup some lights:
	// get all the enabled lights
	static std::vector<g3dLight*> enabledLights;
	enabledLights.clear();
	const std::vector<g3dLight*>& sceneLights = g3dLightMgrDX11::Implementation()->GetLights( );
	int i;
	for ( i = 0; i < sceneLights.size(); i++)
	{
		g3dLight* pLight = sceneLights[i];
		if ( pLight->IsEnabled() ) 
			enabledLights.push_back(pLight);
	}

	int nLights = min(MAX_LIGHTS, enabledLights.size());
	cbRTWorld.numLights[0] = nLights;

	// copy light data into world buffer.
	LightSource curLight;
	for ( i = 0 ; i < nLights; i++ ) 
	{
		g3dLight* light = enabledLights[i];
		GetLightInfo(light, curLight);

		cbRTWorld.LightSource.pos[i] = curLight.pos;
		cbRTWorld.LightSource.diffuseColor[i] = curLight.diffuseColor;	
		cbRTWorld.LightSource.specularColor[i] = curLight.specularColor;			
		cbRTWorld.LightSource.fallOff[i] = curLight.fallOff;	
		cbRTWorld.LightSource.coneInfo[i] = curLight.coneInfo;	
	}

	// gather world meshes
	CB_RT_Meshes cbRTMeshes;
	for (int i = 0; i < MAX_MESHES; i++)
	{
		g_VBSRVs[i] = NULL;
		g_IBSRVs[i] = NULL;
		g_BVHSRVs[i] = NULL;
		g_TexSRVs[i] = NULL;
		g_EnvTexSRVs[i] = NULL;
		cbRTMeshes.Clear(i);
	}
	int nMeshes = GatherMeshes(cbRTMeshes, cbRTWorld, i_pScene);
	cbRTWorld.numMeshes[0] = nMeshes;

	// extract cam matrix vectors.
	maMatrix4x4 camMat;
	i_pCamera->GetCameraMatrix(camMat);
	maVector4d u,v,w;
	u = maVector4d(camMat(0,0), camMat(1,0), camMat(2,0), 0);
	v = maVector4d(camMat(0,1), camMat(1,1), camMat(2,1), 0);
	w = maVector4d(camMat(0,2), camMat(1,2), camMat(2,2), 0);

	SetCamTransforms(cbRTWorld, i_pCamera);

	cbRTWorld.Camera.eye[0] = i_pCamera->GetPosition();
	cbRTWorld.Camera.lookat[0] = i_pCamera->GetTarget();
	cbRTWorld.Camera.up[0] = i_pCamera->GetUp();
	cbRTWorld.Camera.u[0] = u;
	cbRTWorld.Camera.v[0] = v;
	cbRTWorld.Camera.w[0] = w;

	cbRTWorld.rtParams_1[0] = maVector4d(K_EPSILON,K_LARGEVAL,0,g3dPrefs::CurrentPrefs().m_bEnableShadows ? 1.0f : 0.0f);
	cbRTWorld.rtParams_2[0] = maVector4d((float) g3dPrefs::CurrentPrefs().m_RTNumReflBounces,
									   g3dPrefs::CurrentPrefs().m_bRTOnlySecondary ? 1.0f : 0.0f,
									   g3dPrefs::CurrentPrefs().m_RTReflFactor,
									   g3dPrefs::CurrentPrefs().m_bRTReflection ? 1.0f : 0.0f);
	
	cbRTWorld.rtParams_3[0].m_X = g3dPrefs::CurrentPrefs().m_bRTRefraction ? 1.0f : 0.0f;
	cbRTWorld.rtParams_3[0].m_Y = g3dPrefs::CurrentPrefs().m_RTRefrFactor;

	//cbRTWorld.backColor[0] = gray;
	cbRTWorld.backColor[0] = black;

	DoMultiPass(g_pd3dContext, cbRTWorld, cbRTMeshes, NumThreadGroups_X, NumThreadGroups_Y, NumThreadGroups_Z );

}

//--------------------------------------------------------------------
// CreateFullscreenQuad()
//--------------------------------------------------------------------
void CreateFullscreenQuad(ID3D11Device* pd3dDevice)
{
	HRESULT hr;

#ifdef LOAD_SHADERS_FROM_FILE
	void* pBytes = NULL;
	SIZE_T numBytes = 0;
	hr = effShaderUtilWin::LoadShaderFromFile( L"DumpToTexture.o", &numBytes, &pBytes );
	hr = ( pd3dDevice->CreatePixelShader( pBytes, numBytes, NULL, &g_p_DumpBufferPS ) );
	delete [] pBytes;
#else 
	ID3DBlob* pBlob = NULL;
	hr = ( effShaderUtilWin::CompileShaderFromFile( L"C:\\Projects\\SourceCode - SIGGRAPH09\\LibXLT\\GraphicsDX11\\Eff\\private\\Shaders\\DumpToTexture.hlsl", "PSDump", "ps_5_0", &pBlob ) );
    hr = ( pd3dDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_p_DumpBufferPS ) );
    pBlob->Release( );
#endif

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
	rsDesc.FrontCounterClockwise = FALSE;//TRUE;
	/////////
	rsDesc.DepthClipEnable = TRUE;
    g2dDX11Global::g_pDevice->CreateRasterizerState(&rsDesc, &l_DefaultRasterizerState);

}

//--------------------------------------------------------------------------------------
// DumpToTexture() - Convert buffer result output from CS to a texture, used in CS path
//--------------------------------------------------------------------------------------
HRESULT DumpToTexture( ID3D11DeviceContext* pd3dImmediateContext, 
					  DWORD dwSrcWidth, DWORD dwSrcHeight, ID3D11ShaderResourceView* pFromRV, 
					  DWORD dwDstWidth, DWORD dwDstHeight, ID3D11RenderTargetView* pToRTV )
{
    HRESULT hr = S_OK;
    
    ID3D11ShaderResourceView* aSRViews[ 1 ] = { pFromRV };
    pd3dImmediateContext->PSSetShaderResources( 0, 1, aSRViews );

    ID3D11RenderTargetView* aRTViews[ 1 ] = { pToRTV };
    pd3dImmediateContext->OMSetRenderTargets( 1, aRTViews, NULL );          

    D3D11_MAPPED_SUBRESOURCE MappedResource;            
	hr = ( pd3dImmediateContext->Map( g_p_CB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
    UINT* p = (UINT*)MappedResource.pData;
    p[0] = dwSrcWidth;
    p[1] = dwSrcHeight;
    pd3dImmediateContext->Unmap( g_p_CB, 0 );
    ID3D11Buffer* ppCB[1] = { g_p_CB };
    pd3dImmediateContext->PSSetConstantBuffers( 0, 1, ppCB );

//	g2dFullscreenQuad::DrawFullScreenQuad11( g_p_DumpBufferPS, dwSrcWidth, dwSrcHeight );
	g2dFullscreenQuad::DrawFullScreenQuad11( dwSrcWidth, dwSrcHeight );

    return hr;
}

//--------------------------------------------------------------------
// CreateCanvasViews()
//--------------------------------------------------------------------
void CreateCanvasViews(ID3D11Device* pd3dDevice)
{
	HRESULT hr;
    D3D11_BUFFER_DESC DescBuffer;
    ZeroMemory( &DescBuffer, sizeof(DescBuffer) );
    DescBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    DescBuffer.ByteWidth = sizeof(maVector4d) * RES_X * RES_Y;
    DescBuffer.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    DescBuffer.StructureByteStride = sizeof(maVector4d);
    DescBuffer.Usage = D3D11_USAGE_DEFAULT;
    pd3dDevice->CreateBuffer( &DescBuffer, NULL, &g_p_Canvas );

    D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
    ZeroMemory( &DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC) );
    DescUAV.Format = DXGI_FORMAT_UNKNOWN;
    DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    DescUAV.Buffer.FirstElement = 0;
    DescUAV.Buffer.NumElements = DescBuffer.ByteWidth / DescBuffer.StructureByteStride;
	hr = pd3dDevice->CreateUnorderedAccessView( g_p_Canvas, &DescUAV, &g_p_CanvasUAV );

    D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV;
    ZeroMemory( &DescSRV, sizeof( DescSRV ) );
    DescSRV.Format = DXGI_FORMAT_UNKNOWN;
    DescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    DescSRV.Buffer.FirstElement = DescUAV.Buffer.FirstElement;
    DescSRV.Buffer.NumElements = DescUAV.Buffer.NumElements;
    hr = pd3dDevice->CreateShaderResourceView( g_p_Canvas, &DescSRV, &g_p_CanvasSRV ) ;
}

//--------------------------------------------------------------------
// CreateInputRayViews()
//--------------------------------------------------------------------
void CreateRayViews(ID3D11Device* pd3dDevice)
{
	HRESULT hr;
    D3D11_BUFFER_DESC DescBuffer;
    ZeroMemory( &DescBuffer, sizeof(DescBuffer) );
    DescBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    DescBuffer.ByteWidth = RAY_BUFFER_SIZE * sizeof(maVector4d);
    DescBuffer.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    DescBuffer.StructureByteStride = sizeof(maVector4d);
    DescBuffer.Usage = D3D11_USAGE_DEFAULT;
    pd3dDevice->CreateBuffer( &DescBuffer, NULL, &g_p_ReflInputRays );
	pd3dDevice->CreateBuffer( &DescBuffer, NULL, &g_p_RefrInputRays );
	pd3dDevice->CreateBuffer( &DescBuffer, NULL, &g_p_OutputRays );

    D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
    ZeroMemory( &DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC) );
    DescUAV.Format = DXGI_FORMAT_UNKNOWN;
    DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    DescUAV.Buffer.FirstElement = 0;
    DescUAV.Buffer.NumElements = DescBuffer.ByteWidth / DescBuffer.StructureByteStride;
	hr = pd3dDevice->CreateUnorderedAccessView( g_p_ReflInputRays , &DescUAV, &g_p_ReflInputRaysUAV );
	hr = pd3dDevice->CreateUnorderedAccessView( g_p_RefrInputRays , &DescUAV, &g_p_RefrInputRaysUAV );
	hr = pd3dDevice->CreateUnorderedAccessView( g_p_OutputRays, &DescUAV, &g_p_OutputRaysUAV );

    D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV;
    ZeroMemory( &DescSRV, sizeof( DescSRV ) );
    DescSRV.Format = DXGI_FORMAT_UNKNOWN;
    DescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    DescSRV.BufferEx.FirstElement = DescUAV.Buffer.FirstElement;
    DescSRV.BufferEx.NumElements = DescUAV.Buffer.NumElements;
	DescSRV.BufferEx.Flags = 0;
    hr = pd3dDevice->CreateShaderResourceView( g_p_ReflInputRays , &DescSRV, &g_p_ReflInputRaysSRV );
	hr = pd3dDevice->CreateShaderResourceView( g_p_RefrInputRays , &DescSRV, &g_p_RefrInputRaysSRV );
	hr = pd3dDevice->CreateShaderResourceView( g_p_OutputRays, &DescSRV, &g_p_OutputRaysSRV );

}

//--------------------------------------------------------------------
// CreateConstantBuffers()
//--------------------------------------------------------------------
HRESULT CreateConstantBuffers( ID3D11Device* pd3dDevice )
{
    HRESULT hr;

    // Setup constant buffers
    D3D11_BUFFER_DESC cbDesc;
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;    

	cbDesc.ByteWidth = sizeof( CB_RT_World );
    hr = ( pd3dDevice->CreateBuffer( &cbDesc, NULL, &g_p_RT_World_CB ) );

    cbDesc.ByteWidth = sizeof( CB_RT_Meshes );
    hr = ( pd3dDevice->CreateBuffer( &cbDesc, NULL, &g_p_RT_Meshes_CB ) );

    return S_OK;
}

//--------------------------------------------------------------------
// DestroyFullscreenQuad()
//--------------------------------------------------------------------
void DestroyFullscreenQuad()
{
	l_DefaultRasterizerState->Release();
	l_DefaultRasterizerState=NULL;
	g_p_DumpBufferPS->Release();
}

//--------------------------------------------------------------------
// DestroyConstantBuffers()
//--------------------------------------------------------------------
void DestroyConstantBuffers()
{
	g_p_RT_Meshes_CB->Release();
	g_p_RT_World_CB->Release();
}

//--------------------------------------------------------------------
// DestroyComputeShaders()
//--------------------------------------------------------------------
void DestroyComputeShaders()
{
	g_p_PostProcessCS->Release();
	g_p_ComputeRaysCS->Release();
	g_p_RenderPrimaryCS->Release();
	g_p_RenderReflectCS->Release();
	//g_p_RenderRefractCS->Release();
}

//--------------------------------------------------------------------
// DestroyViews()
//--------------------------------------------------------------------
void DestroyRayViews()
{
	g_p_CanvasSRV->Release();
	g_p_CanvasUAV->Release();
	g_p_Canvas->Release();

	g_p_ReflInputRaysSRV->Release();
	g_p_ReflInputRaysUAV->Release();
	g_p_ReflInputRays->Release();

	g_p_RefrInputRaysSRV->Release();
	g_p_RefrInputRaysUAV->Release();
	g_p_RefrInputRays->Release();

	g_p_OutputRaysSRV->Release();
	g_p_OutputRaysUAV->Release();
	g_p_OutputRays->Release();
}

//--------------------------------------------------------------------
// RTCleanUp()
//--------------------------------------------------------------------
void RTCleanUp()
{
	if (!g_bRTCreated)
		return;
	g_bRTCreated = false;

	/*
	if (g_pObject)
	{
		api3dScene::RemoveObject(g_pObject);
		delete g_pObject;
		g_pObject = NULL;

		api3dScene::RemoveObject(g_pObject2);
		delete g_pObject2;
		g_pObject2 = NULL;

		api3dScene::RemoveObject(g_pObject3);
		delete g_pObject3;
		g_pObject3 = NULL;

		api3dScene::RemoveObject(g_pObject4);
		delete g_pObject4;
		g_pObject4 = NULL;

		api3dScene::RemoveObject(g_pObject5);
		delete g_pObject5;
		g_pObject5 = NULL;
	}*/

	DestroyFullscreenQuad();
	DestroyConstantBuffers();
	DestroyComputeShaders();
	DestroyRayViews();
}

//--------------------------------------------------------------------
// CreateComputeShaders()
//--------------------------------------------------------------------
void CreateComputeShaders()
{
	HRESULT hr;

#ifdef LOAD_SHADERS_FROM_FILE
	void* pBytes = NULL;
	SIZE_T numBytes = 0;
	hr = effShaderUtilWin::LoadShaderFromFile( L"RenderPrimaryCS.o", &numBytes, &pBytes );
	hr = g2dDX11Global::g_pDevice->CreateComputeShader( pBytes, numBytes, NULL, &g_p_RenderPrimaryCS );
	delete [] pBytes;
	void* pBytes2 = NULL;
	SIZE_T numBytes2 = 0;
	hr = effShaderUtilWin::LoadShaderFromFile( L"GenRays.o", &numBytes2, &pBytes2 );
	hr = g2dDX11Global::g_pDevice->CreateComputeShader( pBytes2, numBytes2, NULL, &g_p_ComputeRaysCS );
	delete [] pBytes2;
	void* pBytes3 = NULL;
	SIZE_T numBytes3 = 0;
	hr = effShaderUtilWin::LoadShaderFromFile( L"PostProcess.o", &numBytes3, &pBytes3 );
	hr = g2dDX11Global::g_pDevice->CreateComputeShader( pBytes3, numBytes3, NULL, &g_p_PostProcessCS );
	delete [] pBytes3;

	void* pBytes4 = NULL;
	SIZE_T numBytes4 = 0;
	hr = effShaderUtilWin::LoadShaderFromFile( L"RenderReflectCS.o", &numBytes4, &pBytes4 );
	hr = g2dDX11Global::g_pDevice->CreateComputeShader( pBytes4, numBytes4, NULL, &g_p_RenderReflectCS );
	delete [] pBytes4;
	
	void* pBytes5 = NULL;
	SIZE_T numBytes5 = 0;
	hr = effShaderUtilWin::LoadShaderFromFile( L"RenderRefractCS.o", &numBytes5, &pBytes5 );
	hr = g2dDX11Global::g_pDevice->CreateComputeShader( pBytes5, numBytes5, NULL, &g_p_RenderRefractCS );
	delete [] pBytes5;
	

#else
    ID3DBlob* pBlob = NULL;
	hr = effShaderUtilWin::CompileShaderFromFile( L"C:\\Projects\\SourceCode - SIGGRAPH09\\LibXLT\\GraphicsDX11\\Eff\\private\\Shaders\\RayTraceCS.hlsl", "Render", "cs_5_0", &pBlob );
	hr = g2dDX11Global::g_pDevice->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_p_RenderPrimaryCS );
	pBlob->Release();
	hr = effShaderUtilWin::CompileShaderFromFile( L"C:\\Projects\\SourceCode - SIGGRAPH09\\LibXLT\\GraphicsDX11\\Eff\\private\\Shaders\\GenRays.hlsl", "ComputeRays", "cs_5_0", &pBlob );
	hr = g2dDX11Global::g_pDevice->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &g_p_ComputeRaysCS );
	pBlob->Release();
#endif

}

//--------------------------------------------------------------------
// RTInit()
//--------------------------------------------------------------------
void RTInit()
{
	if (g_bRTCreated)
		return;
	g_bRTCreated = true;

	CreateCanvasViews(g2dDX11Global::g_pDevice);
	CreateRayViews(g2dDX11Global::g_pDevice);
	CreateComputeShaders();
	CreateConstantBuffers(g2dDX11Global::g_pDevice);
	CreateFullscreenQuad(g2dDX11Global::g_pDevice);

	/*
	if (g_pObject == NULL)
	{
		g_pObject = api3dShape::CreateRectangle(maFloatRGBA(1,0,0,1), 100.0f, 100.0f, 2, 2);
		g_pObject = api3dShape::CreateSphere(maFloatRGBA(1,0,0,1), 1.0f, 8, 8);
		g_pObject->SetPosition(maPoint3d(0,0,0));
		api3dScene::AddObject(g_pObject);

		g_pObject2 = api3dShape::CreateSphere(maFloatRGBA(0,1,0,1), 200.0f, 16, 16);
		g_pObject2->SetPosition(maPoint3d(-750,-100,500));
		api3dScene::AddObject(g_pObject2);

		g_pObject3 = api3dShape::CreateSphere(maFloatRGBA(0.5,0,0.5,1), 200.0f, 16, 16);
		g_pObject3->SetPosition(maPoint3d(750,100,500));
		api3dScene::AddObject(g_pObject3);

		g_pObject4 = api3dShape::CreateSphere(maFloatRGBA(1,1,0,1), 175.0f, 16, 16);
		g_pObject4->SetPosition(maPoint3d(-300,400,100));
		api3dScene::AddObject(g_pObject4);

		g_pObject5 = api3dShape::CreateSphere(maFloatRGBA(0,0,1,1), 75.0f, 16, 16);
		g_pObject5->SetPosition(maPoint3d(300,-150,0));
		api3dScene::AddObject(g_pObject5);
	}*/	
}


//--------------------------------------------------------------------
// RT() - Entry point
//--------------------------------------------------------------------
void RT(g2dRenderTarget* i_pWindow, const camCamera* i_pCamera, const g3dScene* i_pScene)
{
	if (!g_bRTCreated)
		return;

	//	float startTime = appTime::GetTime();

	g2dDX11Global::g_pDeviceContext->RSSetState( l_DefaultRasterizerState );
	i_pWindow->MakeCurrent();
	//i_pWindow->Clear(maFloatRGBA(0,1,0,1));

	RayTraceCS11(g2dDX11Global::g_pDeviceContext, i_pCamera, i_pScene, RES_X, RES_Y, RES_X*THREAD_REDUCE_FACTOR, 1, 1 );

	// tone map into window backbuffer:
	g2dWindowDX11* pWnd = dynamic_cast<g2dWindowDX11*>(i_pWindow);
	i_pWindow->MakeCurrent();
	g2dD3D11RenderTargetPtr pTarget = pWnd->GetBackBuffer();

	// maybe create texture to receive contents of raytrace?
	DBG_ASSERT(false, "tone map not implemented");
//	shdwToneMap::DoToneMap( i_pCamera,
//		RES_X, RES_Y, g_p_CanvasSRV, 
//		RES_X, RES_Y, pTarget );

	pTarget->Release();

	//	float stopTime = appTime::GetTime();
	//	float diff = stopTime - startTime;
	//	DBG_LOG("elapsed RT time " << diff);
}



		/*
//#if 0
	///////// DEBUG START /////////
        pDebugBuf = CreateAndCopyToDebugBuf( g2dDX11Global::g_pDevice, 
			                                 g2dDX11Global::g_pDeviceContext, 
											 g_p_ReflInputRays ); 
        hr = g2dDX11Global::g_pDeviceContext->Map( pDebugBuf, 0, D3D11_MAP_READ, 0, &MappedResource );
		ptr = (maVector4d*)MappedResource.pData;
		o1 = ptr[0];
		d1 = ptr[1];
		o2 = ptr[2];
		d2 = ptr[3];
        g2dDX11Global::g_pDeviceContext->Unmap( pDebugBuf, 0 );
        SAFE_RELEASE( pDebugBuf );        
	///////// DEBUG END /////////
//#endif
//#if 0
	///////// DEBUG START /////////
        pDebugBuf = CreateAndCopyToDebugBuf( g2dDX11Global::g_pDevice, 
			                                 g2dDX11Global::g_pDeviceContext, 
											 g_p_OutputRays ); 
        hr = g2dDX11Global::g_pDeviceContext->Map( pDebugBuf, 0, D3D11_MAP_READ, 0, &MappedResource );
		ptr = (maVector4d*)MappedResource.pData;
		o1 = ptr[0];
		d1 = ptr[1];
		o2 = ptr[2];
		d2 = ptr[3];
        g2dDX11Global::g_pDeviceContext->Unmap( pDebugBuf, 0 );
        SAFE_RELEASE( pDebugBuf );        
	///////// DEBUG END /////////
//#endif

		// PostProcess ops (PostProcess.hlsl)
		ID3D11ShaderResourceView*  aSRViews_Post[1] = {g_p_OutputRaysSRV};
		ID3D11UnorderedAccessView* aUAViews_Post[1] = {g_p_ReflInputRaysUAV};	
		RunComputeShader( g_pd3dContext, 
						  g_p_PostProcessCS,			// Compute Shader
						  1, aSRViews_Post,				// SRV's
						  NULL, NULL, NULL, NULL,		// CB's
						  1, aUAViews_Post,				// UAV's
						  NumThreadGroups_X, NumThreadGroups_Y, NumThreadGroups_Z );

//#if 0
	///////// DEBUG START /////////
        pDebugBuf = CreateAndCopyToDebugBuf( g2dDX11Global::g_pDevice, 
			                                 g2dDX11Global::g_pDeviceContext, 
											 g_p_ReflInputRays ); 
        hr = g2dDX11Global::g_pDeviceContext->Map( pDebugBuf, 0, D3D11_MAP_READ, 0, &MappedResource );
		ptr = (maVector4d*)MappedResource.pData;
		o1 = ptr[0];
		d1 = ptr[1];
		o2 = ptr[2];
		d2 = ptr[3];
        g2dDX11Global::g_pDeviceContext->Unmap( pDebugBuf, 0 );
        SAFE_RELEASE( pDebugBuf );        
	///////// DEBUG END /////////
//#endif

//#if 0
	///////// DEBUG START /////////
        pDebugBuf = CreateAndCopyToDebugBuf( g2dDX11Global::g_pDevice, 
			                                 g2dDX11Global::g_pDeviceContext, 
											 g_p_OutputRays ); 
        hr = g2dDX11Global::g_pDeviceContext->Map( pDebugBuf, 0, D3D11_MAP_READ, 0, &MappedResource );
		ptr = (maVector4d*)MappedResource.pData;
		o1 = ptr[0];
		d1 = ptr[1];
		o2 = ptr[2];
		d2 = ptr[3];
        g2dDX11Global::g_pDeviceContext->Unmap( pDebugBuf, 0 );
        SAFE_RELEASE( pDebugBuf );        
	///////// DEBUG END /////////
//#endif
*/
