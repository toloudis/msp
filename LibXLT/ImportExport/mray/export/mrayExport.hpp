/*****************************************************************************\
**	mrayExport.hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MRAY_EXPORT_HPP
#error mrayExport.hpp multiply included
#endif
#define MRAY_EXPORT_HPP

//============================================================================
//	Forward References
//============================================================================
struct mrayGlobalData;
struct mrayGeometryData;
struct mrayPointLightData;
struct mrayProjLightData;
class g3dSceneNode;
class g3dFragment;
class fsLocator;
class matMaterial;
struct g3dAmbientEnvState;


#include <fstream>

//============================================================================
//============================================================================
namespace mrayExport
{

	//--------------------------------------------------------------------
	// WriteWorldInclude()
	//--------------------------------------------------------------------
	void WriteWorldInclude(std::ofstream & o_OutFile, mrayGlobalData i_GlobalData);

	//--------------------------------------------------------------------
	// WriteSectionHeader()
	//--------------------------------------------------------------------
	void WriteSectionHeader(std::ostream & o_OutFile, std::string i_Header);

	//--------------------------------------------------------------------
	// WriteSectionHeader()
	//--------------------------------------------------------------------
	void WriteMaterial(std::ostringstream & o_OutFile, mrayGlobalData & io_GlobalData, std::string i_ObjName, std::string i_MatName, 
					   matMaterial* i_Material, g3dFragment* i_pFrag, g3dAmbientEnvState* i_AmbientData);

	//--------------------------------------------------------------------
	// BeginExport()
	//--------------------------------------------------------------------
	void BeginExport(std::ofstream & o_OutFile , 
					 mrayGlobalData & io_GlobalData);

	//--------------------------------------------------------------------
	// ExportPointLight()
	//--------------------------------------------------------------------
	void ExportPointLight(std::ofstream & o_OutFile,		 
						  mrayGlobalData & io_GlobalData,
						  mrayPointLightData i_PointLightData);

	//--------------------------------------------------------------------
	// ExportProjLight()
	//--------------------------------------------------------------------
	void ExportProjLight(std::ofstream & o_OutFile,  
						 mrayGlobalData & io_GlobalData,
						 mrayProjLightData i_ProjLightData);

	//--------------------------------------------------------------------
	// ExportMesh()
	//--------------------------------------------------------------------
	void ExportMesh(std::ofstream & o_OutFile, 
					mrayGlobalData & io_GlobalData,
					mrayGeometryData i_GeometryData, 
					const g3dSceneNode * i_pNode);

	//--------------------------------------------------------------------
	// EndExport()
	//--------------------------------------------------------------------
	void EndExport(std::ofstream & o_OutFile,		 
				   mrayGlobalData & io_GlobalData);


	//--------------------------------------------------------------------
	// Make a unique name for a material shader's texture parameter.
	//--------------------------------------------------------------------
	std::string GenerateTextureParamName(std::string i_ObjFragMatName, 
		std::string i_ParamName, bool i_IsMetaSL);
};
