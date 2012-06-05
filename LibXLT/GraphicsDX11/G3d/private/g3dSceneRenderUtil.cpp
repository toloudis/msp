/****************************************************************************\
**	g3dSceneRenderUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/sc/scBillboard.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"

//#include "profile.h"

//	The reason a macro is used here (instead of a function, an inline function, or a template inline function)
//	is that I need the __FILE__ and __LINE__ macros to resolve to useful values
#define CHECK_D3D_ERROR(op_result, error_string) \
			if( op_result != D3D_OK )	\
			{	\
				g2dDX11Global::PrintDXError(op_result);	\
				DBG_ASSERT(false, error_string);	\
			}	\

namespace
{
	static const g3dAmbientEnvState l_NoAmbientEnvState;


};

namespace g3dSceneRenderUtil
{

	void SetViewingTransforms(const camCamera& i_Camera, g3dLayer::ModelSpace i_ModelSpace /*= g3dLayer::e_World*/)
	{
		maMatrix4x4 cameraMat, projectionMat;//identity
		switch(i_ModelSpace)
		{
		case g3dLayer::e_Screen:
			// screen space: camera and projection matrix are identity. 
			g3dSceneGlobal::SetTransforms(i_Camera.GetPosition(), cameraMat, projectionMat);
			break;

		case g3dLayer::e_Camera:
			// camera space: camera matrix is identity. 

			i_Camera.GetProjectionMatrix(projectionMat);
			//	We must scale this here to correct for D3D's left handed coordinates
			//
			if (!g3dSingleLightRendering::GetDoCubeReflectionGen())
				projectionMat.ScaleBy(-1.0f, 1.0f, 1.0f);

			g3dSceneGlobal::SetTransforms(i_Camera.GetPosition(), cameraMat, projectionMat);
			break;

		case g3dLayer::e_World:
			// world space: use all transforms 

			i_Camera.GetCameraMatrix(cameraMat);

			i_Camera.GetProjectionMatrix(projectionMat);
			//	We must scale this here to correct for D3D's left handed coordinates
			//
			if (!g3dSingleLightRendering::GetDoCubeReflectionGen())
				projectionMat.ScaleBy(-1.0f, 1.0f, 1.0f);

			g3dSceneGlobal::SetTransforms(i_Camera.GetPosition(), cameraMat, projectionMat);
			break;
		};

		scBillboard::SetCameraPosition( i_Camera );
	}

	//------------------------------------------------------------------------
	// make far distance infinite for capping shadow extrusions
	//------------------------------------------------------------------------
	void make_infinite_projection(maMatrix4x4& o_Matrix)
	{
		//	q -> 1
		float q = o_Matrix(2, 2);
		float near_clip = -o_Matrix(3, 2) / q;
		o_Matrix(2, 2) = 1.0f;
		o_Matrix(3, 2) = -near_clip;
	}

	//------------------------------------------------------------------------
	// turn on all lights in LightManager?
	//------------------------------------------------------------------------
	void enable_lights(  )
	{
		// some of these functions might need to go to the base class
		
		g3dLightMgrDX11::Implementation()->EnableAmbient(true);

		const std::vector<g3dLight*>& lights = g3dLightMgrDX11::Implementation()->GetLights();
		for( size_t i = 0; i < lights.size(); ++i )
		{
			g3dLight* pLight = lights[i];
			if( pLight->IsEnabled() )
			{
				if (!g3dSingleLightRendering::GetDoSingleLightRendering() || 
					!pLight->GetCastsShadow())
				{
					g3dLightMgrDX11::Implementation()->SetLight( pLight );
					g3dLightMgrDX11::Implementation()->EnableLight( pLight );
				}
				else
					g3dLightMgrDX11::Implementation()->DisableLight( pLight );
			}
			else
				g3dLightMgrDX11::Implementation()->DisableLight( pLight );
		}

	}

	//------------------------------------------------------------------------
	//	gather_fragment_nodes - fill a list with the scene nodes with 
	//	fragments that are renderable
	//------------------------------------------------------------------------
	void gather_fragment_nodes( g3dSceneNode* i_pNode, SceneNodeVector& o_FragNodes )
	{
		// All children of a non-renderable node should also not render
		if (i_pNode->GetRenderable() && i_pNode->GetActiveInRenderLayer())
		{
			// Update the children
			std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
			std::vector<g3dSceneNode*>::iterator it = children.begin(), end = children.end();
			for( ; it != end; ++it )
			{
				g3dSceneNode* child = (*it);
				gather_fragment_nodes( child, o_FragNodes );
			}

			// Check if it has geometry
			const g3dFragment* pFrag = i_pNode->GetFragment();
			if ( pFrag )
			{
				// Add to the list of scene nodes with fragments
				o_FragNodes.push_back( i_pNode );
			}
		}
	}

	//----------------------------------------------------------------------------
	//	get_box_vis
	//----------------------------------------------------------------------------
	ClipResult get_box_vis( const maAxisBox& i_Box, 
		const maMatrix4x4* i_CameraProjection /*=NULL*/,
		bool i_bIgnoreFarPlane /*= false*/)
	{
		// allocate reusable space for the points.
		static maVector4d l_boxvis_points[8];

		if (i_Box.IsEmpty())
		{
			return e_Reject;
		}

		i_Box.GetBoxPoints( l_boxvis_points );

		int i;
		if (i_CameraProjection == NULL)
		{
			for( i = 0; i < 8; ++i )
			{
				g3dSceneGlobal::GetCameraProjectionTransform().Transform( l_boxvis_points[i] );
			}
		}
		else
		{
			for( i = 0; i < 8; ++i )
			{
				i_CameraProjection->Transform( l_boxvis_points[i] );
			}
		}

		bool some_inside = false;
		bool some_outside = false;

		some_inside = false;
		for( i = 0; i < 8; ++i )
		{
			if( l_boxvis_points[i].m_X > l_boxvis_points[i].m_W )
			{
				some_outside = true;
			}
			else
			{
				some_inside = true;
			}
		}

		if( !some_inside )
		{
			return e_Reject;
		}

		some_inside = false;
		for( i = 0; i < 8; ++i )
		{
			if( l_boxvis_points[i].m_X < -l_boxvis_points[i].m_W )
			{
				some_outside = true;
			}
			else
			{
				some_inside = true;
			}
		}

		if( !some_inside )
		{
			return e_Reject;
		}

		some_inside = false;
		for( i = 0; i < 8; ++i )
		{
			if( l_boxvis_points[i].m_Y > l_boxvis_points[i].m_W )
			{
				some_outside = true;
			}
			else
			{
				some_inside = true;
			}
		}

		if( !some_inside )
		{
			return e_Reject;
		}

		some_inside = false;
		for( i = 0; i < 8; ++i )
		{
			if( l_boxvis_points[i].m_Y < -l_boxvis_points[i].m_W )
			{
				some_outside = true;
			}
			else
			{
				some_inside = true;
			}
		}

		if( !some_inside )
		{
			return e_Reject;
		}

		some_inside = false;
		for( i = 0; i < 8; ++i )
		{
			if( l_boxvis_points[i].m_Z < 0.0f )
			{
				some_outside = true;
			}
			else
			{
				some_inside = true;
			}
		}

		if( !some_inside )
		{
			return e_Reject;
		}

		if (!i_bIgnoreFarPlane)
		{
			some_inside = false;
			for( i = 0; i < 8; ++i )
			{
				if( l_boxvis_points[i].m_Z > l_boxvis_points[i].m_W )
				{
					some_outside = true;
				}
				else
				{
					some_inside = true;
				}
			}

			if( !some_inside )
			{
				return e_Reject;
			}
		}

		if( some_outside )
		{
			return e_Clip;
		}
		else
		{
			return e_NoClip;
		}
	}

	//------------------------------------------------------------------------
	//	nonworld_space_render
	//------------------------------------------------------------------------
	int nonworld_space_render( g3dSceneNode* i_pNode )
	{
		// Check if node is renderable
		if( !i_pNode->GetRenderable() || !i_pNode->GetActiveInRenderLayer())
		{
			return 0;
		}

		// resolve material/effect

//		matShaderEffect* pEffect = matShaderMgr::GetSpecialEffect("HDRLighting.fx");
//		matShaderEffect* pEffect = g3dDX11Util::GetEffect( "HDRLighting.fx" );

		const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;
		matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

		// is this the right technique choice here?
		pEffect->SetTechnique(matShaderEffect::e_Default);

		// set shader globals
		g3dDX11Util::SetupShaderGlobals(pEffect);

		// geometry data
		g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

		// shading data
		pEffect->SetupMaterial(pMaterial);
		//pEffect->SetupAO(g3dPrefs::CurrentPrefs().m_bEnableAO ? i_pNode->GetFragment()->GetOcclusionData() : NULL);

		pEffect->SetupAmbientLighting(i_pNode->GetWorldBox());

		// I think we actually always want to set up the ambient pass,
		// even if that just means we set it up with somethinng with
		// NULL pointers and booleans set to false
		pEffect->SetupAmbientPass(l_NoAmbientEnvState);

		bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
		g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
		int nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
		
		return nTriangles;
	}


	//------------------------------------------------------------------------
	//	screen_space_sort
	//------------------------------------------------------------------------
	bool screen_space_sort( g3dSceneNode* i_pNode1, g3dSceneNode* i_pNode2 )
	{
		return	i_pNode1->GetWorldBox().GetCenter().m_Z >
				i_pNode2->GetWorldBox().GetCenter().m_Z;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void set_transforms(g3dLayer::ModelSpace i_ModelSpace)
	{
/*
		switch( i_ModelSpace )
		{
			case g3dLayer::e_World:
			{
				D3DMATRIX* d3d_camera_mat = (D3DMATRIX*)(&g3dSceneGlobal::g_Camera);
				g2dDX11Global::g_pDevice->SetTransform(D3DTS_VIEW, d3d_camera_mat);

				D3DMATRIX* d3d_projection_mat = (D3DMATRIX*)(&g3dSceneGlobal::g_Projection);
				g2dDX11Global::g_pDevice->SetTransform(D3DTS_PROJECTION, d3d_projection_mat);
			}
			break;

			case g3dLayer::e_Camera:
			{
				g2dDX11Global::g_pDevice->SetTransform(D3DTS_VIEW, &g3dSceneGlobal::g_Identity);

				D3DMATRIX* d3d_projection_mat = (D3DMATRIX*)(&g3dSceneGlobal::g_Projection);
				g2dDX11Global::g_pDevice->SetTransform(D3DTS_PROJECTION, d3d_projection_mat);
			}
			break;

			case g3dLayer::e_Screen:
			{
				g2dDX11Global::g_pDevice->SetTransform(D3DTS_VIEW, &g3dSceneGlobal::g_Identity);

				g2dDX11Global::g_pDevice->SetTransform(D3DTS_PROJECTION, &g3dSceneGlobal::g_Identity);
			}
			break;
		}
*/
	}

	//------------------------------------------------------------------------
	// See if we can cull from resolution. Returns false if resolutions
	//	don't match. May alter the io_LowRes flag if a node has an
	//	override flag set.
	//------------------------------------------------------------------------
	bool check_resolution(g3dSceneNode *i_pNode, bool &io_LowRes)
	{
		if ( i_pNode->GetForceLowResolution() )
			io_LowRes = true;
		
		switch ( i_pNode->GetContentResolution() )
		{
			default:
			case g3dSceneNode::e_Mixed:
				break;
			case g3dSceneNode::e_LowRes:
				if (!io_LowRes)
					return false;
				break;
			case g3dSceneNode::e_HighRes:
				if (io_LowRes)
					return false;
				break;
		}
		return true;
	}

	//------------------------------------------------------------------------
	//	return number of primitives drawn. 
	//------------------------------------------------------------------------
	int DrawNode(const g3dSceneNode* i_pNode)
	{
		D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"DrawNodeAmbient" );

		// resolve material/effect
		const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;
		matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

		// technique e_Default should be able to do batched per vertex lighting.

//		DBG_ASSERT(!g3dSingleLightRendering::GetDoSingleLightRendering(), "default pass used in multipass mode");
		pEffect->SetTechnique(matShaderEffect::e_Default);

		// set shader globals
		g3dDX11Util::SetupShaderGlobals(pEffect);

		// geometry data
		g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode);

		// shading data
		if (g3dSingleLightRendering::GetDoBaking())
		{
			// for bake set diffuse color = 1, specular 0.
			//effShaderData* pData = pMaterial->GetEffectData();
			DBG_WARNING("baking code unimplemented");
		}
		pEffect->SetupMaterial(pMaterial);
	
		pEffect->SetupAmbientLighting(i_pNode->GetWorldBox());
		bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
		g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

		int nTriangles = g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
		pEffect->SetupAmbientPass(l_NoAmbientEnvState);
		D3DPERF_EndEvent();
		return nTriangles;
	}

	//------------------------------------------------------------------------
	//	return number of primitives drawn. 
	//------------------------------------------------------------------------
	int DrawNodeAmbient(const g3dSceneNode* i_pNode, const g3dAmbientEnvState& i_AmbientEnvironment)
	{
		D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"DrawNodeAmbient" );

