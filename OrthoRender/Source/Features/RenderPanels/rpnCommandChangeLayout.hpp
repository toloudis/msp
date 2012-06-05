/*****************************************************************************
**  rpnCommandChangeLayout.hpp
**
**      command to change the layout style of the render panels
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_COMMANDCHANGELAYOUT_HPP
#error rpnCommandChangeLayout.hpp multiply included
#endif
#define RPN_COMMANDCHANGELAYOUT_HPP

#ifndef CMA_COMMAND_HPP
#include "Tool/cma/cmaCommand.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class rpnCommandChangeLayout : public cmaCommand
{
	public:
		enum LayoutStyle
		{
			e_SinglePane = 0,
			e_TwoStacked,
			e_FourPanels,
			e_TwoSideBySide
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static const std::string GetConstTagName();

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		rpnCommandChangeLayout(LayoutStyle  i_Style,
							   const std::string& i_MenuDesc);

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~rpnCommandChangeLayout();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const std::string& GetMenuDesc()
		{
			return m_MenuDesc;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		LayoutStyle GetLayoutStyle()
		{
			return m_Style;
		}

		//--------------------------------------------------------------------
		// Static function so that other parts of code can execute this 
		//	command also.
		//--------------------------------------------------------------------
		static void ChangeLayout(LayoutStyle  i_Style);

	private:
		std::string	m_MenuDesc;
		LayoutStyle m_Style;
};
