/*****************************************************************************
**  cmmSourceState.cpp
**
**      see .hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "Systems/Common/Object/cmmSourceState.hpp"

#include "Core/prty/prtyTextBoxUIInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmSourceState::cmmSourceState()
:	m_SourceState("Source")
{
}

cmmSourceState::~cmmSourceState()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmSourceState::SetSourceState(const std::string& i_Text)
{
	m_SourceState.SetValue(i_Text, false);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyPropertyUIInfo* cmmSourceState::CreateSourceStateUIInfo()
{
	prtyTextBoxUIInfo* pUII = new prtyTextBoxUIInfo(&m_SourceState, "Source",
													"Whether this object still follows the scene file it came from");
	pUII->SetReadOnly(true);
	return pUII;
}
