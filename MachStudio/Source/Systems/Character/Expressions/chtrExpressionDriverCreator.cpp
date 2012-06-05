/*****************************************************************************
**	chtrExpressionDriverCreator.cpp
**
**		Handles parsing of animation drivers on expression channels
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Expressions/chtrExpressionDriverCreator.hpp"

#include "Drivers/Float/tmlnDriverFloat.hpp"
#include "Drivers/Float/tmlnDriverFloatInfo.hpp"
#include "Drivers/Float/tmlnDriverFloatParser.hpp"
#include "Support/tmln/tmlnChannelFloat.hpp"
#include "Systems/Character/Expressions/chtrExpressionObject.hpp"
#include "Systems/Character/Object/chtrScriptObject.hpp"


//============================================================================
//============================================================================
namespace
{
	const chDefs::Name c_EXPF = chDefs::MakeName('E', 'X', 'P', 'F');  // EXPression Float driver
}


//--------------------------------------------------------------------
// Create parsers for driver types created by this creator
//--------------------------------------------------------------------
//static
void chtrExpressionDriverCreator::CreateParsers()
{
	tmlnParser::AddDriverParser(c_EXPF, new tmlnDriverFloatParser(c_EXPF));
}

//--------------------------------------------------------------------
// Gather the possible types of drivers that can be created for
//	for this object.  Driver names are added through the
//	DriverNameList's API
//--------------------------------------------------------------------
void chtrExpressionDriverCreator::GatherPossibleDrivers(const tmlnScriptObject* i_pObject,
														tmlnDriverNameList& io_Drivers)
{
	//	if there are already expressions, add them.
	//
	const chtrScriptObject* pObject = dynamic_cast<const chtrScriptObject*>(i_pObject);
	if (pObject && pObject->GetExpressionObject())
	{
		const chtrExpressionObject *exp_obj = pObject->GetExpressionObject();
		const int num_exps = exp_obj->GetNumExpressions();
		for (int i=0; i<num_exps; i++)
		{
			std::string ctrl("Expression ");
			ctrl += exp_obj->GetExpressionName(i);
			io_Drivers.AddDriver( ctrl.c_str(), "Expression Animation", this);
		}
	}
}

//--------------------------------------------------------------------
// Create driver for this object based on the name used in
//	GatherPossibleDrivers and attach it to the channels
//	on this script object.
//--------------------------------------------------------------------
tmlnDriver* chtrExpressionDriverCreator::CreateDriverByName(	const char* i_DriverName,
																tmlnScriptObject* i_pObject )
{
	chtrScriptObject* chtr_obj = dynamic_cast<chtrScriptObject*>(i_pObject);
	if (!chtr_obj) return NULL;

	if (!::_strnicmp(i_DriverName, "Expression ", ::strlen("Expression ")))
	{
		std::string channel_name = i_DriverName + ::strlen("Expression ");
		tmlnChannelFloat *channel = chtr_obj->GetExpressionChannel(channel_name);
		//DBG_ASSERT(channel, "Couldn't find channel for driver");
		if (!channel) return NULL;

		tmlnDriverFloat * pDS = new tmlnDriverFloat( *channel, c_EXPF );
		pDS->SetName(channel_name.c_str());
		pDS->SetInitialTime( 0.0f ); // short key

		// Attach driver to the appropriate channels
		channel->AddDriver( pDS );
		return pDS;
	}

	return 0;
}

//--------------------------------------------------------------------
// Create static key driver for the given channel
//--------------------------------------------------------------------
tmlnDriver* chtrExpressionDriverCreator::CreateKeyForChannel(	tmlnScriptObject* io_pObject,
																tmlnChannel* i_pChannel)
{
	chtrScriptObject* chtr_obj = dynamic_cast<chtrScriptObject*>(io_pObject);
	if (!chtr_obj) return NULL;

	tmlnChannelFloat* pExpChannel = dynamic_cast<tmlnChannelFloat*>(i_pChannel);
	if (pExpChannel)
	{
		std::string exp_name;
		if (chtr_obj->GetNameFromExpressionChannel(pExpChannel, exp_name))
		{
			tmlnDriverFloat * pDriver = new tmlnDriverFloat( *pExpChannel, c_EXPF );
			pDriver->SetName(exp_name.c_str());
			pDriver->SetInitialTime( 0.0f ); // short key
			i_pChannel->AddDriver( pDriver );
			return pDriver;
		}
	}

	return NULL;
}

//--------------------------------------------------------------------
// Create driver for this object based on the the info structure
//--------------------------------------------------------------------
tmlnDriver* chtrExpressionDriverCreator::CreateDriverFromInfo(	tmlnScriptObject* io_pObject,
																 const tmlnDriverInfo& i_Info)
{
	chtrScriptObject* chtr_obj = dynamic_cast<chtrScriptObject*>(io_pObject);
	if (!chtr_obj) return NULL;

	chDefs::Name name = i_Info.GetBaseChunkName();

	if (name == c_EXPF)
	{
		const tmlnDriverFloatInfo& driver_info = dynamic_cast<const tmlnDriverFloatInfo&>(i_Info);
		tmlnChannelFloat *channel = chtr_obj->GetExpressionChannel(i_Info.m_Name);
		//DBG_ASSERT(channel, "Couldn't find channel for driver");
		if (!channel) return NULL;

		tmlnDriverFloat * pDS = new tmlnDriverFloat( *channel, c_EXPF );
		pDS->SetDriverInfo(driver_info);

		// Attach driver to the appropriate channels
		channel->AddDriver( pDS );
		return pDS;
	}

	return NULL;
}

