/*****************************************************************************
**	chtrReplaceOperation.cpp
**
**		see .hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "Systems/Character/Undo/chtrReplaceOperation.hpp"

#ifndef CHTR_DOCUMENTCHUNK_HPP
#include "Systems/Character/Data/chtrDocumentChunk.hpp"
#endif
#ifndef CHTR_OBJECTMGR_HPP
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#endif
#ifndef CHNL_DIALOGUTIL_HPP
#include "Features/Channels/chnlDialogUtil.hpp"
#endif

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrReplaceOperation::chtrReplaceOperation(const nameString& i_Name, const chtrScriptData& i_OldData, const std::string& i_DisplayName)
:	m_Name(i_Name),
	m_DataBackup(i_OldData),
	m_DisplayName(i_DisplayName)
{
}

std::string chtrReplaceOperation::GetDisplayName()
{
	return m_DisplayName;
}

float chtrReplaceOperation::GetMemoryUsage()
{
	return (sizeof(chtrScriptData) / 1000.0f); // convert to KB
}

void chtrReplaceOperation::Undo()
{
	swap();
}

void chtrReplaceOperation::Redo()
{
	swap();
}

void chtrReplaceOperation::Commit()
{
}

void chtrReplaceOperation::Destroy()
{
}

//--------------------------------------------------------------------
// the character becomes the held data, and the held data what it was
//--------------------------------------------------------------------
void chtrReplaceOperation::swap()
{
	const int index = chtrObjectMgr::GetIndexForObject(m_Name);
	if (index < 0)
	{
		DBG_WARNING("Replace character undo: no character " << m_Name.GetString());
		return;
	}

	chtrScriptData current = chtrObjectMgr::GetScriptData(index);
	chtrObjectMgr::ReplaceCharacter(index, m_DataBackup);
	m_DataBackup = current;

	chtrDocumentChunk::ActiveDataChanged();
	chnlDialogUtil::UpdateChannels();
}
