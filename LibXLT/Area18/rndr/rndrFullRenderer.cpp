#include "rndrFullRenderer.h"

#include "Area18/g3d/g3dSceneGlobal.hpp"
#include "Area18/ogl/oglTypes.hpp"
#include "Area18/ogl/oglView.h"
#include "Area18/mat/matShaderBaseGL.hpp"
#include "Area18/mesh/meshRenderer.hpp"
#include "Area18/mesh/meshTriMeshFrag.hpp"
#include "Area18/shdr/shdrPipeline.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/G2d/g2dRenderTarget.hpp"
#include "Graphics/G3d/g3dLayer.hpp"
#include "Graphics/G3d/g3dScene.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "Graphics/Mat/matShaderMgr.hpp"

rndrFullRenderer::rndrFullRenderer(void)
{
}


rndrFullRenderer::~rndrFullRenderer(void)
{
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy plus additional 
//	viewer specific layers.
//	Returns the number of triangles rendered
//--------------------------------------------------------------------
int rndrFullRenderer::Render( g2dRenderTarget* iWindow, const camCamera& i_Camera,
						const g3dScene &i_Scene,
						const std::vector<g3dLayer*>& i_ViewerLayers,
						float i_fSimTime )
{
	glClearColor(1,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	GLsizei w,h;
	iWindow->GetDimensions(w,h);
	glViewport(0,0,w,h);

	// camera setup for gl:
	oglView v;
	v.setAspectRatio(i_Camera.GetAspect());
	v.setFarClipDistance(i_Camera.GetFarClip());
	v.setNearClipDistance(i_Camera.GetNearClip());
	v.setFieldOfView(i_Camera.GetFOV()*3.14159265/180.0);
	if (i_Camera.IsOrthographic()) {
		// TODO: handle ortho conversion
		//v.setOrthoProjection(i_Camera.GetLeft(), i_Camera.GetOrthoWidth()
	}
	else {
		v.setPerspectiveProjection(i_Camera.GetFOV());// check units and x/y dimension
	}
	float t,b,l,r;
	i_Camera.GetSubViewport(t,b,l,r);
	v.setScreenWindow(l,r,b,t);
	g3dSceneGlobal sceneGlobal;
	sceneGlobal.SetTransforms(v.position(), v.modelView(), v.projection());
	
	// traverse scene
	mNodesToDraw.clear();
	for (int i = 0; i < i_Scene.GetNumLayers(); ++i) {
		TraverseLayer(i_fSimTime, i_Scene.GetLayer(i),
			&i_Camera, false);
	}

	// draw from list.

	int n = 0;
	for (size_t i = 0; i < mNodesToDraw.size(); ++i) {
		g3dSceneNode* node = mNodesToDraw[i];
		g3dFragment* f = node->GetFragment();
		matMaterial* m = f->GetMaterial();//mNodesToDraw[i]->GetMaterial();
		meshTriMeshFrag* mf = (meshTriMeshFrag*)f; 

		// resolve material/effect
		matShaderEffect* pEffect = matShaderMgr::GetEffect(*m);
		matShaderBaseGL* i_pEffect = (matShaderBaseGL*)pEffect;

		//// set shader globals
		//g3dDX11Util::SetupShaderGlobals(pEffect);
		i_pEffect->Begin();

		i_pEffect->GetEffect()->activateVs();

		i_pEffect->SetTime(i_fSimTime);
		i_pEffect->SetVertexUVBakeMode(false);
		i_pEffect->SetIsolateReflections(false);
		i_pEffect->SetAlphaTestRef(0.0f);
		i_pEffect->SetClipPlane(sceneGlobal.g_ClipPlane);


		//// geometry data
		i_pEffect->SetIsDoubleSided(f->GetDoubleSided());

		maPoint2d scale, trans;
		f->GetUVBakeFactors(scale, trans);
		i_pEffect->SetBakingFactors(scale, trans);

		i_pEffect->SetupMatrices(f->IsModelSpaceVertices() ? node->GetTotalTransform() : maMatrix4x4(),
																			sceneGlobal.GetCameraTransform(), 
																		sceneGlobal.GetProjectionTransform(), 
																				sceneGlobal.GetCameraPos() );

		if (f->GetHasSkinning() && i_pEffect->GetHasSkinning())
		{
			i_pEffect->SetupSkinningMatrices(f->GetSkinningPalette());
		}

		i_pEffect->SetupTessellatorMeshTexture( f->GetHardwareTesselateMeshTexture() );

		//derive displacement from root material (cannot override material displacement)
		const matMaterial* pMtl = m;
		matTexture* pTex = NULL;
		float Dscale = 0;
		float Dbias = 0;
		float Dblur = 0;
		maVector2d DObjUVScale;
		if( pMtl )
		{
			// set up uv transform
			maMatrix4x4 muv = pMtl->GetUVTransform(0).MakeUVTransform();
			i_pEffect->SetupUVTransform( muv );

			const effDisplacementData& dData = pMtl->GetDisplacementData();

			Dscale = dData.m_Scale;
			Dbias = dData.m_Bias;
			Dblur = dData.m_Blur;
			pTex = dData.m_pDisplacementMap;
			DObjUVScale = dData.m_ObjUVScale;

			i_pEffect->SetTessellateValue( (dData.m_TessellationValue*2)+1.0f );
		}

		i_pEffect->SetupDisplacementMap( pTex, Dscale, Dbias, Dblur, DObjUVScale );

		const effNormalsData& nData = pMtl->GetNormalsData();
		i_pEffect->SetupNormalMap( nData.m_pNormalMap, nData.m_BumpScale );
		//g3dDX11Util::SetupShaderGeometry(pEffect, i_pNode, layer_index);

			////include environment parameters if valid
			//if ( i_pAmbientEnvironment && g3dSingleLightRendering::IsFirstLight() )
			//{
			//	pEffect->SetupAmbientPass(*i_pAmbientEnvironment);
			//}

			//// light before material so that material can override light settings if needed.
			//pEffect->SetupSingleLight(i_pLight, i_pProjLight, i_pNode->GetFragment()->GetReceivesShadow());

			//// shading data
			//if (g3dSingleLightRendering::GetDoBaking())
			//{
			//	// for bake set diffuse color = (1,1,1,alpha), specular 0.
			//	//effShaderData* pData = pMaterial->GetEffectData();
			//	//DBG_WARNING("baking code unimplemented");
			//}
			//pEffect->SetupMaterial(pMaterial, layer_index);
			////pEffect->SetupAO(g3dPrefs::CurrentPrefs().m_bEnableAO ? i_pNode->GetFragment()->GetOcclusionData() : NULL);
			//bool bDoubleSided = i_pNode->GetFragment()->GetDoubleSided();
			//g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
			//nTriangles += g3dRendererMgr::Render( i_pNode, pMaterial, pEffect );




		n += mf->GetRenderer()->Render( mf, m, NULL, NULL );
	}

	return n;
}

//--------------------------------------------------------------------
//	Let a renderer free up any memory it is holding on to 
//--------------------------------------------------------------------
void rndrFullRenderer::ReleaseResources()
{
}

void rndrFullRenderer::Traverse( g3dSceneNode* i_pNode, 
			  g3dSceneNode::DrawStyle i_DrawStyle,
			  bool i_bRenderLowRes,
			  bool i_bDoClip)
{ 
	const g3dFragment* pFrag = i_pNode->GetFragment();

	// Check if it has geometry
	if( pFrag && !pFrag->IsShadowHull())
	{
		matMaterial* pMatOverride = i_pNode->GetMaterial();
		mNodesToDraw.push_back(i_pNode);
	}

	// Render the children
	std::vector<g3dSceneNode*>& children = i_pNode->GetChildren();
	int nkids = children.size();
	for (int i=0; i<nkids; i++)
		Traverse(children[i], 
			i_DrawStyle,
			i_bRenderLowRes,
			i_bDoClip);
}

void rndrFullRenderer::TraverseLayer(float i_time, const g3dLayer* i_pLayer,
	const camCamera* i_pCamera, bool i_bDoClipping)
{
	g3dSceneNode::DrawStyle draw_style = g3dSceneNode::e_Inherit;

	bool bLowRes = false;
	Traverse(i_pLayer->GetRootNode(),
		draw_style,
		bLowRes,
		i_bDoClipping);
}


