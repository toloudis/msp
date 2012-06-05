// #error THIS_FILE_IS_OBSOLETE

///********************************************************************************************\
//**  tmaManagedConversionUtil.h
//**
//**      Conversions for Terawatt Managed Controls
//**
//**	StudioGPU
//**	Copyright(C) 2003 - All Rights Reserved
//\********************************************************************************************/
//#ifdef	TMA_MANAGEDCONVERSIONUTIL_HPP
//#error	tmaManagedConversionUtil.hpp included recursively.
//#endif
//#define	TMA_MANAGEDCONVERSIONUTIL_HPP
//
//#ifndef MA_FLOATRGBA_HPP
//#include "Core/ma/maFloatRGBA.hpp"
//#endif
//#ifndef MA_POINT3D_HPP
//#include "Core/ma/maPoint3d.hpp"
//#endif
//#ifndef MA_ROTATION_HPP
//#include "Core/ma/maRotation.hpp"
//#endif
//
//#include <string>
//
//#ifdef _MANAGED
//
//using namespace System::Windows::Forms;
//
//
////============================================================================================
////	tmaManagedConversionUtil Functions
////============================================================================================
//namespace tmaManagedConversionUtil
//{
//	//----------------------------------------------------------------------------
//	//	ConvertString()
//	//----------------------------------------------------------------------------
//	void ConvertString(System::String ^i_Str, std::string& o_Str);
//	std::string ConvertString(System::String ^i_Str);
//
//	//----------------------------------------------------------------------------
//	//	Point3 <-> Vector3dEditUpDown
//	//----------------------------------------------------------------------------
//	void SetPoint3(const maPoint3d &i_Pt, TerawattManagedControls::Vector3EditUpDown ^o_VectorEdit);
//	void GetPoint3(TerawattManagedControls::Vector3EditUpDown ^i_VectorEdit, maPoint3d &o_Pt );
//
//	//----------------------------------------------------------------------------
//	//	Point3 <-> Vector3dEdit
//	//----------------------------------------------------------------------------
//	void SetPoint3(const maPoint3d &i_Pt, TerawattManagedControls::Vector3Edit ^o_VectorEdit);
//	void GetPoint3(TerawattManagedControls::Vector3Edit ^i_VectorEdit, maPoint3d &o_Pt );
//
//	//----------------------------------------------------------------------------
//	//	Point3 <-> Vector3dEditRanged
//	//----------------------------------------------------------------------------
//	void SetPoint3(const maPoint3d &i_Pt, TerawattManagedControls::Vector3EditRanged ^o_VectorEdit);
//	void GetPoint3(TerawattManagedControls::Vector3EditRanged ^i_VectorEdit, maPoint3d &o_Pt );
//
//	//----------------------------------------------------------------------------
//	//	Rotation3
//	//----------------------------------------------------------------------------
//	void SetRotation3(const maRotation &i_Pt, TerawattManagedControls::Vector3Edit ^o_VectorEdit);
//
//	//----------------------------------------------------------------------------
//	//	ColorRGB
//	//----------------------------------------------------------------------------
//	maFloatRGBA ConvertColorRGB(System::Drawing::Color &i_Color);
//	System::Drawing::Color SetColorRGB(const maFloatRGBA &i_Color);
//
//	//----------------------------------------------------------------------------
//	//	ColorRGBA
//	//----------------------------------------------------------------------------
//	maFloatRGBA ConvertColorRGBA(System::Drawing::Color &i_Color);
//	System::Drawing::Color SetColorRGBA(const maFloatRGBA &i_Color);
//};
//
//#endif // _MANAGED
