/****************************************************************************\
**	mrayExport.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/mray/export/mrayExport.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/env/envSTLHelpers.hpp"

#include "ImportExport/mray/export/mrayExportData.hpp"
#include "ImportExport/mray/export/private/mrayExportUtil.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/eff/effReflData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dIndexPtr.hpp"
#include "Graphics/g3d/g3dRenderState.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/Mat/matMetaFX.hpp"
#include "Graphics/Mat/matMetaFXParser.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTexture.hpp"


using namespace std;
using namespace mrayExportUtil;

typedef unsigned char BYTE;

//[1] to enable casting the effect,
static const int s_CastOn = 0x0001;
//[2] to enable receiving the effect,
static const int s_ReceiveOn = 0x0002;
//[4] to disable casting the effect,
static const int s_CastOff = 0x0004;
//[8] to disable receiving the effect.
static const int s_ReceiveOff = 0x0008;

enum SHADOWTYPE
{
	SHADOWMAP = 0,
	RAYTRACING
};

const int IBLMAPRESNUM = 5;
const int IBLMAPRES[IBLMAPRESNUM] = {512, 1024, 2048, 4096, 8192};

//--------------------------------------------------------------------
// GetSamplingMode()
//--------------------------------------------------------------------
std::string GetSamplingMode(int i_Val)
{
	std::string str = "\"detail\"";
	if ( i_Val == 0 )
	{
		str = "\"sparse\"";
	}
	return str;
}

//--------------------------------------------------------------------
// GetSamplingPattern()
//--------------------------------------------------------------------
std::string GetSamplingPattern(int i_Val)
{
	std::string str = "\"scatter\"";
	if ( i_Val == 0 )
	{
		str = "\"linear\"";
	}
	return str;
}

//--------------------------------------------------------------------
// WriteHeader()
//--------------------------------------------------------------------
void WriteHeader(std::ofstream & o_OutFile)
{
	o_OutFile << "verbose on" << endl;
	//o_OutFile << "link \"base.so\"" << endl;
	//o_OutFile << "$include <base.mi>" << endl;
	o_OutFile << "registry \"{_MI_REG_METASL_BACKEND}\" value \"LLVM\" end registry" << endl;
	o_OutFile << endl;
}

//--------------------------------------------------------------------
// WriteOptions()
//--------------------------------------------------------------------
void WriteOptions(std::ofstream & o_OutFile , mrayGlobalData& io_GlobalData)
{	
	o_OutFile << "options \"opt\"" << endl;

	int mraySampleMin = 0;
	int mraySampleMax = 2;

	if ( io_GlobalData.m_Options.m_bOverrideMSPSampling )
	{
		mraySampleMin = io_GlobalData.m_Options.m_MinCaptureSamples;
		mraySampleMax = io_GlobalData.m_Options.m_MaxCaptureSamples;
	}
	else
	{
		mraySampleMin = io_GlobalData.m_CaptureSampling - 2;
		mraySampleMax = io_GlobalData.m_CaptureSampling;
	}

	WriteParamInt2(o_OutFile,"samples",mraySampleMin,mraySampleMax);

	o_OutFile << "\tfilter " << GetFilterMRayForm( io_GlobalData.m_FilterFunc ) << " " << 
			  io_GlobalData.m_FilterWidth << " " << io_GlobalData.m_FilterWidth << endl;

	WriteParamFloat4(o_OutFile,"contrast",io_GlobalData.m_Options.m_AAContrast,
										  io_GlobalData.m_Options.m_AAContrast,
										  io_GlobalData.m_Options.m_AAContrast,
										  io_GlobalData.m_Options.m_AAContrast);
	WriteParamInt3(o_OutFile,"trace depth",	io_GlobalData.m_Options.m_NumReflBounces,
											io_GlobalData.m_Options.m_NumRefrBounces,
											io_GlobalData.m_Options.m_MrayMaxTraceDepth);
	//WriteParamStr(o_OutFile,"\"ambient occlusion\"",io_GlobalData.m_Options.m_bAO?"on":"off");

	// turn this on to cast photons from each light
	WriteParamStr(o_OutFile,"globillum","off");
	//WriteParamStr(o_OutFile,"shadow",((io_GlobalData.m_Options.m_bEnableShadows||io_GlobalData.m_bRenderingShadowsOnly)?"on":"off"));

	if ((io_GlobalData.m_Options.m_bEnableShadows || io_GlobalData.m_bRenderingShadowsOnly) && io_GlobalData.m_Options.m_MrayShadowType == SHADOWMAP)
	{
		o_OutFile << "\tshadowmap on" << endl;
	}
	
	if ((io_GlobalData.m_Options.m_bFinalGather || io_GlobalData.m_bRenderingGIOnly) && !io_GlobalData.m_bRenderingAOOnly && !io_GlobalData.m_bRenderingReflectionsOnly)
	{
		WriteParamStr(o_OutFile,"finalgather","on");
		WriteParamInt1(o_OutFile,"finalgather accuracy", io_GlobalData.m_Options.m_FGNRays);
		/*if (io_GlobalData.m_bFGBlur)
		{
			WriteParamFloat3(o_OutFile,"finalgather accuracy", io_GlobalData.m_Options.m_FGNRays,
				io_GlobalData.m_GIData.blurWidth, io_GlobalData.m_GIData.blurSharpness);
		}
		else
		{
			WriteParamFloat3(o_OutFile,"finalgather accuracy", io_GlobalData.m_Options.m_FGNRays,
				0.01f, 0.001f);
		}*/
		
		WriteParamInt3(o_OutFile,"finalgather trace depth", 
			io_GlobalData.m_Options.m_FGNRefl,io_GlobalData.m_Options.m_FGNRefr,io_GlobalData.m_Options.m_FGNDiffuse);

		//WriteParamFloat3(o_OutFile,"finalgather scale", 
		//	io_GlobalData.m_GIData.color.GetX() * io_GlobalData.m_GIData.contrast * 0.2f,
		//	io_GlobalData.m_GIData.color.GetY() * io_GlobalData.m_GIData.contrast * 0.2f,
		//	io_GlobalData.m_GIData.color.GetZ() * io_GlobalData.m_GIData.contrast * 0.2f);
		//float atten = io_GlobalData.m_GIData.attenuation;
		//if (atten == 0.0f)
		//	atten = 0.001f;
		//WriteParamFloat1(o_OutFile,"finalgather falloff", 0); 
		//	io_GlobalData.m_GIData.radiusNear / atten);

		/*WriteParamFloat1(o_OutFile,"\"finalgather normal tolerance\"", 
			io_GlobalData.m_GIData.angleBias);*/
		//WriteParamInt1(o_OutFile,"finalgather filter", 1);
			//floor(io_GlobalData.m_GIData.blurWidth));

		if ( io_GlobalData.m_Options.m_bFGMapEnable )
		{
			if ( io_GlobalData.m_Options.m_FGMapPath.GetNumNames() > 0 )
			{
				fsLocator mapDir = io_GlobalData.m_Options.m_FGMapPath;
				mapDir.Pop();
				if ( fsFileUtil::DirectoryExists( mapDir ) )
				{
					std::string mapPath;
					fsFileUtil::LocatorToANSIFilename(io_GlobalData.m_Options.m_FGMapPath,mapPath);
					mapPath = "\"" + mapPath + "\"";
					WriteParamStr(o_OutFile,"finalgather file",mapPath);

					if ( io_GlobalData.m_Options.m_FGMapRebuild == 0 )
						WriteParamStr(o_OutFile,"finalgather rebuild","off");
					else if ( io_GlobalData.m_Options.m_FGMapRebuild == 1 )
						WriteParamStr(o_OutFile,"finalgather rebuild","on");
					else
						WriteParamStr(o_OutFile,"finalgather rebuild","freeze");
				}
			}
		}

	}
	else
	{
		WriteParamStr(o_OutFile,"finalgather","off");
	}

	if (io_GlobalData.m_Options.m_bEnableIBL)
	{
		WriteParamStr(o_OutFile, "\"environment lighting mode\"", "\"automatic\"");
		WriteParamFloat1(o_OutFile, "\"environment lighting quality\"", io_GlobalData.m_Options.m_IBLQuality);
		WriteParamInt1(o_OutFile, "\"environment lighting resolution\"", IBLMAPRES[io_GlobalData.m_Options.m_IBLMapRes % IBLMAPRESNUM]);
		WriteParamFloat1(o_OutFile, "\"environment lighting scale\"", io_GlobalData.m_Options.m_IBLScale);
		WriteParamInt1(o_OutFile, "\"environment lighting shader samples\"", io_GlobalData.m_Options.m_IBLSampleNum);
	}
	else
	{
		WriteParamStr(o_OutFile, "\"environment lighting mode\"", "\"off\"");
	}

	WriteParamStr(o_OutFile, "\"progressive\"", (io_GlobalData.m_Options.m_bProgressive?"on":"off") );
	if ( io_GlobalData.m_Options.m_bProgressive )
	{
		WriteParamInt1(o_OutFile, "\"progressive subsampling size\"", io_GlobalData.m_Options.m_ProgSubsamplingSize);
		WriteParamStr(o_OutFile, "\"progressive subsampling mode\"", GetSamplingMode(io_GlobalData.m_Options.m_ProgSubsamplingMode));
		WriteParamStr(o_OutFile, "\"progressive subsampling pattern\"", GetSamplingPattern(io_GlobalData.m_Options.m_ProgSubsamplingPattern));
		WriteParamInt1(o_OutFile, "\"progressive min samples\"", io_GlobalData.m_Options.m_ProgMinSamples);
		WriteParamInt1(o_OutFile, "\"progressive max samples\"", io_GlobalData.m_Options.m_ProgMaxSamples);
		WriteParamInt1(o_OutFile, "\"progressive max time\"", io_GlobalData.m_Options.m_ProgMaxTime);
		WriteParamFloat1(o_OutFile, "\"progressive error threshold\"", io_GlobalData.m_Options.m_ProgErrorThreshold);
	}

	o_OutFile << "end options" << endl;

	o_OutFile << endl;
}

//--------------------------------------------------------------------
// GetTexturePath()
//--------------------------------------------------------------------
std::string GetTexturePath( mrayGlobalData & io_GlobalData, std::string i_Str )
{
	std::string out;

	// Ramp or normal map
	if ( mrayExportUtil::TextureIsRewritten(i_Str,io_GlobalData) )
	{
		out = mrayExportUtil::LookupGeneratedTexName(i_Str, io_GlobalData);
	}

	// Any other texture
	else
	{
		fsLocator path = io_GlobalData.m_Textures[i_Str].second;
		fsFileUtil::LocatorToANSIFilename(path,out);
	}
	return out;
}

