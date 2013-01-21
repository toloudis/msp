/****************************************************************************\
**	g3dSceneRenderUtil.cpp
**
**		see .hpp
**
** Area17
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "Area18/g3d/g3dSceneRenderUtil.hpp"

#include "Area18/g3d/g3dSceneGlobal.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/sc/scBillboard.hpp"

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

	void SetViewingTransforms(const camCamera& i_Camera, g3dSceneGlobal& io_SceneGlobal, 
		g3dLayer::ModelSpace i_ModelSpace /*= g3dLayer::e_World*/)
	{
		maMatrix4x4 cameraMat, projectionMat;//identity
		switch(i_ModelSpace)
		{
		case g3dLayer::e_Screen:
			// screen space: camera and projection matrix are identity. 
			io_SceneGlobal.SetTransforms(i_Camera.GetPosition(), cameraMat, projectionMat);
			break;

		case g3dLayer::e_Camera:
			// camera space: camera matrix is identity. 

			i_Camera.GetProjectionMatrix(projectionMat);
			//	We must scale this here to correct for D3D's left handed coordinates
			//
//			if (!g3dSingleLightRendering::GetDoCubeReflectionGen())
//				projectionMat.ScaleBy(-1.0f, 1.0f, 1.0f);

			io_SceneGlobal.SetTransforms(i_Camera.GetPosition(), cameraMat, projectionMat);
			break;

		case g3dLayer::e_World:
			// world space: use all transforms 

			i_Camera.GetCameraMatrix(cameraMat);

			i_Camera.GetProjectionMatrix(projectionMat);
			//	We must scale this here to correct for D3D's left handed coordinates
			//
//			if (!g3dSingleLightRendering::GetDoCubeReflectionGen())
//				projectionMat.ScaleBy(-1.0f, 1.0f, 1.0f);

			io_SceneGlobal.SetTransforms(i_Camera.GetPosition(), cameraMat, projectionMat);
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
		const maMatrix4x4& i_CameraProjection,
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
		for( i = 0; i < 8; ++i )
		{
			i_CameraProjection.Transform( l_boxvis_points[i] );
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
	//	screen_space_sort
	//------------------------------------------------------------------------
	bool screen_space_sort( g3dSceneNode* i_pNode1, g3dSceneNode* i_pNode2 )
	{
		return	i_pNode1->GetWorldBox().GetCenter().m_Z >
				i_pNode2->GetWorldBox().GetCenter().m_Z;
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
