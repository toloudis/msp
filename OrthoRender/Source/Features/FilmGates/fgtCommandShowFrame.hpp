/*****************************************************************************
**  fgtCommandShowFrame.hpp
**
**      command to show/hide a frame gate
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef FGT_COMMANDSHOWFRAME_HPP
#error fgtCommandShowFrame.hpp multiply included
#endif
#define FGT_COMMANDSHOWFRAME_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class fgtCommandShowFrame : public cmaCommand
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static const std::string GetConstTagName();

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		fgtCommandShowFrame(const std::string& i_MenuDesc);

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~fgtCommandShowFrame();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const std::string& GetMenuDesc()
		{
			return m_MenuDesc;
		}

	private:
		std::string	m_MenuDesc;
};
