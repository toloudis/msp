/*****************************************************************************
**	chtrReplaceOperation.hpp
**
**		An undoable replacement of a whole character (its model, materials,
**	values and drivers) by other data. The character is recreated from the
**	data (chtrObjectMgr::ReplaceCharacter), so the operation finds it by
**	name, not by pointer, and swaps the data it holds on Undo and Redo.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef CHTR_REPLACEOPERATION_HPP
#error chtrReplaceOperation.hpp multiply included
#endif
#define CHTR_REPLACEOPERATION_HPP

#ifndef CHTR_SCRIPTDATA_HPP
#include "Systems/Character/Data/chtrScriptData.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef UNDO_UNDOOPERATION_HPP
#include "Core/undo/undoUndoOperation.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class chtrReplaceOperation : public undoUndoOperation
{
public:
	//--------------------------------------------------------------------
	// i_OldData: the character as it was before the replacement
	//--------------------------------------------------------------------
	chtrReplaceOperation(const nameString& i_Name, const chtrScriptData& i_OldData, const std::string& i_DisplayName);

	virtual std::string GetDisplayName();
	virtual float GetMemoryUsage();
	virtual void Undo();
	virtual void Redo();
	virtual void Commit();
	virtual void Destroy();

private:
	void swap();

	nameString m_Name;
	chtrScriptData m_DataBackup;
	std::string m_DisplayName;
};
