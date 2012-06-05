/****************************************************************************\
**	rmanExportInterest.hpp
**
**		A Export Interest is registered by a system that has data
**	to be exported to Maya ascii format.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef RMAN_EXPORTINTEREST_HPP
#error rmanExportInterest.hpp multiply included
#endif
#define RMAN_EXPORTINTEREST_HPP

#include <string>
#include <vector>

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#ifndef RMAN_EXPORTDATA_HPP
#include "ImportExport/rman/export/rmanExportData.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================
class rmanExporter;
class g3dSceneNode;

//============================================================================
//============================================================================
class rmanExportInterest
{
	public:
		//--------------------------------------------------------------------
		//  returns chunk description for display purposes
		//--------------------------------------------------------------------
		virtual const char* GetChunkDesc() const = 0;

		//--------------------------------------------------------------------
		// Gather data for objects that will be exported
		//--------------------------------------------------------------------
		virtual void GatherSceneData( rmanSceneData &o_SceneData, rmanGlobalData &o_GlobalData )  const = 0;

		//--------------------------------------------------------------------
		//	Export only the items in the given list.
		//	The strings will be some subset of what was returned from the
		//	call to GatherItemNames.
		//--------------------------------------------------------------------
		virtual void Export( rmanExporter& i_Exporter, const rmanSceneData &i_SceneData ) = 0;
};
