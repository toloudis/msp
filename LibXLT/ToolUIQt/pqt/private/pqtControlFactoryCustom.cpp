/****************************************************************************\
**	pqtControlFactoryCustom.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/pqtControlFactoryCustom.hpp"

#include "ToolUIQt/pqt/Controls/pqtColor_ColorRGBAEdit.hpp"
#include "ToolUIQt/pqt/Controls/pqtColor_ColorRGBEdit.hpp"
#include "ToolUIQt/pqt/Controls/pqtDirectory_DirPicker.hpp"
#include "ToolUIQt/pqt/Controls/pqtFileName_FilePicker.hpp"
#include "ToolUIQt/pqt/Controls/pqtFilePath_FilePicker.hpp"
#include "ToolUIQt/pqt/Controls/pqtFloat_FloatEdit.hpp"
#include "ToolUIQt/pqt/Controls/pqtFloat_NumericUpDown.hpp"
#include "ToolUIQt/pqt/Controls/pqtFloat_RangedFloat.hpp"
#include "ToolUIQt/pqt/Controls/pqtGradient_GradientColorEdit.hpp"
#include "ToolUIQt/pqt/Controls/pqtHotKey_KeyCombo.hpp"
#include "ToolUIQt/pqt/Controls/pqtInt32_FloatEdit.hpp"
#include "ToolUIQt/pqt/Controls/pqtInt32_NumericUpDown.hpp"
#include "ToolUIQt/pqt/Controls/pqtInt32_RangedFloat.hpp"
#include "ToolUIQt/pqt/Controls/pqtInt8_FloatEdit.hpp"
#include "ToolUIQt/pqt/Controls/pqtInt8_NumericUpDown.hpp"
#include "ToolUIQt/pqt/Controls/pqtInt8_RangedFloat.hpp"
#include "ToolUIQt/pqt/Controls/pqtTextureFileName_TextureFilePicker.hpp"
#include "ToolUIQt/pqt/Controls/pqtRotation_Vector3Edit.hpp"
#include "ToolUIQt/pqt/Controls/pqtRotation_Vector3EditUpDown.hpp"
#include "ToolUIQt/pqt/Controls/pqtVector3d_Vector3Edit.hpp"
#include "ToolUIQt/pqt/Controls/pqtVector3d_Vector3EditUpDown.hpp"


//----------------------------------------------------------------------------
//	Check for Control + Property pairing
//----------------------------------------------------------------------------
//virtual 
pqtControl* pqtControlFactoryCustom::CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
												 QWidget* i_pParent)
{
	//pqtControl* return NULL;
	const std::string& control_name		= i_pUIInfo->GetControlName();

	// only need first one (since the rest should be the same at this point)
	const std::string& property_type	= i_pUIInfo->GetProperty(0)->GetType();

	if ( strcmp(control_name.c_str(),"FloatEdit") == 0)
	{
		if ( strcmp(property_type.c_str(),"Float") == 0)
		{
			return new pqtFloat_FloatEdit(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"Int8") == 0)
		{
			return new pqtInt8_FloatEdit(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"Int32") == 0)
		{
			return new pqtInt32_FloatEdit(i_pUIInfo, i_pParent);
			
		}
	}
	else if ( strcmp(control_name.c_str(),"NumericUpDown") == 0)
	{
		//if ( strcmp(property_type.c_str(),"Angle") == 0)
		//{
		//	return new pqtAngle_NumericUpDown(i_pUIInfo, i_pParent);
		//	
		//}
		//else 
		if ( strcmp(property_type.c_str(),"Float") == 0)
		{
			return new pqtFloat_NumericUpDown(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Int8") == 0)
		{
			return new pqtInt8_NumericUpDown(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Int32") == 0)
		{
			return new pqtInt32_NumericUpDown(i_pUIInfo, i_pParent);
		}
	}
#ifdef QT_FINISH_PORT
	else if ( strcmp(control_name.c_str(),"RangedFloat") == 0)
	{
//		if ( strcmp(property_type.c_str(),"Angle") == 0)
//		{
//			return new pqtangle_rangedfloat(i_puiinfo, i_pparent);			
//		}
//		else
		if ( strcmp(property_type.c_str(),"Float") == 0)
		{
			return new pqtFloat_RangedFloat(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Int8") == 0)
		{
			return new pqtInt8_RangedFloat(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"Int8") == 0)
		{
			return new pqtInt32_RangedFloat(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"ColorRGBAEdit") == 0)
	{
		if ( strcmp(property_type.c_str(),"Color") == 0)
		{
			return new pqtColor_ColorRGBAEdit(i_pUIInfo, i_pParent);
			
		}
	}
	else if ( strcmp(control_name.c_str(),"ColorRGBEdit") == 0)
	{
		if ( strcmp(property_type.c_str(),"Color") == 0)
		{
			return new pqtColor_ColorRGBEdit(i_pUIInfo, i_pParent);
			
		}
	}
	else if ( strcmp(control_name.c_str(),"FolderChooser") == 0)
	{
		if ( strcmp(property_type.c_str(),"Directory") == 0)
		{
			return new pqtDirectory_DirPicker(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"FileChooser") == 0)
	{
		if ( strcmp(property_type.c_str(),"File Path") == 0)
		{
			return new pqtFilePath_FilePicker(i_pUIInfo, i_pParent);
		}
		else if ( strcmp(property_type.c_str(),"FileName") == 0)
		{
			return new pqtFileName_FilePicker(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"TextureFileChooser") == 0)
	{
		if ( strcmp(property_type.c_str(),"TextureFileName") == 0)
		{
			return new pqtTextureFileName_TextureFilePicker(i_pUIInfo, i_pParent);
		}
	}
	else if ( strcmp(control_name.c_str(),"Vector3dEdit") == 0)
	{
		if ( strcmp(property_type.c_str(),"Vector3d") == 0)
		{
			return new pqtVector3d_Vector3Edit(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"Point3d") == 0)
		{
			return new pqtPoint3d_Vector3Edit(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"Rotation") == 0)
		{
			return new pqtRotation_Vector3Edit(i_pUIInfo, i_pParent);
			
		}
	}
	else if ( strcmp(control_name.c_str(),"Vector3dEditUpDown") == 0)
	{
		if ( strcmp(property_type.c_str(),"Vector3d") == 0)
		{
			return new pqtVector3d_Vector3EditUpDown(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"Point3d") == 0)
		{
			return new pqtPoint3d_Vector3EditUpDown(i_pUIInfo, i_pParent);
			
		}
		else if ( strcmp(property_type.c_str(),"Rotation") == 0)
		{
			return new pqtRotation_Vector3EditUpDown(i_pUIInfo, i_pParent);
			
		}
	}
	else if ( strcmp(control_name.c_str(),"KeyCombo") == 0)
	{
		if ( strcmp(property_type.c_str(),"HotKey") == 0)
		{
			return new pqtHotKey_KeyCombo(i_pUIInfo, i_pParent);
			
		}
	}
	else if (strcmp(control_name.c_str(),"GradientColorEdit") == 0)
	{
		if ( strcmp(property_type.c_str(),"Gradient") == 0)
		{
			return new pqtGradient_GradientColorEdit(i_pUIInfo, i_pParent);

		}
	}

	// nobody uses Vector3dEditRanged
	//else if ( strcmp(control_name.c_str(),"Vector3dEditRanged") == 0)
	//{
	//	if ( strcmp(property_type.c_str(),"Vector3d") == 0)
	//	{
	//		return new pqtVector3d_Vector3dEditRanged(i_pUIInfo, i_pParent);
	//		
	//	}
	//	else if ( strcmp(property_type.c_str(),"Point3d") == 0)
	//	{
	//		return new pqtVector3d_Vector3dEditRanged(i_pUIInfo, i_pParent);
	//		
	//	}
	//	else if ( strcmp(property_type.c_str(),"Rotation") == 0)
	//	{
	//		return new pqtRotation_Vector3dEditRanged(i_pUIInfo, i_pParent);
	//		
	//	}
	//}
#endif

	return NULL;
}

