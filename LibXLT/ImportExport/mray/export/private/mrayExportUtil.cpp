/****************************************************************************\
**	mrayExportUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/mray/export/private/mrayExportUtil.hpp"

#include "Core/ma/maFloatRGBA.hpp"
#include "Core/ma/maMatrix4x4.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"

#include "ImportExport/mray/export/mrayExportData.hpp"

#include <fstream>
using namespace std;

namespace mrayExportUtil
{

//--------------------------------------------------------------------
// IsDeadParam() - filter out dead shader params
//--------------------------------------------------------------------
bool IsDeadParam(const std::string& i_ParamName, const std::string& i_ShaderName)
{
	if ( i_ParamName == "tNormalMap" ||
		 i_ParamName == "g_bFlatTessellate" ||
		 i_ParamName == "g_DisplacementMap" ||
		 i_ParamName == "g_DisplacementScale" ||
		 i_ParamName == "g_DisplacementBias" ||
		 i_ParamName == "g_displacementMap" ||
		 i_ParamName == "g_displacementScale" ||
		 i_ParamName == "g_displacementBias" ||
		 (i_ParamName == "g_ambient" && i_ShaderName != "Cartoon") ||
		 (i_ParamName == "g_edgeColor" && i_ShaderName != "Carpaint") ||
		 (i_ParamName == "fresnelClrBias" && i_ShaderName != "Carpaint") ||
		 (i_ParamName == "fresnelClrPower" && i_ShaderName != "Carpaint") )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------
//	TextureIsRewritten()
//--------------------------------------------------------------------
bool TextureIsRewritten(std::string i_Tex , mrayGlobalData & io_GlobalData)
{
	for(std::map<std::string , bool >::const_iterator it = io_GlobalData.m_TextureWrite.begin(); it != io_GlobalData.m_TextureWrite.end(); ++it)
	{		
		if ( i_Tex == it->first )
		{
			return it->second;
		}
	}
	return false;
}

//--------------------------------------------------------------------
//	LookupTexturePath()
//--------------------------------------------------------------------
fsLocator LookupTexturePath(std::string i_Tex , mrayGlobalData & io_GlobalData)
{
	for(std::map<fsLocator , std::vector< std::string >>::const_iterator it = io_GlobalData.m_FullTextureMap.begin(); it != io_GlobalData.m_FullTextureMap.end(); ++it)
	{
		std::vector<std::string> objects = it->second;
		
		if ( envSTLHelpers::Contains(objects,i_Tex) )
		{
			return it->first;
		}
	}
	return fsLocator();
}

//--------------------------------------------------------------------
//	LookupGeneratedTexName()
//--------------------------------------------------------------------
std::string LookupGeneratedTexName(fsLocator i_Tex, mrayGlobalData & io_GlobalData)
{
	for(std::map<fsLocator , std::string >::const_iterator it = io_GlobalData.m_TextureIDs.begin(); it != io_GlobalData.m_TextureIDs.end(); ++it)
	{		
		if ( i_Tex == it->first )
		{
			return it->second;
		}
	}
	return "";
}

//--------------------------------------------------------------------
//	LookupGeneratedTexName()
//--------------------------------------------------------------------
std::string LookupGeneratedTexName(std::string i_Tex, mrayGlobalData & io_GlobalData)
{
	if ( i_Tex == "" ) return i_Tex;
	fsLocator texturePath = LookupTexturePath(i_Tex,io_GlobalData);

	std::string texName = LookupGeneratedTexName(texturePath,io_GlobalData);
	std::string texPath = "";

	if ( texName != "" )
	{
		fsFileUtil::LocatorToANSIFilename( io_GlobalData.m_TexturesLoc , texPath );
		texPath.append("\\").append(texName).append(".png");
	}
	return texPath;
}

//--------------------------------------------------------------------
// GetFilterMRayForm()
//--------------------------------------------------------------------
std::string GetFilterMRayForm(int i_Val)
{
	std::string filter = "box";
	switch ( i_Val )
	{
		case 0:
			filter = "box";
			break;
		case 1:
			filter = "gauss";
			break;
		case 2:
			filter = "mitchell";
			break;
		case 3:
			filter = "triangle";
			break;
		case 4:
			filter = "lanczos";
			break;
		case 5:
			filter = "lanczos";
			break;
		case 6:
			filter = "box";
			break;
		case 7:
			filter = "box";
			break;
	}
	return filter;
}

//--------------------------------------------------------------------
// GetMRayFileFormat()
//--------------------------------------------------------------------
std::string GetMRayFileFormat(int i_Val)
{ 
	std::string outStr = "bmp";
	switch ( i_Val )
	{
		case 0:
			outStr = "bmp";
			break;
		case 1:
			outStr = "jpg";
			break;
		case 2:
			outStr = "tga";
			break;
		case 3:
			outStr = "png";
			break;
		case 4:
			outStr = "ppm";
			break;
		case 5:
			outStr = "hdr";
			break;
		case 6:
			outStr = "tif";
			break;
		case 7:
			outStr = "tifu";
			break;
		case 8:
			outStr = "exr";
			break;
		case 9:
			outStr = "pic";
			break;
		case 10:
			outStr = "alias";
			break;
		case 11:
			outStr = "rgb";
			break;
		case 12:
			outStr = "iff";
			break;
		case 13:
			outStr = "rla";
			break;
		case 14:
			outStr = "rlb";
			break;
		case 15:
			outStr = "picture";
			break;

	}
	return outStr;
}

//--------------------------------------------------------------------
// WriteMatrix() - put a matrix into a string
//--------------------------------------------------------------------
std::string MatrixToStr( const maMatrix4x4& i_Mat )
{
	std::ostringstream s;	
	for ( int i = 0 ; i < 16 ; i++ ){
		s << i_Mat.m_Mat[i] << " ";
	}
	return s.str();
}

//--------------------------------------------------------------------
// WriteColor()
//--------------------------------------------------------------------
void WriteColor(std::ofstream & o_OutFile, const maFloatRGBA& i_Color)
{
	o_OutFile << i_Color.GetRed() << " " << i_Color.GetGreen() << " " << i_Color.GetBlue();
}

//--------------------------------------------------------------------
// WritePoint3d()
//--------------------------------------------------------------------
void WritePoint3d(std::ofstream & o_OutFile, const maPoint3d& i_Point)
{
	o_OutFile << i_Point.GetX() << " " << i_Point.GetY() << " " << i_Point.GetZ();
}

//--------------------------------------------------------------------
// WriteVector3d()
//--------------------------------------------------------------------
void WriteVector3d(std::ofstream & o_OutFile, const maVector3d& i_Point)
{
	o_OutFile << i_Point.GetX() << " " << i_Point.GetY() << " " << i_Point.GetZ();
}

//--------------------------------------------------------------------
// WriteColorComma()
//--------------------------------------------------------------------
void WriteColorComma(std::ofstream & o_OutFile, std::string i_Name, const maFloatRGBA& i_Color)
{
	o_OutFile << "\t\t\"" << i_Name << "\" ";
	o_OutFile << i_Color.GetRed() << " " << i_Color.GetGreen() << " " << i_Color.GetBlue();
	o_OutFile << "," << endl;
}

//--------------------------------------------------------------------
// WriteMatrixComma()
//--------------------------------------------------------------------
void WriteMatrixComma(std::ofstream & o_OutFile, std::string i_Name, const maMatrix4x4& i_Val)
{
	std::string m = MatrixToStr(i_Val);
	o_OutFile << "\t\t\"" << i_Name << "\" " << m << "," << endl;
}

//--------------------------------------------------------------------
// WriteFloatComma()
//--------------------------------------------------------------------
void WriteFloatComma(std::ofstream & o_OutFile, std::string i_Name, float i_Val)
{
	o_OutFile << "\t\t\"" << i_Name << "\" " << i_Val << "," << endl;
}

//--------------------------------------------------------------------
// WriteIntComma()
//--------------------------------------------------------------------
void WriteIntComma(std::ofstream & o_OutFile, std::string i_Name, int i_Val)
{
	o_OutFile << "\t\t\"" << i_Name << "\" " << i_Val << "," << endl;
}

//--------------------------------------------------------------------
// WriteBoolComma()
//--------------------------------------------------------------------
void WriteBoolComma(std::ofstream & o_OutFile, std::string i_Name, bool i_Val)
{
	o_OutFile << "\t\t\"" << i_Name << "\" ";
	o_OutFile << (i_Val ? "true" : "false") << "," << endl;
}

//--------------------------------------------------------------------
// WriteStrComma()
//--------------------------------------------------------------------
void WriteStrComma(std::ofstream & o_OutFile, std::string i_Name, std::string i_Val)
{
	o_OutFile << "\t\t\"" << i_Name << "\" \"" << i_Val << "\"," << endl;
}

//--------------------------------------------------------------------
// WriteVector3dComma()
//--------------------------------------------------------------------
void WriteVector3dComma(std::ofstream & o_OutFile, std::string i_Name, const maVector3d& i_Point)
{
	o_OutFile << "\t\t\"" << i_Name << "\" " << i_Point.GetX() << " " << i_Point.GetY() << " " << i_Point.GetZ() << "," << endl;
}

//--------------------------------------------------------------------
// WritePoint3dComma()
//--------------------------------------------------------------------
void WritePoint3dComma(std::ofstream & o_OutFile, std::string i_Name, const maPoint3d& i_Point)
{
	o_OutFile << "\t\t\"" << i_Name << "\" " << i_Point.GetX() << " " << i_Point.GetY() << " " << i_Point.GetZ() << "," << endl;
}

//--------------------------------------------------------------------
// WriteColorComma()
//--------------------------------------------------------------------
void WriteColorComma(std::ostream & o_OutFile, std::string i_Name, const maFloatRGBA& i_Color)
{
	o_OutFile << "\t\t\"" << i_Name << "\" ";
	o_OutFile << i_Color.GetRed() << " " << i_Color.GetGreen() << " " << i_Color.GetBlue();
	o_OutFile << "," << endl;
}

//--------------------------------------------------------------------
// WriteMatrixComma()
//--------------------------------------------------------------------
void WriteMatrixComma(std::ostream & o_OutFile, std::string i_Name, const maMatrix4x4& i_Val)
{
	std::string m = MatrixToStr(i_Val);
	o_OutFile << "\t\t\"" << i_Name << "\" " << m << "," << endl;
}

//--------------------------------------------------------------------
// WriteFloatComma()
//--------------------------------------------------------------------
void WriteFloatComma(std::ostream & o_OutFile, std::string i_Name, float i_Val)
{
	o_OutFile << "\t\t\"" << i_Name << "\" " << i_Val << "," << endl;
}

//--------------------------------------------------------------------
// WriteIntComma()
//--------------------------------------------------------------------
void WriteIntComma(std::ostream & o_OutFile, std::string i_Name, int i_Val)
{
	o_OutFile << "\t\t\"" << i_Name << "\" " << i_Val << "," << endl;
}

//--------------------------------------------------------------------
// WriteBoolComma()
//--------------------------------------------------------------------
void WriteBoolComma(std::ostream & o_OutFile, std::string i_Name, bool i_Val)
{
	o_OutFile << "\t\t\"" << i_Name << "\" ";
	o_OutFile << (i_Val ? "true" : "false") << "," << endl;
}

//--------------------------------------------------------------------
// WriteStrComma()
//--------------------------------------------------------------------
void WriteStrComma(std::ostream & o_OutFile, std::string i_Name, std::string i_Val)
{
	o_OutFile << "\t\t\"" << i_Name << "\" \"" << i_Val << "\"," << endl;
}

//--------------------------------------------------------------------
// WriteVector3dComma()
//--------------------------------------------------------------------
void WriteVector3dComma(std::ostream & o_OutFile, std::string i_Name, const maVector3d& i_Point)
{
	o_OutFile << "\t\t\"" << i_Name << "\" " << i_Point.GetX() << " " << i_Point.GetY() << " " << i_Point.GetZ() << "," << endl;
}

//--------------------------------------------------------------------
// WritePoint3dComma()
//--------------------------------------------------------------------
void WritePoint3dComma(std::ostream & o_OutFile, std::string i_Name, const maPoint3d& i_Point)
{
	o_OutFile << "\t\t\"" << i_Name << "\" " << i_Point.GetX() << " " << i_Point.GetY() << " " << i_Point.GetZ() << "," << endl;
}

//--------------------------------------------------------------------
// WriteTexDeclaration()
//--------------------------------------------------------------------
void WriteTexDeclaration(std::ostream & o_OutFile, std::string i_TexID, std::string i_TexPath, mrayGlobalData& io_GlobalData, bool i_IsFilter)
{
	if ( !envSTLHelpers::Contains(io_GlobalData.m_DeclaredTextures,i_TexID) && !i_TexPath.empty())
	{
		if (i_IsFilter)
			o_OutFile << "filter color texture \"" << i_TexID << "\" \"" << i_TexPath << "\"" << endl;
		else
			o_OutFile << "color texture \"" << i_TexID << "\" \"" << i_TexPath << "\"" << endl;
		io_GlobalData.m_DeclaredTextures.push_back(i_TexID);
	}		
}

//--------------------------------------------------------------------
// WriteParamStr()
//--------------------------------------------------------------------
void WriteParamStr(std::ofstream & o_OutFile, std::string i_Name, 
				   std::string i_Val0)
{
	o_OutFile << "\t" << i_Name << "\t" << i_Val0 << endl;
}

//--------------------------------------------------------------------
// WriteParamInt1()
//--------------------------------------------------------------------
void WriteParamInt1(std::ofstream & o_OutFile, std::string i_Name, 
					int i_Val0)
{
	std::ostringstream s;
	s << "\t" << i_Name << "\t" << i_Val0 << endl;
	o_OutFile << s.str();
}

//--------------------------------------------------------------------
// WriteParamInt2()
//--------------------------------------------------------------------
void WriteParamInt2(std::ofstream & o_OutFile, std::string i_Name, 
					int i_Val0, int i_Val1)
{
	std::ostringstream s;
	s << "\t" << i_Name << "\t" << i_Val0 << " " << i_Val1 << endl;
	o_OutFile << s.str();
}

//--------------------------------------------------------------------
// WriteParamInt3()
//--------------------------------------------------------------------
void WriteParamInt3(std::ofstream & o_OutFile, std::string i_Name, 
					int i_Val0, int i_Val1, int i_Val2)
{
	std::ostringstream s;
	s << "\t" << i_Name << "\t" << i_Val0 << " " << i_Val1 << " " << i_Val2 << endl;
	o_OutFile << s.str();
}

//--------------------------------------------------------------------
// WriteParamInt4()
//--------------------------------------------------------------------
void WriteParamInt4(std::ofstream & o_OutFile, std::string i_Name, 
					int i_Val0, int i_Val1, int i_Val2, int i_Val3)
{
	std::ostringstream s;
	s << "\t" << i_Name << "\t" << i_Val0 << " " << i_Val1 << " " << i_Val2 << " " << i_Val3 << endl;
	o_OutFile << s.str();
}

//--------------------------------------------------------------------
// WriteParamFloat1()
//--------------------------------------------------------------------
void WriteParamFloat1(std::ofstream & o_OutFile, std::string i_Name, 
					  float i_Val0)
{
	std::ostringstream s;
	s << "\t" << i_Name << "\t" << i_Val0 << endl;
	o_OutFile << s.str();
}

//--------------------------------------------------------------------
// WriteParamFloat2()
//--------------------------------------------------------------------
void WriteParamFloat2(std::ofstream & o_OutFile, std::string i_Name, 
					  float i_Val0, float i_Val1)
{
	std::ostringstream s;
	s << "\t" << i_Name << "\t" << i_Val0 << " " << i_Val1 << endl;
	o_OutFile << s.str();
}

//--------------------------------------------------------------------
// WriteParamFloat3()
//--------------------------------------------------------------------
void WriteParamFloat3(std::ofstream & o_OutFile, std::string i_Name,
					  float i_Val0, float i_Val1, float i_Val2)
{
	std::ostringstream s;
	s << "\t" << i_Name << "\t" << i_Val0 << " " << i_Val1 << " " << i_Val2 << endl;
	o_OutFile << s.str();
}

//--------------------------------------------------------------------
// WriteParamFloat4()
//--------------------------------------------------------------------
void WriteParamFloat4(std::ofstream & o_OutFile, std::string i_Name, 
					  float i_Val0, float i_Val1, float i_Val2, float i_Val3)
{
	std::ostringstream s;
	s << "\t" << i_Name << "\t" << i_Val0 << " " << i_Val1 << " " << i_Val2 << " " << i_Val3 << endl;
	o_OutFile << s.str();
}

};//namespace mrayExportUtil