//--------------------------------------------------------------------
// WriteCamera()
//--------------------------------------------------------------------
void WriteCamera(std::ofstream & o_OutFile , mrayGlobalData& io_GlobalData)
{
	fsLocator toneMappingShaderPath = matShaderMgr::GetDefaultShaderPath();
	toneMappingShaderPath.Push("mray");
	toneMappingShaderPath.Push("HDRLighting");
	o_OutFile << "link \"" << toneMappingShaderPath << ".dll\"" << endl;
	o_OutFile << "$include \"" << toneMappingShaderPath << "_decl.mi\"" << endl;

	if (io_GlobalData.m_Options.m_bEnableIBL || io_GlobalData.m_bRenderingAOOnly || io_GlobalData.m_bRenderingShadowsOnly)
	{
		fsLocator envShaderPath = matShaderMgr::GetDefaultShaderPath();
		std::string envShaderPathStr;
		envShaderPath.Push("mray");
		envShaderPath.Push("Environment");
		fsFileUtil::LocatorToANSIFilename(envShaderPath,envShaderPathStr);
		o_OutFile << "link \"" << envShaderPath << ".dll\"" << endl;
		o_OutFile << "$include \"" << envShaderPath << "_decl.mi\"" << endl;

		std::string envDiffMapName = io_GlobalData.m_EnvData.diffTex;
		std::string texPathString = GetTexturePath(io_GlobalData,envDiffMapName);
		WriteTexDeclaration(o_OutFile,envDiffMapName,texPathString,io_GlobalData);
		
		/*if ( i_env.m_SpecularMap )
		{
			std::string envSpecMapName = i_env.m_Name + envSpecMapEndMray;
			std::string texPathString = GetTexturePath(io_GlobalData,envSpecMapName);
			if ( !envSTLHelpers::Contains(io_GlobalData.m_DeclaredEnvMaps,envSpecMapName) )
			{
				WriteTexDeclaration(o_OutFile,envSpecMapName,texPathString);
				io_GlobalData.m_DeclaredEnvMaps.push_back(envSpecMapName);
			}
		}*/
	}

	o_OutFile << "camera \"cam\"" << endl;

	itString sceneNameIt = io_GlobalData.m_FileName.GetLastName();
	sceneNameIt.StripExtension();
	std::string sceneName = itStringUtil::GetStdString(sceneNameIt);
	std::string ext = GetMRayFileFormat( io_GlobalData.m_Options.m_OutputFormat );

	camHDRData hdr;
	io_GlobalData.m_Camera->GetHDRParams(hdr);

	WriteParamInt1(o_OutFile,"frame",1);
	WriteParamStr(o_OutFile,"output","\"sgpu_HDRLighting\"");
	WriteParamStr(o_OutFile,"","(");
	if (io_GlobalData.m_bRenderingNormalsOnly || io_GlobalData.m_bRenderingShadowsOnly
		|| io_GlobalData.m_bRenderingAOOnly || io_GlobalData.m_bRenderingIlluminationOnly
		|| io_GlobalData.m_bRenderingReflectionsOnly)
	{
		WriteBoolComma(o_OutFile,"g_bEnableTonemap",false );
	}
	else
	{
		WriteBoolComma(o_OutFile,"g_bEnableTonemap",io_GlobalData.m_Options.m_bTonemapEnable );
	}
	/*WriteBoolComma(o_OutFile,"g_bShadowsOnly",io_GlobalData.m_bRenderingShadowsOnly );
	WriteBoolComma(o_OutFile,"g_bAOOnly",io_GlobalData.m_bRenderingAOOnly);*/
	WriteFloatComma(o_OutFile,"g_fixedLuminance",hdr.m_SceneLuminance);
	WriteFloatComma(o_OutFile,"g_fMiddleGray",hdr.m_MiddleGray);
	WriteFloatComma(o_OutFile,"g_fWhiteCutoff",hdr.m_WhiteCutoff);
	WriteParamStr(o_OutFile,"",")");
	if ( io_GlobalData.m_bRenderingReflectionsOnly )
	{
		WriteParamStr(o_OutFile,"framebuffer","\"refl_only\"");
		WriteParamStr(o_OutFile,"\tuser","on");
	}
	else if ( io_GlobalData.m_bRenderingGIOnly  )
	{
		WriteParamStr(o_OutFile,"framebuffer","\"gi_only\"");
		WriteParamStr(o_OutFile,"\tuser","on");
	}
	else
	{
		WriteParamStr(o_OutFile,"framebuffer","\"main\"");
	}	

	WriteParamStr(o_OutFile,"\tdatatype","\"+rgba_fp\"");
	WriteParamStr(o_OutFile,"\tfiletype",std::string("\"" + ext + "\""));
	WriteParamStr(o_OutFile,"\tfilename",std::string("\"" + sceneName + "." + ext + "\""));

	if ( io_GlobalData.m_Camera->IsOrthographic() )
	{
		float screenXMin = -1;
		float screenYMin = -1;
		float screenXMax = 1;
		float screenYMax = 1;
		io_GlobalData.m_Camera->GetSubViewport( screenYMax, screenYMin, screenXMin, screenXMax );

		float w = io_GlobalData.m_Camera->GetOrthoWidth() / 2.0f;
		float left = screenXMin * w;
		float right = screenXMax * w;

		float h = w / io_GlobalData.m_Camera->GetAspect();
		float top = screenYMax * h; 
		float bottom = screenYMin * h;

		WriteParamStr(o_OutFile,"focal","infinity");
		WriteParamFloat1(o_OutFile,"aperture",abs(left) + abs(right) );
	}
	else
	{	
		WriteParamFloat1(o_OutFile,"focal",1);
		WriteParamFloat1(o_OutFile,"aperture",tan((io_GlobalData.m_Camera->GetFOV()/2)*maConstants::c_fAngleToRad) * 2 );
	}

	WriteParamFloat1(o_OutFile,"aspect",((float)io_GlobalData.m_Width)/((float)io_GlobalData.m_Height));
	WriteParamFloat2(o_OutFile,"resolution",io_GlobalData.m_Width, io_GlobalData.m_Height);
	WriteParamFloat2(o_OutFile,"clip",io_GlobalData.m_Camera->GetNearClip(), io_GlobalData.m_Camera->GetFarClip());

	camDOFData dof;
	io_GlobalData.m_Camera->GetDOFParams(dof);

	if ( io_GlobalData.m_Camera->GetEnableALP() && dof.m_bEnableDOF ) 
	{
		//o_OutFile << "\tlens" << endl;
		//o_OutFile << "\t\t\"depth_of_field\" (" << endl;
		//WriteFloatComma(o_OutFile,"focus_plane_distance",io_GlobalData.m_Camera->GetFocalDistance());
		//WriteFloatComma(o_OutFile,"number_of_samples",100);
		//WriteFloatComma(o_OutFile,"lens_radius",0.2);
		//o_OutFile << "\t)" << endl;
	}

	if ( io_GlobalData.m_Options.m_bEnableIBL )
	{
		o_OutFile << "\t environment \"sgpu_environment\" (" << endl;
		WriteColorComma(o_OutFile, "diffColor", io_GlobalData.m_EnvData.diffcolor);
		WriteStrComma(o_OutFile,"diffTexture",io_GlobalData.m_EnvData.diffTex);
		WriteFloatComma(o_OutFile, "diffFactor", io_GlobalData.m_EnvData.diffFactor);
		WriteFloatComma(o_OutFile, "diffAngle", io_GlobalData.m_EnvData.diffAngle);
		o_OutFile << "\t)" << endl;
	}
	else if ( io_GlobalData.m_bRenderingAOOnly ||  io_GlobalData.m_bRenderingShadowsOnly)
	{
		o_OutFile << "\t environment \"sgpu_environment\" (" << endl;
		WriteColorComma(o_OutFile, "diffColor", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f));
		WriteFloatComma(o_OutFile, "diffFactor", 1.0f);
		WriteFloatComma(o_OutFile, "diffAngle", 0.0f);
		o_OutFile << "\t)" << endl;
	}


	o_OutFile << "end camera" << endl;

	o_OutFile << "instance \"cam_inst\" \"cam\"" << endl;

	float top, bottom, left, right;
	io_GlobalData.m_Camera->GetSubViewport(top, bottom, left, right);

	maMatrix4x4 camMat;
	// instance transform must transform from world to object space!
	// therefore this matrix can be used as-is.
	camCamera::ConstructMatrixRH(io_GlobalData.m_Camera->GetPosition(), 
				   io_GlobalData.m_Camera->GetTarget(), 
				   io_GlobalData.m_Camera->GetUp(),
				   camMat);

	std::string camTransformString = MatrixToStr(camMat);
		
	o_OutFile << "\ttransform " << camTransformString << endl;
	o_OutFile << "end instance" << endl;

	o_OutFile << endl;
}

//--------------------------------------------------------------------
// WriteSectionHeader()
//--------------------------------------------------------------------
void mrayExport::WriteSectionHeader(std::ostream & o_OutFile, std::string i_Header)
{
	o_OutFile << endl;
	o_OutFile << "#################################################" << endl;
	o_OutFile << "# " << i_Header << endl;
	o_OutFile << "#################################################" << endl;
}

//--------------------------------------------------------------------
// WriteLinkInclude()
//--------------------------------------------------------------------
void WriteLinkInclude(std::ostringstream & o_OutFile,
					  const std::string& i_SurfaceShader, 
					  const std::string& i_DisplacementShader,
					  const std::string& i_EnvironmentShader = "",
					  bool i_bMetaSL = false)
{
	// This has to happen once for each shader. Link first, then include.
	if (i_bMetaSL)
	{
		o_OutFile << "$include \"" << i_SurfaceShader << "\"" << endl;
	}
	else
	{
		o_OutFile << "link \"" << i_SurfaceShader << ".dll\"" << endl;
		o_OutFile << "$include \"" << i_SurfaceShader << "_decl.mi\"" << endl;
	}
	o_OutFile << "link \"" << i_DisplacementShader << ".dll\"" << endl;
	o_OutFile << "$include \"" << i_DisplacementShader << "_decl.mi\"" << endl;
	/*if (i_EnvironmentShader.length() > 0)
	{
		o_OutFile << "link \"" << i_EnvironmentShader << ".dll\"" << endl;
		o_OutFile << "$include \"" << i_EnvironmentShader << "_decl.mi\"" << endl;
	}*/
}

//--------------------------------------------------------------------
// WriteDisplacementDeclaration()
//--------------------------------------------------------------------
void WriteDisplacementDeclaration(std::ostringstream & o_OutFile, 
								  mrayGlobalData & io_GlobalData,
								  std::string i_ObjName,
								  matMaterial* i_Material)
{
	effDisplacementData displacementData = i_Material->GetDisplacementData();
	if ( i_Material->GetHasDisplacement() )
	{	
		std::string texUniqueName = i_ObjName + dispMapEndMray;
		std::string texPathString = GetTexturePath(io_GlobalData,texUniqueName);
		WriteTexDeclaration(o_OutFile,texUniqueName,texPathString,io_GlobalData,false);
	}
}

