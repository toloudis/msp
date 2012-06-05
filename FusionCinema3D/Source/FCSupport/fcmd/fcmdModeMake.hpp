/*****************************************************************************
**	fcmdModeMake.hpp
**
**		Mode class for the the Make mode
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCMD_MODEMAKE_HPP
#error fcmdModeMake.hpp multiply included
#endif
#define FCMD_MODEMAKE_HPP

#ifndef FCMD_MODETEMPLATE_HPP
#include "FCSupport/fcmd/fcmdModeTemplate.hpp"
#endif

#ifndef MA_TIME_HPP
#include "Core/ma/maTime.hpp"
#endif

//============================================================================
//============================================================================
class fcmdModeMake : public fcmdModeTemplate
{ 
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fcmdModeMake();
	fcmdModeMake(const fsLocator& i_Directory);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~fcmdModeMake();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Activate();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DeActivate();

	///-----------------------------------------------------------------------
	/// Do any mode thinking
	///-----------------------------------------------------------------------
	virtual void Think();

private:
	bool m_bRenderStarted;
	bool m_bStopRender;
	bool m_bContinueRender;
	float m_DirectorMaxTime;
	int m_value;
};