/****************************************************************************\
**	ptltMRayExportInterest.hpp
**
**		A Export Interest is registered by a system that has data
**	to be exported to Maya ascii format.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef PTLT_MRAYEXPORTINTEREST_HPP
#error ptltMRayExportInterest.hpp multiply included
#endif
#define PTLT_MRAYEXPORTINTEREST_HPP

#ifndef MRAY_EXPORTINTEREST_HPP
#include "Support/mray/mrayExportInterest.hpp"
#endif


//============================================================================
//============================================================================
class ptltMRayExportInterest : public mrayExportInterest
{
	public:
		//--------------------------------------------------------------------
		//  returns chunk description for display purposes
		//--------------------------------------------------------------------
		virtual const char* GetChunkDesc() const;

		//--------------------------------------------------------------------
		// Gather data for scene that will be exported
		//--------------------------------------------------------------------
		virtual void GatherSceneData( mraySceneData &o_SceneData , mrayGlobalData &o_GlobalData ) const;

		//--------------------------------------------------------------------
		//	Export only the items in the given list.
		//	The strings will be some subset of what was returned from the
		//	call to GatherItemNames.
		//--------------------------------------------------------------------
		virtual void Export( mrayExporter& i_Exporter,
							 const mraySceneData &i_SceneData);
};
