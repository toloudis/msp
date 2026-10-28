/*****************************************************************************
**  mnmSourceState.cpp
**
**      see .hpp
**
**	Extra Large Technology
\****************************************************************************/
#include "Support/mnm/mnmSourceState.hpp"

#include "Core/prty/prtyTextBoxUIInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmSourceState::mnmSourceState()
:	m_SourceState("Source")
{
}

mnmSourceState::~mnmSourceState()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmSourceState::SetSourceState(const std::string& i_Text)
{
	m_SourceState.SetValue(i_Text, false);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyPropertyUIInfo* mnmSourceState::CreateSourceStateUIInfo()
{
	prtyTextBoxUIInfo* pUII = new prtyTextBoxUIInfo(&m_SourceState, "Source",
													"Whether this object still follows the scene file it came from");
	pUII->SetReadOnly(true);
	return pUII;
}
