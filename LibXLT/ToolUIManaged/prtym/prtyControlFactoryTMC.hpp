#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyControlFactoryTMC.hpp
//**
//**		Control factory for TerawattManagedControls
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_CONTROLFACTORYTMC_HPP
//#error prtyControlFactoryTMC.hpp multiply included
//#endif
//#define PRTY_CONTROLFACTORYTMC_HPP
//
//#ifndef PRTY_ANGLE_FLOATEDIT_HPP
//#include "ToolUIManaged/prtym/prtyAngle_FloatEdit.hpp"
//#endif
//#ifndef PRTY_ANGLE_RANGEDFLOAT_HPP
//#include "ToolUIManaged/prtym/prtyAngle_RangedFloat.hpp"
//#endif
//#ifndef PRTY_COLOR_COLORRGBEDIT_HPP
//#include "ToolUIManaged/prtym/prtyColor_ColorRGBEdit.hpp"
//#endif
//#ifndef PRTY_COLOR_COLORRGBAEDIT_HPP
//#include "ToolUIManaged/prtym/prtyColor_ColorRGBAEdit.hpp"
//#endif
//#ifndef PRTY_CONTROLFACTORY_HPP
//#include "ToolUIManaged/prtym/prtyControlFactory.hpp"
//#endif
//#ifndef PRTY_DIRECTORY_FOLDERCHOOSER_HPP
//#include "ToolUIManaged/prtym/prtyDirectory_FolderChooser.hpp"
//#endif
//#ifndef PRTY_FILENAME_FILECHOOSER_HPP
//#include "ToolUIManaged/prtym/prtyFileName_FileChooser.hpp"
//#endif
//#ifndef PRTY_FILEPATH_FILECHOOSER_HPP
//#include "ToolUIManaged/prtym/prtyFilePath_FileChooser.hpp"
//#endif
//#ifndef PRTY_FLOATEDITUIINFO_HPP
//#include "Core/prty/prtyFloatEditUIInfo.hpp"
//#endif
//#ifndef PRTY_FLOAT_FLOATEDIT_HPP
//#include "ToolUIManaged/prtym/prtyFloat_FloatEdit.hpp"
//#endif
//#ifndef PRTY_FLOAT_RANGEDFLOAT_HPP
//#include "ToolUIManaged/prtym/prtyFloat_RangedFloat.hpp"
//#endif
//#ifndef PRTY_HOTKEY_KEYCOMBO_HPP
//#include "ToolUIManaged/prtym/prtyHotKey_KeyCombo.hpp"
//#endif
//#ifndef PRTY_INT8_FLOATEDIT_HPP
//#include "ToolUIManaged/prtym/prtyInt8_FloatEdit.hpp"
//#endif
//#ifndef PRTY_INT8_RANGEDFLOAT_HPP
//#include "ToolUIManaged/prtym/prtyInt8_RangedFloat.hpp"
//#endif
//#ifndef PRTY_INT32_FLOATEDIT_HPP
//#include "ToolUIManaged/prtym/prtyInt32_FloatEdit.hpp"
//#endif
//#ifndef PRTY_INT32_RANGEDFLOAT_HPP
//#include "ToolUIManaged/prtym/prtyInt32_RangedFloat.hpp"
//#endif
//#ifndef PRTY_PROPERTYUIINFO_HPP
//#include "Core/prty/prtyPropertyUIInfo.hpp"
//#endif
//#ifndef PRTY_ROTATION_VECTOR3DEDIT_HPP
//#include "ToolUIManaged/prtym/prtyRotation_Vector3dEdit.hpp"
//#endif
//#ifndef PRTY_ROTATION_VECTOR3DEDITUPDOWN_HPP
//#include "ToolUIManaged/prtym/prtyRotation_Vector3dEditUpDown.hpp"
//#endif
//#ifndef PRTY_ROTATION_VECTOR3DEDITRANGED_HPP
//#include "ToolUIManaged/prtym/prtyRotation_Vector3dEditRanged.hpp"
//#endif
//#ifndef PRTY_VECTOR3D_VECTOR3DEDIT_HPP
//#include "ToolUIManaged/prtym/prtyVector3d_Vector3dEdit.hpp"
//#endif
//#ifndef PRTY_VECTOR3D_VECTOR3DEDITUPDOWN_HPP
//#include "ToolUIManaged/prtym/prtyVector3d_Vector3dEditUpDown.hpp"
//#endif
//#ifndef PRTY_VECTOR3D_VECTOR3DEDITRANGED_HPP
//#include "ToolUIManaged/prtym/prtyVector3d_Vector3dEditRanged.hpp"
//#endif
//
//#include <limits>
//#include <string>
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//public ref class prtyControlFactoryTMC : public prtyControlFactory
//{
//	public:
//		//----------------------------------------------------------------------------
//		//	Check for Control + Property pairing
//		//----------------------------------------------------------------------------
//		virtual prtyControl^ prtyControlFactoryTMC::CreateControl(prtyPropertyUIInfo* i_pUIInfo) override
//		{
//			prtyControl^ pPropertyControl = nullptr;
//			const std::string& control_name		= i_pUIInfo->GetControlName();
//
//			// only need first one (since the rest should be the same at this point)
//			const std::string& property_type	= i_pUIInfo->GetProperty(0)->GetType();
//
//			if ( strcmp(control_name.c_str(),"FloatEdit") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"Angle") == 0)
//				{
//					pPropertyControl = gcnew prtyAngle_FloatEdit(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Float") == 0)
//				{
//					pPropertyControl = gcnew prtyFloat_FloatEdit(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Int8") == 0)
//				{
//					pPropertyControl = gcnew prtyInt8_FloatEdit(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Int32") == 0)
//				{
//					pPropertyControl = gcnew prtyInt32_FloatEdit(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"RangedFloat") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"Angle") == 0)
//				{
//					pPropertyControl = gcnew prtyAngle_RangedFloat(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Float") == 0)
//				{
//					pPropertyControl = gcnew prtyFloat_RangedFloat(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Int8") == 0)
//				{
//					pPropertyControl = gcnew prtyInt8_RangedFloat(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Int8") == 0)
//				{
//					pPropertyControl = gcnew prtyInt32_RangedFloat(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"ColorRGBAEdit") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"Color") == 0)
//				{
//					pPropertyControl = gcnew prtyColor_ColorRGBAEdit(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"ColorRGBEdit") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"Color") == 0)
//				{
//					pPropertyControl = gcnew prtyColor_ColorRGBEdit(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"FolderChooser") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"Directory") == 0)
//				{
//					pPropertyControl = gcnew prtyDirectory_FolderChooser(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"FileChooser") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"FilePath") == 0)
//				{
//					pPropertyControl = gcnew prtyFilePath_FileChooser(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"FileName") == 0)
//				{
//					pPropertyControl = gcnew prtyFileName_FileChooser(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"Vector3dEdit") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"Vector3d") == 0)
//				{
//					pPropertyControl = gcnew prtyVector3d_Vector3dEdit(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Point3d") == 0)
//				{
//					pPropertyControl = gcnew prtyVector3d_Vector3dEdit(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Rotation") == 0)
//				{
//					pPropertyControl = gcnew prtyRotation_Vector3dEdit(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"Vector3dEditUpDown") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"Vector3d") == 0)
//				{
//					pPropertyControl = gcnew prtyVector3d_Vector3dEditUpDown(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Point3d") == 0)
//				{
//					pPropertyControl = gcnew prtyVector3d_Vector3dEditUpDown(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Rotation") == 0)
//				{
//					pPropertyControl = gcnew prtyRotation_Vector3dEditUpDown(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"Vector3dEditRanged") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"Vector3d") == 0)
//				{
//					pPropertyControl = gcnew prtyVector3d_Vector3dEditRanged(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Point3d") == 0)
//				{
//					pPropertyControl = gcnew prtyVector3d_Vector3dEditRanged(i_pUIInfo);
//					return pPropertyControl;
//				}
//				else if ( strcmp(property_type.c_str(),"Rotation") == 0)
//				{
//					pPropertyControl = gcnew prtyRotation_Vector3dEditRanged(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			else if ( strcmp(control_name.c_str(),"KeyCombo") == 0)
//			{
//				if ( strcmp(property_type.c_str(),"HotKey") == 0)
//				{
//					pPropertyControl = gcnew prtyHotKey_KeyCombo(i_pUIInfo);
//					return pPropertyControl;
//				}
//			}
//			return nullptr;
//		};
//};
//
//#endif // _MANAGED
