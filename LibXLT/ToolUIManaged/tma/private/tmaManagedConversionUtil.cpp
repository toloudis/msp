///********************************************************************************************\
//**  tmaManagedConversionUtil.cpp
//**
//**      Conversions for Terawatt Managed Controls
//**
//**	StudioGPU
//**	Copyright(C) 2003 - All Rights Reserved
//\********************************************************************************************/
//#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
//
//#include "Core/ma/maConstants.hpp"
//#include "Core/ma/maFunctions.hpp"
//
//#ifdef _MANAGED
//
//
////----------------------------------------------------------------------------
////	ConvertString()
////----------------------------------------------------------------------------
//void tmaManagedConversionUtil::ConvertString(System::String ^i_Str, std::string& o_Str)
//{
//	int len = i_Str->Length;
//	o_Str.clear();
//	for (int i=0; i<len; i++)
//		o_Str += (char) i_Str[i];
//}
//std::string tmaManagedConversionUtil::ConvertString( System::String ^i_Str )
//{
//	std::string str;
//	tmaManagedConversionUtil::ConvertString(i_Str, str);
//	return str;
//}
//
////----------------------------------------------------------------------------
////	Point3 <-> Vector3dEditUpDown
////----------------------------------------------------------------------------
//void tmaManagedConversionUtil::SetPoint3( const maPoint3d &i_Pt,
//										  TerawattManagedControls::Vector3EditUpDown ^o_VectorEdit )
//{
//	// local copy to make sure updates don't alter original data
//	maPoint3d pos = i_Pt;
//	o_VectorEdit->ValueX = System::Decimal(pos.m_X);
//	o_VectorEdit->ValueY = System::Decimal(pos.m_Y);
//	o_VectorEdit->ValueZ = System::Decimal(pos.m_Z);
//}
//
//void tmaManagedConversionUtil::GetPoint3(TerawattManagedControls::Vector3EditUpDown ^i_VectorEdit, maPoint3d &o_Pt )
//{
//	o_Pt.SetX( (float)(i_VectorEdit->ValueX) );
//	o_Pt.SetY( (float)(i_VectorEdit->ValueY) );
//	o_Pt.SetZ( (float)(i_VectorEdit->ValueZ) );
//}
//
////----------------------------------------------------------------------------
////	Point3 <-> Vector3dEdit
////----------------------------------------------------------------------------
//void tmaManagedConversionUtil::SetPoint3( const maPoint3d &i_Pt,
//										  TerawattManagedControls::Vector3Edit ^o_VectorEdit )
//{
//	// local copy to make sure updates don't alter original data
//	maPoint3d pos = i_Pt;
//	o_VectorEdit->ValueX = pos.m_X;
//	o_VectorEdit->ValueY = pos.m_Y;
//	o_VectorEdit->ValueZ = pos.m_Z;
//}
//
//void tmaManagedConversionUtil::GetPoint3(TerawattManagedControls::Vector3Edit ^i_VectorEdit, maPoint3d &o_Pt )
//{
//	o_Pt.SetX( (float)(i_VectorEdit->ValueX) );
//	o_Pt.SetY( (float)(i_VectorEdit->ValueY) );
//	o_Pt.SetZ( (float)(i_VectorEdit->ValueZ) );
//}
//
////----------------------------------------------------------------------------
////	Point3 <-> Vector3dEditRanged
////----------------------------------------------------------------------------
//void tmaManagedConversionUtil::SetPoint3( const maPoint3d &i_Pt,
//										 TerawattManagedControls::Vector3EditRanged ^o_VectorEdit )
//{
//	// local copy to make sure updates don't alter original data
//	maPoint3d pos = i_Pt;
//	o_VectorEdit->ValueX = pos.m_X;
//	o_VectorEdit->ValueY = pos.m_Y;
//	o_VectorEdit->ValueZ = pos.m_Z;
//}
//
//void tmaManagedConversionUtil::GetPoint3(TerawattManagedControls::Vector3EditRanged ^i_VectorEdit, maPoint3d &o_Pt )
//{
//	// NOTE: this was more compact before but microsoft compiler threw up an INTERNAL COMPILER ERROR, so it had to be split apart
//	double tempval = i_VectorEdit->ValueX;
//	o_Pt.SetX( (float)tempval );
//	tempval = i_VectorEdit->ValueY;
//	o_Pt.SetY( (float)tempval );
//	tempval = i_VectorEdit->ValueZ;
//	o_Pt.SetZ( (float)tempval );
//}
//
////----------------------------------------------------------------------------
////	Rotation3
////----------------------------------------------------------------------------
//void tmaManagedConversionUtil::SetRotation3( const maRotation &i_Rot,
//											 TerawattManagedControls::Vector3Edit ^o_VectorEdit )
//{
//	// local copy to make sure updates don't alter original data
//	float xAngle = 0, yAngle = 0, zAngle = 0;
//	i_Rot.GetEuler(xAngle, yAngle, zAngle);
//	o_VectorEdit->ValueX = ::floorf(xAngle * maConstants::c_fRadToAngle);
//	o_VectorEdit->ValueY = ::floorf(yAngle * maConstants::c_fRadToAngle);
//	o_VectorEdit->ValueZ = ::floorf(zAngle * maConstants::c_fRadToAngle);
//}
//
////----------------------------------------------------------------------------
////	ConvertColorRGB()
////----------------------------------------------------------------------------
//maFloatRGBA tmaManagedConversionUtil::ConvertColorRGB(System::Drawing::Color &i_Color)
//{
//	return maFloatRGBA(	i_Color.R / 255.0f, i_Color.G / 255.0f,
//						i_Color.B / 255.0f, 1.0f);
//}
//
//
////----------------------------------------------------------------------------
////	SetColorRGB()
////----------------------------------------------------------------------------
//System::Drawing::Color tmaManagedConversionUtil::SetColorRGB(const maFloatRGBA &i_Color)
//{
//	int red = (int)(255.0f * i_Color.GetRed());
//	maFunctions::Clamp(red, 0, 255);
//	int green = (int)(255.0f * i_Color.GetGreen());
//	maFunctions::Clamp(green, 0, 255);
//	int blue = (int)(255.0f * i_Color.GetBlue());
//	maFunctions::Clamp(blue, 0, 255);
//	return System::Drawing::Color::FromArgb(red, green, blue);
//}
//
////----------------------------------------------------------------------------
////	ConvertColorRGBA()
////----------------------------------------------------------------------------
//maFloatRGBA tmaManagedConversionUtil::ConvertColorRGBA(System::Drawing::Color &i_Color)
//{
//	return maFloatRGBA(	i_Color.R / 255.0f, i_Color.G / 255.0f,
//						i_Color.B / 255.0f, i_Color.A / 255.0f);
//}
//
////----------------------------------------------------------------------------
////	SetColorRGBA()
////----------------------------------------------------------------------------
//System::Drawing::Color tmaManagedConversionUtil::SetColorRGBA(const maFloatRGBA &i_Color)
//{
//	int alpha = (int)(255.0f * i_Color.GetAlpha());
//	maFunctions::Clamp(alpha, 0, 255);
//	int red = (int)(255.0f * i_Color.GetRed());
//	maFunctions::Clamp(red, 0, 255);
//	int green = (int)(255.0f * i_Color.GetGreen());
//	maFunctions::Clamp(green, 0, 255);
//	int blue = (int)(255.0f * i_Color.GetBlue());
//	maFunctions::Clamp(blue, 0, 255);
//	return System::Drawing::Color::FromArgb(alpha, red, green, blue);
//}
//#endif // _MANAGED
