/*****************************************************************************
**  SupportLayer.hpp
**
**      SupportLayer contains the initialization functions
**	for the all packages within the MachStudio Support Layer.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef SUPPORT_LAYER_HPP
#error SupportLayer.hpp multiply included
#endif
#define SUPPORT_LAYER_HPP

class g2dSystem;

class SupportLayer
{
	public:

		//------------------------------------------------------------------------
		//	Init
		//------------------------------------------------------------------------
		static void Init(g2dSystem* i_pSystem);

		//------------------------------------------------------------------------
		//	CleanUp
		//------------------------------------------------------------------------
		static void CleanUp() throw();
};
