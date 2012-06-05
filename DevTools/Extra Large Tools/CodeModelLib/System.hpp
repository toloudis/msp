//****************************************************************************
///  \file System.hpp
///
///	CodeModelLib system namespace
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
#ifdef SYSTEM_HPP
#error System.hpp multiply included
#endif
#define SYSTEM_HPP


//============================================================================
//============================================================================
using namespace System;
using namespace EnvDTE;


//============================================================================
/// CodeModelLib
//============================================================================
namespace CodeModelLib
{
	//------------------------------------------------------------------------
	/// FileCode
	//------------------------------------------------------------------------
	public __gc class System
	{
		public:
			__value enum eFileType
			{
				Header = 0,	// .hpp or .h
				Source = 1,	// .cpp
			};

			//----------------------------------------------------------------
			///	set up the variables
			//----------------------------------------------------------------
			static void Init(Object* i_pApp);

			//----------------------------------------------------------------
			/// GetStartPoint
			//----------------------------------------------------------------
			static TextPoint* GetStartPoint(CodeElement* i_pCE, vsCMPart i_Part, eFileType i_FromFile);

			//----------------------------------------------------------------
			//----------------------------------------------------------------
			static _DTE* GetApplication()
			{
				return m_pApplication;
			};

		private:
			static _DTE* m_pApplication;
	};
}
