/****************************************************************************\
**	keyfPython.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Features/Keyframing/keyfPython.hpp"
#include "Features/Keyframing/keyfKeyframeUtil.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/name/nameMgr.hpp"
#include "Core/name/nameObject.hpp"
#include "Tool/pick3d/pick3dPickObject.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"
#include "Support/tmln/tmlnChannel.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"

#include <vector>

namespace
{
#if defined(PYTHON_ENABLED)
	//--------------------------------------------------------------------
	// Get script object by name
	//--------------------------------------------------------------------
	tmlnScriptObject* get_script_object(const char *i_ObjectName)
	{
		nameString name_str(i_ObjectName);
		nameObject *pNameObj = nameMgr::GetObjectByName(name_str);
		if (!pNameObj)
			return NULL;

		tmlnScriptObject* script_obj = dynamic_cast<tmlnScriptObject*>(pNameObj);
		pick3dPickObject* pPickObject = dynamic_cast<pick3dPickObject*>(pNameObj);
		if (pPickObject && !script_obj)
		{
			// If this is not a scripted object, it may be associated 
			// with a script object through the "parent object" relationship
			pick3dPickObject* cur_obj = pPickObject;
			while (!script_obj)
			{
				cur_obj = cur_obj->GetParentObject();
				if (!cur_obj) break;
				script_obj = dynamic_cast<tmlnScriptObject*>(cur_obj);
			}
		}

		return script_obj;
	}

	//============================================================================
	// Commands made available to python
	//============================================================================

	//--------------------------------------------------------------------
	// Create key driver on channel on object of given name
	//--------------------------------------------------------------------
	PyObject *
	key_channel(PyObject *self, PyObject *args)
	{
		const char *objectName, *channelName;
		if (!PyArg_ParseTuple(args, "ss", &objectName, &channelName))
			return NULL;

		tmlnScriptObject* pScriptObj =  get_script_object(objectName);
		if (!pScriptObj)
		{
			PyErr_SetString(PyExc_NameError, "Script object with given name does not exist.");
			return NULL;
		}

		// Find channel by name
		tmlnChannelSet &channel_set = pScriptObj->ChannelSet();
		std::string desired_channel(channelName);
		for (int i=0; i<channel_set.GetNumChannels(); ++i)
		{
			tmlnChannel& channel = channel_set.Channel(i);
			std::string tokenName = pythUtil::MakeToken(channel.GetName());
			if (tokenName == desired_channel)
			{
				if (keyfKeyframeUtil::AlterOrCreateKey(pScriptObj, &channel))
				{
					// Should be returning driver somehow here
					Py_INCREF(Py_None);
					return Py_None;
				}
				else
				{
					PyErr_SetString(PyExc_TypeError, "Could not create key driver.");
					return NULL;
				}
			}
		}

		PyErr_SetString(PyExc_NameError, "Could not find channel of given name for this object.");
		return NULL;
	}

#endif
}	// end of namespace

//--------------------------------------------------------------------
// Add commands related to the selection list
//--------------------------------------------------------------------
void keyfPython::AddCommands(const std::string &i_ModuleName)
{
#if defined(PYTHON_ENABLED)
	pythModules::AddCommand(i_ModuleName, "keyChannel", 
		"Create driver key on channel with given name on object of given name.\n"
		"	If driver already exists at this time, it will try to update its value.\n"
		"	If no driver already exists it will create one, even if there are no differences.\n"
		"	keyChannel(objectName, channelName)", 
		key_channel);
#endif

}