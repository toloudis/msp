/*****************************************************************************
**	prtclChannelEmit.hpp
**
**	 Channel to start/stop emitting of particles
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRTCL_CHANNELEMIT_HPP
#error prtclChannelEmit.hpp multiply included
#endif
#define PRTCL_CHANNELEMIT_HPP

#ifndef TMLN_CHANNEL_HPP
#include "Support/tmln/tmlnChannel.hpp"
#endif


//============================================================================
//============================================================================
class prtParticleGenerator;


//============================================================================
//============================================================================
class prtclChannelEmit : public tmlnChannel
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	prtclChannelEmit(const char* i_Name, prtParticleGenerator* i_pParticle);

	//--------------------------------------------------------------------
	//	activate or deactivate
	//--------------------------------------------------------------------
	void Activate( float i_fBeginTime, float i_fDuration );
	void Deactivate();

private:
	prtParticleGenerator* m_pPrtclGen;
};
