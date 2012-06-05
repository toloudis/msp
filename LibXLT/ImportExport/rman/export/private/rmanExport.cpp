/****************************************************************************\
**	rmanExport.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/rman/export/rmanExport.hpp"

#include "Tool/cam3d/cam3dUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maRotation.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/name/nameString.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effShaderParams.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dIndexPtr.hpp"
#include "Graphics/g3d/g3dProjectedLight.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"

#include "ImportExport/rman/export/private/rmanUtil.hpp"

#include <sstream>
#include <string>
#include <map>
#include <utility>
#include <vector>

using namespace std;

//--------------------------------------------------------------------
// WriteVector()
//--------------------------------------------------------------------
std::string WriteVector( std::string i_Name , maPoint3d i_Vec )
{
	std::ostringstream s;
	s << "\"vector " << i_Name << "\"" << " [ " << i_Vec.GetX() << " " 
														<< i_Vec.GetY() << " "
														<< i_Vec.GetZ() << " " 
														<< "] ";
	return s.str();
}

//--------------------------------------------------------------------
// WriteColor()
//--------------------------------------------------------------------
std::string WriteColor( std::string i_Name , maPoint3d i_Vec )
{
	std::ostringstream s;
	s << "\"color " << i_Name << "\"" << " [ " << i_Vec.GetX() << " " 
													   << i_Vec.GetY() << " "
													   << i_Vec.GetZ() << " " 
													   << "] ";
	return s.str();
}

//--------------------------------------------------------------------
// WritePoint()
//--------------------------------------------------------------------
std::string WritePoint( std::string i_Name , maPoint3d i_Vec )
{
	std::ostringstream s;
	s << "\"point " << i_Name << "\"" << " [ " << i_Vec.GetX() << " " 
													   << i_Vec.GetY() << " "
													   << i_Vec.GetZ() << " " 
													   << "] ";
	return s.str();
}

//--------------------------------------------------------------------
// WriteFloat()
//--------------------------------------------------------------------
std::string WriteFloat( std::string i_Name , float i_Float )
{
	std::ostringstream s;
	s << "\"float " << i_Name << "\"" << " [ " << i_Float << " ] ";
	return s.str();
}

//--------------------------------------------------------------------
// WriteInt()
//--------------------------------------------------------------------
std::string WriteInt( std::string i_Name , int i_Int )
{
	std::ostringstream s;
	s << "\"float " << i_Name << "\"" << " [ " << i_Int << " ] ";
	return s.str();
}

//--------------------------------------------------------------------
// WriteBool()
//--------------------------------------------------------------------
std::string WriteBool( std::string i_Name , bool i_Bool )
{
	std::ostringstream s;
	s << "\"float " << i_Name << "\"" << " [ " << (i_Bool?1:0) << " ] ";
	return s.str();
}

//--------------------------------------------------------------------
// WriteString()
//--------------------------------------------------------------------
std::string WriteString( std::string i_Name , std::string i_Str )
{
	std::ostringstream s;
	s << "\"string " << i_Name << "\"" << " [ \"" << i_Str << "\" ] ";
	return s.str();
}

//--------------------------------------------------------------------
// WriteMatrix() - put a matrix into a string
//--------------------------------------------------------------------
std::string WriteMatrix( maMatrix4x4 i_Mat )
{
	std::ostringstream s;
	s << "[ ";		
	for ( int i = 0 ; i < 16 ; i++ ){
		s << i_Mat.m_Mat[i] << " ";
	}
	s << "]";
	return s.str();
}

//--------------------------------------------------------------------
// WriteFloatMatrix() - put a matrix into a string
//--------------------------------------------------------------------
std::string WriteFloatMatrix( std::string i_BaseName, maMatrix4x4 i_Mat )
{
	std::ostringstream s;			
	for ( int i = 0 ; i < 16 ; i++ ){
		s << "\"float " << i_BaseName << "_Sub" << i << "\" [ " << i_Mat.m_Mat[i] << " ] ";
	}
	return s.str();
}

//--------------------------------------------------------------------
// ExportLightSets()
//--------------------------------------------------------------------
void ExportLightSets( rmanGlobalData & io_GlobalData, std::string i_ObjName , std::ostringstream& i_Stream )
{
	std::map< std::string , std::vector< std::string > >  objectLightMap = io_GlobalData.m_ObjectLightMap;
	std::vector< std::string > lightSetLights = io_GlobalData.m_LightSetLights;
	std::vector< std::string > activatedLights = objectLightMap[i_ObjName];
	std::vector< std::string > sceneLights = io_GlobalData.m_SceneLights;
	
	for ( int i = 0 ; i < sceneLights.size() ; i++ )
	{		
		if ( envSTLHelpers::Contains(lightSetLights, sceneLights[i] ) )
		{
			i_Stream << "Illuminate " << i << " " << (envSTLHelpers::Contains(activatedLights, sceneLights[i])==true?1:0) << endl;		
		}
		else
		{
			i_Stream << "Illuminate " << i << " " << 1 << endl;
		}
		
	}
}

//------------------------------------------------------------------------
//	Replace a parameter of an attribute
//------------------------------------------------------------------------
void replaceParam(std::string& i_FullStr, std::string i_OldStr, std::string i_NewStr)
{
	if ( strstr(i_FullStr.c_str(),i_OldStr.c_str()) != NULL )
	{
		int begin = i_FullStr.find(i_OldStr);
		int end = i_FullStr.find("] \"",begin);
		i_FullStr.replace(begin,end-begin+1,i_NewStr);
	}
}

//------------------------------------------------------------------------
//	Write out value of map if key != exclusion
//------------------------------------------------------------------------
void writeOutMap(std::string i_Exclude, std::map<std::string,std::string> i_Map, std::ofstream& i_Stream)
{
    for(std::map<std::string,std::string>::const_iterator it = i_Map.begin(); it != i_Map.end(); ++it)
    {
		// Writing out master rib
		if ( i_Exclude == "" )
		{
			i_Stream << it->second;
		}
		// Writing out reflection rib
		else
		{
			if ( strstr( (it->first).c_str() , i_Exclude.c_str() ) == NULL )
			{        
				// Remove cube & planar map dependencies
				std::string attributes = it->second;
				replaceParam(attributes,"\"string cubeMap\"","\"string cubeMap\" [ \"\" ]");
				replaceParam(attributes,"\"string planarSampler\"","\"string planarSampler\" [ \"\" ]");
				i_Stream << attributes;
			}
		}
    }
}

//--------------------------------------------------------------------
// replace_all() - replace all occurences of a char with another char
//--------------------------------------------------------------------
void replace_all(std::string& context, const std::string& from, const std::string& to) {
	size_t lookHere = 0;
	size_t foundHere;
	while( (foundHere = context.find(from, lookHere)) != std::string::npos ) {
		context.replace(foundHere, from.size(), to);
		lookHere = foundHere + to.size();
	}
} 

//--------------------------------------------------------------------
// remove_extension()
//--------------------------------------------------------------------
std::string remove_extension(std::string context) {
	int pos = context.rfind('.');
	return context.substr(0,pos);
} 

//--------------------------------------------------------------------
// get_filter_rib_form()
//--------------------------------------------------------------------
std::string get_bucket_order(int i_Val)
{
	std::string order = "\"spacefill\"";
	switch ( i_Val )
	{
		case 0:
			order = "\"spacefill\"";
			break;
		case 1:
			order = "\"horizontal\"";
			break;
		case 2:
			order = "\"vertical\"";
			break;
		case 3:
			order = "\"zigzag-x\"";
			break;
		case 4:
			order = "\"zigzag-y\"";
			break;
		case 5:
			order = "\"spiral\"";
			break;
		case 6:
			order = "\"random\"";
			break;
	}
	return order;
}

//--------------------------------------------------------------------
// get_filter_rib_form()
//--------------------------------------------------------------------
std::string get_filter_rib_form(int i_Val)
{
	std::string filter = "\"box\"";
	switch ( i_Val )
	{
		case 0:
			filter = "\"box\"";
			break;
		case 1:
			filter = "\"gaussian\"";
			break;
		case 2:
			filter = "\"mitchell\"";
			break;
		case 3:
			filter = "\"triangle\"";
			break;
		case 4:
			filter = "\"sinc\"";
			break;
		case 5:
			filter = "\"sinc\"";
			break;
		case 6:
			filter = "\"blackman-harris\"";
			break;
		case 7:
			filter = "\"catmull-rom\"";
			break;
	}
	return filter;
}

//--------------------------------------------------------------------
// set_display_info()
//--------------------------------------------------------------------
void set_display_info(int i_Val, std::string & o_ext, std::string & o_driver)
{
	// defaults
	o_ext = "";
	o_driver = "framebuffer";
	 
	switch ( i_Val )
	{
		case 0:
			o_ext = "";
			o_driver = "framebuffer";
			break;
		case 1:
			o_ext = ".tif";
			o_driver = "tiff";
			break;
		case 2:
			o_ext = ".tga";
			o_driver = "targa";
			break;
		case 3:
			o_ext = ".sgi";
			o_driver = "sgif";
			break;
		case 4:
			o_ext = ".cin";
			o_driver = "cineon";
			break;
		case 5:
			o_ext = ".pic";
			o_driver = "softimage";
			break;
		case 6:
			o_ext = ".iff";
			o_driver = "mayaiff";
			break;
		case 7:
			o_ext = ".exr";
			o_driver = "openexr";
			break;
		case 8:
			o_ext = ".alias";
			o_driver = "alias";
			break;
	}
}


//--------------------------------------------------------------------
// ExportAttributeList()
//--------------------------------------------------------------------
std::string ExportAttributeList( const std::string & i_ID, g3dFragment * i_pFrag )
{
	std::ostringstream s;

	// Export custom attributes
	if ( i_pFrag->GetMaterial()->GetHasRendermanOverride() && i_pFrag->GetMaterial()->GetRendermanOverrideData().m_bOverrideAttributes )
	{
		effRendermanOverrideData rmOverrideData = i_pFrag->GetMaterial()->GetRendermanOverrideData();
		std::string customAttributes = rmOverrideData.m_AttributeList;
		s << customAttributes << endl;
	}

	// Export MSP attributes
	else
	{
		s << "  Attribute \"identifier\" \"name\" [\"" << i_ID << "\"]" << endl;
		if ( i_pFrag->GetMaterial()->GetHasDisplacement() )
		{	
			float maxDisplacement = (abs(i_pFrag->GetMaterial()->GetDisplacementData().m_Scale) + 
									 abs(i_pFrag->GetMaterial()->GetDisplacementData().m_Bias) * 2 * 10 );
			s << "  Attribute \"displacementbound\" \"float sphere\" [" << maxDisplacement << "] \"coordinatesystem\" [\"shader\"]" << endl; 
			s << "  Attribute \"trace\" \"int displacements\" [1]" << endl;
		}
		s << "  Attribute \"visibility\" \"trace\" [1]" << endl;
		s << "  Attribute \"visibility\" \"int transmission\" [" << (i_pFrag->GetCastsShadow()?1:0) << "]" << endl;
		s << "  Attribute \"shade\" \"string transmissionhitmode\" [ \"shader\" ]" << endl;
		s << "  Attribute \"photon\" \"shadingmodel\" \"matte\"" << endl;

		//s << "  Attribute \"shade\" \"string diffusehitmode\" [\"primitive\"|\"shader\"]" << endl;
	} 

	return s.str();
}

//--------------------------------------------------------------------
// ExportOptions()
//--------------------------------------------------------------------
void ExportOptions(rmanGlobalData & io_GlobalData, std::ofstream & s )
{
	std::ostringstream bucketsize;
	bucketsize << io_GlobalData.m_Options.m_RmanBucketSize;
	std::ostringstream gridsize;
	gridsize << ((io_GlobalData.m_Options.m_RmanBucketSize*io_GlobalData.m_Options.m_RmanBucketSize)/io_GlobalData.m_Options.m_RmanShadingRate);

	s << "Option \"trace\" \"int maxdepth\" [" << io_GlobalData.m_Options.m_RmanRayDepth << "]" << endl;
	s << "Option \"limits\" \"int threads\" [" << io_GlobalData.m_Options.m_RmanNumCores << "]" << endl;
	s << "Option \"limits\" \"bucketsize\" [" << bucketsize.str() << " " << bucketsize.str() << "]" << endl;
	s << "Option \"limits\" \"gridsize\" [" << gridsize.str() << "]" << endl;
	s << "Option \"limits\" \"texturememory\" [" << io_GlobalData.m_Options.m_RmanTexMemory << "]" << endl;
	s << "Option \"bucket\" \"string order\" [" << get_bucket_order(io_GlobalData.m_Options.m_RmanBucketOrder) << "]" << endl;
}

//--------------------------------------------------------------------
//	GetSceneLightID()
//--------------------------------------------------------------------
int GetSceneLightID( std::string i_LightName, rmanGlobalData & io_GlobalData )
{
	for ( int i = 0 ; i < io_GlobalData.m_SceneLights.size() ; i++ )
	{
		if ( io_GlobalData.m_SceneLights[i] == i_LightName )
		{
			return i;
		}
	}
	return -1;	
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ExportShaderInfo(rmanGlobalData & io_GlobalData, 
								  std::ostringstream& currAttribute,
								  g3dFragment * i_pFrag,
								  bool i_bVisible,
								  g3dAmbientEnvState * i_AmbientData,
								  maMatrix4x4 i_Transform,
								  std::string i_ConstructedObjectName,
								  float& o_transparency)
{	
	std::string texName = "";
	std::string dispMapName = "";
	std::string normMapName = "";
	std::string envDiffMapName = "";
	std::string envSpecMapName = "";
	
	currAttribute << "  Displacement \"Tessellate\" ";
	
	effDisplacementData dispData = i_pFrag->GetMaterial()->GetDisplacementData();

	if ( dispData.m_NameDisplacementMap.GetNumNames() > 0 )
	{
		dispMapName = rmanUtil::LookupGeneratedTexName(i_ConstructedObjectName + dispMapEnd,io_GlobalData);
	}

	std::ostringstream fullDisplacementStr;
	fullDisplacementStr << "  Displacement \"Tessellate\" ";
	fullDisplacementStr << "\"float " << "g_DisplacementEnabled" << "\"" << " [ " << (i_pFrag->GetMaterial()->GetHasDisplacement()?1:0) << " ] ";
	fullDisplacementStr << "\"string " << "displacementMap" << "\"" << " [ \"" << dispMapName << "\" ] ";
	fullDisplacementStr << "\"float " << "g_displacementScale" << "\"" << " [ " << dispData.m_Scale << " ] ";
	fullDisplacementStr << "\"float " << "g_displacementBias" << "\"" << " [ " << dispData.m_Bias << " ] ";
	fullDisplacementStr << "\"float " << "g_displacementBlur" << "\"" << " [ " << dispData.m_Blur << " ] ";
	fullDisplacementStr << "\"float " << "g_ObjectUVScale" << "\"" << " [ " << dispData.m_ObjUVScale.GetX() << " ] ";
	currAttribute << WriteBool("g_DisplacementEnabled",i_pFrag->GetMaterial()->GetHasDisplacement());
	currAttribute << WriteString("displacementMap",dispMapName );
	currAttribute << WriteFloat("g_displacementScale",dispData.m_Scale);
	currAttribute << WriteFloat("g_displacementBias",dispData.m_Bias);
	currAttribute << WriteFloat("g_displacementBlur",dispData.m_Blur);
	currAttribute << WriteFloat("g_ObjectUVScale",dispData.m_ObjUVScale.GetX());	

	effUVTransform uvTransData = i_pFrag->GetMaterial()->GetUVTransform();
	std::ostringstream fullShadowSurfaceStr;
	fullShadowSurfaceStr << "  Surface \"ShadowMap\" ";
	fullShadowSurfaceStr << "\"float " << "UScale" << "\"" << " [ " << uvTransData.m_UScale << " ] ";
	fullShadowSurfaceStr << "\"float " << "VScale" << "\"" << " [ " << uvTransData.m_VScale << " ] ";
	fullShadowSurfaceStr << "\"float " << "UTrans" << "\"" << " [ " << uvTransData.m_UTrans << " ] ";
	fullShadowSurfaceStr << "\"float " << "VTrans" << "\"" << " [ " << uvTransData.m_VTrans << " ] ";
	fullShadowSurfaceStr << "\"float " << "UVAngle" << "\"" << " [ " << uvTransData.m_UVAngle << " ] ";
	currAttribute << WriteFloat("UScale",uvTransData.m_UScale);
	currAttribute << WriteFloat("VScale",uvTransData.m_VScale);
	currAttribute << WriteFloat("UTrans",uvTransData.m_UTrans);
	currAttribute << WriteFloat("VTrans",uvTransData.m_VTrans);
	currAttribute << WriteFloat("UVAngle",uvTransData.m_UVAngle);

	currAttribute << endl;

	if ( i_pFrag->GetMaterial()->GetHasRendermanOverride() && i_pFrag->GetMaterial()->GetRendermanOverrideData().m_bOverrideShader )
	{
		effRendermanOverrideData rmOverrideData = i_pFrag->GetMaterial()->GetRendermanOverrideData();
		fsLocator cusotmShaderLoc = rmOverrideData.m_ShaderLocation;
		std::string shaderParams = rmOverrideData.m_ParamList;
		std::string customShaderPathStr;
		fsFileUtil::LocatorToANSIFilename(cusotmShaderLoc,customShaderPathStr);
		customShaderPathStr = remove_extension(customShaderPathStr);
		replace_all(customShaderPathStr, "\\", "/");

		currAttribute << "  Surface \"" << customShaderPathStr << "\"" << shaderParams << endl;	
	}
	else
	{

		if ( io_GlobalData.m_bRenderingNormalsOnly )
		{
			//currAttribute << "  Orientation \"rh\"" << endl;
			currAttribute << "  Surface \"Normals\"" << endl;
			effNormalsData normalsData = i_pFrag->GetMaterial()->GetNormalsData();
			if ( normalsData.m_NameNormalMap.GetNumNames() > 0 )
			{
				normMapName = rmanUtil::LookupGeneratedTexName(i_ConstructedObjectName + normMapEnd,io_GlobalData);
			}
			currAttribute << WriteString("normalMap",normMapName );
			currAttribute << WriteFloat("normalMapScale",normalsData.m_BumpScale);
			currAttribute << WriteBool("g_DisplacementEnabled",i_pFrag->GetMaterial()->GetHasDisplacement());
			currAttribute << WriteFloat("g_DisplacementNormalScale",dispData.m_ObjUVScale.GetX());
			currAttribute << WriteFloat("UScale",uvTransData.m_UScale);
			currAttribute << WriteFloat("VScale",uvTransData.m_VScale);
			currAttribute << WriteFloat("UTrans",uvTransData.m_UTrans);
			currAttribute << WriteFloat("VTrans",uvTransData.m_VTrans);
			currAttribute << WriteFloat("UVAngle",uvTransData.m_UVAngle);
			maMatrix4x4 camMat;
			camCamera* cam = io_GlobalData.m_SceneCamera;
			cam->GetCameraMatrix(camMat);		
			currAttribute << WriteFloatMatrix("camMat",camMat);
			currAttribute << endl;
		}
		else if ( io_GlobalData.m_bRenderingIlluminationOnly )
		{
			currAttribute << "  Surface \"Illumination\"";

			effNormalsData normalsData = i_pFrag->GetMaterial()->GetNormalsData();

			if ( normalsData.m_NameNormalMap.GetNumNames() > 0 )
			{
				normMapName = rmanUtil::LookupGeneratedTexName(i_ConstructedObjectName + normMapEnd,io_GlobalData);
			}
			currAttribute << WriteString("normalMap",normMapName );
			currAttribute << WriteFloat("normalMapScale",normalsData.m_BumpScale);
			currAttribute << WriteBool("g_DisplacementEnabled",i_pFrag->GetMaterial()->GetHasDisplacement());
			currAttribute << WriteFloat("g_DisplacementNormalScale",dispData.m_ObjUVScale.GetX());
			currAttribute << WriteFloat("UScale",uvTransData.m_UScale);
			currAttribute << WriteFloat("VScale",uvTransData.m_VScale);
			currAttribute << WriteFloat("UTrans",uvTransData.m_UTrans);
			currAttribute << WriteFloat("VTrans",uvTransData.m_VTrans);
			currAttribute << WriteFloat("UVAngle",uvTransData.m_UVAngle);	
			currAttribute << WriteBool("bReceivesShadow", i_pFrag->GetReceivesShadow() );
			currAttribute << WriteBool("bRenderTransparent",io_GlobalData.m_bRenderTransparent);
			currAttribute << endl;

			std::vector<effParamFloat*> floatParams;
			i_pFrag->GetMaterial()->GetMaterialLayer(0)->GetShaderParams()->GetAllFloatParams(floatParams);
			for ( int i = 0 ; i < floatParams.size() ; i++ )
			{
				effParamFloat* currParam = floatParams[i];
				if ( currParam->GetName() == "g_transparency" )
				{
					fullShadowSurfaceStr << "\"float " << "g_transparency" << "\"" << " [ " << currParam->GetProperty().GetValue() << " ] ";
					currAttribute << WriteFloat( currParam->GetName() , currParam->GetProperty().GetValue() );
				}
			}
			std::vector<effParamTexture*> textureParams;

			i_pFrag->GetMaterial()->GetMaterialLayer(0)->GetShaderParams()->GetAllTextureParams(textureParams);
			for ( int i = 0 ; i < textureParams.size() ; i++ )
			{
				effParamTexture* currParam = textureParams[i];	
				std::string currParamName = currParam->GetName();

				if ( currParamName == "transparencyMap" )
				{			
					if ( currParam->GetRampTexture() || currParam->GetProperty().GetValue().GetNumNames() > 0 )
					{
						texName = rmanUtil::LookupGeneratedTexName(i_ConstructedObjectName + "-" + currParamName,io_GlobalData);
						fullShadowSurfaceStr << "\"string " << "transMap" << "\"" << " [ \"" << texName << "\" ] ";
						currAttribute << WriteString( currParamName , texName );
					}
				}
			}

		}
		else if ( io_GlobalData.m_bRenderingShadowsOnly )
		{
			effNormalsData normalsData = i_pFrag->GetMaterial()->GetNormalsData();
			if ( normalsData.m_NameNormalMap.GetNumNames() > 0 )
			{
				normMapName = rmanUtil::LookupGeneratedTexName(i_ConstructedObjectName + normMapEnd,io_GlobalData);
			}
			currAttribute << "  Surface \"Shadows\"";
			currAttribute << WriteString("normalMap",normMapName );
			currAttribute << WriteFloat("normalMapScale",normalsData.m_BumpScale);
			currAttribute << WriteBool("g_DisplacementEnabled",i_pFrag->GetMaterial()->GetHasDisplacement());
			currAttribute << WriteFloat("g_DisplacementNormalScale",dispData.m_ObjUVScale.GetX());
			currAttribute << WriteFloat("UScale",uvTransData.m_UScale);
			currAttribute << WriteFloat("VScale",uvTransData.m_VScale);
			currAttribute << WriteFloat("UTrans",uvTransData.m_UTrans);
			currAttribute << WriteFloat("VTrans",uvTransData.m_VTrans);
			currAttribute << WriteFloat("UVAngle",uvTransData.m_UVAngle);
			currAttribute << WriteBool("bReceivesShadow", i_pFrag->GetReceivesShadow() );
			currAttribute << WriteBool("bRenderTransparent",io_GlobalData.m_bRenderTransparent);
			currAttribute << endl;

			std::vector<effParamFloat*> floatParams;
			i_pFrag->GetMaterial()->GetMaterialLayer(0)->GetShaderParams()->GetAllFloatParams(floatParams);
			for ( int i = 0 ; i < floatParams.size() ; i++ )
			{
				effParamFloat* currParam = floatParams[i];
				if ( currParam->GetName() == "g_transparency" )
				{
					fullShadowSurfaceStr << "\"float " << "g_transparency" << "\"" << " [ " << currParam->GetProperty().GetValue() << " ] ";
					currAttribute << WriteFloat( currParam->GetName() , currParam->GetProperty().GetValue() );
				}
			}
			std::vector<effParamTexture*> textureParams;

			i_pFrag->GetMaterial()->GetMaterialLayer(0)->GetShaderParams()->GetAllTextureParams(textureParams);
			for ( int i = 0 ; i < textureParams.size() ; i++ )
			{
				effParamTexture* currParam = textureParams[i];	
				std::string currParamName = currParam->GetName();

				if ( currParamName == "transparencyMap" )
				{			
					if ( currParam->GetRampTexture() || currParam->GetProperty().GetValue().GetNumNames() > 0 )
					{
						texName = rmanUtil::LookupGeneratedTexName(i_ConstructedObjectName + "-" + currParamName,io_GlobalData);
						fullShadowSurfaceStr << "\"string " << "transMap" << "\"" << " [ \"" << texName << "\" ] ";
						currAttribute << WriteString( currParamName , texName );
					}
				}
			}

		}
		else if ( io_GlobalData.m_bRenderingAOOnly )
		{
			currAttribute << "  Surface \"AO\"" << endl;
			effNormalsData normalsData = i_pFrag->GetMaterial()->GetNormalsData();
			if ( normalsData.m_NameNormalMap.GetNumNames() > 0 )
			{
				normMapName = rmanUtil::LookupGeneratedTexName(i_ConstructedObjectName + normMapEnd,io_GlobalData);
			}
			currAttribute << WriteString("normalMap",normMapName );
			currAttribute << WriteFloat("normalMapScale",normalsData.m_BumpScale);
			currAttribute << WriteBool("g_DisplacementEnabled",i_pFrag->GetMaterial()->GetHasDisplacement());
			currAttribute << WriteFloat("g_DisplacementNormalScale",dispData.m_ObjUVScale.GetX());
			currAttribute << WriteFloat("UScale",uvTransData.m_UScale);
			currAttribute << WriteFloat("VScale",uvTransData.m_VScale);
			currAttribute << WriteFloat("UTrans",uvTransData.m_UTrans);
			currAttribute << WriteFloat("VTrans",uvTransData.m_VTrans);
			currAttribute << WriteFloat("UVAngle",uvTransData.m_UVAngle);
			currAttribute << WriteBool("bRenderTransparent",io_GlobalData.m_bRenderTransparent);
			currAttribute << WriteBool("bRenderAO",i_pFrag->GetReceivesOcclusion());
			currAttribute << WriteInt("AOSamples",io_GlobalData.m_Options.m_RmanAOsamples);
			currAttribute << WriteFloat("AOMaxVariation", ((100-io_GlobalData.m_Options.m_RmanAOMaxVariation)/1000) );

			currAttribute << WriteColor("AOColor", io_GlobalData.m_AOData.color );
			currAttribute << WriteFloat("AORadiusNear", io_GlobalData.m_AOData.radiusNear );
			currAttribute << WriteFloat("AORadiusFar", io_GlobalData.m_AOData.radiusFar );
			currAttribute << WriteFloat("AOAngleBias", io_GlobalData.m_AOData.angleBias );
			currAttribute << WriteFloat("AOAttenuation", io_GlobalData.m_AOData.attenuation );
			currAttribute << WriteFloat("AOContrast", io_GlobalData.m_AOData.contrast );
			//currAttribute << WriteFloat("AOBlurWidth", aoData.blurWidth );
			//currAttribute << WriteFloat("AOBlurSharpness", aoData.blurSharpness );
			//currAttribute << WriteInt("AOOverscanPixels", aoData.overscanPixels );	
			currAttribute << WriteFloat("camNearClip",  io_GlobalData.m_SceneCamera->GetNearClip() );
			currAttribute << WriteFloat("camFarClip",  io_GlobalData.m_SceneCamera->GetFarClip() );

			std::vector<effParamFloat*> floatParams;
			i_pFrag->GetMaterial()->GetMaterialLayer(0)->GetShaderParams()->GetAllFloatParams(floatParams);
			for ( int i = 0 ; i < floatParams.size() ; i++ )
			{
				effParamFloat* currParam = floatParams[i];
				if ( currParam->GetName() == "g_transparency" )
				{
					currAttribute << WriteFloat( currParam->GetName() , currParam->GetProperty().GetValue() );
				}
			}
			std::vector<effParamTexture*> textureParams;

			i_pFrag->GetMaterial()->GetMaterialLayer(0)->GetShaderParams()->GetAllTextureParams(textureParams);
			for ( int i = 0 ; i < textureParams.size() ; i++ )
			{
				effParamTexture* currParam = textureParams[i];	
				std::string currParamName = currParam->GetName();

				if ( currParamName == "transparencyMap" )
				{			
					if ( currParam->GetRampTexture() || currParam->GetProperty().GetValue().GetNumNames() > 0 )
					{
						texName = rmanUtil::LookupGeneratedTexName(i_ConstructedObjectName + "-" + currParamName,io_GlobalData);
						currAttribute << WriteString( currParamName , texName );
					}
				}
			}

			currAttribute << endl;
		}
	
		// Beauty pass
		else 
		{
			fsLocator shader = fsLocator( i_pFrag->GetMaterial()->GetMaterialLayer(0)->GetShaderParams()->GetShaderName() );
			std::string shaderName;
			fsFileUtil::LocatorToANSIFilename(shader.GetLastName(),shaderName,false);
			shaderName = shaderName.substr(0,shaderName.length()-3);
			currAttribute << "  Surface \"" << shaderName << "\" " ;
			
			std::vector<std::string> tempNames;
			std::vector<std::string> tempVals;
			
			std::vector<effParamTexture*> textureParams;
			i_pFrag->GetMaterial()->GetMaterialLayer(0)->GetShaderParams()->GetAllTextureParams(textureParams);
			for ( int i = 0 ; i < textureParams.size() ; i++ )
			{
				effParamTexture* currParam = textureParams[i];	
				std::string currParamName = currParam->GetName();				

				if ( currParam->GetRampTexture() || currParam->GetProperty().GetValue().GetNumNames() > 0 )
				{
					texName = rmanUtil::LookupGeneratedTexName(i_ConstructedObjectName + "-" + currParamName,io_GlobalData);

					currAttribute << WriteString( currParam->GetName() , texName );	
					if ( currParamName == "transparencyMap" )
					{
						fullShadowSurfaceStr << "\"string " << "transMap" << "\"" << " [ \"" << texName << "\" ] ";
					}				
				}
			}
			
			std::vector<effParamColor*> colorParams;
			i_pFrag->GetMaterial()->GetMaterialLayer(0)->GetShaderParams()->GetAllColorParams(colorParams);
			for ( int i = 0 ; i < colorParams.size() ; i++ )
			{
				effParamColor* currParam = colorParams[i];

				maPoint3d currColor = maPoint3d(currParam->GetProperty().GetValue().GetRed(),
												currParam->GetProperty().GetValue().GetGreen(),
												currParam->GetProperty().GetValue().GetBlue());

				currAttribute << WriteColor(currParam->GetName(),currColor);
			}

			std::vector<effParamFloat*> floatParams;
			i_pFrag->GetMaterial()->GetMaterialLayer(0)->GetShaderParams()->GetAllFloatParams(floatParams);
			for ( int i = 0 ; i < floatParams.size() ; i++ )
			{
				effParamFloat* currParam = floatParams[i];
				currAttribute << WriteFloat( currParam->GetName() , currParam->GetProperty().GetValue() );
				if ( currParam->GetName() == "g_transparency" )
				{
					fullShadowSurfaceStr << "\"float " << "g_transparency" << "\"" << " [ " << currParam->GetProperty().GetValue() << " ] ";
				}
			}

			std::vector<effParamBool*> boolParams;
			i_pFrag->GetMaterial()->GetMaterialLayer(0)->GetShaderParams()->GetAllBoolParams(boolParams);
			for ( int i = 0 ; i < boolParams.size() ; i++ )
			{
				effParamBool* currParam = boolParams[i];
				currAttribute << WriteBool( currParam->GetName() , currParam->GetProperty().GetValue() );
			}

			std::vector<effParamInt*> intParams;
			i_pFrag->GetMaterial()->GetMaterialLayer(0)->GetShaderParams()->GetAllIntParams(intParams);
			for ( int i = 0 ; i < intParams.size() ; i++ )
			{
				effParamInt* currParam = intParams[i];
				currAttribute << WriteInt( currParam->GetName() , currParam->GetProperty().GetValue() );
			}

			maPoint3d diffuseColor = maPoint3d(i_AmbientData->m_DiffuseColor.GetRed(),
											   i_AmbientData->m_DiffuseColor.GetGreen(),
											   i_AmbientData->m_DiffuseColor.GetBlue());

			maPoint3d specularColor = maPoint3d(i_AmbientData->m_SpecularColor.GetRed(),
												i_AmbientData->m_SpecularColor.GetGreen(),
												i_AmbientData->m_SpecularColor.GetBlue());
			
			if ( i_AmbientData->m_DiffuseMap )
			{
				envDiffMapName = rmanUtil::LookupGeneratedTexName(i_AmbientData->m_Name + envDiffMapEnd,io_GlobalData);
			}
			if ( i_AmbientData->m_SpecularMap )
			{
				envSpecMapName = rmanUtil::LookupGeneratedTexName(i_AmbientData->m_Name + envSpecMapEnd,io_GlobalData);
			}

			currAttribute << WriteString("diffuseEnvMap",envDiffMapName);
			currAttribute << WriteFloat("g_diffuseFactor",i_AmbientData->m_DiffuseFactor);
			currAttribute << WriteFloat("g_diffuseEnvAngle",i_AmbientData->m_DiffuseAngle);
			currAttribute << WriteColor("g_envDiffuseColor",diffuseColor);

			currAttribute << WriteString("specularEnvMap",envSpecMapName);
			currAttribute << WriteFloat("g_specularFactor",i_AmbientData->m_SpecularFactor);
			currAttribute << WriteFloat("g_specularEnvAngle",i_AmbientData->m_SpecularAngle);
			currAttribute << WriteColor("g_envSpecularColor",specularColor);

			currAttribute << WriteFloat("UScale",uvTransData.m_UScale);
			currAttribute << WriteFloat("VScale",uvTransData.m_VScale);
			currAttribute << WriteFloat("UTrans",uvTransData.m_UTrans);
			currAttribute << WriteFloat("VTrans",uvTransData.m_VTrans);
			currAttribute << WriteFloat("UVAngle",uvTransData.m_UVAngle);

			effNormalsData normalsData = i_pFrag->GetMaterial()->GetNormalsData();

			if ( normalsData.m_NameNormalMap.GetNumNames() > 0 )
			{
				normMapName = rmanUtil::LookupGeneratedTexName(i_ConstructedObjectName + normMapEnd,io_GlobalData);
			}

			currAttribute << WriteString("normalMap",normMapName );
			currAttribute << WriteFloat("normalMapScale",normalsData.m_BumpScale);

			currAttribute << WriteBool("g_DisplacementEnabled",i_pFrag->GetMaterial()->GetHasDisplacement());
			currAttribute << WriteFloat("g_DisplacementNormalScale",dispData.m_ObjUVScale.GetX());

			currAttribute << WriteBool("bRenderEnvironments",io_GlobalData.m_bRenderEnvironments);
			currAttribute << WriteBool("bRenderLit",io_GlobalData.m_bRenderLit);
			currAttribute << WriteBool("bRenderDiffuse",io_GlobalData.m_bRenderDiffuse);
			currAttribute << WriteBool("bRenderSpecular",io_GlobalData.m_bRenderSpecular);
			currAttribute << WriteBool("bRenderTransparent",io_GlobalData.m_bRenderTransparent);

			currAttribute << WriteBool("bRenderAO",io_GlobalData.m_Options.m_bRmanAOEnable & i_pFrag->GetReceivesOcclusion());
			currAttribute << WriteInt("AOSamples",io_GlobalData.m_Options.m_RmanAOsamples);	
			currAttribute << WriteFloat("AOMaxVariation", ((100-io_GlobalData.m_Options.m_RmanAOMaxVariation)/1000) );

			currAttribute << WriteColor("AOColor", io_GlobalData.m_AOData.color );
			currAttribute << WriteFloat("AORadiusNear", io_GlobalData.m_AOData.radiusNear );
			currAttribute << WriteFloat("AORadiusFar", io_GlobalData.m_AOData.radiusFar );
			currAttribute << WriteFloat("AOAngleBias", io_GlobalData.m_AOData.angleBias );
			currAttribute << WriteFloat("AOAttenuation", io_GlobalData.m_AOData.attenuation );
			currAttribute << WriteFloat("AOContrast", io_GlobalData.m_AOData.contrast );
			//currAttribute << WriteFloat("AOBlurWidth", aoData.blurWidth );
			//currAttribute << WriteFloat("AOBlurSharpness", aoData.blurSharpness );
			//currAttribute << WriteInt("AOOverscanPixels", aoData.overscanPixels );
			currAttribute << WriteFloat("camNearClip",  io_GlobalData.m_SceneCamera->GetNearClip() );
			currAttribute << WriteFloat("camFarClip",  io_GlobalData.m_SceneCamera->GetFarClip() );

			currAttribute << WriteBool("bRenderGI",( io_GlobalData.m_bRenderingGIOnly || io_GlobalData.m_Options.m_bRmanGIEnable ) && i_pFrag->GetReceivesGI() );
			currAttribute << WriteInt("GISamples",io_GlobalData.m_Options.m_RmanGIsamples);	
			currAttribute << WriteFloat("GIMaxVariation", ((100-io_GlobalData.m_Options.m_RmanGIMaxVariation)/1000) );

			currAttribute << WriteColor("GIColor", io_GlobalData.m_GIData.color );
			currAttribute << WriteFloat("GIRadiusNear", io_GlobalData.m_GIData.radiusNear );
			currAttribute << WriteFloat("GIRadiusFar", io_GlobalData.m_GIData.radiusFar );
			currAttribute << WriteFloat("GIAngleBias", io_GlobalData.m_GIData.angleBias );
			currAttribute << WriteFloat("GIAttenuation", io_GlobalData.m_GIData.attenuation );
			currAttribute << WriteFloat("GIContrast", io_GlobalData.m_GIData.contrast );
			//currAttribute << WriteFloat("GIBlurWidth", giData.blurWidth );
			//currAttribute << WriteFloat("GIBlurSharpness", giData.blurSharpness );
			//currAttribute << WriteInt("GIOverscanPixels", giData.overscanPixels );

			effReflectionMap reflData = i_pFrag->GetMaterial()->GetReflectionData();	
			currAttribute << WriteBool("g_isPlanar",reflData.m_bIsPlanar);
			currAttribute << WriteBool("dynamicReflection",reflData.m_bAutoGenEnvMap);

			if ( shaderName == "PhongReflection" || shaderName == "BlinnReflection" )
			{
				if ( (io_GlobalData.m_Options.m_bRmanReflEnable || io_GlobalData.m_bRenderingReflectionsOnly) && 
					 io_GlobalData.m_Options.m_RmanReflType == 0 )
				{
					io_GlobalData.m_ReflObjPositions.push_back( i_pFrag->GetBoundingBox().GetCenter() );
					io_GlobalData.m_ReflObjTransforms.push_back( i_Transform );
					io_GlobalData.m_ReflObjFragments.push_back( i_pFrag );
					io_GlobalData.m_ReflObjNames.push_back( i_ConstructedObjectName );
					io_GlobalData.m_ReflObjIsPlanar.push_back( reflData.m_bIsPlanar );
					io_GlobalData.m_ReflCubemapResolutions.push_back( reflData.m_ReflMapResolution );
				}
				currAttribute << WriteString("cubeMap",i_ConstructedObjectName+"_CUBEREFLECTIONMAP.tex");
				currAttribute << WriteString("planarSampler",i_ConstructedObjectName+"_PLANARREFLECTIONMAP.tex");
				currAttribute << WriteInt("reflType", io_GlobalData.m_Options.m_RmanReflType );
				currAttribute << WriteBool("reflEnable", (io_GlobalData.m_bRenderingBeautyOnly && io_GlobalData.m_Options.m_bRmanReflEnable)
														  || io_GlobalData.m_bRenderingReflectionsOnly );
			}

			currAttribute << WriteBool("g_IsolateReflection", io_GlobalData.m_bRenderingReflectionsOnly );
			currAttribute << WriteBool("bReceivesShadow", i_pFrag->GetReceivesShadow() );
			currAttribute << WriteBool("objVisible", ( i_bVisible && (!i_pFrag->IsShadowHull()) ));

			currAttribute << endl;

			fullShadowSurfaceStr << "\"float " << "bRenderTransparent" << "\"" << " [ " << io_GlobalData.m_bRenderTransparent << " ] ";
			fullShadowSurfaceStr << "\"float " << "objVisible" << "\"" << " [ " << i_bVisible << " ] ";
			fullShadowSurfaceStr << "\"float " << "castsShadow" << "\"" << " [ " << i_pFrag->GetCastsShadow() << " ] ";

		}
	}

	io_GlobalData.m_ShadowMapAttributes.push_back( fullDisplacementStr.str() );
	io_GlobalData.m_ShadowMapAttributes.push_back( fullShadowSurfaceStr.str() );
}

//--------------------------------------------------------------------
// ExportPolygonMesh()
//--------------------------------------------------------------------
void rmanExport::ExportPolygonMesh(rmanGlobalData & io_GlobalData, rmanGeometryData i_GeometryData , const g3dSceneNode * i_pNode)
{
	g3dFragment * currFrag = (g3dFragment*)i_pNode->GetFragment();

	if ( currFrag && currFrag->GetFragmentName() != "" )
	{
		// Create archive file
		std::string modName = i_GeometryData.m_Name;
		if ( i_GeometryData.m_SubName != "" )
			modName += "-" + i_GeometryData.m_SubName;
		std::string fragName = currFrag->GetFragmentName();
		std::string matName = currFrag->GetMaterial()->GetName();
		std::string objName = i_GeometryData.m_Name + "-" + fragName + "-" + matName;

		if ( !envSTLHelpers::Contains(io_GlobalData.m_ExportedObjects,objName) )
		{

			std::string archiveName = objName + std::string(".rib");
			fsLocator loc = io_GlobalData.m_ArchivesLoc;
			loc.Push(archiveName.c_str());
			itString itFileName;
			fsFileUtil::LocatorToUnicodeString( loc, itFileName );

			std::ostringstream currAttribute;

			std::ostringstream sidedness;
			sidedness << "Sides " << (currFrag->GetDoubleSided()?2:1);
			currAttribute << sidedness.str() << endl;
			io_GlobalData.m_ShadowMapAttributes.push_back( sidedness.str() );

			ExportLightSets( io_GlobalData, objName, currAttribute );
			currAttribute << "AttributeBegin" << endl;
			io_GlobalData.m_ShadowMapAttributes.push_back( "AttributeBegin" );

			// Object name		
			currAttribute << ExportAttributeList( objName , currFrag );

			// Transform
			std::string transformString = WriteMatrix( i_pNode->GetTotalTransform() );
			currAttribute << "  ConcatTransform " << transformString << endl;
			io_GlobalData.m_ShadowMapAttributes.push_back( "  ConcatTransform " + transformString );

			// Shader
			float transparency = 1;
			ExportShaderInfo(io_GlobalData, currAttribute, currFrag, i_GeometryData.m_bVisible,
							 i_GeometryData.m_AmbientData, i_pNode->GetTotalTransform(), objName, transparency);

			currAttribute << "  ReadArchive \"" << archiveName << "\"" << endl;
			
			if ( transparency > 0 )
			{
				io_GlobalData.m_ShadowMapAttributes.push_back( "  ReadArchive \"" + archiveName + "\"" );
			}

			if ( !fsFileUtil::FileExists( fsLocator(itFileName) ) || 
				  io_GlobalData.m_Options.m_bRmanRewriteAssets )
			{
				std::vector<int> faces;
				std::vector<int> indices;
				std::vector<maPoint3d> vertexParamList;
				std::vector<maPoint3d> normalParamList;
				std::vector<maPoint2d> uvParamList;

				int numFaces = currFrag->GetNumIndices() / 3;

				for ( int i = 0 ; i < numFaces ; i++ )
				{
					faces.push_back( 3 );			
				}

				// if there's a sys mem copy of the vtx or index buffer, we should use it!
				BYTE* pIndexBuffer = currFrag->ReadOnlyLockIndices();

				int maxIndexBufferVal = 0;

				// need to know whether 16 or 32 bit indices.
				bool indexSize32 = g3dIndexPtr::Needs32Bit( currFrag->GetNumVertices() );
				if (indexSize32)
				{
					envType::UInt32 *buffer = reinterpret_cast<envType::UInt32*>(pIndexBuffer);
					for ( int i = 0 ; i < currFrag->GetNumIndices() ; i++ )
					{
						int currIdx = buffer[i];
						indices.push_back( currIdx );
						if ( currIdx >= maxIndexBufferVal )
							maxIndexBufferVal = currIdx;
					}
				}
				else
				{
					envType::UInt16 *buffer = reinterpret_cast<envType::UInt16*>(pIndexBuffer);
					for ( int i = 0 ; i < currFrag->GetNumIndices() ; i++ )
					{
						int currIdx = buffer[i];
						indices.push_back( currIdx );
						if ( currIdx >= maxIndexBufferVal )
							maxIndexBufferVal = currIdx;
					}
				}
				// done with buffer.
				currFrag->ReadOnlyUnlockIndices();

				// now get the vertices.
				BYTE* pVertexBuffer = currFrag->ReadOnlyLock();
				g3dType::BumpTex1Vertex* vBuffer = reinterpret_cast<g3dType::BumpTex1Vertex*>(pVertexBuffer);
				for ( int i = 0 ; i <= maxIndexBufferVal ; i++ )
				{
					vertexParamList.push_back( vBuffer[ i ].m_Vertex );
					normalParamList.push_back( vBuffer[ i ].m_Normal );
					uvParamList.push_back( vBuffer[ i ].m_TexCoord );
				}

				// done with buffer.
				currFrag->ReadOnlyUnlock();

				std::ofstream archiveFile( itFileName.GetString() , std::ios::out );

				archiveFile << "#******************************************************************" << endl;
				archiveFile << "# RenderMan Interface Bytestream generated by MachStudio - GEOMETRY"  << endl; 
				archiveFile << "#******************************************************************" << endl << endl;
			
				if ( i_GeometryData.m_NumSubdivs > 0 )
					archiveFile << "  SubdivisionMesh \"catmull-clark\"" << endl;
				else
					archiveFile << "  PointsPolygons" << endl;

				// Faces
				archiveFile << "    [ ";
				for ( int i = 0 ; i < faces.size() ; i++ )
				{
					archiveFile << faces[i] << " ";
				}
				archiveFile << "]" << endl;

				// Indices
				archiveFile << "    [ ";
				for ( int i = 0 ; i < indices.size() ; i++ )
				{
					archiveFile << indices[i] << " ";
				}
				archiveFile << "]" << endl;

				if ( i_GeometryData.m_NumSubdivs > 0 )
					archiveFile << "    [\"interpolateboundary\"] [0 0] [] []" << endl;

				// Vertices (params)
				archiveFile << "    \"P\" [ ";
				for ( int i = 0 ; i < vertexParamList.size() ; i++ )
				{
					archiveFile << vertexParamList[i][0] << " ";
					archiveFile << vertexParamList[i][1] << " ";
					archiveFile << vertexParamList[i][2] << " ";		
				}
				archiveFile << "]" << endl;

				if ( i_GeometryData.m_NumSubdivs == 0 )
				{
					// Normals (params)
					archiveFile << "    \"N\" [ ";
					for ( int i = 0 ; i < normalParamList.size() ; i++ )
					{
						archiveFile << normalParamList[i][0] << " ";
						archiveFile << normalParamList[i][1] << " ";
						archiveFile << normalParamList[i][2] << " ";		
					}
					archiveFile << "]" << endl;
				}
				
				// UVs (params)
				archiveFile << "    \"st\" [ ";
				for ( int i = 0 ; i < uvParamList.size() ; i++ )
				{
					archiveFile << uvParamList[i][0] << " ";
					archiveFile << uvParamList[i][1] << " ";	
				}
				archiveFile << "]" << endl;	

				archiveFile.close();
			}

			currAttribute << "AttributeEnd" << endl << endl;

			io_GlobalData.m_MasterAttributeMap.insert( std::pair<std::string,std::string>(objName,currAttribute.str()));
			io_GlobalData.m_ShadowMapAttributes.push_back( "AttributeEnd" );
			io_GlobalData.m_ExportedObjects.push_back( objName );
		}

	}

	const int num_kids = i_pNode->GetNumChildren();
	for (int i=0; i<num_kids; i++)
	{
		ExportPolygonMesh(io_GlobalData, i_GeometryData,i_pNode->GetChild(i));
	}

}

//--------------------------------------------------------------------
// ExportProjLight()
//--------------------------------------------------------------------
void rmanExport::ExportProjLight(rmanGlobalData & io_GlobalData, rmanProjLightData i_ProjLightData)
{
	std::ostringstream s;

	s << "TransformBegin" << endl;
	maMatrix4x4 inverted = i_ProjLightData.m_CameraTransform;
	inverted.Invert();
	s << "  ConcatTransform " << WriteMatrix(inverted) << endl;

	s << "  LightSource" << " ";
	s << "\"ProjLight\"" << " " << GetSceneLightID(i_ProjLightData.m_Name, io_GlobalData) << " ";

	s << WriteFloat("intensity",i_ProjLightData.m_Intensity);
	s << WriteColor("lightColor",i_ProjLightData.m_Color);
	s << WriteFloat("falloff_x",i_ProjLightData.m_Falloff.GetX());
	s << WriteFloat("falloff_y",i_ProjLightData.m_Falloff.GetY());
	s << WriteFloat("falloff_z",i_ProjLightData.m_Falloff.GetZ());
	//s << WriteFloat("falloff_w",i_ProjLightData.m_Falloff.GetW());
	s << WriteFloat("falloffStart",i_ProjLightData.m_FalloffStart);
	s << WriteFloat("scale",i_ProjLightData.m_Scale);
	s << WriteFloat("range",i_ProjLightData.m_Range);
	s << WriteFloat("aspect",i_ProjLightData.m_Aspect);
	s << WriteBool("bEnableLight",i_ProjLightData.m_bEnableLight);
	s << WriteBool("bEnableDiffuse",i_ProjLightData.m_bEnableDiffuse);
	s << WriteBool("bEnableSpecular",i_ProjLightData.m_bEnableSpecular);
	s << WriteBool("bAffectsGlow",i_ProjLightData.m_bAffectsGlow);
	s << WriteString("shadowMapName",(i_ProjLightData.m_Name+"_SHADOWMAP.tex"));
	s << WriteString("shadowMapNameRGB",(i_ProjLightData.m_Name+"_SHADOWMAP_RGB.tex"));
	s << WriteBool("shadowSource",i_ProjLightData.m_ShadowSource & (io_GlobalData.m_Options.m_bRmanShadowEnable | 
													io_GlobalData.m_bRenderingShadowsOnly | 
													io_GlobalData.m_bRenderingIlluminationOnly ) );
	s << WriteFloat("shadowSoftness",i_ProjLightData.m_ShadowSoftness*0.05f);
	s << WriteFloat("shadowBias",i_ProjLightData.m_ShadowDepthBias*0.02f);
	s << WriteFloat("shadowIntensity",i_ProjLightData.m_ShadowIntensity);
	s << WriteInt("shadowMapRes",i_ProjLightData.m_ShadowMapRes);
	s << WriteFloat("shadowPCSS",i_ProjLightData.m_ShadowPCSS);
	s << WriteColor("shadowColor",i_ProjLightData.m_ShadowColor);
	s << WriteInt("shadowQuality",i_ProjLightData.m_ShadowQuality+3);
	s << WriteBool("shadowsOnly",io_GlobalData.m_bRenderingShadowsOnly);

	s << WriteBool("bDirectional",i_ProjLightData.m_bDirectional);
	s << WriteBool("bPureDirectional",i_ProjLightData.m_bPureDirectional);
	s << WriteBool("bConeLighting",i_ProjLightData.m_bConeLighting);

	s << WriteFloat("pos_x",i_ProjLightData.m_Position.GetX());
	s << WriteFloat("pos_y",i_ProjLightData.m_Position.GetY());
	s << WriteFloat("pos_z",i_ProjLightData.m_Position.GetZ());

	s << WriteFloat("tar_x",i_ProjLightData.m_Target.GetX());
	s << WriteFloat("tar_y",i_ProjLightData.m_Target.GetY());
	s << WriteFloat("tar_z",i_ProjLightData.m_Target.GetZ());

	s << WriteFloat("innerAngle",i_ProjLightData.m_InnerAngle);
	if ( i_ProjLightData.m_bConeLighting )
	{
		float total_angle = i_ProjLightData.m_Penumbra+i_ProjLightData.m_InnerAngle;
		maFunctions::Clamp(total_angle, 0.0f, 179.9f);
		s << WriteFloat("outerAngle",total_angle);
	}
	else
	{
		s << WriteFloat("outerAngle",i_ProjLightData.m_Angle);
	}

	s << WriteInt("shadowType",io_GlobalData.m_Options.m_RmanShadowType);

	camCamera lightCam;
	i_ProjLightData.m_ProjectedLight->OrientCamera(lightCam);

	maMatrix4x4 camMatrix;	
	maMatrix4x4 projMatrix;
	lightCam.GetCameraMatrix( camMatrix );
	lightCam.GetProjectionMatrix( projMatrix );

	s << WriteFloatMatrix("camProjMat",camMatrix*projMatrix);
	s << WriteFloatMatrix("camMat",camMatrix);

	//maMatrix4x4 viewProjTransformInversed = camMatrix*projMatrix;
	//viewProjTransformInversed.Invert();

	//maPoint3d viewFrustumCorners[8];
	//viewFrustumCorners[0] = maPoint3d(-1.0f, 1.0f, 0.0f);
	//viewFrustumCorners[1] = maPoint3d(1.0f, 1.0f, 0.0f);
	//viewFrustumCorners[2] = maPoint3d(1.0f, -1.0f, 0.0f);
	//viewFrustumCorners[3] = maPoint3d(-1.0f, -1.0f, 0.0f);
	//viewFrustumCorners[4] = maPoint3d(-1.0f, 1.0f, 1.0f);
	//viewFrustumCorners[5] = maPoint3d(1.0f, 1.0f, 1.0f);
	//viewFrustumCorners[6] = maPoint3d(1.0f, -1.0f, 1.0f);
	//viewFrustumCorners[7] = maPoint3d(-1.0f, -1.0f, 1.0f);
	//
	//for (int i = 0; i < 8; i++)
	//{
	//	viewFrustumCorners[i] = viewProjTransformInversed * viewFrustumCorners[i];
	//}	
	//bbox.Union(&viewFrustumCorners[0],8);

	maPoint3d mins( -i_ProjLightData.m_Scale/2 , -(i_ProjLightData.m_Scale/2) * (1/i_ProjLightData.m_Aspect) , 0 );
	maPoint3d maxes( i_ProjLightData.m_Scale/2 , (i_ProjLightData.m_Scale/2) * (1/i_ProjLightData.m_Aspect) , i_ProjLightData.m_Range );

	maAxisBox bbox(mins,maxes);

	s << WriteFloat("bbox_min_x",bbox.GetMinX());
	s << WriteFloat("bbox_min_y",bbox.GetMinY());
	s << WriteFloat("bbox_min_z",bbox.GetMinZ());

	s << WriteFloat("bbox_max_x",bbox.GetMaxX());
	s << WriteFloat("bbox_max_y",bbox.GetMaxY());
	s << WriteFloat("bbox_max_z",bbox.GetMaxZ());

	std::string texName = rmanUtil::LookupGeneratedTexName(i_ProjLightData.m_Ramp,io_GlobalData);
	s << WriteString("gobo", texName) << endl;

	s << "TransformEnd" << endl << endl;

	io_GlobalData.m_ProjLightMap.insert( std::pair<std::string,std::string>(i_ProjLightData.m_Name,s.str()));

	if ( i_ProjLightData.m_ShadowSource )
	{
		if ( (io_GlobalData.m_Options.m_bRmanShadowEnable || io_GlobalData.m_bRenderingShadowsOnly || io_GlobalData.m_bRenderingIlluminationOnly ) && 
			  io_GlobalData.m_Options.m_RmanShadowType == 0 )
		{
			io_GlobalData.m_ProjectedLights.push_back( i_ProjLightData.m_ProjectedLight );
			io_GlobalData.m_ProjectedLightNames.push_back( i_ProjLightData.m_Name );
			io_GlobalData.m_ProjectedLightResolutions.push_back( i_ProjLightData.m_ShadowMapRes );
			io_GlobalData.m_ProjectedLightIsConeLighting.push_back( i_ProjLightData.m_bConeLighting );
		}
	}		

}

//--------------------------------------------------------------------
// ExportPointLight()
//--------------------------------------------------------------------
void rmanExport::ExportPointLight(rmanGlobalData & io_GlobalData, rmanPointLightData i_PointLightData)
{

	std::ostringstream s;
	s << "LightSource" << " ";
	s << "\"PointLight\"" << " " << GetSceneLightID(i_PointLightData.m_Name, io_GlobalData) << " ";

	s << WriteFloat("intensity",i_PointLightData.m_Intensity);
	s << WritePoint("pos", i_PointLightData.m_Position);
	s << WriteColor("lightColor",i_PointLightData.m_Color);
	s << WriteFloat("falloff_x",i_PointLightData.m_Falloff.GetX());
	s << WriteFloat("falloff_y",i_PointLightData.m_Falloff.GetY());
	s << WriteFloat("falloff_z",i_PointLightData.m_Falloff.GetZ());
	s << WriteFloat("falloffStart",i_PointLightData.m_FalloffStart);
	s << WriteBool("bEnableLight",i_PointLightData.m_bEnableLight);
	s << WriteBool("bEnableDiffuse",i_PointLightData.m_bEnableDiffuse);
	s << WriteBool("bEnableSpecular",i_PointLightData.m_bEnableSpecular);
	s << WriteBool("bAffectsGlow",i_PointLightData.m_bAffectsGlow);
	s << WriteBool("shadowsOnly",io_GlobalData.m_bRenderingShadowsOnly);

	s << endl << endl;

	io_GlobalData.m_PointLightMap.insert( std::pair<std::string,std::string>(i_PointLightData.m_Name,s.str()));
}

//--------------------------------------------------------------------
// ExportMasterRib()
//--------------------------------------------------------------------
void rmanExport::ExportMasterRib(rmanGlobalData & io_GlobalData)
{
	itString itFileNameMaster;
	fsFileUtil::LocatorToUnicodeString( io_GlobalData.m_RibPath, itFileNameMaster );
	std::ofstream masterRib( itFileNameMaster.GetString() , std::ios::out );

	masterRib << "#****************************************************************" << endl;
	masterRib << "# RenderMan Interface Bytestream generated by MachStudio - MASTER"  << endl; 
	masterRib << "#****************************************************************" << endl << endl;

	fsLocator sloPath = matShaderMgr::GetDefaultShaderPath();
	sloPath.Push("rman");
	sloPath.Push( io_GlobalData.m_Engine.c_str() );
	std::string sloPathStr;
	fsFileUtil::LocatorToANSIFilename(sloPath,sloPathStr);
	replace_all(sloPathStr, "\\", "/");

	fsLocator texLoc = io_GlobalData.m_TexturesLoc;
	std::string texPathStr;
	fsFileUtil::LocatorToANSIFilename(texLoc,texPathStr);	
	replace_all(texPathStr, "\\", "/");

	fsLocator archLoc = io_GlobalData.m_ArchivesLoc;
	std::string archPathStr;
	fsFileUtil::LocatorToANSIFilename(archLoc,archPathStr);	
	replace_all(archPathStr, "\\", "/");

	std::string fullPath;
	fsFileUtil::LocatorToANSIFilename(io_GlobalData.m_RibPath,fullPath);
	fullPath = remove_extension(fullPath);
	replace_all(fullPath, "\\", "/");

	masterRib << "Option \"searchpath\" \"shader\" [\"" << sloPathStr <<"\"]" << endl;
	io_GlobalData.m_SearchPaths.push_back(sloPathStr);

	masterRib << "Option \"searchpath\" \"texture\" [\"" << texPathStr <<"\"]" << endl;
	io_GlobalData.m_SearchPaths.push_back(texPathStr);

	masterRib << "Option \"searchpath\" \"archive\" [\"" << archPathStr <<"\"]" << endl;
	io_GlobalData.m_SearchPaths.push_back(archPathStr);

	ExportOptions(io_GlobalData,masterRib);

	std::string driver_ext;
	std::string driver_type;
	set_display_info(io_GlobalData.m_Options.m_RmanOutType,driver_ext,driver_type);

	if ( io_GlobalData.m_bRenderingReflectionsOnly )
	{
		masterRib << "Declare \"outRefl\" \"varying color\"" << endl;
		masterRib << "Display \"" << fullPath << driver_ext << "\" \"" << driver_type << "\" \"outRefl\" \"quantize\" [0 255 0 255]" << endl;
	}
	else if ( io_GlobalData.m_bRenderingGIOnly )
	{
		masterRib << "Declare \"outGI\" \"varying color\"" << endl;
		masterRib << "Display \"" << fullPath << driver_ext << "\" \"" << driver_type << "\" \"outGI\" \"quantize\" [0 255 0 255]" << endl;
	}
	else
	{
		masterRib << "Display \"" << fullPath << driver_ext << "\" \"" << driver_type << "\" \"rgba\"" << endl;
	}

	if ( driver_type == "openexr" )
	{
		masterRib << "Quantize \"rgba\" 0 0 0 0" << endl;
		masterRib << "Quantize \"z\"    0 0 0 0" << endl;
	}

	masterRib << "Format " << io_GlobalData.m_width << " " << io_GlobalData.m_height << " " << io_GlobalData.m_PixelAspectRatio << endl;

	float filterWidth = io_GlobalData.m_RmanFilterWidth;
	if ( filterWidth < 1 ) filterWidth = 1;
	masterRib << "PixelFilter " << get_filter_rib_form(io_GlobalData.m_RmanFilterType) << " " 
								<< filterWidth << " "
								<< filterWidth << endl;

	masterRib << "PixelSamples " << io_GlobalData.m_RmanAArate << " " << io_GlobalData.m_RmanAArate << endl;

	masterRib << "ShadingRate " << io_GlobalData.m_Options.m_RmanShadingRate << endl;

	camHDRData hdr;
	io_GlobalData.m_SceneCamera->GetHDRParams(hdr);

	if ( io_GlobalData.m_bRenderingShadowsOnly || io_GlobalData.m_bRenderingAOOnly )
	{
		masterRib << "Imager \"background\" \"background\" [ 1 1 1 ]" << endl;
	}
	else
	{
		masterRib << "Imager \"HDRLighting\" ";
		masterRib << WriteBool("g_bEnableToneMap",io_GlobalData.m_Options.m_bTonemapEnable) << " ";
		masterRib << WriteFloat("g_fixedLuminance",hdr.m_SceneLuminance) << " ";
		masterRib << WriteFloat("g_fMiddleGray",hdr.m_MiddleGray) << " ";
		masterRib << WriteFloat("g_fWhiteCutoff",hdr.m_WhiteCutoff) << endl;
		//masterRib << WriteFloat("g_fBloomScale",hdr.m_BloomScale) << " ";
		//masterRib << WriteFloat("g_fStarScale",hdr.m_StarScale) << " ";
		//masterRib << WriteColor("vBloom",maPoint3d(1,1,1)) << " ";
		//masterRib << WriteColor("vStar",maPoint3d(1,1,1)) << endl;
	}

	//masterRib << "Atmosphere \"Fog\" " << endl;

	camDOFData dof;
	io_GlobalData.m_SceneCamera->GetDOFParams(dof);

	if ( io_GlobalData.m_SceneCamera->GetEnableALP() && dof.m_bEnableDOF && io_GlobalData.m_bRenderDOF ) 
	{
		masterRib << "DepthOfField " << io_GlobalData.m_SceneCamera->GetFStop() << " "
									 << io_GlobalData.m_SceneCamera->GetFocalLength()/1000 << " " 
									 << io_GlobalData.m_SceneCamera->GetFocalDistance() << endl;
	}
	
	masterRib << "Clipping " << io_GlobalData.m_SceneCamera->GetNearClip() << " " << io_GlobalData.m_SceneCamera->GetFarClip() << endl;

	if ( io_GlobalData.m_SceneCamera->IsOrthographic() )
	{
		float screenXMin = -1;
		float screenYMin = -1;
		float screenXMax = 1;
		float screenYMax = 1;
		io_GlobalData.m_SceneCamera->GetSubViewport( screenYMax, screenYMin, screenXMin, screenXMax );

		float w = io_GlobalData.m_SceneCamera->GetOrthoWidth() / 2.0f;
		float left = screenXMin * w;
		float right = screenXMax * w;

		float h = w / io_GlobalData.m_SceneCamera->GetAspect();
		float top = screenYMax * h; 
		float bottom = screenYMin * h;

		masterRib << "Projection \"orthographic\"" << endl;
		masterRib << "ScreenWindow " << left << " " << right << " " << top << " " << bottom << endl;
	}
	else
	{		
		masterRib << "Projection \"perspective\" \"fov\" " << io_GlobalData.m_SceneCamera->GetFOV() << endl;
		masterRib << "ScreenWindow -1 1 -" << 1/((float)io_GlobalData.m_width/(float)io_GlobalData.m_height) << " " << 
											  1/((float)io_GlobalData.m_width/(float)io_GlobalData.m_height) << endl;
	}

	masterRib << "Scale -1 1 1" << endl;
	
	maMatrix4x4 projMat;
	io_GlobalData.m_SceneCamera->GetProjectionMatrix(projMat);
	//std::string projTransformString = WriteMatrix(projMat);
	//masterRib << "Transform " << projTransformString << endl;

	maMatrix4x4 camMat;
	io_GlobalData.m_SceneCamera->GetCameraMatrix(camMat);
	std::string camTransformString = WriteMatrix(camMat);
	masterRib << "ConcatTransform " << camTransformString << endl;

	masterRib << endl;

	fsLocator worldLoc = io_GlobalData.m_ArchivesLoc;	
	std::string worldFileName = io_GlobalData.m_FrameName + "_WORLD.rib";
	worldLoc.Push(worldFileName.c_str());

	std::string worldPath;
	fsFileUtil::LocatorToANSIFilename(worldLoc,worldPath);
	replace_all(worldPath, "\\", "/");

	masterRib << "ReadArchive \"" << worldPath << "\"" << endl;

	masterRib.close();

	itString itFileNameWorld;
	fsFileUtil::LocatorToUnicodeString( worldLoc, itFileNameWorld );
	std::ofstream worldRib( itFileNameWorld.GetString() , std::ios::out );

	worldRib << "WorldBegin" << endl ;
	worldRib << endl;

	writeOutMap("",io_GlobalData.m_PointLightMap,worldRib);
	writeOutMap("",io_GlobalData.m_ProjLightMap,worldRib);
	writeOutMap("",io_GlobalData.m_MasterAttributeMap,worldRib);

	worldRib << "WorldEnd" << endl ;
	worldRib << endl;
	worldRib.close();
}

//--------------------------------------------------------------------
// ExportShadowMaps()
//--------------------------------------------------------------------
void rmanExport::ExportShadowMaps(rmanGlobalData & io_GlobalData)
{
	for ( int i = 0 ; i < io_GlobalData.m_ProjectedLightNames.size() ; i++ )
	{	
		itString fileName = itString(io_GlobalData.m_ProjectedLightNames[i].c_str());
		fileName += itString("_SHADOWMAP.rib");

		fsLocator loc = io_GlobalData.m_ShadowMapsLoc;
		loc.Push(fileName);
		itString itFileName;
		std::string plain_name;
		fsFileUtil::LocatorToANSIFilename( loc, plain_name);
		plain_name = remove_extension(plain_name);
		replace_all(plain_name, "\\", "/");
		fsFileUtil::LocatorToUnicodeString( loc, itFileName );
		std::ofstream shadowRib( itFileName.GetString() , std::ios::out );			

		shadowRib << "#********************************************************************" << endl;
		shadowRib << "# RenderMan Interface Bytestream generated by MachStudio - SHADOW MAP"  << endl; 
		shadowRib << "#********************************************************************" << endl << endl;

		shadowRib << "Option \"searchpath\" \"shader\" [\"" << io_GlobalData.m_SearchPaths[0] << "\"]" << endl;
		shadowRib << "Option \"searchpath\" \"texture\" [\"" << io_GlobalData.m_SearchPaths[1] <<"\"]" << endl;
		shadowRib << "Option \"searchpath\" \"archive\" [\"" << io_GlobalData.m_SearchPaths[2] <<"\"]" << endl;

		ExportOptions(io_GlobalData,shadowRib);

		shadowRib << "Display \"" << plain_name << ".z\" \"zfile\" \"z\"" << endl;
		shadowRib << "#Display \"view_from_light\" \"framebuffer\" \"rgb\"" << endl;

		shadowRib << "Format " << io_GlobalData.m_ProjectedLightResolutions[i] << " " << io_GlobalData.m_ProjectedLightResolutions[i] << " 1" << endl;
		shadowRib << "Hider \"hidden\" \"jitter\" [0]" << endl;
		shadowRib << "ShadingRate 1" << endl;
		shadowRib << "PixelSamples 1 1" << endl;
		shadowRib << "PixelFilter \"box\" 1 1" << endl;
		
		g3dProjectedLight * light = io_GlobalData.m_ProjectedLights[i];

		camCamera lightCam;
		light->OrientCamera(lightCam);

		if ( lightCam.IsOrthographic() )
		{
			float screenXMin = -1;
			float screenYMin = -1;
			float screenXMax = 1;
			float screenYMax = 1;
			lightCam.GetSubViewport( screenYMax, screenYMin, screenXMin, screenXMax );

			float w = lightCam.GetOrthoWidth() / 2.0f;
			float left = screenXMin * w;
			float right = screenXMax * w;

			float h = w / lightCam.GetAspect();
			float top = screenYMax * h; 
			float bottom = screenYMin * h;

			shadowRib << "Projection \"orthographic\"" << endl;
			shadowRib << "ScreenWindow " << left << " " << right << " " << top << " " << bottom << endl;
		}
		else
		{
			if ( io_GlobalData.m_ProjectedLightIsConeLighting[i] )
			{
				float total_angle = light->GetInnerAngle()+light->GetAngle();
				maFunctions::Clamp(total_angle, 0.0f, 179.9f);
				shadowRib << "Projection \"perspective\" \"fov\" " << total_angle << endl;
			}
			else
			{
				shadowRib << "Projection \"perspective\" \"fov\" " << light->GetAngle() << endl;
			}
		}

		shadowRib << "Scale -1 1 1" << endl;

		maMatrix4x4 camTransform;
		lightCam.GetCameraMatrix(camTransform);
		std::string transString = WriteMatrix(camTransform);
		shadowRib << "ConcatTransform " << transString << endl << endl;

		shadowRib << "WorldBegin" << endl << endl;
		for ( int j = 0 ; j < io_GlobalData.m_ShadowMapAttributes.size() ; j++ )
		{
			shadowRib << io_GlobalData.m_ShadowMapAttributes[j] << endl;
		}
		shadowRib << endl <<  "WorldEnd" << endl;

		shadowRib.close();	
	}
}

//--------------------------------------------------------------------
// ExportReflectionMaps()
//--------------------------------------------------------------------
void rmanExport::ExportReflectionMaps(rmanGlobalData & io_GlobalData)
{
	for ( int i = 0 ; i < io_GlobalData.m_ReflObjNames.size() ; i++ )
	{
		camCamera cubeCam;
		cubeCam.SetAspect(1.0f);
		cubeCam.SetFOV(90);

		static const maVector3d directions[] = 
		{
			// eyeVec, upVec
			maVector3d(1,0,0),	maVector3d(0,1,0),
			maVector3d(-1,0,0),	maVector3d(0,1,0),
			maVector3d(0,1,0),	maVector3d(0,0,-1),
			maVector3d(0,-1,0),	maVector3d(0,0,1),
			maVector3d(0,0,1),	maVector3d(0,1,0),
			maVector3d(0,0,-1),	maVector3d(0,1,0),
		};

		maPoint3d target;
		maPoint3d pos = io_GlobalData.m_ReflObjTransforms[i] * io_GlobalData.m_ReflObjPositions[i];
		std::string objName = io_GlobalData.m_ReflObjNames[i];
		int cubeMapRes = io_GlobalData.m_ReflCubemapResolutions[i];
		bool isPlanar = io_GlobalData.m_ReflObjIsPlanar[i];

		if ( isPlanar )
		{		
			g3dFragment* frag = io_GlobalData.m_ReflObjFragments[i];
			maMatrix4x4 totalTransform = io_GlobalData.m_ReflObjTransforms[i];

			maPoint3d facepts[3];
			maVector3d facenorms[3];
			maVector3d p, v;
			bool got_face = frag->GetFaceInfo(0, facepts, facenorms);
			DBG_ASSERT(got_face == true, "Renderer could not get plane for planar reflection");
			// point and normal define plane (object space)
			p += facepts[0];
			p += facepts[1];
			p += facepts[2];
			p *= 0.333333f;
			// calculate geometric face normal rather than using vertex normals.
			v = (facepts[1] - facepts[0]).Cross(facepts[2]-facepts[1]);
			v.Normalize();

			// bring into world space
			totalTransform.Transform(p);
			maMatrix4x4 invtransp = totalTransform;
			invtransp.Invert();
			invtransp.Transpose();
			invtransp.TransformDir(v);

			v.Normalize();
			// try to round the normal vector to an axis if it's close.
			if ((v.m_X) > 0.99f)
				v = maVector3d(1,0,0);
			else if ((v.m_X) < -0.99f)
				v = maVector3d(-1,0,0);
			else if ((v.m_Y) > 0.99f)
				v = maVector3d(0,1,0);
			else if ((v.m_Y) < -0.99f)
				v = maVector3d(0,-1,0);
			else if ((v.m_Z) > 0.99f)
				v = maVector3d(0,0,1);
			else if ((v.m_Z) < -0.99f)
				v = maVector3d(0,0,-1);

			float pdotn = v*p;
			// defaults to identity

			maMatrix4x4 mrefl;
			mrefl(0,0) = 1 - 2.0f * v.m_X * v.m_X;
			mrefl(1,1) = 1 - 2.0f * v.m_Y * v.m_Y;
			mrefl(2,2) = 1 - 2.0f * v.m_Z * v.m_Z;
			mrefl(3,3) = 1;
			mrefl(0,1) = -2.0f * v.m_X * v.m_Y;
			mrefl(0,2) = -2.0f * v.m_X * v.m_Z;
			mrefl(1,2) = -2.0f * v.m_Y * v.m_Z;
			mrefl(1,0) = -2.0f * v.m_X * v.m_Y;
			mrefl(2,0) = -2.0f * v.m_X * v.m_Z;
			mrefl(2,1) = -2.0f * v.m_Y * v.m_Z;
			mrefl(0,3) = 2.0f * pdotn * v.m_X;
			mrefl(1,3) = 2.0f * pdotn * v.m_Y;
			mrefl(2,3) = 2.0f * pdotn * v.m_Z;
			mrefl.Transpose();

			maMatrix4x4 camTransform;
			io_GlobalData.m_SceneCamera->GetCameraMatrix(camTransform);

			maMatrix4x4 reflMatrix = mrefl * camTransform;

			std::ostringstream reflName;
			reflName << objName << "_PLANARREFLECTIONMAP.rib";
			fsLocator loc = io_GlobalData.m_ReflectionMapsLoc;
			loc.Push(reflName.str().c_str());
			itString itFileName;
			fsFileUtil::LocatorToUnicodeString( loc , itFileName );
			std::ofstream reflectionRib( itFileName.GetString() , std::ios::out );

			std::string plain_name;
			fsFileUtil::LocatorToANSIFilename( loc, plain_name);
			replace_all(plain_name, "\\", "/");
			plain_name = remove_extension(plain_name);

			reflectionRib << "#*******************************************************************************" << endl;
			reflectionRib << "# RenderMan Interface Bytestream generated by MachStudio - PLANAR REFLECTION MAP"  << endl; 
			reflectionRib << "#*******************************************************************************" << endl << endl;

			reflectionRib << "Option \"searchpath\" \"shader\" [\"" << io_GlobalData.m_SearchPaths[0] <<"\"]" << endl;
			reflectionRib << "Option \"searchpath\" \"texture\" [\"" << io_GlobalData.m_SearchPaths[1] <<"\"]" << endl;
			reflectionRib << "Option \"searchpath\" \"archive\" [\"" << io_GlobalData.m_SearchPaths[2] <<"\"]" << endl;

			ExportOptions(io_GlobalData,reflectionRib);

			reflectionRib << "Display \"" << plain_name << ".tif\" \"" << "tiff" << "\" \"rgba\"" << endl;
			reflectionRib << "Format " << io_GlobalData.m_width << " " << io_GlobalData.m_height << " " << io_GlobalData.m_PixelAspectRatio << endl;
	
			float filterWidth = io_GlobalData.m_RmanFilterWidth;
			if ( filterWidth < 1 ) filterWidth = 1;
			reflectionRib << "PixelFilter " << get_filter_rib_form(io_GlobalData.m_RmanFilterType) << " " 
											<< filterWidth << " "
											<< filterWidth << endl;

			reflectionRib << "PixelSamples " << io_GlobalData.m_RmanAArate << " " << io_GlobalData.m_RmanAArate << endl;

			reflectionRib << "ShadingRate " << io_GlobalData.m_Options.m_RmanShadingRate << endl;

			camDOFData dof;
			io_GlobalData.m_SceneCamera->GetDOFParams(dof);

			if ( io_GlobalData.m_SceneCamera->IsOrthographic() )
			{
				float screenXMin = -1;
				float screenYMin = -1;
				float screenXMax = 1;
				float screenYMax = 1;
				io_GlobalData.m_SceneCamera->GetSubViewport( screenYMax, screenYMin, screenXMin, screenXMax );

				float w = io_GlobalData.m_SceneCamera->GetOrthoWidth() / 2.0f;
				float left = screenXMin * w;
				float right = screenXMax * w;

				float h = w / io_GlobalData.m_SceneCamera->GetAspect();
				float top = screenYMax * h; 
				float bottom = screenYMin * h;

				reflectionRib << "Projection \"orthographic\"" << endl;
				reflectionRib << "ScreenWindow " << left << " " << right << " " << top << " " << bottom << endl;
			}
			else
			{	
				reflectionRib << "Projection \"perspective\" \"fov\" " << io_GlobalData.m_SceneCamera->GetFOV() << endl;
				reflectionRib << "ScreenWindow -1 1 -" << 1/((float)io_GlobalData.m_width/(float)io_GlobalData.m_height) << " " << 
													  1/((float)io_GlobalData.m_width/(float)io_GlobalData.m_height) << endl;
			}
		
			reflectionRib << "Clipping " << io_GlobalData.m_SceneCamera->GetNearClip() << " " << io_GlobalData.m_SceneCamera->GetFarClip() << endl;

			
			reflectionRib << "Scale -1 1 1" << endl;
			reflectionRib << "ConcatTransform " << WriteMatrix(reflMatrix) << endl;

			reflectionRib << endl;

			itFileName.StripExtension();
			itFileName += itString("_WORLD.rib");

			std::string worldPath = itStringUtil::GetStdString( itFileName );
			replace_all(worldPath, "\\", "/");

			reflectionRib << "ReadArchive \"" << worldPath << "\"" << endl;
			reflectionRib.close();

			std::ofstream worldRib( itFileName.GetString() , std::ios::out );

			worldRib << "WorldBegin" << endl ;
			worldRib << endl;

			writeOutMap("",io_GlobalData.m_PointLightMap,worldRib);
			writeOutMap("",io_GlobalData.m_ProjLightMap,worldRib);
			writeOutMap(objName,io_GlobalData.m_MasterAttributeMap,worldRib);

			worldRib << "WorldEnd" << endl ;
			worldRib << endl;
			worldRib.close();

			io_GlobalData.m_ReflPlanarRibs.push_back( loc.GetLastName() );

		}
		else
		{
			
			for (int j = 0; j < 6; j++)
			{
				target = pos + directions[j*2];
				cubeCam.LookAt( pos, target, directions[j*2+1] );

				maMatrix4x4 camTransform;
				cubeCam.GetCameraMatrix(camTransform);
				std::string transString = WriteMatrix(camTransform);

				std::ostringstream reflName;
				reflName << objName << "_CUBEREFLECTIONMAP_FACE" << j << ".rib";
				fsLocator loc = io_GlobalData.m_ReflectionMapsLoc;
				loc.Push(reflName.str().c_str());
				itString itFileName;
				fsFileUtil::LocatorToUnicodeString( loc , itFileName );
				std::ofstream reflectionRib( itFileName.GetString() , std::ios::out );

				std::string plain_name;
				fsFileUtil::LocatorToANSIFilename( loc, plain_name);
				replace_all(plain_name, "\\", "/");
				plain_name = remove_extension(plain_name);

				reflectionRib << "#**********************************************************************************" << endl;
				reflectionRib << "# RenderMan Interface Bytestream generated by MachStudio - CUBE REFLECTION MAP FACE"  << endl; 
				reflectionRib << "#**********************************************************************************" << endl << endl;

				reflectionRib << "Option \"searchpath\" \"shader\" [\"" << io_GlobalData.m_SearchPaths[0] <<"\"]" << endl;
				reflectionRib << "Option \"searchpath\" \"texture\" [\"" << io_GlobalData.m_SearchPaths[1] <<"\"]" << endl;
				reflectionRib << "Option \"searchpath\" \"archive\" [\"" << io_GlobalData.m_SearchPaths[2] <<"\"]" << endl;

				reflectionRib << "Display \"" << plain_name << ".tif\" \"" << "tiff" << "\" \"rgba\"" << endl;
				reflectionRib << "Format " << cubeMapRes << " " << cubeMapRes << " " << io_GlobalData.m_PixelAspectRatio << endl;
				reflectionRib << "PixelSamples " << io_GlobalData.m_RmanAArate << " " << io_GlobalData.m_RmanAArate << endl;

				reflectionRib << "ShadingRate " << io_GlobalData.m_Options.m_RmanShadingRate << endl;

				reflectionRib << "Projection \"perspective\" \"fov\" " << 90 << endl;
				reflectionRib << "Clipping " << 0.1 << " " << 10000.0 << endl;
				reflectionRib << "ScreenWindow -1 1 -" << 1/((float)cubeMapRes/(float)cubeMapRes) << " " << 
														  1/((float)cubeMapRes/(float)cubeMapRes) << endl;
				
				reflectionRib << "ConcatTransform " << transString << endl;

				reflectionRib << endl;

				itFileName.StripExtension();
				itFileName += itString("_WORLD.rib");

				std::string worldPath = itStringUtil::GetStdString( itFileName );
				replace_all(worldPath, "\\", "/");

				reflectionRib << "ReadArchive \"" << worldPath << "\"" << endl;
				reflectionRib.close();

				std::ofstream worldRib( itFileName.GetString() , std::ios::out );

				worldRib << "WorldBegin" << endl ;
				worldRib << endl;

				writeOutMap("",io_GlobalData.m_PointLightMap,worldRib);
				writeOutMap("",io_GlobalData.m_ProjLightMap,worldRib);
				writeOutMap(objName,io_GlobalData.m_MasterAttributeMap,worldRib);

				worldRib << "WorldEnd" << endl ;
				worldRib << endl;
				worldRib.close();

				io_GlobalData.m_ReflCubeRibs.push_back( loc.GetLastName() );
			}
		}
	}	
}


//--------------------------------------------------------------------
// ExportPhotonMap()
//--------------------------------------------------------------------
//void rmanExport::ExportPhotonMap(rmanGlobalData & io_GlobalData)
//{
//	itString fileName = io_GlobalData.m_RibPath.GetLastName();
//	fileName.StripExtension();
//	fileName += itString("_PHOTONMAP.rib");
//
//	fsLocator loc = io_GlobalData.m_PhotonMapLoc;
//	loc.Push(fileName);
//	itString itFileName;
//	std::string plain_name;
//	fsFileUtil::LocatorToANSIFilename( loc, plain_name);
//	plain_name = remove_extension(plain_name);
//	replace_all(plain_name, "\\", "/");
//	fsFileUtil::LocatorToUnicodeString( loc, itFileName );
//	std::ofstream photonRib( itFileName.GetString() , std::ios::out );	
//
//	photonRib << "#************************************************************************" << endl;
//	photonRib << "# RenderMan Interface Bytestream generated by MachStudio - PHOTON MAPPING"  << endl; 
//	photonRib << "#************************************************************************" << endl << endl;
//
//	photonRib << "Option \"searchpath\" \"shader\" [\"" << io_GlobalData.m_SearchPaths[0] <<"\"]" << endl;
//	photonRib << "Option \"searchpath\" \"texture\" [\"" << io_GlobalData.m_SearchPaths[1] <<"\"]" << endl;
//	photonRib << "Option \"searchpath\" \"archive\" [\"" << io_GlobalData.m_SearchPaths[2] <<"\"]" << endl;
//
//	photonRib << "Hider \"photon\" \"emit\" 300000" << endl;
//	photonRib << "Attribute \"photon\" \"globalmap\" \"" << plain_name << ".gpm\"" << endl;
//	photonRib << "Attribute \"trace\" \"maxspeculardepth\" 5" << endl;
//	photonRib << "Attribute \"trace\" \"maxdiffusedepth\" 5" << endl;
//
//	photonRib << "Format " << io_GlobalData.m_width << " " << io_GlobalData.m_height << " " << io_GlobalData.m_PixelAspectRatio << endl;
//
//	if ( io_GlobalData.m_SceneCamera->IsOrthographic() )
//	{
//		float screenXMin = -1;
//		float screenYMin = -1;
//		float screenXMax = 1;
//		float screenYMax = 1;
//		io_GlobalData.m_SceneCamera->GetSubViewport( screenYMax, screenYMin, screenXMin, screenXMax );
//
//		float w = io_GlobalData.m_SceneCamera->GetOrthoWidth() / 2.0f;
//		float left = screenXMin * w;
//		float right = screenXMax * w;
//
//		float h = w / io_GlobalData.m_SceneCamera->GetAspect();
//		float top = screenYMax * h; 
//		float bottom = screenYMin * h;
//
//		photonRib << "Projection \"orthographic\"" << endl;
//		photonRib << "ScreenWindow " << left << " " << right << " " << top << " " << bottom << endl;
//	}
//	else
//	{		
//		photonRib << "Projection \"perspective\" \"fov\" " << io_GlobalData.m_SceneCamera->GetFOV() << endl;
//		photonRib << "ScreenWindow -1 1 -" << 1/((float)io_GlobalData.m_width/(float)io_GlobalData.m_height) << " " << 
//											  1/((float)io_GlobalData.m_width/(float)io_GlobalData.m_height) << endl;
//	}
//
//	photonRib << "Scale -1 1 1" << endl;
//	
//	maMatrix4x4 camMat;
//	io_GlobalData.m_SceneCamera->GetCameraMatrix(camMat);
//	std::string camTransformString = WriteMatrix(camMat);
//	photonRib << "ConcatTransform " << camTransformString << endl;
//
//	photonRib << endl;
//
//	fsLocator worldLoc = io_GlobalData.m_ArchivesLoc;	
//	std::string worldFileName = io_GlobalData.m_FrameName + "_WORLD.rib";
//	worldLoc.Push(worldFileName.c_str());
//
//	std::string worldPath;
//	fsFileUtil::LocatorToANSIFilename(worldLoc,worldPath);
//	replace_all(worldPath, "\\", "/");
//
//	photonRib << "ReadArchive \"" << worldPath << "\"" << endl;
//
//	photonRib.close();
//
//	itString itFileNameWorld;
//	fsFileUtil::LocatorToUnicodeString( worldLoc, itFileNameWorld );
//	std::ofstream worldRib( itFileNameWorld.GetString() , std::ios::out );
//
//	worldRib << "WorldBegin" << endl ;
//	worldRib << endl;
//
//	writeOutMap("",io_GlobalData.m_PointLightMap,worldRib);
//	writeOutMap("",io_GlobalData.m_ProjLightMap,worldRib);
//	writeOutMap("",io_GlobalData.m_MasterAttributeMap,worldRib);
//
//	worldRib << "WorldEnd" << endl ;
//	worldRib << endl;
//	worldRib.close();
//
//}



//--------------------------------------------------------------------
// ExportSubdivisionMesh()
//--------------------------------------------------------------------
//void rmanExport::ExportSubdivisionMesh( std::vector<envType::UInt32>* i_Indices,
//										  std::vector<maPoint3d>* i_Vertices,
//										  std::vector<maPoint2d>* i_UVs,
//										  std::string i_SubdivName,
//										  const g3dSceneNode * i_pNode, 
//										  bool i_bVisible,
//										  maMatrix4x4 i_Transform,
//										  std::string i_BaseName,
//										  std::vector< g3dFragment* > i_Fragment, 
//										  g3dAmbientEnvState * i_AmbientData)
//{
//
//	std::ostringstream currAttribute;
//
//	// Create archive file
//	std::string fragName = i_Fragment[0]->GetFragmentName();
//	std::string matName = i_Fragment[0]->GetMaterial()->GetName();
//	std::string objName = i_BaseName + "-" + fragName + "-" + matName;
//
//	currAttribute << "Sides " << (i_Fragment[0]->GetDoubleSided()?2:1) << endl;
//	ExportLightSets( objName , currAttribute );
//	currAttribute << "AttributeBegin" << endl;
//	io_GlobalData.m_ShadowMapAttributes.push_back( "AttributeBegin" );
//
//	//std::string constructedObjectName = objName;// + std::string("-") + buffer;
//	std::string archiveName = objName + std::string(".rib");
//	fsLocator loc = io_GlobalData.m_ArchivesLoc;
//	loc.Push(archiveName.c_str());
//	itString itFileName;
//	fsFileUtil::LocatorToUnicodeString( loc, itFileName );
//	std::ofstream archiveFile( itFileName.GetString() , std::ios::out );
//
//	currAttribute << ExportAttributeList( objName , i_Fragment[0] );
//
//	std::string transformString = WriteMatrix( i_Transform );
//	currAttribute << "  ConcatTransform " << transformString << endl;
//	io_GlobalData.m_ShadowMapAttributes.push_back( "  ConcatTransform " + transformString );
//
//	float transparency = 1;
//	ExportShaderInfo(currAttribute, i_Fragment[0], i_bVisible,
//					 i_AmbientData, i_Transform, objName, transparency);
//
//	currAttribute << "  ReadArchive \"" << archiveName << "\"" << endl;
//	if ( transparency > 0 )
//	{
//		io_GlobalData.m_ShadowMapAttributes.push_back( "  ReadArchive \"" + archiveName + "\"" );
//	}
//
//	archiveFile << "#******************************************************************" << endl;
//	archiveFile << "# RenderMan Interface Bytestream generated by MachStudio - GEOMETRY"  << endl; 
//	archiveFile << "#******************************************************************" << endl << endl;
//
//	archiveFile << "  SubdivisionMesh \"catmull-clark\"" << endl;
//		
//	std::vector<int> faces;
//	std::vector<int> indices;
//
//	int indicesSize = i_Indices->size();
//
//	int startIdx = 0;
//	while( startIdx < i_Indices->size() )
//	{
//		int currFace = (*i_Indices)[startIdx];
//		faces.push_back( currFace );
//
//		for ( int j = 1 ; j <= currFace ; j++ )
//		{
//			indices.push_back( (*i_Indices)[j+startIdx] );
//
//			if ( j == currFace )
//			{
//				startIdx += j+1;
//			}
//		}
//	}
//
//	int faceSum = 0;
//	for ( int i = 0 ; i < faces.size() ; i++ )
//	{
//		faceSum += faces[i];
//	}
//
//	// Faces (params)
//	archiveFile << "    [ ";
//	for ( int i = 0 ; i < faces.size() ; i++ )
//	{
//		archiveFile << faces[i] << " ";
//	}
//	archiveFile << "]" << endl;
//
//	// Indices (params)
//	archiveFile << "    [ ";
//	for ( int i = 0 ; i < indices.size() ; i++ )
//	{
//		archiveFile << indices[i] << " ";
//	}
//	archiveFile << "]" << endl;
//
//	// Unused params
//	archiveFile << "    [\"interpolateboundary\"] [0 0] [] []" << endl;
//
//	// Vertices (params)
//	archiveFile << "    \"P\" [ ";
//	for ( int i = 0 ; i < i_Vertices->size() ; i++ )
//	{
//		archiveFile << (*i_Vertices)[i][0] << " ";
//		archiveFile << (*i_Vertices)[i][1] << " ";
//		archiveFile << (*i_Vertices)[i][2] << " ";		
//	}
//	archiveFile << "]" << endl;
//
//	// UVs (params)
//	archiveFile << "    \"st\" [ ";
//	for ( int i = 0 ; i < i_UVs->size() ; i++ )
//	{
//		archiveFile << (*i_UVs)[i][0] << " ";
//		archiveFile << (*i_UVs)[i][1] << " ";	
//	}
//	archiveFile << "]" << endl;	
//
//	archiveFile.close();
//
//	currAttribute << "AttributeEnd" << endl << endl;
//
//	io_GlobalData.m_MasterAttributeMap.insert( std::pair<std::string,std::string>(objName,currAttribute.str()));
//	io_GlobalData.m_ShadowMapAttributes.push_back( "AttributeEnd" );
//
//}