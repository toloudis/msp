/*****************************************************************************
**	prtclAdapterEmit.hpp
**
**	 Adapter for setting prtcl animation
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef PRTCL_ADAPTEREMIT_HPP
#error prtclAdapterEmit.hpp multiply included
#endif
#define PRTCL_ADAPTEREMIT_HPP

#ifndef PRT_PARTICLEGENERATOR_HPP
#include "Graphics/prt/prtParticleGenerator.hpp"
#endif

#include <map>


class prtclAdapterEmit
{
public:
	typedef std::map<int,std::string> AnimMap;  // anim index + anim name

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	prtclAdapterEmit( prtParticleGenerator* i_pObject );

	//--------------------------------------------------------------------
	//  Start
	//--------------------------------------------------------------------
	void  Start(float i_SimulationTime);

	//--------------------------------------------------------------------
	//  Stop
	//--------------------------------------------------------------------
	void  Stop();

//private:
	prtParticleGenerator* m_pParticle;
};