//--------------------------------------------------------------------
// WriteNormalMapDeclaration()
//--------------------------------------------------------------------
void WriteNormalMapDeclaration(std::ostringstream & o_OutFile, 
							   mrayGlobalData & io_GlobalData,
							   std::string i_ObjName,
							   matMaterial* i_Material)
{
	effNormalsData normalsData = i_Material->GetNormalsData();
	if ( normalsData.m_pNormalMap )
	{
		std::string texUniqueName = i_ObjName + normMapEndMray;
		std::string texPathString = GetTexturePath(io_GlobalData,texUniqueName);
		WriteTexDeclaration(o_OutFile,texUniqueName,texPathString,io_GlobalData);
	}
}

//--------------------------------------------------------------------
// WriteShaderDeclaration()
//--------------------------------------------------------------------
void WriteShaderDeclarations(std::ostringstream & o_OutFile, 
							 mrayGlobalData & io_GlobalData,
							 std::string i_ObjName,
							 matMaterial* i_Material,
							 bool i_IsMetaSL)
{
	// Texture declarations
	std::vector<effParamTexture*> textureParams;
	i_Material->GetMaterialLayer(0)->GetShaderParams()->GetAllTextureParams(textureParams);
	for ( int i = 0 ; i < textureParams.size() ; i++ )
	{
		effParamTexture* currParam = textureParams[i];	
		if ( currParam->GetRampTexture() || currParam->GetProperty().GetValue().GetNumNames() > 0 )
		{
			std::string texUniqueName = mrayExport::GenerateTextureParamName(i_ObjName, currParam->GetName(), i_IsMetaSL);
			std::string texPathString = GetTexturePath(io_GlobalData,texUniqueName);
			WriteTexDeclaration(o_OutFile,texUniqueName,texPathString,io_GlobalData);
		}
	}
}

//--------------------------------------------------------------------
// WriteEnvironmentDeclaration()
//--------------------------------------------------------------------
void WriteEnvironmentDeclarations(std::ostringstream & o_OutFile, 
								  const g3dAmbientEnvState& i_env,
								  mrayGlobalData & io_GlobalData)
{
	if ( i_env.m_DiffuseMap )
	{
		std::string envDiffMapName = i_env.m_Name + envDiffMapEndMray;
		std::string texPathString = GetTexturePath(io_GlobalData,envDiffMapName);
		WriteTexDeclaration(o_OutFile,envDiffMapName,texPathString,io_GlobalData);
	}
	if ( i_env.m_SpecularMap )
	{
		std::string envSpecMapName = i_env.m_Name + envSpecMapEndMray;
		std::string texPathString = GetTexturePath(io_GlobalData,envSpecMapName);
		WriteTexDeclaration(o_OutFile,envSpecMapName,texPathString,io_GlobalData);
	}
}

//--------------------------------------------------------------------
// WriteGIData()
//--------------------------------------------------------------------
void WriteGIData(std::ostringstream & o_OutFile,
				 mrayGlobalData& io_GlobalData,
				 g3dFragment* i_pFrag)
{
	WriteBoolComma(o_OutFile,"bEnableGI",(io_GlobalData.m_Options.m_bFinalGather  || io_GlobalData.m_bRenderingGIOnly) && 
										 !io_GlobalData.m_bRenderingAOOnly && !io_GlobalData.m_bRenderingReflectionsOnly &&
										 i_pFrag->GetReceivesGI() );
}

//--------------------------------------------------------------------
// WriteAOData()
//--------------------------------------------------------------------
void WriteAOData(std::ostringstream & o_OutFile,
				 mrayGlobalData& io_GlobalData,
				 g3dFragment* i_pFrag)
{
	WritePoint3dComma(o_OutFile,"aoColor",io_GlobalData.m_AOData.color);
	WriteFloatComma(o_OutFile,"aoRadiusNear",io_GlobalData.m_AOData.radiusNear);
	WriteFloatComma(o_OutFile,"aoRadiusFar",io_GlobalData.m_AOData.radiusFar);
	WriteFloatComma(o_OutFile,"aoAngleBias",io_GlobalData.m_AOData.angleBias);
	WriteFloatComma(o_OutFile,"aoAttenuation",io_GlobalData.m_AOData.attenuation);
	WriteFloatComma(o_OutFile,"aoContrast",io_GlobalData.m_AOData.contrast);
	WriteFloatComma(o_OutFile,"aoCamNear",io_GlobalData.m_Camera->GetNearClip());
	WriteFloatComma(o_OutFile,"aoCamFar",io_GlobalData.m_Camera->GetFarClip());
	WriteFloatComma(o_OutFile,"aoSamples",io_GlobalData.m_Options.m_AOSamples);
	WriteBoolComma(o_OutFile,"bEnableAO",(io_GlobalData.m_Options.m_bAO || io_GlobalData.m_bRenderingAOOnly) && 
										 !io_GlobalData.m_bRenderingGIOnly &&
										 i_pFrag->GetReceivesOcclusion() );
}

//--------------------------------------------------------------------
// WriteUVData()
//--------------------------------------------------------------------
void WriteUVData(std::ostringstream & o_OutFile,
				 matMaterial* i_Material)
{
	effUVTransform uvTransData = i_Material->GetUVTransform();
	WriteFloatComma(o_OutFile,"u_scale",uvTransData.m_UScale);
	WriteFloatComma(o_OutFile,"v_scale",uvTransData.m_VScale);
	WriteFloatComma(o_OutFile,"u_offset",uvTransData.m_UTrans);
	WriteFloatComma(o_OutFile,"v_offset",uvTransData.m_VTrans);
	WriteFloatComma(o_OutFile,"uv_rotation",(uvTransData.m_UVAngle)*maConstants::c_fAngleToRad);
}

//--------------------------------------------------------------------
// WriteSwlEnvironmentData()
//--------------------------------------------------------------------
void WriteSwlEnvironmentData(std::ostringstream & o_OutFile,
						const g3dAmbientEnvState& i_env)
{
	if (!i_env.m_bEnableSwlEnv)
		return;

	// the material is an instance of a shader.
	o_OutFile << "\t environment \"sgpu_environment\" (" << endl;

	WriteColorComma(o_OutFile, "envColor", i_env.m_DiffuseColor);
	//WriteColorComma(o_OutFile, "envColor", maFloatRGBA(1.0f, 0.0f, 0.0f, 1.0f));

	o_OutFile << "\t)" << endl;
}

//--------------------------------------------------------------------
// WriteDisplacementData()
//--------------------------------------------------------------------
void WriteDisplacementData(std::ostringstream & o_OutFile, 
						   std::string i_ObjName,
						   matMaterial* i_Material,
						   mrayGlobalData& io_GlobalData
						   )
{
	effDisplacementData displacementData = i_Material->GetDisplacementData();
	// no displacement map, return
	if (!i_Material->GetHasDisplacement() || GetTexturePath(io_GlobalData,i_ObjName + dispMapEndMray) == "")
		return;

	// the material is an instance of a shader.
	o_OutFile << "\t displace \"sgpu_displacement\" (" << endl;

	{
		WriteStrComma(o_OutFile,"dispMap",i_ObjName + dispMapEndMray);
		WriteFloatComma(o_OutFile,"dispMapScale",displacementData.m_Scale);
		WriteFloatComma(o_OutFile,"dispMapBias",displacementData.m_Bias);
		WriteFloatComma(o_OutFile,"dispMapBlur",displacementData.m_Blur);

		maVector3d vec = maVector3d(displacementData.m_ObjUVScale.m_X, displacementData.m_ObjUVScale.m_Y, 0);
		WriteVector3dComma(o_OutFile,"dispMapUVScale",vec);
		vec.Set(1,1,1);
		if (displacementData.m_pDisplacementMap)
		{
			vec.Set(displacementData.m_pDisplacementMap->GetWidth(),
					displacementData.m_pDisplacementMap->GetHeight(),
					0);
		}
		WriteVector3dComma(o_OutFile,"dispMapSize",vec);
		//WriteFloatComma(o_OutFile,"objScale",vec);
	}

	WriteUVData(o_OutFile,i_Material);

	o_OutFile << "\t)" << endl;
}

//--------------------------------------------------------------------
// WriteNormalMapData()
//--------------------------------------------------------------------
void WriteNormalMapData(std::ostringstream & o_OutFile,
						std::string i_ObjName,
						matMaterial* i_Material,
						mrayGlobalData& io_GlobalData)
{
	effNormalsData normalsData = i_Material->GetNormalsData();
	if ( normalsData.m_pNormalMap && GetTexturePath(io_GlobalData,i_ObjName + normMapEndMray) != "" )
	{
		WriteStrComma(o_OutFile,"normalMap",i_ObjName + normMapEndMray);
		WriteFloatComma(o_OutFile,"normalMapScale",normalsData.m_BumpScale);
	}	
}

//--------------------------------------------------------------------
// TrimMetaSLVariableName()
//--------------------------------------------------------------------
void TrimMetaSLVariableName(std::string& io_Name, bool i_IsMetaSL)
{
	static const std::string sgpuPrefix = "sgpu_";
	if (i_IsMetaSL)
	{
		std::string prefix = io_Name.substr(0,sgpuPrefix.size());
		if (prefix == sgpuPrefix)
		{
			io_Name = io_Name.substr(sgpuPrefix.size());
		}
	}
}

//--------------------------------------------------------------------
// Make a unique name for a material shader's texture parameter.
//--------------------------------------------------------------------
std::string mrayExport::GenerateTextureParamName(std::string i_ObjFragMatName, std::string i_ParamName, bool i_IsMetaSL)
{
	std::string trimmedName = i_ParamName;
	TrimMetaSLVariableName(trimmedName, i_IsMetaSL);
	return i_ObjFragMatName + "-" + trimmedName;
}

