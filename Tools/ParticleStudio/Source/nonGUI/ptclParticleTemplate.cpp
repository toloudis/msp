/*****************************************************************************
**  ptclParticleTemplate.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ptclParticleTemplate.hpp"


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
namespace ptclParticleTemplate
{
	prtParticleGeneratorTemplate* m_pParticleGeneratorTemplate;
}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
prtParticleGeneratorTemplate* ptclParticleTemplate::GetTemplate()
{
	DBG_ASSERT0( m_pParticleGeneratorTemplate != 0, "Particle Generator Template shouldn't be NULL -- something called CleanUp too soon" );
	return m_pParticleGeneratorTemplate;
}


//--------------------------------------------------------------------------
//	create the template
//--------------------------------------------------------------------------
void ptclParticleTemplate::Init()
{
	m_pParticleGeneratorTemplate = new prtParticleGeneratorTemplate();
}

//--------------------------------------------------------------------------
//	clear out the template (free texture, etc.)
//--------------------------------------------------------------------------
void ptclParticleTemplate::CleanUp()
{
	delete m_pParticleGeneratorTemplate;
	m_pParticleGeneratorTemplate = 0;
}

