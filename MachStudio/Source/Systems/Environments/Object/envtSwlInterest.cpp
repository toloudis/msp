/****************************************************************************\
**	envtSwlInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Object/envtSwlInterest.hpp"

#include "Support/evmt/evmtEnvironment.hpp"
#include "Systems/Environments/Data/envtData.hpp"
//#include "Support/swl/swlData.hpp"


//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
envtSwlInterest::envtSwlInterest(evmtEnvironment* i_EnvObj, envtData& i_EnvtData)
		: m_EnvObj(i_EnvObj), m_EnvData(i_EnvtData)
{
}

envtSwlInterest::~envtSwlInterest()
{
	m_EnvObj = NULL;
}

//--------------------------------------------------------------------
// Gather data for objects that will be exported
//--------------------------------------------------------------------
//virtual 
void envtSwlInterest::GatherData(const swlData &i_Data ) const
{
	m_EnvData.m_SwlData = i_Data;
	//m_EnvObj->UpdateSwlData(i_Data);
}