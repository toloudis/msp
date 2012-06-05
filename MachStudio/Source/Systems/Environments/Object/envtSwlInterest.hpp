/****************************************************************************\
**	envtSwlInterest.hpp
**
**		A Software Lighting Interest is registered by a system that has data
**	to be exported to Mray/Rman environmental light setting.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef ENVT_SWLNTEREST_HPP
#error envtSwlInterest.hpp multiply included
#endif
#define ENVT_SWLINTEREST_HPP

#ifndef SWL_INTEREST_HPP
#include "Support/swl/swlInterest.hpp"
#endif

class evmtEnvironment;
class envtData;
//============================================================================
//============================================================================
class envtSwlInterest : public swlInterest
{
	public:
		//--------------------------------------------------------------------
		// Constructor
		//--------------------------------------------------------------------
		envtSwlInterest(evmtEnvironment* i_EnvObj, envtData& i_EnvtData);

		virtual ~envtSwlInterest();

		//--------------------------------------------------------------------
		// Gather data for objects that will be exported
		//--------------------------------------------------------------------
		virtual void GatherData(const swlData &i_Data ) const;

	evmtEnvironment* m_EnvObj;
	envtData& m_EnvData;
};
