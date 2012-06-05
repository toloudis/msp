/*****************************************************************************
**  rpnCommandFocusAllPanels.hpp
**
**      command to Focus Camera in all open panels
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_COMMANDFOCUSALLPANELS_HPP
#error rpnCommandFocusAllPanels.hpp multiply included
#endif
#define RPN_COMMANDFOCUSALLPANELS_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class rpnCommandFocusAllPanels : public cmaCommand
{
	public:
		

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static const std::string GetConstTagName();

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		rpnCommandFocusAllPanels(const std::string& i_MenuDesc);
		
		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~rpnCommandFocusAllPanels();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const std::string& GetMenuDesc()
		{
			return m_MenuDesc;
		}

		//--------------------------------------------------------------------
		// Static function so that other parts of code can execute this 
		//	command also.
		//--------------------------------------------------------------------
		static void FocusAllPanels();

	private:
		std::string	m_MenuDesc;
};
