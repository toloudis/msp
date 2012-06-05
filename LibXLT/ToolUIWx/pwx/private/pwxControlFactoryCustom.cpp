/****************************************************************************\
**	pwxControlFactoryCustom.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/pwxControlFactoryCustom.hpp"

#include "ToolUIWx/pwx/Controls/pwxColor_ColorRGBAEdit.hpp"
#include "ToolUIWx/pwx/Controls/pwxColor_ColorRGBEdit.hpp"
#include "ToolUIWx/pwx/Controls/pwxDirectory_DirPicker.hpp"
#include "ToolUIWx/pwx/Controls/pwxFileName_FilePicker.hpp"
#include "ToolUIWx/pwx/Controls/pwxFilePath_FilePicker.hpp"
#include "ToolUIWx/pwx/Controls/pwxFloat_FloatEdit.hpp"
#include "ToolUIWx/pwx/Controls/pwxFloat_NumericUpDown.hpp"
#include "ToolUIWx/pwx/Controls/pwxFloat_RangedFloat.hpp"
#include "ToolUIWx/pwx/Controls/pwxGradient_GradientColorEdit.hpp"
#include "ToolUIWx/pwx/Controls/pwxHotKey_KeyCombo.hpp"
#include "ToolUIWx/pwx/Controls/pwxInt32_FloatEdit.hpp"
#include "ToolUIWx/pwx/Controls/pwxInt32_NumericUpDown.hpp"
#include "ToolUIWx/pwx/Controls/pwxInt32_RangedFloat.hpp"
#include "ToolUIWx/pwx/Controls/pwxInt8_FloatEdit.hpp"
#include "ToolUIWx/pwx/Controls/pwxInt8_NumericUpDown.hpp"
#include "ToolUIWx/pwx/Controls/pwxInt8_RangedFloat.hpp"
#include "ToolUIWx/pwx/Controls/pwxTextureFileName_TextureFilePicker.hpp"
#include "ToolUIWx/pwx/Controls/pwxRotation_Vector3Edit.hpp"
#include "ToolUIWx/pwx/Controls/pwxRotation_Vector3EditUpDown.hpp"
#include "ToolUIWx/pwx/Controls/pwxVector3d_Vector3Edit.hpp"
#include "ToolUIWx/pwx/Controls/pwxVector3d_Vector3EditUpDown.hpp"


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//	Check for Control + Property pairing
//----------------------------------------------------------------------------
//virtual 
pwxControl* pwxControlFactoryCustom::CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
												 wxWindow* i_pParent)
{
	//pwxControl* return NULL;
	const std::string& control_name		= i_pUIInfo->GetControlName();

	// only need first one (since the rest should be the same at this point)
	const std::string& property_type	= i_pUIInfo->GetProperty(0)->GetType();

	if ( strcmp(control_name.c_str(),"FloatEdit") == 0)
	{
		if ( strcmp(property_type.c_str(),"Float") == 0)
		{
			return new pwxFloat_FloatEdit(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"Int8") == 0)
		{
			return new pwxInt8_FloatEdit(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"Int32") == 0)
		{
			return new pwxInt32_FloatEdit(i_pUIInfo, i_pParent);
			
		}
	}
	else if ( strcmp(control_name.c_str(),"NumericUpDown") == 0)
	{
		//if ( strcmp(property_type.c_str(),"Angle") == 0)
		//{
		//	return new pwxAngle_NumericUpDown(i_pUIInfo, i_pParent);
		//	
		//}
		//else 
		if ( strcmp(property_type.c_str(),"Float") == 0)
		{
			return new pwxFloat_NumericUpDown(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Int8") == 0)
		{
			return new pwxInt8_NumericUpDown(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Int32") == 0)
		{
			return new pwxInt32_NumericUpDown(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"RangedFloat") == 0)
	{
//		if ( strcmp(property_type.c_str(),"Angle") == 0)
//		{
//			return new pwxangle_rangedfloat(i_puiinfo, i_pparent);			
//		}
//		else
		if ( strcmp(property_type.c_str(),"Float") == 0)
		{
			return new pwxFloat_RangedFloat(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Int8") == 0)
		{
			return new pwxInt8_RangedFloat(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Int8") == 0)
		{
			return new pwxInt32_RangedFloat(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"ColorRGBAEdit") == 0)
	{
		if ( strcmp(property_type.c_str(),"Color") == 0)
		{
			return new pwxColor_ColorRGBAEdit(i_pUIInfo, i_pParent);
			
		}
	}
	else if ( strcmp(control_name.c_str(),"ColorRGBEdit") == 0)
	{
		if ( strcmp(property_type.c_str(),"Color") == 0)
		{
			return new pwxColor_ColorRGBEdit(i_pUIInfo, i_pParent);
			
		}
	}
	else if ( strcmp(control_name.c_str(),"FolderChooser") == 0)
	{
		if ( strcmp(property_type.c_str(),"Directory") == 0)
		{
			return new pwxDirectory_DirPicker(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"FileChooser") == 0)
	{
		if ( strcmp(property_type.c_str(),"File Path") == 0)
		{
			return new pwxFilePath_FilePicker(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"FileName") == 0)
		{
			return new pwxFileName_FilePicker(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"TextureFileChooser") == 0)
	{
		if ( strcmp(property_type.c_str(),"TextureFileName") == 0)
		{
			return new pwxTextureFileName_TextureFilePicker(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"Vector3dEdit") == 0)
	{
		if ( strcmp(property_type.c_str(),"Vector3d") == 0)
		{
			return new pwxVector3d_Vector3Edit(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"Point3d") == 0)
		{
			return new pwxPoint3d_Vector3Edit(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"Rotation") == 0)
		{
			return new pwxRotation_Vector3Edit(i_pUIInfo, i_pParent);
			
		}
	}
	else if ( strcmp(control_name.c_str(),"Vector3dEditUpDown") == 0)
	{
		if ( strcmp(property_type.c_str(),"Vector3d") == 0)
		{
			return new pwxVector3d_Vector3EditUpDown(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"Point3d") == 0)
		{
			return new pwxPoint3d_Vector3EditUpDown(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"Rotation") == 0)
		{
			return new pwxRotation_Vector3EditUpDown(i_pUIInfo, i_pParent);
			
		}
	}
	else if ( strcmp(control_name.c_str(),"KeyCombo") == 0)
	{
		if ( strcmp(property_type.c_str(),"HotKey") == 0)
		{
			return new pwxHotKey_KeyCombo(i_pUIInfo, i_pParent);
			
		}
	}
	else if (strcmp(control_name.c_str(),"GradientColorEdit") == 0)
	{
		if ( strcmp(property_type.c_str(),"Gradient") == 0)
		{
			return new pwxGradient_GradientColorEdit(i_pUIInfo, i_pParent);

		}
	}

	// nobody uses Vector3dEditRanged
	//else if ( strcmp(control_name.c_str(),"Vector3dEditRanged") == 0)
	//{
	//	if ( strcmp(property_type.c_str(),"Vector3d") == 0)
	//	{
	//		return new pwxVector3d_Vector3dEditRanged(i_pUIInfo, i_pParent);
	//		
	//	}
	//	else if ( strcmp(property_type.c_str(),"Point3d") == 0)
	//	{
	//		return new pwxVector3d_Vector3dEditRanged(i_pUIInfo, i_pParent);
	//		
	//	}
	//	else if ( strcmp(property_type.c_str(),"Rotation") == 0)
	//	{
	//		return new pwxRotation_Vector3dEditRanged(i_pUIInfo, i_pParent);
	//		
	//	}
	//}


	return NULL;
}

#endif // USE_WXWIDGETS