//--------------------------------------------------------------------
// WriteShaderData()
//--------------------------------------------------------------------
void WriteShaderData(std::ostringstream & o_OutFile,
					 std::string i_ObjName,
					 matMaterial* i_Material,
					 std::string i_ShaderName,
					 mrayGlobalData& io_GlobalData,
					 bool i_IsMetaSL = false)
{
	std::vector<effParamTexture*> textureParams;
	i_Material->GetMaterialLayer(0)->GetShaderParams()->GetAllTextureParams(textureParams);
	for ( int i = 0 ; i < textureParams.size() ; i++ )
	{
		effParamTexture* currParam = textureParams[i];	
		if ( mrayExportUtil::IsDeadParam(currParam->GetName(),i_ShaderName) ) continue;
		if ( currParam->GetRampTexture() || currParam->GetProperty().GetValue().GetNumNames() > 0 )
		{
			std::string currParamName = currParam->GetName();
			std::string texUniqueName = mrayExport::GenerateTextureParamName(i_ObjName, currParam->GetName(), i_IsMetaSL);
			if ( GetTexturePath(io_GlobalData,texUniqueName) != "" )
			{
				TrimMetaSLVariableName(currParamName, i_IsMetaSL);
				WriteStrComma(o_OutFile,currParamName,texUniqueName);
			}
		}
	}

	std::vector<effParamColor*> colorParams;
	i_Material->GetMaterialLayer(0)->GetShaderParams()->GetAllColorParams(colorParams);
	for ( int i = 0 ; i < colorParams.size() ; i++ )
	{
		effParamColor* currParam = colorParams[i];
		if ( mrayExportUtil::IsDeadParam(currParam->GetName(),i_ShaderName) ) continue;
		std::string currParamName = currParam->GetName();
		TrimMetaSLVariableName(currParamName, i_IsMetaSL);
		WriteColorComma(o_OutFile,currParamName,currParam->GetProperty().GetValue());
	}

	std::vector<effParamFloat*> floatParams;
	i_Material->GetMaterialLayer(0)->GetShaderParams()->GetAllFloatParams(floatParams);
	for ( int i = 0 ; i < floatParams.size() ; i++ )
	{
		effParamFloat* currParam = floatParams[i];
		if ( mrayExportUtil::IsDeadParam(currParam->GetName(),i_ShaderName) ) continue;
		std::string currParamName = currParam->GetName();
		TrimMetaSLVariableName(currParamName, i_IsMetaSL);
		WriteFloatComma(o_OutFile,currParamName,currParam->GetProperty().GetValue());
	}

	std::vector<effParamBool*> boolParams;
	i_Material->GetMaterialLayer(0)->GetShaderParams()->GetAllBoolParams(boolParams);
	for ( int i = 0 ; i < boolParams.size() ; i++ )
	{
		effParamBool* currParam = boolParams[i];
		if ( mrayExportUtil::IsDeadParam(currParam->GetName(),i_ShaderName) ) continue;
		std::string currParamName = currParam->GetName();
		TrimMetaSLVariableName(currParamName, i_IsMetaSL);
		WriteBoolComma(o_OutFile,currParamName,currParam->GetProperty().GetValue());
	}

	std::vector<effParamInt*> intParams;
	i_Material->GetMaterialLayer(0)->GetShaderParams()->GetAllIntParams(intParams);
	for ( int i = 0 ; i < intParams.size() ; i++ )
	{
		effParamInt* currParam = intParams[i];
		if ( mrayExportUtil::IsDeadParam(currParam->GetName(),i_ShaderName) ) continue;
		std::string currParamName = currParam->GetName();
		TrimMetaSLVariableName(currParamName, i_IsMetaSL);
		WriteIntComma(o_OutFile,currParamName,currParam->GetProperty().GetValue());
	}
}

//--------------------------------------------------------------------
// WriteEnvironmentData()
//--------------------------------------------------------------------
void WriteEnvironmentData(std::ostringstream & o_OutFile, 
						  const g3dAmbientEnvState& i_env,
						  bool i_bEnableIBL = false)
{
	WriteBoolComma(o_OutFile,"bEnableSWL", i_bEnableIBL);
	if ( i_env.m_DiffuseMap )
	{
		std::string envDiffMapName = i_env.m_Name + envDiffMapEndMray;
		WriteStrComma(o_OutFile,"diffuseEnvMap",envDiffMapName);
	}
	if ( i_env.m_SpecularMap )
	{
		std::string envSpecMapName = i_env.m_Name + envSpecMapEndMray;
		WriteStrComma(o_OutFile,"specularEnvMap",envSpecMapName);
	}
	WriteColorComma(o_OutFile,"g_envDiffuseColor",i_env.m_DiffuseColor);
	WriteFloatComma(o_OutFile,"g_diffuseFactor",i_env.m_DiffuseFactor);
	WriteFloatComma(o_OutFile,"g_diffuseEnvAngle",i_env.m_DiffuseAngle);
	WriteColorComma(o_OutFile,"g_envSpecularColor",i_env.m_SpecularColor);
	WriteFloatComma(o_OutFile,"g_specularFactor",i_env.m_SpecularFactor);
	WriteFloatComma(o_OutFile,"g_specularEnvAngle",i_env.m_SpecularAngle);
}

//--------------------------------------------------------------------
// WriteTransparencyData()
//--------------------------------------------------------------------
void WriteTransparencyData(std::ostringstream & o_OutFile,
						   std::string i_ObjName,
						   matMaterial* i_Material,
						   mrayGlobalData& io_GlobalData)
{

	std::vector<effParamFloat*> floatParams;
	i_Material->GetMaterialLayer(0)->GetShaderParams()->GetAllFloatParams(floatParams);
	for ( int i = 0 ; i < floatParams.size() ; i++ )
	{
		effParamFloat* currParam = floatParams[i];
		if ( currParam->GetName() == "g_transparency" )
		{
			WriteFloatComma(o_OutFile,currParam->GetName(),currParam->GetProperty().GetValue());
		}
	}

	const bool isMetaSL = false;

	std::vector<effParamTexture*> textureParams;
	i_Material->GetMaterialLayer(0)->GetShaderParams()->GetAllTextureParams(textureParams);
	for ( int i = 0 ; i < textureParams.size() ; i++ )
	{
		effParamTexture* currParam = textureParams[i];	
		if ( currParam->GetName() == "transparencyMap" )
		{		
			if ( currParam->GetRampTexture() || currParam->GetProperty().GetValue().GetNumNames() > 0 )
			{
				std::string currParamName = currParam->GetName();				
				std::string texUniqueName = mrayExport::GenerateTextureParamName(i_ObjName, currParamName, isMetaSL);
				if ( GetTexturePath(io_GlobalData,texUniqueName) != "" )
				{
					TrimMetaSLVariableName(currParamName, isMetaSL);
					WriteStrComma(o_OutFile,currParamName,texUniqueName);
				}
			}
		}
	}
}


//--------------------------------------------------------------------
// WriteGenericMaterial()
//--------------------------------------------------------------------
void WriteGenericMaterial(std::ostringstream & o_OutFile, mrayGlobalData & io_GlobalData, std::string i_MatName)
{
	o_OutFile << "material \"" << i_MatName << "\" opaque" << endl;
	o_OutFile << "\t\"mib_illum_phong\" (" << endl;
	o_OutFile << "\t\t\"ambience\"  .3 .3 .3," << endl;
	o_OutFile << "\t\t\"ambient\"   .5 .5 .5," << endl;
	o_OutFile << "\t\t\"diffuse\"   .7 .7 .7," << endl;
	o_OutFile << "\t\t\"specular\"  0 0 0," << endl;
	o_OutFile << "\t\t\"exponent\"  50," << endl;
	o_OutFile << "\t\t\"mode\"      4" << endl;
	o_OutFile << "\t)" << endl;
	o_OutFile << "end material" << endl;
}

//--------------------------------------------------------------------
// WriteAOMaterial()
//--------------------------------------------------------------------
void WriteAOMaterial(std::ostringstream & o_OutFile, 
						  mrayGlobalData & io_GlobalData, 
						  std::string i_ObjName,  
						  std::string i_MatName, 
						  matMaterial* i_Material, 
						  g3dFragment* i_pFrag,
						  const fsLocator& i_SurfaceShaderPath,
						  const std::string& i_DisplacementShaderPath)
{
	//o_OutFile << "material \"" << i_MatName << "\" opaque" << endl;
	// generic ao
	//o_OutFile << "\t\"mib_amb_occlusion\" (" << endl;
	//o_OutFile << "\t\t\"samples\"            64," << endl;
	//o_OutFile << "\t\t\"bright\"             1 1 1 1," << endl;
	//o_OutFile << "\t\t\"dark\"               0 0 0 0," << endl;
	//o_OutFile << "\t\t\"spread\"             0.8," << endl;
	//o_OutFile << "\t\t\"max_distance\"       200," << endl;
	//o_OutFile << "\t\t\"reflective\"         off," << endl;
	//o_OutFile << "\t\t\"output_mode\"        0," << endl;
	//o_OutFile << "\t\t\"occlusion_in_alpha\" off," << endl;
	// Version 2 parameters
	//o_OutFile << "\t\t\"falloff\"            1.0," << endl;
	//o_OutFile << "\t\t\"id_inclexcl\"        0," << endl;
	//o_OutFile << "\t\t\"id_nonself\"         0," << endl;
	//o_OutFile << "\t)" << endl;
	//o_OutFile << "end material" << endl;

	fsLocator normalShaderPath = i_SurfaceShaderPath;
	normalShaderPath.Push("AO");
	std::string shaderPathStr;
	fsFileUtil::LocatorToANSIFilename(normalShaderPath,shaderPathStr);

	// Write link & include
	WriteLinkInclude(o_OutFile, shaderPathStr,i_DisplacementShaderPath);

	// Declarations
	WriteDisplacementDeclaration(o_OutFile, io_GlobalData, i_ObjName, i_Material);
	WriteNormalMapDeclaration(o_OutFile, io_GlobalData, i_ObjName, i_Material);

	o_OutFile << "material \"" << i_MatName << "\"" << endl;
	o_OutFile << "\t\"sgpu_illum_AO\" (" << endl;

	// Data
	WriteTransparencyData(o_OutFile,i_ObjName,i_Material,io_GlobalData);
	WriteUVData(o_OutFile,i_Material);
	WriteNormalMapData(o_OutFile,i_ObjName,i_Material,io_GlobalData);
	WriteAOData(o_OutFile,io_GlobalData,i_pFrag);
	
	o_OutFile << "\t\t\"mode\"      4" << endl;
	o_OutFile << "\t)" << endl;

	WriteDisplacementData(o_OutFile, i_ObjName, i_Material,io_GlobalData);
	o_OutFile << "end material" << endl;
}

//--------------------------------------------------------------------
// WriteNormalsMaterial()
//--------------------------------------------------------------------
void WriteNormalsMaterial(std::ostringstream & o_OutFile, 
						  mrayGlobalData & io_GlobalData, 
						  std::string i_ObjName,  
						  std::string i_MatName, 
						  matMaterial* i_Material,
						  const fsLocator& i_SurfaceShaderPath,
						  const std::string& i_DisplacementShaderPath)
{
	fsLocator normalShaderPath = i_SurfaceShaderPath;
	normalShaderPath.Push("Normals");
	std::string shaderPathStr;
	fsFileUtil::LocatorToANSIFilename(normalShaderPath,shaderPathStr);

	// Write link & include
	WriteLinkInclude(o_OutFile, shaderPathStr,i_DisplacementShaderPath);

	// Declarations
	WriteDisplacementDeclaration(o_OutFile, io_GlobalData, i_ObjName, i_Material);
	WriteNormalMapDeclaration(o_OutFile, io_GlobalData, i_ObjName, i_Material);

	o_OutFile << "material \"" << i_MatName << "\"" << endl;
	o_OutFile << "\t\"sgpu_illum_Normals\" (" << endl;

	maMatrix4x4 camMat;
	io_GlobalData.m_Camera->GetCameraMatrix(camMat);	

	// Data
	WriteMatrixComma(o_OutFile, "CameraMatrix", camMat);
	WriteUVData(o_OutFile,i_Material);
	WriteNormalMapData(o_OutFile,i_ObjName,i_Material,io_GlobalData);
	
	o_OutFile << "\t)" << endl;

	WriteDisplacementData(o_OutFile, i_ObjName, i_Material,io_GlobalData);
	o_OutFile << "end material" << endl;
}

