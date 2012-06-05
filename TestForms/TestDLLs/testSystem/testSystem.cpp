/*****************************************************************************
**  testSystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "StdAfx.h"
#include "testSystem.hpp"
#include "SpecialForm.h"

#include "aclass.hpp"

//#include <iostream>


namespace testSystem
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void testSystem::Init()
{
	//cout << "testSystem init";

	int x;
	x = 5;

	FormInDLL::SpecialForm* pForm = new FormInDLL::SpecialForm();
	pForm->Show();

	//	test the static lib
	theclass* xkd = new theclass;

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void testSystem::CleanUp()
{
	//cout << "testSystem clean-up";

	int x;
	x = 5;
}


