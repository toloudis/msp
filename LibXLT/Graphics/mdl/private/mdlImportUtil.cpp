/****************************************************************************\
**	mdlImportUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlImportUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/mdl/mdlFragCreate.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlSplitFragInfo.hpp"


//============================================================================
//============================================================================
namespace mdlImportUtil
{
	//------------------------------------------------------------------------
	// Make one or more fragments from the given mdlFragInfo
	//------------------------------------------------------------------------
	void MakeFragments( mdlFragInfo& i_FragInfo,
						 //const fsResourceFinder& i_TextureFinder,
						 std::vector<g3dFragment*>& o_Fragments,
						 std::vector<matMaterial*>& o_Materials,
						 //std::vector<matTexture*>& o_Textures,
						 entFragInfoSink* o_Sink,
						 bool i_Morphable,
						 bool i_CreateMaterials)
	{
		int num_materials = i_FragInfo.m_Materials.size();
		for (int mi=0; mi<num_materials; mi++)
		{
			//	load the textures and prepare the material array for fragment creation
			mdlMatInfo& mat_info = (*i_FragInfo.m_Materials[mi]);
			if (i_CreateMaterials)
			{
				//bga - Not loading textures anymore, just creating the material pointer
				//mat_info.LoadTextures(i_TextureFinder, o_Textures);
				mat_info.CreateMaterial();
				o_Materials.push_back(mat_info.m_pMaterial);
			}
		}

		std::vector<mdlSplitFragInfo> split_frags;
		mdlFragUtil::SplitFragments(i_FragInfo, split_frags);

		//std::vector<mdlSplitFragInfo> init_split_frags;
		//mdlFragUtil::SplitFragments(i_FragInfo, init_split_frags);

		//const int c_MaxTriangles = 20000;
		//std::vector<mdlSplitFragInfo> split_frags;
		//for (int i=0; i<init_split_frags.size(); i++)
		//{
		//	DBG_LOG("Init Number of vertices in fragment: " << init_split_frags[i].m_Vertices.size());
		//	DBG_LOG("Init Number of triangles in fragment: " << init_split_frags[i].m_Indices.size() / 3);
		//	mdlFragCreate::SpatiallyPartition(init_split_frags[i],
		//						split_frags,
		//						c_MaxTriangles);
		//}

			
		{
			// Only create one fragment at a time 
			//envScopedLock fragment_lock(mdlReader::GetReaderMutex());

			std::vector<g3dFragment*> fragments;
			int num_split_frags = split_frags.size();
			for (int i=0; i<num_split_frags; i++)
			{
				//DBG_LOG("Number of vertices in fragment: " << split_frags[i].m_Vertices.size());
				//DBG_LOG("Number of triangles in fragment: " << split_frags[i].m_Indices.size() / 3);
				if (split_frags[i].m_Vertices.size() > 0)
				{
					mdlFragCreate::OptimizeFragment( split_frags[i] );
					g3dFragment *fragment = mdlFragCreate::CreateFragment(split_frags[i], i_Morphable);

					o_Fragments.push_back(fragment);
					fragments.push_back(fragment);
				}
				else
				{
					DBG_LOG("Split fragment (" << i << ") has 0 vertices");
				}
			}

			// Sink receives array of fragments for one mdlFragInfo now
			if( o_Sink )
				o_Sink->ReceiveFragInfo( fragments, i_FragInfo );

		}
	}

}	// end of namespace