//--------------------------------------------------------------------
// WriteIlluminationMaterial()
//--------------------------------------------------------------------
void WriteIlluminationMaterial(std::ostringstream & o_OutFile, 
						  mrayGlobalData & io_GlobalData, 
						  std::string i_ObjName,  
						  std::string i_MatName, 
						  matMaterial* i_Material,
						  const fsLocator& i_SurfaceShaderPath,
						  const std::string& i_DisplacementShaderPath)
{
	fsLocator normalShaderPath = i_SurfaceShaderPath;
	normalShaderPath.Push("Illumination");
	std::string shaderPathStr;
	fsFileUtil::LocatorToANSIFilename(normalShaderPath,shaderPathStr);

	// Write link & include
	WriteLinkInclude(o_OutFile, shaderPathStr,i_DisplacementShaderPath);

	// Declarations
	WriteDisplacementDeclaration(o_OutFile, io_GlobalData, i_ObjName, i_Material);
	WriteNormalMapDeclaration(o_OutFile, io_GlobalData, i_ObjName, i_Material);

	o_OutFile << "material \"" << i_MatName << "\"" << endl;
	o_OutFile << "\t\"sgpu_illum_Illumination\" (" << endl;

	// Data
	WriteTransparencyData(o_OutFile,i_ObjName,i_Material,io_GlobalData);
	WriteUVData(o_OutFile,i_Material);
	WriteNormalMapData(o_OutFile,i_ObjName,i_Material,io_GlobalData);
	
	o_OutFile << "\t\t\"mode\"      4" << endl;
	o_OutFile << "\t)" << endl;

	WriteDisplacementData(o_OutFile, i_ObjName, i_Material,io_GlobalData);
	o_OutFile << "end material" << endl;
}

//--------------------------------------------------------------------
// WriteShadowsMaterial()
//--------------------------------------------------------------------
void WriteShadowsMaterial(std::ostringstream & o_OutFile, 
						  mrayGlobalData & io_GlobalData, 
						  std::string i_ObjName,  
						  std::string i_MatName, 
						  matMaterial* i_Material,
						  const fsLocator& i_SurfaceShaderPath,
						  const std::string& i_DisplacementShaderPath)
{
	fsLocator normalShaderPath = i_SurfaceShaderPath;
	normalShaderPath.Push("Shadows");
	std::string shaderPathStr;
	fsFileUtil::LocatorToANSIFilename(normalShaderPath,shaderPathStr);

	// Write link & include
	WriteLinkInclude(o_OutFile, shaderPathStr,i_DisplacementShaderPath);

	// Declarations
	WriteDisplacementDeclaration(o_OutFile, io_GlobalData, i_ObjName, i_Material);
	WriteNormalMapDeclaration(o_OutFile, io_GlobalData, i_ObjName, i_Material);

	o_OutFile << "material \"" << i_MatName << "\"" << endl;
	o_OutFile << "\t\"sgpu_illum_Shadows\" (" << endl;

	// Data
	WriteTransparencyData(o_OutFile,i_ObjName,i_Material,io_GlobalData);
	WriteUVData(o_OutFile,i_Material);
	WriteNormalMapData(o_OutFile,i_ObjName,i_Material,io_GlobalData);
	
	o_OutFile << "\t\t\"mode\"      4" << endl;
	o_OutFile << "\t)" << endl;

	WriteDisplacementData(o_OutFile, i_ObjName, i_Material,io_GlobalData);
	o_OutFile << "end material" << endl;
}

//--------------------------------------------------------------------
// WriteMetaSLMaterial()
//--------------------------------------------------------------------
void WriteMetaSLMaterial(std::ostringstream & o_OutFile, 
						 mrayGlobalData & io_GlobalData, 
						 std::string i_ObjName, 
						 std::string i_MatName, 
						 std::string i_ShaderName,
						 matMaterial* i_Material, 
						 g3dFragment* i_pFrag,
						 g3dAmbientEnvState* i_AmbientData,
						 const fsLocator& i_SurfaceShaderPath,
						 const std::string& i_DisplacementShaderPath,
						 const std::string& i_EnvironmentShaderPath)
{
	// get shader name. This should be part of the metaSL data that could be passed in
	// without need for modifying it.
	std::string shaderPathStr;
	fsFileUtil::LocatorToANSIFilename(i_SurfaceShaderPath,shaderPathStr);

	// Write link & include
	WriteLinkInclude(o_OutFile,shaderPathStr,i_DisplacementShaderPath,i_EnvironmentShaderPath, true);

	// Declarations
	WriteShaderDeclarations(o_OutFile, io_GlobalData, i_ObjName, i_Material, true);
	WriteEnvironmentDeclarations( o_OutFile, *i_AmbientData, io_GlobalData );
//	WriteNormalMapDeclaration(o_OutFile, io_GlobalData, i_ObjName, i_Material);
	WriteDisplacementDeclaration(o_OutFile, io_GlobalData, i_ObjName, i_Material);

	// The material is an instance of a shader.
	o_OutFile << "material \"" << i_MatName << "\"" << endl;
	o_OutFile << "\t\"" << i_ShaderName << "\" (" << endl;

	// Data
	WriteShaderData(o_OutFile, i_ObjName, i_Material, i_ShaderName, io_GlobalData, true);
//	WriteEnvironmentData(o_OutFile, environment);
//	WriteUVData(o_OutFile,i_Material);
//	WriteNormalMapData(o_OutFile,i_ObjName,i_Material);
//	WriteAOData(o_OutFile,io_GlobalData,i_pFrag);
//	WriteGIData(o_OutFile,io_GlobalData,i_pFrag);

	o_OutFile << "\t\t\"mode\"      4" << endl;
	o_OutFile << "\t)" << endl;

	WriteSwlEnvironmentData(o_OutFile, *i_AmbientData);
	WriteDisplacementData(o_OutFile, i_ObjName, i_Material,io_GlobalData);

	o_OutFile << "end material" << endl;
}

//--------------------------------------------------------------------
// WriteBeautyMaterial()
//--------------------------------------------------------------------
void WriteBeautyMaterial(std::ostringstream & o_OutFile, 
						 mrayGlobalData & io_GlobalData, 
						 std::string i_ObjName, 
						 std::string i_MatName, 
						 matMaterial* i_Material, 
						 g3dFragment* i_pFrag,
						 g3dAmbientEnvState* i_AmbientData,
						 const fsLocator& i_SurfaceShaderPath,
						 const std::string& i_DisplacementShaderPath,
						 const std::string& i_EnvironmentShaderPath)
{
	// Setup environment
	g3dAmbientEnvState environment;
	if (io_GlobalData.m_bRenderEnvironments)
	{
		environment = *i_AmbientData;
		if (!io_GlobalData.m_bRenderDiffuse)
		{
			environment.m_DiffuseMap = NULL;
			environment.m_DiffuseFactor = 0;
			environment.m_DiffuseColor = maFloatRGBA(0,0,0,0);
		}
		if (!io_GlobalData.m_bRenderSpecular)
		{
			environment.m_SpecularMap = NULL;
			environment.m_SpecularFactor = 0;
			environment.m_SpecularColor = maFloatRGBA(0,0,0,0);
		}
	}

	// Get shader info
	fsLocator shaderLoc = i_Material->GetMaterialLayer(0)->GetShaderParams()->GetShaderName();
	itString shaderNameIt = shaderLoc.GetLastName();
	itString ext;
	shaderNameIt.GetExtension(ext);
	itStringUtil::ToLower(ext);
	bool bUseMetaSL = false;
	fsLocator mslShaderLoc;
	std::string mslShaderNameStr;
	//*****************
	// TODO : re-enable this by removing the "false &&" when ready to test.
	// TODO : get an accurate shader name from the mfx file and pass it through to the mi.
	//*****************
	if (ext == itString("mfx"))
	{
		// Get the shader name
		std::string shaderFileName = itStringUtil::GetStdString(shaderNameIt);

		// look up shader path in the map
		shaderLoc = matShaderMgr::ResolveShaderPath(shaderLoc);
		std::map<fsLocator, mrayMetaSLShaderData>::iterator shaderLocEntry = io_GlobalData.m_MSLShaderMap.find(shaderLoc);
		if (shaderLocEntry != io_GlobalData.m_MSLShaderMap.end())
		{
			bUseMetaSL = true;
			mslShaderLoc = shaderLocEntry->second.m_MSLLocator;
			mslShaderNameStr = shaderLocEntry->second.m_ShaderName;
		}
	}

	if (bUseMetaSL)
	{
		WriteMetaSLMaterial(o_OutFile, 
						 io_GlobalData, 
						 i_ObjName, 
						 i_MatName, 
						 mslShaderNameStr,
						 i_Material, 
						 i_pFrag,
						 &environment,
						 mslShaderLoc,
						 i_DisplacementShaderPath,
						 i_EnvironmentShaderPath);
		return;
	}


	std::string shaderPathStr;
	
	shaderNameIt.StripExtension();
	if (shaderNameIt == itString("Phong_wBump"))
		shaderNameIt = itString("Phong");
	fsLocator currShaderPath = i_SurfaceShaderPath;
	currShaderPath.Push(shaderNameIt);
	fsFileUtil::LocatorToANSIFilename(currShaderPath,shaderPathStr);

	// Write link & include
	WriteLinkInclude(o_OutFile,shaderPathStr,i_DisplacementShaderPath,i_EnvironmentShaderPath, bUseMetaSL);

	// Declarations
	WriteShaderDeclarations(o_OutFile, io_GlobalData, i_ObjName, i_Material, bUseMetaSL);
	WriteEnvironmentDeclarations( o_OutFile, environment, io_GlobalData );
	WriteNormalMapDeclaration(o_OutFile, io_GlobalData, i_ObjName, i_Material);
	WriteDisplacementDeclaration(o_OutFile, io_GlobalData, i_ObjName, i_Material);

	// The material is an instance of a shader.
	o_OutFile << "material \"" << i_MatName << "\"" << endl;
	o_OutFile << "\t\"sgpu_illum_" << itStringUtil::GetStdString(shaderNameIt) << "\" (" << endl;

	// Data
	WriteShaderData(o_OutFile, i_ObjName, i_Material, itStringUtil::GetStdString(shaderNameIt), io_GlobalData);
	if ( shaderNameIt == itString("PhongReflection") || shaderNameIt == itString("BlinnReflection") )
	{
		effReflectionMap reflData = i_Material->GetReflectionData();
		WriteBoolComma(o_OutFile,"reflEnable",( io_GlobalData.m_Options.m_bEnableReflections || io_GlobalData.m_bRenderingReflectionsOnly ) &&
												!io_GlobalData.m_bRenderingGIOnly );
		WriteIntComma(o_OutFile,"reflSamples",io_GlobalData.m_Options.m_ReflSamples);
	}
	WriteEnvironmentData(o_OutFile, environment, io_GlobalData.m_Options.m_bEnableIBL);
	WriteUVData(o_OutFile,i_Material);
	WriteNormalMapData(o_OutFile,i_ObjName,i_Material,io_GlobalData);
	WriteAOData(o_OutFile,io_GlobalData,i_pFrag);
	WriteGIData(o_OutFile,io_GlobalData,i_pFrag);

	o_OutFile << "\t\t\"mode\"      4" << endl;
	o_OutFile << "\t)" << endl;

	//WriteSwlEnvironmentData(o_OutFile, environment);
	WriteDisplacementData(o_OutFile, i_ObjName, i_Material,io_GlobalData);

	o_OutFile << "end material" << endl;

	//	float trans = 1;
	//	effParamFloat* transP = i_Material->GetMaterialLayer(0)->GetShaderParams()->m_pTransparency;
	//	if (transP)
	//	{
	//		const prtyFloat& pf = transP->GetProperty();
	//		trans = pf.GetValue();
	//	}

		// SHADOW SHADER
	//	if (trans < 1)
	//	{
	//		o_OutFile << "\tshadow \"mib_shadow_transparency\" (" << endl;
	//		o_OutFile << "\t\t\"transp\" " << trans << "," << endl;
	//		o_OutFile << "\t\t\"mode\"      0" << endl;
	//		o_OutFile << "\t)" << endl;
	//	}

		// PHOTON SHADER
	//	o_OutFile << "\tphoton \"sgpu_illum_" << itStringUtil::GetStdString(shaderNameIt) << "\" ()" << endl;
	//	o_OutFile << "\tphoton = \"sgpu_Phong_photon\" " << endl;

	//	o_OutFile << "\tphoton \"mib_photon_basic\" (" << endl;
	//	o_OutFile << "\t\t\"diffuse\"      1 1 1 1," << endl;
	//	o_OutFile << "\t\t\"specular\"      1 1 1," << endl;
	//	o_OutFile << "\t)" << endl;
}

