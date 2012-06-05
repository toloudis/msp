/*****************************************************************************
**  envSTLHelpersTest.cpp
**
**      envSTLHelpersTest is a test of the envSTLHelpers
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "envSTLHelpersTest.hpp"

#include "Core/env/envSTLHelpers.hpp"

#include <algorithm>
#include <functional>
#include <vector>

//====================================================================
// Anonymous Namespace for local variables and functions
//====================================================================
namespace 
{
	
	class SomeClass
	{
		public:

			SomeClass()
				: m_nResult( 0 ) {}

			void Increment()
			{
				++m_nResult ;
			}

			void Set( int i_nValue )
			{
				m_nResult = i_nValue;
			}

		private :

			int m_nResult;
	};

	//========================================================================
	//	test_mem_fun
	//========================================================================
	void test_mem_fun()
	{
		// Test MemFun
		std::vector<SomeClass*> some_class_p_vector;

		for( int i = 0; i < 10; ++i )
		{
			SomeClass* pSomeClass = new SomeClass();
			some_class_p_vector.push_back( pSomeClass );
		}

		envSTLHelpers::ForAll( some_class_p_vector, envSTLHelpers::MemFun( &SomeClass::Increment ) );
		envSTLHelpers::MemFun( &SomeClass::Set )( some_class_p_vector[0], 5 );

		envSTLHelpers::DeleteContainer( some_class_p_vector );

		// Test MemFunRef
		std::vector<SomeClass> some_class_r_vector;
		some_class_r_vector.resize( 10 );
		envSTLHelpers::ForAll( some_class_r_vector, envSTLHelpers::MemFunRef( &SomeClass::Increment ) );
		envSTLHelpers::MemFunRef( &SomeClass::Set )( some_class_r_vector[0], 5 );
	}
}

//========================================================================
//	RunTest - executes the envSTLHelpers tests
//========================================================================
void envSTLHelpersTest::RunTest()
{
	test_mem_fun();
}