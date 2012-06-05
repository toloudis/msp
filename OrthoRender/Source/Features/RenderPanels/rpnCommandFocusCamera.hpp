/*****************************************************************************
**  rpnCommandFocusCamera.hpp
**
**      command to Focus Camera
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_COMMANDFOCUSCAMERA_HPP
#error rpnCommandFocusCamera.hpp multiply included
#endif
#define RPN_COMMANDFOCUSCAMERA_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class rpnCommandFocusCamera : public cmaCommand
{
	public:
		

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static const std::string GetConstTagName();

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		rpnCommandFocusCamera(const std::string& i_MenuDesc);
		
		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~rpnCommandFocusCamera();

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
		static void FocusCamera();


	private:
		std::string	m_MenuDesc;
};