//--------------------------------------------------------------------
// WriteMaterial()
//--------------------------------------------------------------------
void mrayExport::WriteMaterial(std::ostringstream & o_OutFile, mrayGlobalData & io_GlobalData, std::string i_ObjName, std::string i_MatName, 
								matMaterial* i_Material, g3dFragment* i_pFrag, g3dAmbientEnvState* i_AmbientData)
{
	fsLocator mrayShaderPath = matShaderMgr::GetDefaultShaderPath();
	mrayShaderPath.Push("mray");

	fsLocator dispShaderPath = mrayShaderPath;
	std::string dispShaderPathStr;
	dispShaderPath.Push("Displacement");
	fsFileUtil::LocatorToANSIFilename(dispShaderPath,dispShaderPathStr);

	fsLocator envShaderPath = mrayShaderPath;
	std::string envShaderPathStr;
	envShaderPath.Push("Environment");
	fsFileUtil::LocatorToANSIFilename(envShaderPath,envShaderPathStr);

	WriteSectionHeader(o_OutFile,i_ObjName);

	// AO pass
	if ( io_GlobalData.m_bRenderingAOOnly )
	{
		WriteAOMaterial(o_OutFile, io_GlobalData, i_ObjName, i_MatName, i_Material, i_pFrag,
						 mrayShaderPath,dispShaderPathStr);
	}

	// Normals pass
	else if ( io_GlobalData.m_bRenderingNormalsOnly )
	{
		WriteNormalsMaterial(o_OutFile, io_GlobalData, i_ObjName, i_MatName, i_Material, 
							 mrayShaderPath,dispShaderPathStr);
	}

	// Illumination pass
	else if ( io_GlobalData.m_bRenderingIlluminationOnly )
	{
		WriteIlluminationMaterial(o_OutFile, io_GlobalData, i_ObjName, i_MatName, i_Material, 
								  mrayShaderPath,dispShaderPathStr);
	}

	// Shadows Only pass
	else if ( io_GlobalData.m_bRenderingShadowsOnly )
	{
		WriteShadowsMaterial(o_OutFile, io_GlobalData, i_ObjName, i_MatName, i_Material, 
							 mrayShaderPath,dispShaderPathStr);
	}
	
	// Beauty pass
	else
	{
		WriteBeautyMaterial(o_OutFile, io_GlobalData, i_ObjName, i_MatName, i_Material, i_pFrag,
							i_AmbientData, mrayShaderPath,dispShaderPathStr,envShaderPathStr);
	}
}

//--------------------------------------------------------------------
// WriteWorldInclude()
//--------------------------------------------------------------------
void mrayExport::WriteWorldInclude(std::ofstream & o_OutFile, mrayGlobalData i_GlobalData)
{
	std::string worldStr;
	fsFileUtil::LocatorToANSIFilename(i_GlobalData.m_WorldName,worldStr);
	o_OutFile << "$include <" << worldStr << ">" << endl;
}

//--------------------------------------------------------------------
// BeginExport()
//--------------------------------------------------------------------
void mrayExport::BeginExport(std::ofstream & o_OutFile , 
							 mrayGlobalData & io_GlobalData)
{
	WriteSectionHeader(o_OutFile,"Header");
	WriteHeader(o_OutFile);

	WriteSectionHeader(o_OutFile,"Options");
	WriteOptions(o_OutFile,io_GlobalData);

	WriteSectionHeader(o_OutFile,"Camera");
	WriteCamera(o_OutFile,io_GlobalData);
}

//--------------------------------------------------------------------
// ExportPointLight()
//--------------------------------------------------------------------
void mrayExport::ExportPointLight(std::ofstream & o_OutFile,
								  mrayGlobalData & io_GlobalData,
								  mrayPointLightData i_PointLightData)
{
	fsLocator shaderPath = matShaderMgr::GetDefaultShaderPath();
	shaderPath.Push("mray");
	shaderPath.Push("PointLight");
	std::string shaderPathStr;
	fsFileUtil::LocatorToANSIFilename(shaderPath,shaderPathStr);

	o_OutFile << "link \"" << shaderPathStr << ".dll\"" << endl;
	o_OutFile << "$include <" << shaderPathStr << "_decl.mi>" << endl;

	o_OutFile << "light \"" << i_PointLightData.m_Name << "\"" << endl;

	o_OutFile << "\t\"sgpu_light_PointLight\" (" << endl;
	WriteColorComma(o_OutFile, "color", i_PointLightData.m_Color*i_PointLightData.m_Intensity);
	WritePoint3dComma(o_OutFile, "Pos", i_PointLightData.m_Position);
	WriteFloatComma(o_OutFile, "FalloffX", i_PointLightData.m_Falloff.GetX() );
	WriteFloatComma(o_OutFile, "FalloffY", i_PointLightData.m_Falloff.GetY() );
	WriteFloatComma(o_OutFile, "FalloffZ", i_PointLightData.m_Falloff.GetZ() );
	WriteFloatComma(o_OutFile, "FalloffW", 0 );
	WriteFloatComma(o_OutFile, "FalloffStart", 0 );
	WriteBoolComma(o_OutFile, "bShadowsOnly", io_GlobalData.m_bRenderingShadowsOnly);
	WriteBoolComma(o_OutFile, "bEnabled", i_PointLightData.m_bEnabled && !io_GlobalData.m_bRenderingShadowsOnly && !io_GlobalData.m_bRenderingGIOnly);
	WriteBoolComma(o_OutFile, "AffectsDiffuse", i_PointLightData.m_bAffectsDiffuse && io_GlobalData.m_bRenderDiffuse);
	WriteBoolComma(o_OutFile, "AffectsSpecular", i_PointLightData.m_bAffectsSpecular && io_GlobalData.m_bRenderSpecular);
	o_OutFile << "\t)" << endl;

/*
	o_OutFile << "\t\"mib_light_point\" (" << endl;

	o_OutFile << "\t\t\"color\" ";
	WriteColor(o_OutFile, i_PointLightData.m_Color*i_PointLightData.m_Intensity);
	o_OutFile << "," << endl;
	o_OutFile << "\t\t\"shadow\" off," << endl;
	o_OutFile << "\t\t\"factor\" 0," << endl;
//	o_OutFile << "\t\t\"factor\" " << i_PointLightData.m_Intensity << endl;

	if (i_PointLightData.m_Atten)
	{
		o_OutFile << "\t\t\"atten\" on" << endl;
		o_OutFile << "\t\t\"start\" " << i_PointLightData.m_StartAtten << endl;
		o_OutFile << "\t\t\"stop\" " << i_PointLightData.m_StopAtten << endl;
	}
	else
	{
		o_OutFile << "\t\t\"atten\" off" << endl;
	}

	o_OutFile << "\t)" << endl;
*/
	o_OutFile << "\torigin ";
	WritePoint3d(o_OutFile, i_PointLightData.m_Position);
	o_OutFile << endl;
	
//	o_OutFile << "\tenergy ";
//	WriteColor(o_OutFile, i_PointLightData.m_Color*i_PointLightData.m_Intensity);
//	o_OutFile << endl;

	o_OutFile << "end light" << endl;

	o_OutFile << "instance \"" << i_PointLightData.m_InstanceName << "\" \"" << i_PointLightData.m_Name << "\"" << endl;
	o_OutFile << "end instance" << endl;

	o_OutFile << endl;

}