//		g3dFogDX11::EnableFog( i_pNode->GetFogged() );

		// resolve material/effect
		const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;

		int nTriangles = 0;
		const int num_layers = pMaterial->GetNumMaterialLayers();
		for (int layer_index=0; layer_index<num_layers; ++layer_index)
		{
			matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial, layer_index);

			// technique e_Default should be able to do batched per vertex lighting.

	//		DBG_ASSERT(!g3dSingleLightRendering::GetDoSingleLightRendering(), "default pass used in multipass mode");
			pEffect->SetTechnique(matShaderEffect::e_Default);

			// set shader globals
			g3dDX11Util::SetupShaderGlobals(pEffect);

			// geometry data
			g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode, layer_index);

			// shading data
			if (g3dSingleLightRendering::GetDoBaking())
			{
				// for bake set diffuse color = 1, specular 0.
				//effShaderData* pData = pMaterial->GetEffectData();
				//DBG_WARNING("baking code unimplemented");
			}
			pEffect->SetupMaterial(pMaterial, layer_index);
			//pEffect->SetupAO(g3dPrefs::CurrentPrefs().m_bEnableAO ? i_pNode->GetFragment()->GetOcclusionData() : NULL);

			pEffect->SetupAmbientLighting(i_pNode->GetWorldBox());

			// this state is set in the environment pass.
			// must be nulled out here to skip the env approximation step in the single-pass (shadows off) render.
			if (g3dSingleLightRendering::GetDoSingleLightRendering())
			{
				pEffect->SetupAmbientPass(l_NoAmbientEnvState);
			}
			else
			{
				pEffect->SetupAmbientPass(i_AmbientEnvironment);
			}
			bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
			g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
			nTriangles += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
			pEffect->SetupAmbientPass(l_NoAmbientEnvState);
		}
		D3DPERF_EndEvent();
		return nTriangles;
	}

	//------------------------------------------------------------------------
	//	return number of primitives drawn. 
	//------------------------------------------------------------------------
	int DrawNodeEnvironment(const g3dSceneNode* i_pNode, const g3dAmbientEnvState& i_AmbientEnvironment)
	{
		D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"DrawNodeEnvironment" );

		// resolve material/effect
		const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;

		int nTriangles = 0;
		const int num_layers = pMaterial->GetNumMaterialLayers();
		for (int layer_index=0; layer_index<num_layers; ++layer_index)
		{
			matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial, layer_index);

			// e_Environment is a different type of ambient pass that is ambient-only.
			pEffect->SetTechnique(matShaderEffect::e_Environment);

			// set shader globals
			g3dDX11Util::SetupShaderGlobals(pEffect);

			// geometry data
			g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode, layer_index);

			// shading data
			pEffect->SetupMaterial(pMaterial, layer_index);
			//pEffect->SetupAO(g3dPrefs::CurrentPrefs().m_bEnableAO ? i_pNode->GetFragment()->GetOcclusionData() : NULL);

			// I think we actually always want to set up the ambient pass,
			// even if that just means we set it up with somethinng with
			// NULL pointers and booleans set to false
			if (!g3dPrefs::CurrentPrefs().m_bEnableEnvironment)
			{
				pEffect->SetupAmbientPass(l_NoAmbientEnvState);
			}
			else
			{
				pEffect->SetupAmbientPass(i_AmbientEnvironment);
			}
			bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
			g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
			nTriangles += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );
			pEffect->SetupAmbientPass(l_NoAmbientEnvState);

			// prepare blend for next layer:
			// assume premul alpha from pix shader:
			
		}
		D3DPERF_EndEvent();	
		return nTriangles;
	}

	//------------------------------------------------------------------------
	//	return number of primitives drawn. 
	//------------------------------------------------------------------------
	int DrawNodeLit(const g3dSceneNode* i_pNode, g3dLight* i_pLight, const g3dProjectedLight* i_pProjLight, const g3dAmbientEnvState* i_pAmbientEnvironment )
	{
		D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"DrawNodeLit" );

		// resolve material/effect
		const matMaterial* pMaterial = g3dDX11Util::GetMaterial(i_pNode);//m_pAlphaMaskMat;

		int nTriangles = 0;
		const int num_layers = pMaterial->GetNumMaterialLayers();
		for (int layer_index=0; layer_index<num_layers; ++layer_index)
		{
			matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial, layer_index);

			// if effect does not support lighting, then don't render it in lit passes.
			if (!pEffect->DoesLighting())
				continue;

			DBG_ASSERT(i_pLight != NULL, "null light in shdwpasslit drawnode");
			if (i_pProjLight != NULL)
			{
				matShaderEffect::Technique tec = SelectShadowTechnique(i_pProjLight);
				pEffect->SetTechnique(tec);
			}
			else
				pEffect->SetTechnique(matShaderEffect::e_SingleLight);

			// set shader globals
			g3dDX11Util::SetupShaderGlobals(pEffect);

			// geometry data
			g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode, layer_index);

			//include environment parameters if valid
			if ( i_pAmbientEnvironment && g3dSingleLightRendering::IsFirstLight() )
			{
				pEffect->SetupAmbientPass(*i_pAmbientEnvironment);
			}

			// light before material so that material can override light settings if needed.
			pEffect->SetupSingleLight(i_pLight, i_pProjLight, i_pNode->GetFragment()->GetReceivesShadow());

			// shading data
			if (g3dSingleLightRendering::GetDoBaking())
			{
				// for bake set diffuse color = (1,1,1,alpha), specular 0.
				//effShaderData* pData = pMaterial->GetEffectData();
				//DBG_WARNING("baking code unimplemented");
			}
			pEffect->SetupMaterial(pMaterial, layer_index);
			//pEffect->SetupAO(g3dPrefs::CurrentPrefs().m_bEnableAO ? i_pNode->GetFragment()->GetOcclusionData() : NULL);
			bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
			g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
			nTriangles += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );

			ID3D11ShaderResourceView* nullPSR[5] = {NULL};
			g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0,5,nullPSR);

			// prepare blend for next layer:
			// assume premul alpha from pix shader:
			
		}
		D3DPERF_EndEvent();
		return nTriangles;
	}

	//------------------------------------------------------------------------
	//	decide which shadow technique to use 
	//------------------------------------------------------------------------
	matShaderEffect::Technique SelectShadowTechnique(const g3dProjectedLight* i_pProjLight)
	{
		matShaderEffect::Technique tec = matShaderEffect::e_ProjectedLight;
		switch(g3dSingleLightRendering::GetShadowQualityOverride())
		{
			case g3dSingleLightRendering::SQ_NONE:
				switch(i_pProjLight->GetShadowQuality())
				{
					case 3: tec = matShaderEffect::e_ProjectedLightSuperSample3; break;
					case 2: tec = matShaderEffect::e_ProjectedLightSuperSample2; break;
					case 1: tec = matShaderEffect::e_ProjectedLightSuperSample; break;
					default: tec = matShaderEffect::e_ProjectedLight; break;
				}
				break;
			case g3dSingleLightRendering::SQ_VERY_HIGH:
				tec = matShaderEffect::e_ProjectedLightSuperSample3; 
				break;
			case g3dSingleLightRendering::SQ_HIGH:
				tec = matShaderEffect::e_ProjectedLightSuperSample2; 
				break;
			case g3dSingleLightRendering::SQ_MED:
				tec = matShaderEffect::e_ProjectedLightSuperSample; 
				break;
			default:
				tec = matShaderEffect::e_ProjectedLight; 
				break;
		}
		return tec;
	}
}
