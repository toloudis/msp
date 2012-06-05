/****************************************************************************\
**	xtraPython.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/xtra/xtraPython.hpp"
#include "Support/xtra/xtraPropertyData.hpp"
#include "Support/xtra/xtraScriptObject.hpp"
#include "Support/xtra/GUI/xtraOperations.hpp"

#include "Support/pyth/pythFunctionUtil.hpp"
#include "Support/pyth/pythModules.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/name/nameObject.hpp"
#include "Core/name/nameMgr.hpp"

#include <vector>


namespace xtraPython
{

	namespace
	{
#if defined(PYTHON_ENABLED)
		//--------------------------------------------------------------------
		// Get xtraScript object by name
		//--------------------------------------------------------------------
		xtraScriptObject* get_xtra_object(const char *i_ObjectName)
		{
			nameString name_str(i_ObjectName);
			nameObject *pNameObj = nameMgr::GetObjectByName(name_str);
			if (!pNameObj)
				return NULL;

			xtraScriptObject* script_obj = dynamic_cast<xtraScriptObject*>(pNameObj);
			relObject* pPickObject = dynamic_cast<relObject*>(pNameObj);
			if (pPickObject && !script_obj)
			{
				// If this is not a scripted object, it may be associated 
				// with a script object through the "parent object" relationship
				relObject* cur_obj = pPickObject;
				while (!script_obj)
				{
					cur_obj = cur_obj->GetParentObject();
					if (!cur_obj) break;
					script_obj = dynamic_cast<xtraScriptObject*>(cur_obj);
				}
			}

			return script_obj;
		}
		
		//============================================================================
		// Commands made available to python
		//============================================================================

		//--------------------------------------------------------------------
		// set value of given property on an object
		//--------------------------------------------------------------------
		PyObject* add_custom_property(PyObject *self, PyObject *args, PyObject *keywds)
		{
			static char *kwlist[] = {"object", "type", "name", "category", "description", 
									 "showalpha", "minimum", "maximum", "decimalplaces", NULL};

			char *objectName = NULL, *type = NULL, *name = NULL;
			char *category = "Extras";
			char *description = "Custom property";
			bool show_alpha = true;
			float minimum = 0;
			float maximum = 100;
			int decimalplaces = 0;
			if (!PyArg_ParseTupleAndKeywords(args, keywds, "sss|ssbffk", kwlist, 
											 &objectName, &type, &name, &category, &description, 
											 &show_alpha, &minimum, &maximum, &decimalplaces))
				return NULL; 

			xtraScriptObject* pExtraObj = get_xtra_object(objectName);
			if (!pExtraObj)
			{
				PyErr_SetString(PyExc_NameError, "Could not find object by name.");
				return NULL;
			}

			std::string type_name(type); // make a token? make lowercase?
			if (type_name == "boolean")
			{
				xtraBooleanPropertyData xtra_data;
				xtra_data.m_Name = name;
				xtra_data.m_Category = category;
				xtra_data.m_Description = description;
				xtraOperations::CreateCustomProperty(pExtraObj, xtra_data);
				return pythFunctionUtil::ReturnNone();
			}
			else if (type_name == "color")
			{
				xtraColorPropertyData xtra_data;
				xtra_data.m_Name = name;
				xtra_data.m_Category = category;
				xtra_data.m_Description = description;
				xtra_data.m_bShowAlpha = show_alpha;
				xtraOperations::CreateCustomProperty(pExtraObj, xtra_data);
				return pythFunctionUtil::ReturnNone();
			}
			else if (type_name == "number")
			{
				xtraFloatPropertyData xtra_data;
				xtra_data.m_Name = name;
				xtra_data.m_Category = category;
				xtra_data.m_Description = description;
				xtra_data.m_Minimum = minimum;
				xtra_data.m_Maximum = maximum;
				xtra_data.m_DecimalPlaces = decimalplaces;
				xtraOperations::CreateCustomProperty(pExtraObj, xtra_data);
				return pythFunctionUtil::ReturnNone();
			}
			else if (type_name == "orientation")
			{
				xtraOrientationPropertyData xtra_data;
				xtra_data.m_Name = name;
				xtra_data.m_Category = category;
				xtra_data.m_Description = description;
				xtraOperations::CreateCustomProperty(pExtraObj, xtra_data);
				return pythFunctionUtil::ReturnNone();
			}
			else if (type_name == "position")
			{
				xtraPositionPropertyData xtra_data;
				xtra_data.m_Name = name;
				xtra_data.m_Category = category;
				xtra_data.m_Description = description;
				xtraOperations::CreateCustomProperty(pExtraObj, xtra_data);
				return pythFunctionUtil::ReturnNone();
			}
			else if (type_name == "string")
			{
				xtraStringPropertyData xtra_data;
				xtra_data.m_Name = name;
				xtra_data.m_Category = category;
				xtra_data.m_Description = description;
				xtraOperations::CreateCustomProperty(pExtraObj, xtra_data);
				return pythFunctionUtil::ReturnNone();
			}
			else if (type_name == "texture")
			{
				xtraTexturePropertyData xtra_data;
				xtra_data.m_Name = name;
				xtra_data.m_Category = category;
				xtra_data.m_Description = description;
				xtraOperations::CreateCustomProperty(pExtraObj, xtra_data);
				return pythFunctionUtil::ReturnNone();
			}
			else
			{
				PyErr_SetString(PyExc_AttributeError, "Unsupported type name");
				return NULL;
			}
		}


#endif
	}	// end of namespace


#if defined(PYTHON_ENABLED)
	//--------------------------------------------------------------------
	// Add commands related to the selection list
	//--------------------------------------------------------------------
	void AddCommands(const std::string &i_ModuleName)
	{
		const bool c_bKeywordsArgs = true;
		pythModules::AddCommand(i_ModuleName, 
			"addCustomProperty", 
			"Add a custom property to an object."
			"	addCustomProperty(objectName, typeOfProperty, propertyName)"
			" typeOfProperty can be one of: boolean, color, number, orientation, position, string, texture",
			(pythModules::CommandFunctionPtr)add_custom_property, c_bKeywordsArgs);
	}
#endif

}	// end of namespace

