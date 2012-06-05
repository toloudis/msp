/*****************************************************************************
**	prtGeneratorUtil.hpp
**
**		The prtGeneratorUtil provides useful functions relating to 
**	prtParticleGenerators and prtParticleGeneratorTemplates.
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_GENERATORUTIL_HPP
#error prtGeneratorUtil.hpp multiply included
#endif
#define PRT_GENERATORUTIL_HPP

#ifndef PRT_PARTICLEGENERATORTEMPLATE_HPP
#include "Graphics/prt/prtParticleGeneratorTemplate.hpp"
#endif


//============================================================================
//============================================================================
class fsLocator;


//============================================================================
//============================================================================
namespace prtGeneratorUtil
{
	//----------------------------------------------------------------------------
	//	Read reads the template from a binary chunk file given by the locator.
	//----------------------------------------------------------------------------
	void Read(const fsLocator& i_Locator, prtParticleGeneratorTemplate& o_Template);

	//----------------------------------------------------------------------------
	//	Write writes the template to a binary chunk file given by the locator.
	//----------------------------------------------------------------------------
	void Write(const fsLocator& i_Locator, const prtParticleGeneratorTemplate& i_Template);

	//----------------------------------------------------------------------------
	//	MakeTemplateFromGenerator does just that.  To make a generator from a
	//	template, you'll want to use the scScene.
	//----------------------------------------------------------------------------
	void MakeTemplateFromGenerator(	prtParticleGeneratorTemplate& o_Template,
									const prtParticleGenerator& i_Generator);

	//----------------------------------------------------------------------------
	//	MakeGenerator makes a generator from the parameters in the given template.
	//
	//	NOTE: this function will return a NULL if the SetAllowParticles is set
	//	to false.
	//----------------------------------------------------------------------------
	prtParticleGenerator* MakeGenerator( const prtParticleGeneratorTemplate& i_Template, 
										float i_SimulationTime );

	//----------------------------------------------------------------------------
	//	Replace the emitter in the generator with the passed in type.  if the 
	//	types are the same, this function will do nothing.
	//----------------------------------------------------------------------------
	void ReplaceGeneratorEmitter( prtParticleGenerator* io_pGenerator,
								  prtParticleGeneratorTemplate::EmitterType i_EmitterType );

	//----------------------------------------------------------------------------
	//	resize the parameters for the template given the current type
	//----------------------------------------------------------------------------
	void ResizeTemplateParameters( prtParticleGeneratorTemplate& io_Template );

	//----------------------------------------------------------------------------
	//	set the template to default values based on the type
	//	this will also resize the template parameters
	//----------------------------------------------------------------------------
	void SetTemplateToDefault( prtParticleGeneratorTemplate& io_Template, bool i_bReplaceGenericParamsToo = true );

	//----------------------------------------------------------------------------
	//	SetAllowParticles() - allow generation of particles to happen or not
	//----------------------------------------------------------------------------
	void SetAllowParticles( bool i_bAllowParticles );
}
