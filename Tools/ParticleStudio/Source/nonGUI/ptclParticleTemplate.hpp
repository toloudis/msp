/*****************************************************************************
**  ptclParticleTemplate.hpp
**
**      The ptclParticleTemplate represents the particle template data.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PTCL_PARTICLETEMPLATE_HPP
#error ptclParticleTemplate.hpp multiply included
#endif
#define PTCL_PARTICLETEMPLATE_HPP

#ifndef PRT_PARTICLEGENERATORTEMPLATE_HPP
#include "Graphics/prt/prtParticleGeneratorTemplate.hpp"
#endif


//============================================================================
//============================================================================
namespace ptclParticleTemplate
{
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	prtParticleGeneratorTemplate* GetTemplate();

	//--------------------------------------------------------------------------
	//	create the template
	//--------------------------------------------------------------------------
	void Init();

	//--------------------------------------------------------------------------
	//	clear out the template (free texture, etc.)
	//--------------------------------------------------------------------------
	void CleanUp();
};
