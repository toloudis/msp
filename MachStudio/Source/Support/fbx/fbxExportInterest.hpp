/****************************************************************************\
**	fbxExportInterest.hpp
**
**		A Export Interest is registered by a system that has data
**	to be exported to Maya ascii format.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef FBX_EXPORTINTEREST_HPP
#error fbxExportInterest.hpp multiply included
#endif
#define FBX_EXPORTINTEREST_HPP

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

#ifndef FBX_EXPORTDATA_HPP
#include "ImportExport/fbx/export/fbxExportData.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================
class fbxExporter;
class g3dSceneNode;

//============================================================================
//============================================================================
class fbxExportInterest
{
	public:
		//--------------------------------------------------------------------
		//  returns chunk description for display purposes
		//--------------------------------------------------------------------
		virtual const char* GetChunkDesc() const = 0;

		//--------------------------------------------------------------------
		// Gather data for objects that will be exported
		//--------------------------------------------------------------------
		virtual void GatherSceneData( fbxSceneData &o_Data , bool &o_SceneHasBeenBaked ) const = 0;

		//--------------------------------------------------------------------
		// Export()
		//--------------------------------------------------------------------
		virtual void Export( fbxExporter& i_Exporter, const fbxSceneData &i_SceneData ) = 0;
};
