/****************************************************************************\
**	cmraRendefbxExportInterest.hpp
**
**		A Export Interest is registered by a system that has data
**	to be exported to Maya ascii format.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef CMRA_FBXEXPORTINTEREST_HPP
#error cmraRendefbxExportInterest.hpp multiply included
#endif
#define CMRA_FBXEXPORTINTEREST_HPP

#ifndef FBX_EXPORTINTEREST_HPP
#include "Support/fbx/fbxExportInterest.hpp"
#endif


//============================================================================
//============================================================================
class cmraFBXExportInterest : public fbxExportInterest
{
	public:
		//--------------------------------------------------------------------
		//  returns chunk description for display purposes
		//--------------------------------------------------------------------
		virtual const char* GetChunkDesc() const;

		//--------------------------------------------------------------------
		// Gather data for scene that will be exported
		//--------------------------------------------------------------------
		virtual void GatherSceneData( fbxSceneData &o_SceneData , bool &o_SceneHasBeenBaked ) const;

		//--------------------------------------------------------------------
		//	Export only the items in the given list.
		//--------------------------------------------------------------------
		virtual void Export( fbxExporter& i_Exporter, const fbxSceneData &i_SceneData );

};
