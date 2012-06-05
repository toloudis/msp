/****************************************************************************\
**	aoRendermanExportInterest.hpp
**
**		A Export Interest is registered by a system that has data
**	to be exported to Maya ascii format.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef AO_RENDERMANEXPORTINTEREST_HPP
#error aoRendermanExportInterest.hpp multiply included
#endif
#define AO_RENDERMANEXPORTINTEREST_HPP

#ifndef RMAN_EXPORTINTEREST_HPP
#include "Support/rman/rmanExportInterest.hpp"
#endif


//============================================================================
//============================================================================
class aoRendermanExportInterest : public rmanExportInterest
{
	public:
		//--------------------------------------------------------------------
		//  returns chunk description for display purposes
		//--------------------------------------------------------------------
		virtual const char* GetChunkDesc() const;

		//--------------------------------------------------------------------
		// Gather data for scene that will be exported
		//--------------------------------------------------------------------
		virtual void GatherSceneData( rmanSceneData &o_SceneData, rmanGlobalData &o_GlobalData ) const;

		//--------------------------------------------------------------------
		//	Export only the items in the given list.
		//	The strings will be some subset of what was returned from the
		//	call to GatherItemNames.
		//--------------------------------------------------------------------
		virtual void Export( rmanExporter& i_Exporter, const rmanSceneData &i_SceneData);
};
