/****************************************************************************\
**	mainInitApplication.hpp
**
**		Object that initializes systems for our application on
**	constructor and then closes them down on destructor.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MAIN_INITAPPLICATION_HPP
#error mainInitApplication.hpp multiply included
#endif
#define MAIN_INITAPPLICATION_HPP


//============================================================================
//	Forward References
//============================================================================
class g2dSystem;


//============================================================================
//============================================================================
class mainInitApplication
{
public:
	//--------------------------------------------------------------------
	// Initializes library packages
	//--------------------------------------------------------------------
	mainInitApplication(g2dSystem* i_pSystem);

	//--------------------------------------------------------------------
	// DeInitializes library packages
	//--------------------------------------------------------------------
	~mainInitApplication();
};
