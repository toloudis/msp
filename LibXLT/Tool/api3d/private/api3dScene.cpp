/*****************************************************************************
**	api3dScene.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dScene.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/Ma/maFunctions.hpp"
#include "Graphics/ent/entEntity.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/sc/scBillboard.hpp"
#include "Graphics/sc/scParticleGenerator.hpp"
#include "Tool/api3d/api3dBillboard.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dObjectGeom.hpp"
#include "Tool/api3d/api3dParticleGenerator.hpp"
#include "Tool/api3d/private/api3dSceneImpl.hpp"


//============================================================================
//============================================================================
namespace api3dScene
{
	namespace
	{
		api3dSceneImpl* l_pScene = NULL;
		struct sLightCamera
		{
			g3dProjectedLight* m_pLight;
			camCamera* m_pShadowCamera;
		};
		std::vector<sLightCamera> l_ResizeLights; // lights that should be auto resized to scene

		//--------------------------------------------------------------------
		// Compute the world box for the surfaces that receive
		// light from the given light. Tracks the render state
		// variables to see when the light is influencing the nodes.
		//--------------------------------------------------------------------
		void compute_receiving_bbox( g3dSceneNode& i_Node, 
									 g3dLight *i_pLight,
									 maAxisBox& o_WorldBox )
		{		
			if (!i_Node.GetRenderable())
				return;

			// See if this node has the given light in its render state
			if (g3dRenderState *pState = i_Node.GetRenderState())
			{
				if (envSTLHelpers::Contains(pState->m_Lights, i_pLight))
				{
					// found the light, accumulate bbox
					const maAxisBox& node_box = i_Node.GetWorldBox();
					if (!node_box.IsEmpty())
					{
						o_WorldBox.Union( node_box );

						// Can prune traversal here - don't have to traverse children 
						// because we have the sum of their bboxes already in i_Node.

						//bga - Later, there might be a desire to traverse more in order to 
						// separate out the light receiving fragments from the others.
						return;
					}
				}
			}

			// Update the children
			std::vector<g3dSceneNode*>& children = i_Node.GetChildren();
			std::vector<g3dSceneNode*>::iterator it = children.begin(), end = children.end();
			for( ; it != end; ++it )
			{
				g3dSceneNode* child = (*it);
				if (child)
				{
					compute_receiving_bbox( *child, i_pLight, o_WorldBox );
				}
			}
		}

		//--------------------------------------------------------------------
		// Determine the position of the directional light needed to cast 
		// shadows on the bounding sphere of the light's focus. Look for all
		// shadow casting meshes and project them to the main ray of the
		// directional light. Determine the near distance needed for the
		// light based on the minimum value of the projection.
		// i_LightDirection should be normalized.
		//--------------------------------------------------------------------
		float find_tightest_near_plane(g3dSceneNode& i_Node,
										   const maVector3d &i_LightDirection,
										   const maPoint3d &i_SphereCenter,
										   float i_SphereRadius)
		{
			// Looking only for shadow casting nodes
			if (!i_Node.GetRenderable() || !i_Node.GetCastsShadow())
				return 0;

			float min_proj = 0.0f;
			const maAxisBox& node_box = i_Node.GetWorldBox();
			if (!node_box.IsEmpty())
			{
				// Project the min/max box
				//bga - Actually, we only have to project one of these points,
				// which one could be determined by the light's direction.
				for (int i=0; i<8;++i)
				{
					maPoint3d pos = node_box.GetBoxPoint(i);
					float proj = (pos - i_SphereCenter) * i_LightDirection;
					if (proj < min_proj)
						min_proj = proj;
				}

				// We are only concerned with objects that are "earlier"
				// along the light direction than the sphere of focus
				// because the sphere of focus will be included anyway
				// and anything past the sphere can't cast shadows on 
				// the objects in the sphere.
				if (min_proj < -i_SphereRadius)
				{
					// See if this bounding box intersects the ortho frustrum
					// by checking the distance from bbox center to the light's ray.
					maVector3d dist_vec = node_box.GetCenter() - i_SphereCenter;
					float proj = dist_vec * i_LightDirection;
					dist_vec -= proj * i_LightDirection; // dist_vec is now plane distance
					float dist = dist_vec.Length();
					float bbox_radius = node_box.GetRadius();
					if (dist > bbox_radius+i_SphereRadius)
					{
						// no overlap in frustrum, this node will not cast shadow on 
						// sphere of focus
						return 0.0f; 
					}
					else
					{
						// If this node has a fragment that casts shadows, then submit the 
						// minimum projection
						if (i_Node.GetFragment())
						{
							if (i_Node.GetFragment()->GetCastsShadow())
								return min_proj;
							else
								return 0.0f;
						}
						else
						{
							// Otherwise look through child nodes for fragments
							// that do cast shadow
							min_proj = 0.0f; // reset minimum for more granular search
							std::vector<g3dSceneNode*>& children = i_Node.GetChildren();
							std::vector<g3dSceneNode*>::iterator it = children.begin(), end = children.end();
							for( ; it != end; ++it )
							{
								g3dSceneNode* child = (*it);
								if (child)
								{		
									float proj = find_tightest_near_plane(*child,
											   i_LightDirection, i_SphereCenter, i_SphereRadius);
									if (proj < min_proj)
										min_proj = proj;
								}
							}
						}
					}
				}
			}
			
			return min_proj;
		}								   
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize()
	{
		l_pScene = new api3dSceneImpl;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize()
	{
		delete l_pScene;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Think(float i_fSimulationTime)
	{
		l_pScene->Think(i_fSimulationTime);

		if (!l_ResizeLights.empty())
		{
			// Resize lights that are registered so that they view the full scene
			// bonding box.
			g3dSceneNode *pRootNode = api3dScene::GetRoot( api3dScene::WorldLayerIndex() );
			if (pRootNode)
			{		
				//maAxisBox scene_bbox = pRootNode->GetWorldBox();
				std::vector<sLightCamera>::iterator it;
				for (it = l_ResizeLights.begin(); it != l_ResizeLights.end(); ++it)
				{
					g3dProjectedLight *pLight = (it->m_pLight);

					// Assuming that the light is directional...
					if (pLight->GetIsDirectional())
					{
						maVector3d light_dir = pLight->GetDirection();

						maAxisBox scene_bbox;
						compute_receiving_bbox( *pRootNode, pLight, scene_bbox );

						if (!scene_bbox.IsEmpty())
						{
							maPoint3d scene_center = scene_bbox.GetCenter();
							float radius = scene_bbox.GetRadius();

							// near_dist will be a negative number based on 
							// the projection of the near plane along the
							// ray passing through scene_center with light_dir direction.
							float near_dist = find_tightest_near_plane(*pRootNode,
											   light_dir,
											   scene_center,
											   radius);
							near_dist = maFunctions::Lowest(near_dist, -radius);

							// Expand bounds a little larger than the sphere and near dist
							near_dist *= 1.025f;
							float scale = 1.025f * radius;
							float range = scale - near_dist; 
							//DBG_LOG("Light's influence: " << near_dist << " scale " << scale << " range " << range << " around point: " << scene_center);


							// Move light to sphere bounding the bbox based on light's
							// direction. Then resize scale and range to surround the sphere.
							pLight->SetTarget( scene_center );
							pLight->SetPosition( scene_center + light_dir * near_dist); // near_dist is negative
							pLight->SetScale( 2.0f * scale ); // turn radius into diameter for scale value
							pLight->SetRange( range );

							if (it->m_pShadowCamera)
							{
								pLight->OrientCamera( *it->m_pShadowCamera );
							}
						}
					}
				}
			}
		}
	}

	//--------------------------------------------------------------------
	//	allow any LOD objects to update themselves (i.e. swap models)
	//--------------------------------------------------------------------
	void UpdateLOD( const maPoint3d& i_OriginPoint )
	{
		l_pScene->UpdateLOD( i_OriginPoint );
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	g3dScene*	GetScene()
	{
		return l_pScene;
	}

	//========================================================================
	//	SetFog sets fog parameters for this scene
	//========================================================================
	void SetFog( const fogParams& i_FogParams )
	{
		l_pScene->SetFog(i_FogParams);
	}

	//------------------------------------------------------------------------
	//	SetSSAO sets ssao parameters for this scene
	//------------------------------------------------------------------------
	void SetSSAO(const ssaoParams& i_SSAOParams)
	{
		l_pScene->SetSSAOSettings(i_SSAOParams);
	}

	//------------------------------------------------------------------------
	//	SetSSGI sets ssgi parameters for this scene
	//------------------------------------------------------------------------
	void SetSSGI(const ssgiParams& i_SSGIParams)
	{
		l_pScene->SetSSGISettings(i_SSGIParams);
	}

	//------------------------------------------------------------------------
	//	SetMotionBlur sets Motion Blur parameters for this scene
	//------------------------------------------------------------------------
	void SetMotionBlur(const MotionBlurParams& i_MotionBlurParams)
	{
		l_pScene->SetMotionBlurSettings(i_MotionBlurParams);
	}

	//------------------------------------------------------------------------
	// Settings of global ambient state
	//------------------------------------------------------------------------
	void SetGlobalAmbient(const g3dAmbientEnvState& i_GlobalAmbient)
	{
		l_pScene->SetGlobalAmbient(i_GlobalAmbient);
	}


	//------------------------------------------------------------------------
	//	AddLayer - add a layer to the scene.  keep the index to access the
	//	scene node
	//------------------------------------------------------------------------
	int AddLayer( g3dLayer::SortMethod i_SortMethod,
				  g3dLayer::ModelSpace i_ModelSpace,
				  g3dLayer::BlendMethod i_BlendMethod,
				  bool i_bFogEnabled,
				  bool i_bShadows,
				  bool i_bPreLit,
				  bool i_bClearDepth)
	{
		return l_pScene->AddLayer( i_SortMethod, i_ModelSpace, i_BlendMethod, i_bFogEnabled, i_bShadows, i_bPreLit, i_bClearDepth );
	}

	//------------------------------------------------------------------------
	//	get the scene root for a given layer
	//------------------------------------------------------------------------
	g3dSceneNode* GetRoot( int i_LayerIndex )
	{
		return l_pScene->GetRoot( i_LayerIndex );
	}

	//--------------------------------------------------------------------
	// These functions return the index of the default world layer
	//	and the camera space layer.
	//--------------------------------------------------------------------
	int	WorldLayerIndex()
	{
		return l_pScene->WorldLayerIndex();
	}
	int	CameraLayerIndex()
	{
		return l_pScene->CameraLayerIndex();
	}


	//--------------------------------------------------------------------
	//  Adds object to scene
	//--------------------------------------------------------------------
	void  AddObject( api3dObject *i_pObj, int i_LayerIndex )
	{
		DBG_ASSERT(i_pObj, "Object pointer is NULL");
		api3dObjectEntity* ent_obj = dynamic_cast<api3dObjectEntity*>(i_pObj);
		if ( ent_obj )
		{
			l_pScene->AddEntity( ent_obj->GetEntity() );
			return;
		}

		api3dObjectSingle* geom_obj = dynamic_cast<api3dObjectSingle*>(i_pObj);
		if ( geom_obj )
		{
			l_pScene->AddObject( geom_obj->Object(), i_LayerIndex );
			return;
		}
		
		DBG_ASSERT( 0 , "Unexpected object type" );
	}

	//--------------------------------------------------------------------
	//  Removes object from scene
	//--------------------------------------------------------------------
	void  RemoveObject( api3dObject *i_pObj, int i_LayerIndex )
	{
		api3dObjectEntity* ent_obj = dynamic_cast<api3dObjectEntity*>(i_pObj);
		if ( ent_obj )
		{
			l_pScene->RemoveEntity( ent_obj->GetEntity() );
			return;
		}

		api3dObjectSingle* geom_obj = dynamic_cast<api3dObjectSingle*>(i_pObj);
		if ( geom_obj )
		{
			l_pScene->RemoveObject( geom_obj->Object(), i_LayerIndex );
			return;
		}

		DBG_ASSERT( 0 , "Unexpected object type" );
	}

	//--------------------------------------------------------------------
	//  Adds ParticleGenerator to scene
	//--------------------------------------------------------------------
	void  AddParticleGenerator( api3dParticleGenerator *i_pObj )
	{
		l_pScene->AddObject( i_pObj->GetParticleGenerator() );
	}

	//--------------------------------------------------------------------
	//  Removes ParticleGenerator from scene
	//--------------------------------------------------------------------
	void  RemoveParticleGenerator( api3dParticleGenerator *i_pObj )
	{
		l_pScene->RemoveObject( i_pObj->GetParticleGenerator() );
	}

	//--------------------------------------------------------------------
	// Register/deregister a light to be resized to view whole scene
	//--------------------------------------------------------------------
	void AddAutoResizeLight(g3dProjectedLight *i_pLight, camCamera *i_pShadowCamera)
	{
		sLightCamera light_camera = { i_pLight, i_pShadowCamera };
		l_ResizeLights.push_back(light_camera);
	}
	void RemoveAutoResizeLight(g3dProjectedLight *i_pLight)
	{	
		std::vector<sLightCamera>::iterator it;
		for (it = l_ResizeLights.begin(); it != l_ResizeLights.end(); ++it)
		{
			if (it->m_pLight == i_pLight)
			{
				l_ResizeLights.erase(it);
				break;
			}
		}
	}

}	// end of namespace
