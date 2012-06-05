/*****************************************************************************
**  chnlCommandCreateDriver.hpp
**
**      command to create (and attach) a driver for the current selected 
**	object.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_COMMANDCREATEDRIVER_HPP
#error chnlCommandCreateDriver.hpp multiply included
#endif
#define CHNL_COMMANDCREATEDRIVER_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class chnlCommandCreateDriver : public cmaCommand
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static const std::string GetConstTagName();

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		chnlCommandCreateDriver();

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~chnlCommandCreateDriver();

		//--------------------------------------------------------------------
		//	driver name for creating the driver
		//--------------------------------------------------------------------
		void SetDriverName( std::string i_DriverName );

		//--------------------------------------------------------------------
		//	driver name for creating the driver
		//--------------------------------------------------------------------
		std::string& GetDriverName();

	private:
		std::string m_DriverName;
};
