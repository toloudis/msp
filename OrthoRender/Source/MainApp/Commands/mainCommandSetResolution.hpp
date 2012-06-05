/*****************************************************************************
**  mainCommandSetResolution.hpp
**
**      command to Set the resolution
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MAIN_COMMANDSETRESOLUTION_HPP
#error mainCommandSetResolution.hpp multiply included
#endif
#define MAIN_COMMANDSETRESOLUTION_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class mainCommandSetResolution : public cmaCommand
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static const std::string GetConstTagName();

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		mainCommandSetResolution(int i_Width, int i_Height);

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~mainCommandSetResolution() {};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int GetWidth();
		int GetHeight();

	private:
		int m_Width;
		int m_Height;
};
