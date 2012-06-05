/****************************************************************************\
**	mainInitSecurity.hpp
**
**		Object that initializes security for the application
**	constructor and then closes them down on destructor.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef MAIN_INITSECURITY_HPP
#error mainInitSecurity.hpp multiply included
#endif
#define MAIN_INITSECURITY_HPP


//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class mainInitSecurity
{
public:
	//--------------------------------------------------------------------
	// Initializes library packages
	//--------------------------------------------------------------------
	mainInitSecurity();

	//--------------------------------------------------------------------
	//	Check if there is a valid license (once during pre-app run)
	//--------------------------------------------------------------------
	bool ValidateLicense();

	//--------------------------------------------------------------------
	// DeInitializes library packages
	//--------------------------------------------------------------------
	~mainInitSecurity();
};
