/****************************************************************************\
**  hairImport.cpp
**
**      hairImport.cpp supplies functions used to import files from Maya
**	(written by our Maya plugin).
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/hair/hairImport.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/hair/HairReader.h"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/Mat/matShaderMgr.hpp"
#include "Graphics/mdl/mdlHairInfo.hpp"

#include <fstream>


//============================================================================
//============================================================================
using namespace std;


//============================================================================
//============================================================================
namespace hairImport
{

void LoadHair( const fsLocator& i_Locator,
			   mdlHairInfo &o_HairInfo,
			   mdlMatInfoTable& o_MaterialTable,
			   std::vector<matMaterial*>& o_Materials )
{
	ifstream file;
	string path;

	fsFileUtil::LocatorToANSIFilename( i_Locator, path );

	file.open( path.c_str(), ios_base::in );
	if( !file.is_open() )
	{
		DBG_WARNING("file " << path << " not found!" );
		return;
	}

	if( !HairReader::ReadData( file, o_HairInfo ) )
	{
		DBG_WARNING("LoadHair had errors!" );
	}

	boost::shared_ptr<mdlMatInfo> new_mat_info(new mdlMatInfo());
	new_mat_info->m_Info.SetMaterialName("Hair_Mat");
	o_MaterialTable[new_mat_info->m_Info.GetMaterialName()] = new_mat_info;
	new_mat_info->CreateMaterial();
	o_Materials.push_back(new_mat_info->m_pMaterial);

	fsLocator hair_shader_loc(itString("Hair_SH.fx"));
	matShaderEffect* eff = matShaderMgr::GetEffect(hair_shader_loc);
	if (eff)
	{
		boost::shared_ptr<effShaderParams> hair_params(new effShaderParams());
		hair_params->SetShaderName(hair_shader_loc, eff);
		eff->BuildPrtyObject(hair_params.get());
		new_mat_info->m_Info.SetShaderParams(hair_params);
	}

	o_HairInfo.m_Material = new_mat_info;

	file.close();
}

}	// end of namespace

