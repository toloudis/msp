/**********************************************************
**  envAuditor.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\*********************************************************/
#include "Core/env/envAuditor.hpp"

#include "Core/env/envSTLHelpers.hpp"


//------------------------------------------------------------------------
//------------------------------------------------------------------------
//virtual 
envAuditor::~envAuditor()
{
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
//virtual 
envAuditable::~envAuditable()
{
	// Go through the loop backwards because it may be tempting
	// for the auditors to call RemoveAuditor in the callback.
	const size_t num_auditors = m_Auditors.size();
	if (num_auditors > 0) {
		for (size_t i=num_auditors-1; i>=0; i--)
		{
			m_Auditors[i]->AuditorNotify(this);
		}
	}
}

//------------------------------------------------------------------------
// Add observer to list. This pointer is just used, not owned.
//------------------------------------------------------------------------
void envAuditable::AddAuditor(envAuditor* i_pObserver)
{
	m_Auditors.push_back(i_pObserver);
}

//------------------------------------------------------------------------
// Remove observer from list.
//------------------------------------------------------------------------
void envAuditable::RemoveAuditor(envAuditor* i_pObserver)
{
	envSTLHelpers::RemoveOneValue(m_Auditors, i_pObserver);
}