//--------------------------------------------------------------------
// ExportProjLight()
//--------------------------------------------------------------------
void mrayExport::ExportProjLight(std::ofstream & o_OutFile,								 		 
								 mrayGlobalData & io_GlobalData,
								 mrayProjLightData i_ProjLightData)
{
	fsLocator shaderPath = matShaderMgr::GetDefaultShaderPath();
	shaderPath.Push("mray");
	shaderPath.Push("ProjectedLight");
	std::string shaderPathStr;
	fsFileUtil::LocatorToANSIFilename(shaderPath,shaderPathStr);
	bool bShadowMap = (io_GlobalData.m_Options.m_bEnableShadows || io_GlobalData.m_bRenderingShadowsOnly) && io_GlobalData.m_Options.m_MrayShadowType == SHADOWMAP;

	o_OutFile << "link \"" << shaderPathStr << ".dll\"" << endl;
	o_OutFile << "$include <" << shaderPathStr << "_decl.mi>" << endl;

	std::string texPathString;
	if ( !i_ProjLightData.m_TextureName.empty() )
	{
		texPathString = GetTexturePath(io_GlobalData,i_ProjLightData.m_TextureName);
		WriteTexDeclaration(o_OutFile,i_ProjLightData.m_TextureName,texPathString,io_GlobalData);
	}

	if ( bShadowMap && i_ProjLightData.m_bDirectional)
	{
		o_OutFile << "camera \"" << i_ProjLightData.m_Name << "_cam" << "\"" << endl;
		WriteParamFloat2(o_OutFile,"resolution",i_ProjLightData.m_DepthMapSize, i_ProjLightData.m_DepthMapSize);
		WriteParamFloat1(o_OutFile,"focal", i_ProjLightData.m_Scale);
		WriteParamFloat1(o_OutFile,"aperture", i_ProjLightData.m_Scale * 2.0f);
		WriteParamFloat1(o_OutFile,"aspect", i_ProjLightData.m_Aspect);
		o_OutFile << "end camera" << endl;
	}

	o_OutFile << "light \"" << i_ProjLightData.m_Name << "\"" << endl;

	o_OutFile << "\t\"sgpu_light_ProjectedLight\" (" << endl;
	WriteColorComma(o_OutFile, "color", i_ProjLightData.m_Color*i_ProjLightData.m_Intensity);
	WritePoint3dComma(o_OutFile, "Pos", i_ProjLightData.m_Position);
	WriteFloatComma(o_OutFile, "PosW", i_ProjLightData.m_bDirectional ? 0 : 1 );
	WriteBoolComma(o_OutFile, "shadow", i_ProjLightData.m_bCastShadow && (io_GlobalData.m_Options.m_bEnableShadows || 
																		  io_GlobalData.m_bRenderingShadowsOnly ||
																		  io_GlobalData.m_bRenderingIlluminationOnly) );
	WriteFloatComma(o_OutFile, "factor", 0 );
	WriteColorComma(o_OutFile, "ShadowColor", i_ProjLightData.m_ShadowColor);
	WriteFloatComma(o_OutFile, "ShadowIntensity", i_ProjLightData.m_ShadowIntensity);
	WriteFloatComma(o_OutFile, "FalloffX", i_ProjLightData.m_Falloff.GetX() );
	WriteFloatComma(o_OutFile, "FalloffY", i_ProjLightData.m_Falloff.GetY() );
	WriteFloatComma(o_OutFile, "FalloffZ", i_ProjLightData.m_Falloff.GetZ() );
	WriteFloatComma(o_OutFile, "FalloffW", 0 );
	WriteFloatComma(o_OutFile, "FalloffStart", 0 );
	WriteFloatComma(o_OutFile, "Scale", i_ProjLightData.m_Scale );
	WriteFloatComma(o_OutFile, "Range", i_ProjLightData.m_Range );
	WriteFloatComma(o_OutFile, "Aspect", i_ProjLightData.m_Aspect );
	WriteFloatComma(o_OutFile, "OuterAngle", i_ProjLightData.m_Angle );
	WriteFloatComma(o_OutFile, "InnerAngle", i_ProjLightData.m_InnerAngle );
	WriteMatrixComma(o_OutFile, "CameraMatrix", i_ProjLightData.m_CameraMatrix);
	WriteMatrixComma(o_OutFile, "ProjMatrix", i_ProjLightData.m_ProjMatrix);
	
	WriteMatrixComma(o_OutFile, "CamProjMatrix", i_ProjLightData.m_CameraMatrix * i_ProjLightData.m_ProjMatrix);

	if ( !i_ProjLightData.m_TextureName.empty() )
	{
		WriteStrComma(o_OutFile, "Texture", i_ProjLightData.m_TextureName);
	}

	WriteBoolComma(o_OutFile, "bDirectional", i_ProjLightData.m_bDirectional);
	WriteBoolComma(o_OutFile, "bPureDirectional", i_ProjLightData.m_bPureDirectional);
	WriteBoolComma(o_OutFile, "bConeLighting", i_ProjLightData.m_bConeLighting);

	WriteBoolComma(o_OutFile, "bShadowsOnly", io_GlobalData.m_bRenderingShadowsOnly);
	WriteBoolComma(o_OutFile, "bEnabled", i_ProjLightData.m_bEnabled && !io_GlobalData.m_bRenderingGIOnly);

	maPoint3d mins( -i_ProjLightData.m_Scale/2 , -(i_ProjLightData.m_Scale/2) * (1/i_ProjLightData.m_Aspect) , 0 );
	maPoint3d maxes( i_ProjLightData.m_Scale/2 , (i_ProjLightData.m_Scale/2) * (1/i_ProjLightData.m_Aspect) , i_ProjLightData.m_Range );

	maAxisBox bbox(mins,maxes);

	WriteFloatComma(o_OutFile, "bbox_min_x", bbox.GetMinX());
	WriteFloatComma(o_OutFile, "bbox_min_y", bbox.GetMinY());
	WriteFloatComma(o_OutFile, "bbox_min_z", bbox.GetMinZ());

	WriteFloatComma(o_OutFile, "bbox_max_x", bbox.GetMaxX());
	WriteFloatComma(o_OutFile, "bbox_max_y", bbox.GetMaxY());
	WriteFloatComma(o_OutFile, "bbox_max_z", bbox.GetMaxZ());
	
	WriteBoolComma(o_OutFile, "AffectsDiffuse", i_ProjLightData.m_bAffectsDiffuse && io_GlobalData.m_bRenderDiffuse);
	WriteBoolComma(o_OutFile, "AffectsSpecular", i_ProjLightData.m_bAffectsSpecular && io_GlobalData.m_bRenderSpecular);
	WriteFloatComma(o_OutFile, "DepthBias", i_ProjLightData.m_DepthBias * 0.02f);
	o_OutFile << "\t)" << endl;

/*
	o_OutFile << "\t\"mib_light_spot\" (" << endl;

	o_OutFile << "\t\t\"color\" ";
	WriteColor(o_OutFile, i_ProjLightData.m_Color*i_ProjLightData.m_Intensity);
	o_OutFile << "," << endl;
	o_OutFile << "\t\t\"shadow\" ";
	if (i_ProjLightData.m_bCastShadow)
		o_OutFile << "on,";
	else 
		o_OutFile << "off,";
	o_OutFile << endl;
	o_OutFile << "\t\t\"factor\" 0," << endl;
//	o_OutFile << "\t\t\"factor\" " << i_ProjLightData.m_Intensity << "," << endl;
	float cosine = cosf(0.5f * i_ProjLightData.m_Angle * maConstants::c_fAngleToRad);
	o_OutFile << "\t\t\"cone\" " << cosine << "," << endl;

	if (i_ProjLightData.m_Falloff.GetY() > 0 || i_ProjLightData.m_Falloff.GetZ() > 0)
	{
		o_OutFile << "\t\t\"atten\" on," << endl;
		o_OutFile << "\t\t\"start\" " << 0 << "," << endl;
		o_OutFile << "\t\t\"stop\" " << i_ProjLightData.m_Range << "," << endl;
	}
	else
	{
		o_OutFile << "\t\t\"atten\" off," << endl;
	}

	o_OutFile << "\t)" << endl;
*/
	if ( !i_ProjLightData.m_bPureDirectional )
	{
		o_OutFile << "\torigin ";
		maPoint3d origin = i_ProjLightData.m_Position;

		// Push light back by scale amount
		if ( !i_ProjLightData.m_bDirectional )
		{
			maPoint3d origin_c = i_ProjLightData.m_CameraMatrix * origin;
			maMatrix4x4 camInv = i_ProjLightData.m_CameraMatrix;		
			camInv.Invert();
			origin_c.SetZ( origin_c.GetZ() - i_ProjLightData.m_Scale );
			origin = camInv * origin_c;
		}

		WritePoint3d(o_OutFile, origin);
		o_OutFile << endl;
	}

	o_OutFile << "\tdirection ";
	maVector3d d = i_ProjLightData.m_Direction;
	d.Normalize();
	WriteVector3d(o_OutFile, d);
	o_OutFile << endl;

	if ( !i_ProjLightData.m_bDirectional )
	{
		if ( i_ProjLightData.m_bAreaLight )
		{
			// Rectangle
			if ( i_ProjLightData.m_AreaLightType == 0 )
			{
				o_OutFile << "\trectangle " << i_ProjLightData.m_Left.GetX() << " " << i_ProjLightData.m_Left.GetY() << " " << i_ProjLightData.m_Left.GetZ() 
					<< " " << i_ProjLightData.m_Up.GetX() << " " << i_ProjLightData.m_Up.GetY() << " " << i_ProjLightData.m_Up.GetZ() << " " << i_ProjLightData.m_AreaLightSampling;
				o_OutFile << endl;
			}

			// Disc
			else
			{
				maPoint3d normal = i_ProjLightData.m_Target - i_ProjLightData.m_Position;
				normal.Normalize();
				float radius = ( i_ProjLightData.m_Left.Length() + i_ProjLightData.m_Up.Length() ) / 4; // half of the average of the two sides
				o_OutFile << "\tdisc " << normal.GetX() << " " << normal.GetY() << " " << normal.GetZ() << " " << radius << " " << i_ProjLightData.m_AreaLightSampling;					
				o_OutFile << endl;
			}
		}

		float cosine = cosf(0.5f * i_ProjLightData.m_Angle * maConstants::c_fAngleToRad * sqrt(2.0));
		o_OutFile << "\tspread " << cosine << endl;
	}
	/*else
	{
		float cosine = cosf(0.5f * 45.0f * maConstants::c_fAngleToRad * sqrt(2.0));
		o_OutFile << "\tspread " << cosine << endl;
	}*/
	
	
//	o_OutFile << "\tenergy ";
//	WriteColor(o_OutFile, i_ProjLightData.m_Color*i_ProjLightData.m_Intensity);
//	o_OutFile << endl;

	// Write shadow map property
	if ( bShadowMap )
	{
		o_OutFile << "\tshadowmap " << (i_ProjLightData.m_bAreaLight?"off":"on") << endl;
		o_OutFile << "\tshadowmap resolution " << i_ProjLightData.m_DepthMapSize << endl;
		o_OutFile << "\tshadowmap samples " << i_ProjLightData.m_ShadowQuality * i_ProjLightData.m_ShadowQuality << endl;
		if (!i_ProjLightData.m_bDirectional)
			o_OutFile << "\tshadowmap softness " << i_ProjLightData.m_LightSize / (i_ProjLightData.m_Scale ? i_ProjLightData.m_Scale : 1.0f) << endl;
		else
			o_OutFile << "\tshadowmap softness " << i_ProjLightData.m_LightSize * 0.05 * i_ProjLightData.m_Scale * 2.0f << endl;
		o_OutFile << "\tshadowmap bias 0.001" << endl;
		if (i_ProjLightData.m_bDirectional)
			o_OutFile << "\tshadowmap camera \"" << i_ProjLightData.m_Name << "_cam" << "\"" << endl;
	}

	o_OutFile << "end light" << endl;

	o_OutFile << "instance \"" << i_ProjLightData.m_InstanceName << "\" \"" << i_ProjLightData.m_Name << "\"" << endl;
	int shadowFlag = 0;
	shadowFlag |= i_ProjLightData.m_bCastShadow ? s_CastOn : s_CastOff;
	shadowFlag |= io_GlobalData.m_bRenderingShadowsOnly ? 1 : 0;
	o_OutFile << "\tshadow " << shadowFlag << endl;
	o_OutFile << "end instance" << endl;

}

