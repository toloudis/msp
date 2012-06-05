/*****************************************************************************
**ltstReAttachUtil.cpp
**
**	Utility for finding object by name. 
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/ltst/GUI/ltstReAttachUtil.hpp"
#include "Support/ltst/GUI/ltstReAttachForm.h"
#include "Support/ltst/ltstLightSetMgr.hpp"

#include "Support/gsup/gsupReAttachUtil.hpp"

#include "Core/dbg/dbgLog.hpp"

#include <map>

//============================================================================
//============================================================================
namespace ltstReAttachUtil
{
	namespace
	{
		bool l_bAllowDialogs = true;
		std::map<std::string, nameObject*> l_ReattachCache;

		nameObject* attempt_get( nameString& i_String )
		{
			// Find object by name
			nameObject* obj = nameMgr::GetObjectByName( i_String );	// name of prop to target

			// Because this is attachment, need to have a nameObject
			if (obj && dynamic_cast<nameObject*>(obj))
			{
				return obj;
				//nameObject *pObj = dynamic_cast<nameObject*>(obj);
				//return pObj;
			}
			return NULL;
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetAllowDialogs(bool i_bAllow)
	{
		l_bAllowDialogs = i_bAllow;
	}

	//--------------------------------------------------------------------
	// Clear out names that have been cached from previous dialogs.
	// Should be cleared when reading a new file.
	//--------------------------------------------------------------------
	void ClearCache()
	{
		l_ReattachCache.clear();
	}

	//--------------------------------------------------------------------
	//	Returns object that matches nameString. May display a dialog
	//	if object is not found. May return NULL.
	//--------------------------------------------------------------------
	nameObject* GetObjByName(const nameString& i_String, bool i_IsObject)
	{
		if (i_String.IsEmpty()) return NULL;

		// Is there really any need to use the name uids here? 
		// Often, they can't be trusted.

		// Find object by name (including ID)
//		nameObject *id_match = attempt_get(i_String);

		// Also, find object which matches string (using invalid UID)
		nameString no_uid = i_String;
		no_uid.SetUID(nameString::e_InvalidUID);
		nameObject *string_match = attempt_get(no_uid);

		// If they are the same, then everything is fine. 
//		if (id_match != NULL && id_match == string_match)
//			return id_match;

		// If there is a string match and it doesn't match the UID match,
		// then something has gone wrong in the name uids (importing, for instance)
		// So, just use the one that matches the string.
		if (string_match) return string_match;

		// uid match, no string match. Return the id_match 
//		if (id_match) return id_match;

		if (l_bAllowDialogs)
		{
			// Check our map to see if the replacement has already been answered
			// in an earlier dialog
			std::map<std::string, nameObject*>::iterator it = l_ReattachCache.find(i_String.GetString());
			if (it != l_ReattachCache.end())
			{
				return it->second;
			}
	
#ifdef _MANAGED		
			// If our name doesn't match anything, open a dialog to find 
			// new object to reattach to.
			LTSTFramework::ltstReAttachForm ^dialog = gcnew LTSTFramework::ltstReAttachForm(i_String.GetString(), i_IsObject);


			if (dialog->ShowDialog() == ::DialogResult::OK)
			{
				std::string new_name;
				dialog->GetAttachObject(new_name);
				nameObject *new_object =  attempt_get(nameString(new_name));
				l_ReattachCache[i_String.GetString()] = new_object;

				delete dialog;
				return new_object;
			}

			delete dialog;
#else
			// Prompt for replacement for object that was not found
			std::vector<nameString> nameList;
			std::string message = i_String.GetString();
			std::string title;
			if (i_IsObject)
			{
				ltstLightSetMgr::GetAllObjects(nameList);
				message += "\r\ncannot be found. Choose a new object or press Cancel.";
				title = "Replace light set object";
			}
			else
			{
				ltstLightSetMgr::GetAllLights(nameList);
				message += "\r\ncannot be found. Choose a new light or press Cancel.";
				title = "Replace light set light";

			}
			nameString new_name;
			if (gsupReAttachUtil::PromptReattach(nameList, title, message, new_name))
			{
				nameObject *new_object =  attempt_get(new_name);
				l_ReattachCache[i_String.GetString()] = new_object;
				return new_object;
			}
#endif

			// Even if they press Cancel, we still need to cache that they chose
			// not to replace this object.
			l_ReattachCache[i_String.GetString()] = NULL;
		}
		
		return NULL;
	}

}	// end of namespace
