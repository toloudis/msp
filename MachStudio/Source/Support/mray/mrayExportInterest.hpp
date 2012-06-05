/****************************************************************************\
**	mrayExportInterest.hpp
**
**		A Export Interest is registered by a system that has data
**	to be exported to Maya ascii format.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MRAY_EXPORTINTEREST_HPP
#error mrayExportInterest.hpp multiply included
#endif
#define MRAY_EXPORTINTEREST_HPP

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

#ifndef MRAY_EXPORTDATA_HPP
#include "ImportExport/mray/export/mrayExportData.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================
class mrayExporter;
class g3dSceneNode;

//============================================================================
//============================================================================
class mrayExportInterest
{
	public:
		//--------------------------------------------------------------------
		//  returns chunk description for display purposes
		//--------------------------------------------------------------------
		virtual const char* GetChunkDesc() const = 0;

		//--------------------------------------------------------------------
		// Gather data for objects that will be exported
		//--------------------------------------------------------------------
		virtual void GatherSceneData( mraySceneData &o_Data , mrayGlobalData &o_GlobalData ) const = 0;

		//--------------------------------------------------------------------
		// Export()
		//--------------------------------------------------------------------
		virtual void Export( mrayExporter& i_Exporter, const mraySceneData &i_SceneData ) = 0;
};
