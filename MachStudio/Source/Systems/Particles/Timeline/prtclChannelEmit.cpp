/*****************************************************************************
**	prtclChannelEmit.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Timeline/prtclChannelEmit.hpp"


#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/prt/prtParticleGenerator.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtclChannelEmit::prtclChannelEmit(const char* i_Name, prtParticleGenerator* i_pParticle)
:	tmlnChannel(i_Name), 
	m_pPrtclGen(i_pParticle)
{
}

//--------------------------------------------------------------------
//	activate
//--------------------------------------------------------------------
void prtclChannelEmit::Activate( float i_fBeginTime, float i_fDuration )
{
	//	set the parameters of the particle generator
	//
	m_pPrtclGen->SetCreationTime( i_fBeginTime );
	m_pPrtclGen->SetGeneratorLifetime( i_fDuration );
	m_pPrtclGen->UnPause();
	if (m_pPrtclGen->IsExpired())
	{
		m_pPrtclGen->Reset();
	}

	//	Upon activation, do a pre-sim on the generator
	m_pPrtclGen->PreSim( i_fBeginTime, i_fDuration );

	//DBG_LOG2( "emit! %6.2f (%6.2f)", i_fBeginTime, i_fDuration );
}

//--------------------------------------------------------------------
//	deactivate
//--------------------------------------------------------------------
void prtclChannelEmit::Deactivate()
{
	m_pPrtclGen->Pause();

	//DBG_LOG( "STOP emit!" );
}
