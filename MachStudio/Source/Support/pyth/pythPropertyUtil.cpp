/****************************************************************************\
**	pythPropertyUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/pythPropertyUtil.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/Ma/maConstants.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Core/prty/prtyBoolean.hpp"
#include "Core/prty/prtyColor.hpp"
#include "Core/prty/prtyDirectory.hpp"
#include "Core/prty/prtyEnum.hpp"
#include "Core/prty/prtyFileName.hpp"
#include "Core/prty/prtyFilePath.hpp"
#include "Core/prty/prtyFloat.hpp"
#include "Core/prty/prtyInt32.hpp"
#include "Core/prty/prtyListChecked.hpp"
#include "Core/prty/prtyName.hpp"
#include "Core/prty/prtyPoint3d.hpp"
#include "Core/prty/prtyRotation.hpp"
#include "Core/prty/prtyText.hpp"
#include "Core/prty/prtyTextureFileName.hpp"
#include "Core/prty/prtyTime.hpp"
#include "Core/prty/prtyVector3d.hpp"


//============================================================================
//============================================================================
namespace pythPropertyUtil
{

	//--------------------------------------------------------------------
	// Get property by name from given object
	//--------------------------------------------------------------------
	prtyProperty* get_property(prtyObject* i_pObject, const std::string &i_PropertyName)
	{
		//DBG_LOG("----------------------------get property");
		const PropertyUIIList &prty_list = i_pObject->GetList();
		PropertyUIIList::const_iterator it;
		for (it = prty_list.begin(); it != prty_list.end(); ++it)
		{
			for (int i=0; i<(*it)->GetNumberOfProperties(); i++)
			{
				std::string prty_name = pythUtil::MakeToken((*it)->GetProperty(i)->GetPropertyName());
				
				//DBG_LOG4("%03d) [%20s] (%20s) vs (%20s)", i, (*it)->GetControlName().c_str(), prty_name.c_str(), i_PropertyName.c_str() );

				if (i_PropertyName == prty_name)
				{
					return (*it)->GetProperty(i);
				}
			}
		}
		return NULL;
	}

	#if defined(PYTHON_ENABLED)
	//--------------------------------------------------------------------
	// get list of all properties for given object
	//--------------------------------------------------------------------
	PyObject* get_properties(prtyObject *pPrtyObj)
	{
		std::vector<prtyProperty*> properties;
		const PropertyUIIList &prty_list = pPrtyObj->GetList();
		PropertyUIIList::const_iterator it;
		for (it = prty_list.begin(); it != prty_list.end(); ++it)
		{
			for (int i=0; i<(*it)->GetNumberOfProperties(); i++)
			{
				properties.push_back((*it)->GetProperty(i));
			}
		}

		const int num_names = properties.size();
		PyObject* PrtyList = PyList_New(num_names);
		if (PrtyList != NULL)
		{
			for (int i=0; i<num_names; ++i)
			{
				// Convert property into a single token
				std::string prty_name = pythUtil::MakeToken(properties[i]->GetPropertyName());
				PyList_SetItem(PrtyList, i, PyString_FromString(prty_name.c_str()));
			}

		//? Py_DECREF(MyList);
		}
		
		return PrtyList;
	}


	PyObject* set_value(prtyProperty *pProperty, prtyObject *pPrtyObj, 
						PyObject *pValueArg, bool i_bPreserveNameUID)
	{
		if (!pProperty) return NULL;

		// Create undo operation for property we are about to change
		if (pPrtyObj)
			pPrtyObj->CreateUndoForProperty(*pProperty);

		// Set value based on type, sometimes as tuple
		const bool bSetDirty = true;
		if (prtyBoolean *pPropCast = dynamic_cast<prtyBoolean*>(pProperty))
		{
			if (pValueArg == Py_True)
				pPropCast->SetValue(true, bSetDirty);
			else 
			if (pValueArg == Py_False)
				pPropCast->SetValue(false, bSetDirty);
			else
				return NULL;

			//bool value = (0 != PyInt_AsLong( pValueArg )); // only integer type in python is Long?
			//if (!PyErr_Occurred())
			//	pPropCast->SetValue(value, bSetDirty);
			//else
			//	return NULL;
		}
		else if (prtyColor *pPropCast = dynamic_cast<prtyColor*>(pProperty))
		{
			float r,g,b,a;
			if (!PyArg_ParseTuple(pValueArg, "ffff", &r, &g, &b, &a))
				return NULL;
			pPropCast->SetValue(maFloatRGBA(r,g,b,a), bSetDirty);
		}
		else if (prtyDirectory *pPropCast = dynamic_cast<prtyDirectory*>(pProperty))
		{
			const char *pFilename = PyString_AsString( pValueArg );
			if (!pFilename)
				return NULL;
			fsLocator locator;
			fsFileUtil::ANSIFilenameToLocator(pFilename, locator);
			pPropCast->SetValue(locator, bSetDirty);
		}
		else if (prtyEnum *pPropCast = dynamic_cast<prtyEnum*>(pProperty))
		{
			const char *pTag = PyString_AsString( pValueArg );
			if (!pTag)
				return NULL;

			std::string tag(pTag);
			const int max_tags = pPropCast->GetNumTags();
			int i;
			for (i=0; i<max_tags; i++)
			{
				if (tag == pPropCast->GetEnumTag(i))
				{
					pPropCast->SetValue(i, bSetDirty);
					break;
				}
			}

			if (i == max_tags)
			{
				PyErr_SetString(PyExc_NameError, "Do not recognize enumeration string.");
				return NULL;
			}
		}
		else if (prtyFileName *pPropCast = dynamic_cast<prtyFileName*>(pProperty))
		{
			const char *pFilename = PyString_AsString( pValueArg );
			if (!pFilename)
				return NULL;
			pPropCast->SetValue(itString(pFilename), bSetDirty);
		}
		else if (prtyFilePath *pPropCast = dynamic_cast<prtyFilePath*>(pProperty))
		{
			const char *pFilename = PyString_AsString( pValueArg );
			if (!pFilename)
				return NULL;
			fsLocator locator;
			fsFileUtil::ANSIFilenameToLocator(pFilename, locator);
			pPropCast->SetValue(locator, bSetDirty);
		}
		else if (prtyTextureFileName *pPropCast = dynamic_cast<prtyTextureFileName*>(pProperty))
		{
			const char *pFilename = PyString_AsString( pValueArg );
			if (!pFilename)
				return NULL;
			prtyTextureFileData val = pPropCast->GetFullValue();
			fsLocator locator;
			fsFileUtil::ANSIFilenameToLocator(pFilename, locator);
			val.m_TextureLocator = locator;
			pPropCast->SetValue(val, bSetDirty);
		}
		else if (prtyFloat *pPropCast = dynamic_cast<prtyFloat*>(pProperty))
		{
			float value = (float) PyFloat_AsDouble( pValueArg );
			if (!PyErr_Occurred())
				pPropCast->SetValue(value, bSetDirty);
			else
				return NULL;
		}
		else if (prtyInt32 *pPropCast = dynamic_cast<prtyInt32*>(pProperty))
		{
			int value = PyInt_AsLong( pValueArg );
			//DBG_LOG2("setting property int32 %s = %d", pProperty->GetPropertyName().c_str(), value );
			if (!PyErr_Occurred())
				pPropCast->SetValue(value, bSetDirty);
			else
				return NULL;
		}
		else if (prtyInt8 *pPropCast = dynamic_cast<prtyInt8*>(pProperty))
		{
			envType::Int8 value = (envType::Int8)PyInt_AsLong( pValueArg ); // only integer type in python is Long?
			if (!PyErr_Occurred())
				pPropCast->SetValue(value, bSetDirty);
			else
				return NULL;
		}
		else if (prtyListChecked *pPropCast = dynamic_cast<prtyListChecked*>(pProperty))
		{
			int n;
			n = (int)PyList_Size(pValueArg);
			if (n < 0)
				return NULL;		// Not a list

			PyObject *item;
			char* tag;
			bool checked;
			checked_list_type the_list;
			the_list.resize(n);
			for (int i = 0; i < n; i++) 
			{
				item = PyList_GetItem(pValueArg, i);	// Can't fail
				int result = PyArg_ParseTuple(item, "sb", &tag, &checked);
				if (result == 0) 
					continue;							// Skip non-integers

				the_list[i].m_bChecked = checked;
				the_list[i].m_Text = std::string(tag);
			}

			pPropCast->SetValue(the_list, bSetDirty);
		}
		else if (prtyName *pPropCast = dynamic_cast<prtyName*>(pProperty))
		{
			const char *pName = PyString_AsString( pValueArg );
			if (!pName)
				return NULL;

			if (i_bPreserveNameUID)
			{
				// This function should set the name string without changing the UID
				pPropCast->SetValue(pName, bSetDirty);
			}
			else
			{
				// This function resets the name id and would be used when 
				// the name proeprty is being used to refer to another object
				// as opposed to it being the name property of the object itself.
				nameString name_str(pName);
				pPropCast->SetValue(name_str, bSetDirty);
			}
		}
		else if (prtyPoint3d *pPropCast = dynamic_cast<prtyPoint3d*>(pProperty))
		{
			float x,y,z;
			if (!PyArg_ParseTuple(pValueArg, "fff", &x, &y, &z))
				return NULL;
			pPropCast->SetValue(maPoint3d(x,y,z), bSetDirty);
		}
		else if (prtyRotation *pPropCast = dynamic_cast<prtyRotation*>(pProperty))
		{
			float x,y,z;
			if (!PyArg_ParseTuple(pValueArg, "fff", &x, &y, &z))
				return NULL;
			pPropCast->SetEuler(x * maConstants::c_fAngleToRad, 
				y * maConstants::c_fAngleToRad, 
				z * maConstants::c_fAngleToRad, 
				bSetDirty);
		}
		else if (prtyText *pPropCast = dynamic_cast<prtyText*>(pProperty))
		{
			const char *pString = PyString_AsString( pValueArg );
			if (!pString)
				return NULL;
			pPropCast->SetValue(pString, bSetDirty);
		}
		else if (prtyTime *pPropCast = dynamic_cast<prtyTime*>(pProperty))
		{
			float value = (float) PyFloat_AsDouble( pValueArg );
			if (!PyErr_Occurred())
				pPropCast->SetValue(maTime::FromSeconds(value), bSetDirty);
			else
				return NULL;
		}
		else if (prtyVector3d *pPropCast = dynamic_cast<prtyVector3d*>(pProperty))
		{
			float x,y,z;
			if (!PyArg_ParseTuple(pValueArg, "fff", &x, &y, &z))
				return NULL;
			pPropCast->SetValue(maVector3d(x,y,z), bSetDirty);
		}
		else
		{
			PyErr_SetString(PyExc_TypeError, "Do not recognize property type.");
			return NULL;
		}

		Py_INCREF(Py_None);
		return Py_None;
	}

	//--------------------------------------------------------------------
	// set value of given property
	//--------------------------------------------------------------------
	PyObject* set_property(prtyObject *pPrtyObj, PyObject *args, bool i_bPreserveNameUID)
	{
		char *propertyName = PyString_AsString( PyTuple_GetItem( args, 1 ) );

		prtyProperty *pProperty = get_property(pPrtyObj, propertyName);
		if (!pProperty)
		{
			PyErr_SetString(PyExc_NameError, "Could not find property by name.");
			return NULL;
		}

		// Set value based on type, sometimes as tuple
		PyObject *pValueArg = PyTuple_GetItem( args, 2 );
		return set_value(pProperty, pPrtyObj, pValueArg, i_bPreserveNameUID);
	}

	//--------------------------------------------------------------------
	// get value of given property
	//--------------------------------------------------------------------
	PyObject* get_value(prtyProperty* pProperty)
	{
		// Return value based on type, sometimes as tuple
		if (prtyBoolean *pPropCast = dynamic_cast<prtyBoolean*>(pProperty))
			//return Py_BuildValue("b", pPropCast->GetValue());
			return pythFunctionUtil::ConvertBoolean(pPropCast->GetValue());
		if (prtyColor *pPropCast = dynamic_cast<prtyColor*>(pProperty))
			return Py_BuildValue("(f,f,f,f)", pPropCast->GetValue().GetRed(), pPropCast->GetValue().GetGreen(), pPropCast->GetValue().GetBlue(), pPropCast->GetValue().GetAlpha());
		if (prtyDirectory *pPropCast = dynamic_cast<prtyDirectory*>(pProperty))
			return Py_BuildValue("s", pPropCast->GetString().c_str());
		if (prtyEnum *pPropCast = dynamic_cast<prtyEnum*>(pProperty))
			return Py_BuildValue("s", pPropCast->GetEnumTag(pPropCast->GetValue()).c_str());
		if (prtyFileName *pPropCast = dynamic_cast<prtyFileName*>(pProperty))
			return Py_BuildValue("s", pPropCast->GetString().c_str());
		if (prtyFilePath *pPropCast = dynamic_cast<prtyFilePath*>(pProperty))
			return Py_BuildValue("s", pPropCast->GetString().c_str());
		if (prtyTextureFileName *pPropCast = dynamic_cast<prtyTextureFileName*>(pProperty))
			return Py_BuildValue("s", pPropCast->GetString().c_str());
		if (prtyFloat *pPropCast = dynamic_cast<prtyFloat*>(pProperty))
			return Py_BuildValue("f", pPropCast->GetValue());
		if (prtyTime *pPropCast = dynamic_cast<prtyTime*>(pProperty))
			return Py_BuildValue("f", pPropCast->GetValue().AsSeconds());
		if (prtyInt32 *pPropCast = dynamic_cast<prtyInt32*>(pProperty))
			return Py_BuildValue("i", pPropCast->GetValue());
		if (prtyInt8 *pPropCast = dynamic_cast<prtyInt8*>(pProperty))
			return Py_BuildValue("b", pPropCast->GetValue());
		if (prtyListChecked *pPropCast = dynamic_cast<prtyListChecked*>(pProperty))
		{
			PyObject *plist = Py_BuildValue("[]");
			int num = pPropCast->GetNumberOfItems();
			for ( unsigned int i=0; i < num; i++ )
			{
				bool checked = pPropCast->GetValueFlag(i);
				std::string text = pPropCast->GetValueText(i);

				PyObject *lc = Py_BuildValue("(s,b)", 
						text.c_str(),
						checked );

				int append_return = PyList_Append(plist, lc);
				if (lc == NULL || append_return < 0)
				{
					// Method failed. Release res.
					Py_DECREF(plist); // Destroys the partly constructed object
					PyErr_SetString(PyExc_TypeError, "Invalid building of checked list in python");
					return NULL;	// Append should have raised the error.
				}
				Py_DECREF(lc);		// release our reference to the tuple.
			}
			return plist;
		}
		if (prtyName *pPropCast = dynamic_cast<prtyName*>(pProperty))
			return Py_BuildValue("s", pPropCast->GetString().c_str());
		if (prtyPoint3d *pPropCast = dynamic_cast<prtyPoint3d*>(pProperty))
			return Py_BuildValue("(f,f,f)", pPropCast->GetValue()[0], pPropCast->GetValue()[1], pPropCast->GetValue()[2]);
		if (prtyRotation *pPropCast = dynamic_cast<prtyRotation*>(pProperty))
		{
			float x,y,z;
			pPropCast->GetEuler(x,y,z);
			return Py_BuildValue("(f,f,f)", maConstants::c_fRadToAngle * x, maConstants::c_fRadToAngle * y, maConstants::c_fRadToAngle * z );
		}
		if (prtyText *pPropCast = dynamic_cast<prtyText*>(pProperty))
			return Py_BuildValue("s", pPropCast->GetValue().c_str());
		if (prtyVector3d *pPropCast = dynamic_cast<prtyVector3d*>(pProperty))
			return Py_BuildValue("(f,f,f)", pPropCast->GetValue()[0], pPropCast->GetValue()[1], pPropCast->GetValue()[2]);

		PyErr_SetString(PyExc_TypeError, "Do not recognize property type.");
		return NULL;
	}

	//--------------------------------------------------------------------
	// get value of given property by a given property name
	//--------------------------------------------------------------------
	PyObject* get_value(prtyObject *pPrtyObj, const char *propertyName)
	{
		prtyProperty *pProperty = get_property(pPrtyObj, propertyName);
		if (!pProperty)
		{
			PyErr_SetString(PyExc_NameError, "Could not find property by name.");
			return NULL;
		}
		return get_value(pProperty);
	}
	#endif

}