/*****************************************************************************
**	xtraOperations.cpp
**
**	Undoable operations related to custom properties
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/xtra/GUI/xtraOperations.hpp"

#include "Support/xtra/xtraPropertyData.hpp"
#include "Support/xtra/xtraScriptObject.hpp"
#include "Systems/Common/Gui/cmmObjectDialogUtil.hpp"

#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiPropertyDialog.hpp"


//============================================================================
//============================================================================
namespace xtraOperations
{
	namespace
	{
		const char *c_AddOperationDisplayName = "Create Custom Property";
		//const char *c_DeleteOperationDisplayName = "Delete Custom Property";

		void setup_basic_prtys(xtraPropertyData& io_Data,
							   prtyPropertyUIInfoContainer &io_AttrInfo)
		{
			io_AttrInfo.Add(new prtyTextBoxUIInfo(&io_Data.m_Name));
			io_AttrInfo.Add(new prtyTextBoxUIInfo(&io_Data.m_Category));
			io_AttrInfo.Add(new prtyTextBoxUIInfo(&io_Data.m_Description));
		}

		typedef bool (*ValidateDataFunction)(xtraPropertyData& io_Data);

		void do_dialog(	xtraScriptObject* i_pObject,
						xtraPropertyData& io_Data,
						prtyPropertyUIInfoContainer &io_AttrInfo,
						const char* i_DialogTitle,
						const char* i_DialogMessage,
						ValidateDataFunction i_ValidateFunction = NULL)
		{
			while (guiPropertyDialog::ShowModal(i_DialogTitle, 
											 io_AttrInfo, 
											 i_DialogMessage) == guiPropertyDialog::e_OK)
			{
				if (io_Data.m_Name.GetValue().empty())
				{
					guiMessageBox::Show("Please choose a name for the property.", "Property name required", guiMessageBox::e_OKOnly);
				}
				else if (i_pObject->HasPropertyWithName(io_Data.m_Name.GetValue()))
				{
					guiMessageBox::Show("A property with this name already exists.", "Property names must be unique", guiMessageBox::e_OKOnly);
				}
				else
				{
					bool bValidated = true;
					if (i_ValidateFunction != NULL)
					{
						bValidated = (*i_ValidateFunction)(io_Data);
					}
					if (bValidated)
					{
						if (io_Data.m_Category.GetValue().empty())
							io_Data.m_Category.SetValue("Extras");
						CreateCustomProperty(i_pObject, io_Data);
						break;
					}
				}
			}
		}
		
		bool validate_float_data(xtraPropertyData& io_Data)
		{
			xtraFloatPropertyData *pFloatPrty = dynamic_cast<xtraFloatPropertyData*>(&io_Data);
			DBG_ASSERT(pFloatPrty, "Wrong data structure type for custom property");

			if (pFloatPrty->m_Maximum.GetValue() <= pFloatPrty->m_Minimum.GetValue()) 
			{
				guiMessageBox::Show("Minimum value must be less than maximum value.", "Range required", guiMessageBox::e_OKOnly);
				return false;
			}
			
			// Clamp initial value to within range
			if (pFloatPrty->m_Value.GetValue() < pFloatPrty->m_Minimum.GetValue())
				pFloatPrty->m_Value.SetValue( pFloatPrty->m_Minimum.GetValue() );
			else if (pFloatPrty->m_Value.GetValue() > pFloatPrty->m_Maximum.GetValue())
				pFloatPrty->m_Value.SetValue( pFloatPrty->m_Maximum.GetValue() );

			return true;
		}

	}	// end of namespace


	//--------------------------------------------------------------------
	//	Prompt user for new boolean property
	//--------------------------------------------------------------------
	void  CreateCustomBoolean(xtraScriptObject* i_pObject)
	{
		xtraBooleanPropertyData data;
		prtyPropertyUIInfoContainer attrInfo;
		setup_basic_prtys(data, attrInfo);
		do_dialog(i_pObject, data, attrInfo, "Boolean property", "Create new boolean property");
	}


	//--------------------------------------------------------------------
	//	Prompt user for new float property
	//--------------------------------------------------------------------
	void  CreateCustomFloat(xtraScriptObject* i_pObject)
	{		
		xtraFloatPropertyData data;
		data.m_Type = e_Float;
		prtyPropertyUIInfoContainer attrInfo;
		setup_basic_prtys(data, attrInfo);
		attrInfo.Add(new prtyFloatEditUIInfo(&data.m_Minimum));
		attrInfo.Add(new prtyFloatEditUIInfo(&data.m_Maximum));
		attrInfo.Add(new prtyFloatEditUIInfo(&data.m_DecimalPlaces));
		do_dialog(i_pObject, data, attrInfo, "Number property", "Create new ranged number property", &validate_float_data);
	}

	//--------------------------------------------------------------------
	//	Prompt user for new color property
	//--------------------------------------------------------------------
	void  CreateCustomColor(xtraScriptObject* i_pObject)
	{
		xtraColorPropertyData data;
		prtyPropertyUIInfoContainer attrInfo;
		setup_basic_prtys(data, attrInfo);
		attrInfo.Add(new prtyCheckBoxUIInfo(&data.m_bShowAlpha));
		do_dialog(i_pObject, data, attrInfo, "Color property", "Create new color property");
	}

	//--------------------------------------------------------------------
	//	Prompt user for new string property
	//--------------------------------------------------------------------
	void  CreateCustomString(xtraScriptObject* i_pObject)
	{
		xtraStringPropertyData data;
		prtyPropertyUIInfoContainer attrInfo;
		setup_basic_prtys(data, attrInfo);
		//bga - having trouble with multiple line "ENTER" callbacks, commenting out for now
		//attrInfo.Add(new prtyCheckBoxUIInfo(&data.m_bMultiline));
		do_dialog(i_pObject, data, attrInfo, "String property", "Create new string property");
	}

	//--------------------------------------------------------------------
	//	Prompt user for new position property
	//--------------------------------------------------------------------
	void  CreateCustomPosition(xtraScriptObject* i_pObject)
	{
		xtraPositionPropertyData data;
		prtyPropertyUIInfoContainer attrInfo;
		setup_basic_prtys(data, attrInfo);
		do_dialog(i_pObject, data, attrInfo, "Position property", "Create new position property");
	}

	//--------------------------------------------------------------------
	//	Prompt user for new orientation property
	//--------------------------------------------------------------------
	void  CreateCustomOrientation(xtraScriptObject* i_pObject)
	{
		xtraOrientationPropertyData data;
		prtyPropertyUIInfoContainer attrInfo;
		setup_basic_prtys(data, attrInfo);
		do_dialog(i_pObject, data, attrInfo, "Orientation property", "Create new orientation property");
	}

	//--------------------------------------------------------------------
	//	Prompt user for new texture property
	//--------------------------------------------------------------------
	void  CreateCustomTexture(xtraScriptObject* i_pObject)
	{
		xtraTexturePropertyData data;
		prtyPropertyUIInfoContainer attrInfo;
		setup_basic_prtys(data, attrInfo);
		do_dialog(i_pObject, data, attrInfo, "Texture property", "Create new texture property");
	}

	//--------------------------------------------------------------------
	//	Create new custom property based on given data.
	//  Returns true if the property was created, may return false if 
	//	name conflicts.
	//--------------------------------------------------------------------
	bool  CreateCustomProperty(xtraScriptObject* i_pObject,
							   const xtraPropertyData& i_Data)
	{

		bool bSuccess = i_pObject->AddCustomProperty(i_Data);
		if (bSuccess)
		{
		//	undoUndoMgr::AddOperation(new xtraCreateOperation(i_pObject, i_Data.m_Name.GetValue(), c_AddOperationDisplayName));
			cmmObjectDialogUtil::UpdateDialog();
		}
		return bSuccess;
	}

	//--------------------------------------------------------------------
	//	Delete control with given name.
	//--------------------------------------------------------------------
	//void  DeleteControl(xtraScriptObject* i_pObject,
	//					const std::string& i_PartName)
	//{
	//	// make the undo operation before the data is removed from the system
	//	undoUndoMgr::AddOperation(new xtraDeleteOperation(i_pObject, i_PartName, c_DeleteOperationDisplayName));

	//	const bool bDeleteDrivers = true;
	//	i_pObject->DeleteControl(i_PartName, bDeleteDrivers);
	//}

}	// end of namespace