//--------------------------------------------------------------------
// WriteObjectLights()
//--------------------------------------------------------------------
void WriteObjectLights( std::string i_ObjName , std::ostringstream& o_Stream,
					    mrayGlobalData & io_GlobalData )
{
	// skip this step if lighting is disabled.
	if (!io_GlobalData.m_bRenderLit)
	{
		o_Stream << "\tlight [ ]" << endl;
		return;
	}

	std::map< std::string , std::vector< std::string > >  objectLightMap = io_GlobalData.m_ObjectLightMap;
	std::vector< std::string > lightSetLights = io_GlobalData.m_LightSetLights;
	std::vector< std::string > sceneLights = io_GlobalData.m_SceneLights;
	std::vector< std::string > activatedLights = objectLightMap[i_ObjName];
	
	o_Stream << "\tlight [ " << endl;
	for ( int i = 0 ; i < sceneLights.size() ; i++ )
	{		
		if ( envSTLHelpers::Contains(lightSetLights, sceneLights[i] ) )
		{
			if ( envSTLHelpers::Contains(activatedLights, sceneLights[i]) )
			{
				o_Stream << "\t\t\"" << sceneLights[i] << "_inst\" " << endl;
			}	
		}
		else
		{
			o_Stream << "\t\t\"" << sceneLights[i] << "_inst\" " << endl;
		}		
	}

	if ( io_GlobalData.m_Options.m_bEnableIBL)
	{
		o_Stream << "\t\t\"builtin_ibl_light_inst\"" << endl;
	}

	o_Stream << "\t      ]" << endl;
}

//--------------------------------------------------------------------
// ExportMesh()
//--------------------------------------------------------------------
void mrayExport::ExportMesh(std::ofstream & o_OutFile,		 
						    mrayGlobalData & io_GlobalData,
							mrayGeometryData i_GeometryData, 
							const g3dSceneNode * i_pNode)
{	

	g3dFragment * currFrag = (g3dFragment*)i_pNode->GetFragment();

	// for some reason, without the empty str check, a few irrelevant frags are traversed
	if ( currFrag && !currFrag->GetFragmentName().empty() && !currFrag->IsShadowHull() && i_GeometryData.m_bVisible)
	{
		std::string fragName = currFrag->GetFragmentName();
		std::string matName = currFrag->GetMaterial()->GetName();
		std::string objName = i_GeometryData.m_Name + "-" + fragName + "-" + matName;

		if ( !envSTLHelpers::Contains(io_GlobalData.m_ExportedObjects,objName) )
		{
			std::string instName = objName + "_inst";
			std::string fragMatName = objName + "_mtrl";

			std::string archiveName = objName + std::string(".mi");
			fsLocator loc = io_GlobalData.m_ArchivesLoc;
			loc.Push(archiveName.c_str());
			itString archiveNameItPath;
			fsFileUtil::LocatorToUnicodeString( loc, archiveNameItPath );

			if (!fsFileUtil::FileExists( fsLocator(archiveNameItPath) ) || 
				  io_GlobalData.m_Options.m_bRewriteAssets )
			{
				std::vector<int> indices;
				std::vector<maPoint3d> vertexParamList;
				std::vector<maPoint3d> normalParamList;
				std::vector<maPoint2d> uvParamList;
				std::vector<maPoint3d> uvTangentList;	
				std::vector<maPoint3d> uvBinormalList;	

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
					uvTangentList.push_back( vBuffer[ i ].m_S );
					uvBinormalList.push_back( vBuffer[ i ].m_T );
				}
				// done with buffer.
				currFrag->ReadOnlyUnlock();
				
				std::ofstream archiveFile( archiveNameItPath.GetString() , std::ios::out );

				archiveFile << "object \"" << objName << "\"" << endl;

				archiveFile << "\tvisible on" << endl;
				archiveFile << "\ttrace on" << endl;
				archiveFile << "\ttransparency 3" << endl;
				archiveFile << "\treflection 3" << endl;
				archiveFile << "\trefraction 3" << endl;
				archiveFile << "\tfinalgather 3" << endl;
				archiveFile << "\tglobillum 3" << endl;
				archiveFile << "\tmax displace 3" << endl;

				archiveFile << "\ttrilist" << endl;
				archiveFile << "\t\tvertex " << vertexParamList.size() << " p n b 2 t 2" << endl;
				archiveFile << "\t\ttriangle " << indices.size()/3 << endl;
				archiveFile << "\t\t[" << endl;

				// Mesh UVs are in D3D-ready form.
				// Flip mesh uv for mental ray rendering.
				for ( int i = 0 ; i < vertexParamList.size() ; i++ )
				{
					archiveFile << "\t\t";
					archiveFile << vertexParamList[i].GetX() << " ";
					archiveFile << vertexParamList[i].GetY() << " ";
					archiveFile << vertexParamList[i].GetZ() << " ";
					archiveFile << normalParamList[i].GetX() << " ";
					archiveFile << normalParamList[i].GetY() << " ";
					archiveFile << normalParamList[i].GetZ() << " ";
					archiveFile << uvTangentList[i].GetX() << " ";
					archiveFile << uvTangentList[i].GetY() << " ";
					archiveFile << uvTangentList[i].GetZ() << " ";
					archiveFile << uvBinormalList[i].GetX() << " ";
					archiveFile << uvBinormalList[i].GetY() << " ";
					archiveFile << uvBinormalList[i].GetZ() << " ";
					archiveFile << uvParamList[i].GetX() << " ";
					archiveFile << 1.0f - uvParamList[i].GetY() << " ";
					archiveFile << endl;
				}

				archiveFile << "\t\t]" << endl;
				archiveFile << "\t\t[" << endl;
				for ( int i = 0 ; i < indices.size() ; i+=3 )
				{
					archiveFile << "\t\t";
					archiveFile << indices[i] << " ";
					archiveFile << indices[i+1] << " ";
					archiveFile << indices[i+2];
					archiveFile << endl;
				}
				archiveFile << "\t\t]" << endl;
				archiveFile << "\tend trilist" << endl;
				
				// write displacement approximation
				if (currFrag->GetMaterial()->GetHasDisplacement())
				{
					effDisplacementData displacementData = currFrag->GetMaterial()->GetDisplacementData();
					float disp_amount = floor((displacementData.m_TessellationValue * 2 + 1.0f) / 3.0f);
					archiveFile << "\tapproximate regular parametric " << 
									disp_amount << " " << disp_amount << endl;
				}

				archiveFile << "end object" << endl;		  
				archiveFile << endl;
			}

			io_GlobalData.m_MeshInstances.push_back(instName);

			std::ostringstream worldFile;
			WriteMaterial(worldFile, io_GlobalData, objName, fragMatName, currFrag->GetMaterial(), currFrag, i_GeometryData.m_EnvData );
			worldFile << "$include <" << itStringUtil::GetStdString(archiveNameItPath) << ">" << endl;


			worldFile << "instance \"" << instName << "\" \"" << objName << "\"" << endl;

			WriteObjectLights(objName,worldFile,io_GlobalData);

			worldFile << "\tmaterial [ \"" << fragMatName << "\" ]" << endl;

			maMatrix4x4 totalTransformMat = i_pNode->GetTotalTransform();
			// instance transform must transform from world to object space!
			// therefore we invert this matrix.
			totalTransformMat.Invert();

			std::string totalTransform = MatrixToStr(totalTransformMat);
			
			worldFile << "\ttransform " << totalTransform << endl;
			worldFile << "\tface " << (currFrag->GetDoubleSided()? "both" : "front") << endl;

			int shadowFlag = 0;
			if ( currFrag->GetCastsShadow() && currFrag->GetReceivesShadow() )
			{
				shadowFlag = 3;
			}
			else if ( currFrag->GetCastsShadow() && !currFrag->GetReceivesShadow() )
			{
				shadowFlag = 1;
			}
			else if ( !currFrag->GetCastsShadow() && currFrag->GetReceivesShadow() )
			{
				shadowFlag = 2;
			}
			worldFile << "\tshadow " << shadowFlag << endl;

			worldFile << "end instance" << endl;
		
			worldFile << endl;

			io_GlobalData.m_WorldData.push_back( worldFile.str() );
			io_GlobalData.m_ExportedObjects.push_back( objName );
		}
	}

	const int num_kids = i_pNode->GetNumChildren();
	for (int i=0; i<num_kids; i++)
	{
		ExportMesh(o_OutFile, io_GlobalData, i_GeometryData, i_pNode->GetChild(i));
	}
	
}

//--------------------------------------------------------------------
// EndExport()
//--------------------------------------------------------------------
void mrayExport::EndExport(std::ofstream & o_OutFile,
						   mrayGlobalData & io_GlobalData)
{
	o_OutFile << "instgroup \"world\"" << endl;
	o_OutFile << "\t\"cam_inst\" " << endl;

	for ( int i = 0 ; i < io_GlobalData.m_PointLightInstances.size() ; i++ )
	{
		o_OutFile << "\t\"" << io_GlobalData.m_PointLightInstances[i] << "\" " << endl;
	}
	for ( int i = 0 ; i < io_GlobalData.m_ProjLightInstances.size() ; i++ )
	{
		o_OutFile << "\t\"" << io_GlobalData.m_ProjLightInstances[i] << "\" " << endl;
	}
	for ( int i = 0 ; i < io_GlobalData.m_MeshInstances.size() ; i++ )
	{
		o_OutFile << "\t\"" << io_GlobalData.m_MeshInstances[i] << "\" " << endl;
	}
	o_OutFile << "end instgroup" << endl;

	o_OutFile << endl;

	o_OutFile << "render \"world\" \"cam_inst\" \"opt\"" << endl;

	// Write out world .mi
	itString worldName;
	fsFileUtil::LocatorToUnicodeString(io_GlobalData.m_WorldName,worldName);
	std::ofstream worldFile( worldName.GetString() , std::ios::out );
	for ( int i = 0 ; i < io_GlobalData.m_WorldData.size() ; i++ )
	{
		worldFile << io_GlobalData.m_WorldData[i] << endl;
	}
}