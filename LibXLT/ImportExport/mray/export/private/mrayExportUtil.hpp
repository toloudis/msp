/*****************************************************************************\
**	mrayExportUtil.hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MRAY_EXPORTUTIL_HPP
#error mrayExportUtil.hpp multiply included
#endif
#define MRAY_EXPORTUTIL_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <string>

struct mrayGlobalData;
class maMatrix4x4;
class maFloatRGBA;

namespace mrayExportUtil
{

	//--------------------------------------------------------------------
	// IsDeadParam() - filter out dead shader params
	//--------------------------------------------------------------------
	bool IsDeadParam(const std::string& i_ParamName, const std::string& i_ShaderName);

	//--------------------------------------------------------------------
	//	TextureIsRewritten()
	//--------------------------------------------------------------------
	bool TextureIsRewritten(std::string i_Tex , mrayGlobalData & io_GlobalData);

	//--------------------------------------------------------------------
	//	LookupTexturePath()
	//--------------------------------------------------------------------
	fsLocator LookupTexturePath(std::string i_Tex , mrayGlobalData & io_GlobalData);

	//--------------------------------------------------------------------
	//	LookupGeneratedTexName()
	//--------------------------------------------------------------------
	std::string LookupGeneratedTexName(fsLocator i_Tex, mrayGlobalData & io_GlobalData);

	//--------------------------------------------------------------------
	//	LookupGeneratedTexName()
	//--------------------------------------------------------------------
	std::string LookupGeneratedTexName(std::string i_Tex, mrayGlobalData & io_GlobalData);

	//--------------------------------------------------------------------
	// GetFilterMRayForm()
	//--------------------------------------------------------------------
	std::string GetFilterMRayForm(int i_Val);

	//--------------------------------------------------------------------
	// GetMRayFileFormat()
	//--------------------------------------------------------------------
	std::string GetMRayFileFormat(int i_Val);

	//--------------------------------------------------------------------
	// WriteMatrix() - put a matrix into a string
	//--------------------------------------------------------------------
	std::string MatrixToStr(const maMatrix4x4& i_Mat );

	//--------------------------------------------------------------------
	// WriteColor()
	//--------------------------------------------------------------------
	void WriteColor(std::ofstream & o_OutFile, const maFloatRGBA& i_Color);

	//--------------------------------------------------------------------
	// WritePoint3d()
	//--------------------------------------------------------------------
	void WritePoint3d(std::ofstream & o_OutFile, const maPoint3d& i_Point);

	//--------------------------------------------------------------------
	// WriteVector3d()
	//--------------------------------------------------------------------
	void WriteVector3d(std::ofstream & o_OutFile, const maVector3d& i_Point);

	//--------------------------------------------------------------------
	// WriteColorComma()
	//--------------------------------------------------------------------
	void WriteColorComma(std::ofstream & o_OutFile, std::string i_Name, const maFloatRGBA& i_Color);

	//--------------------------------------------------------------------
	// WriteMatrixComma()
	//--------------------------------------------------------------------
	void WriteMatrixComma(std::ofstream & o_OutFile, std::string i_Name, const maMatrix4x4& i_Val);

	//--------------------------------------------------------------------
	// WriteFloatComma()
	//--------------------------------------------------------------------
	void WriteFloatComma(std::ofstream & o_OutFile, std::string i_Name, float i_Val);

	//--------------------------------------------------------------------
	// WriteIntComma()
	//--------------------------------------------------------------------
	void WriteIntComma(std::ofstream & o_OutFile, std::string i_Name, int i_Val);

	//--------------------------------------------------------------------
	// WriteBoolComma()
	//--------------------------------------------------------------------
	void WriteBoolComma(std::ofstream & o_OutFile, std::string i_Name, bool i_Val);

	//--------------------------------------------------------------------
	// WriteStrComma()
	//--------------------------------------------------------------------
	void WriteStrComma(std::ofstream & o_OutFile, std::string i_Name, std::string i_Val);

	//--------------------------------------------------------------------
	// WriteVector3dComma()
	//--------------------------------------------------------------------
	void WriteVector3dComma(std::ofstream & o_OutFile, std::string i_Name, const maVector3d& i_Point);

	//--------------------------------------------------------------------
	// WritePoint3dComma()
	//--------------------------------------------------------------------
	void WritePoint3dComma(std::ofstream & o_OutFile, std::string i_Name, const maPoint3d& i_Point);

	//--------------------------------------------------------------------
	// WriteColorComma()
	//--------------------------------------------------------------------
	void WriteColorComma(std::ostream & o_OutFile, std::string i_Name, const maFloatRGBA& i_Color);

	//--------------------------------------------------------------------
	// WriteMatrixComma()
	//--------------------------------------------------------------------
	void WriteMatrixComma(std::ostream & o_OutFile, std::string i_Name, const maMatrix4x4& i_Val);

	//--------------------------------------------------------------------
	// WriteFloatComma()
	//--------------------------------------------------------------------
	void WriteFloatComma(std::ostream & o_OutFile, std::string i_Name, float i_Val);

	//--------------------------------------------------------------------
	// WriteIntComma()
	//--------------------------------------------------------------------
	void WriteIntComma(std::ostream & o_OutFile, std::string i_Name, int i_Val);

	//--------------------------------------------------------------------
	// WriteBoolComma()
	//--------------------------------------------------------------------
	void WriteBoolComma(std::ostream & o_OutFile, std::string i_Name, bool i_Val);

	//--------------------------------------------------------------------
	// WriteStrComma()
	//--------------------------------------------------------------------
	void WriteStrComma(std::ostream & o_OutFile, std::string i_Name, std::string i_Val);

	//--------------------------------------------------------------------
	// WriteVector3dComma()
	//--------------------------------------------------------------------
	void WriteVector3dComma(std::ostream & o_OutFile, std::string i_Name, const maVector3d& i_Point);

	//--------------------------------------------------------------------
	// WritePoint3dComma()
	//--------------------------------------------------------------------
	void WritePoint3dComma(std::ostream & o_OutFile, std::string i_Name, const maPoint3d& i_Point);

	//--------------------------------------------------------------------
	// WriteTexDeclaration()
	//--------------------------------------------------------------------
	void WriteTexDeclaration(std::ostream & o_OutFile, std::string i_TexID, std::string i_TexPath, mrayGlobalData& io_GlobalData, bool i_IsFilter = true);

	//--------------------------------------------------------------------
	// WriteParamStr()
	//--------------------------------------------------------------------
	void WriteParamStr(std::ofstream & o_OutFile, std::string i_Name, 
					   std::string i_Val0);

	//--------------------------------------------------------------------
	// WriteParamInt1()
	//--------------------------------------------------------------------
	void WriteParamInt1(std::ofstream & o_OutFile, std::string i_Name, 
						int i_Val0);

	//--------------------------------------------------------------------
	// WriteParamInt2()
	//--------------------------------------------------------------------
	void WriteParamInt2(std::ofstream & o_OutFile, std::string i_Name, 
						int i_Val0, int i_Val1);

	//--------------------------------------------------------------------
	// WriteParamInt3()
	//--------------------------------------------------------------------
	void WriteParamInt3(std::ofstream & o_OutFile, std::string i_Name, 
						int i_Val0, int i_Val1, int i_Val2);

	//--------------------------------------------------------------------
	// WriteParamInt4()
	//--------------------------------------------------------------------
	void WriteParamInt4(std::ofstream & o_OutFile, std::string i_Name, 
						int i_Val0, int i_Val1, int i_Val2, int i_Val3);

	//--------------------------------------------------------------------
	// WriteParamFloat1()
	//--------------------------------------------------------------------
	void WriteParamFloat1(std::ofstream & o_OutFile, std::string i_Name, 
						  float i_Val0);

	//--------------------------------------------------------------------
	// WriteParamFloat2()
	//--------------------------------------------------------------------
	void WriteParamFloat2(std::ofstream & o_OutFile, std::string i_Name, 
						  float i_Val0, float i_Val1);

	//--------------------------------------------------------------------
	// WriteParamFloat3()
	//--------------------------------------------------------------------
	void WriteParamFloat3(std::ofstream & o_OutFile, std::string i_Name,
						  float i_Val0, float i_Val1, float i_Val2);

	//--------------------------------------------------------------------
	// WriteParamFloat4()
	//--------------------------------------------------------------------
	void WriteParamFloat4(std::ofstream & o_OutFile, std::string i_Name, 
						  float i_Val0, float i_Val1, float i_Val2, float i_Val3);

}; //namespace mrayExportUtil
