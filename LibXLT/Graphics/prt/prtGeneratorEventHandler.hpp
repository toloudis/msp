/*****************************************************************************
**	prtGeneratorEventHandler.hpp
**
**		prtGeneratorEventHandler is a base class for things which need to
**	know when a prtParticleGenerator is destroyed.  To use it, you must
**	call scScene::RegisterGeneratorEventHandler.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PRT_GENERATOREVENTHANDLER_HPP
#error prtGeneratorEventHandler.hpp multiply included
#endif
#define PRT_GENERATOREVENTHANDLER_HPP


//============================================================================
//============================================================================
class prtParticleGenerator;


//============================================================================
//============================================================================
class prtGeneratorEventHandler
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtGeneratorEventHandler();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtGeneratorEventHandler();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void GeneratorDestroyed(prtParticleGenerator* i_Generator) = 0;
};
