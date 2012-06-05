//****************************************************************************
///  \file System.hpp
///
///	see .hpp
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
#include "System.hpp"


//============================================================================
//============================================================================
using namespace Microsoft::VisualStudio::VCCodeModel;


//============================================================================
//============================================================================
namespace CodeModelLib
{
//----------------------------------------------------------------
///	set up the variables
//----------------------------------------------------------------
//static 
void System::Init(Object* i_pApp)
{
	m_pApplication = dynamic_cast<_DTE*>(i_pApp);
}

//----------------------------------------------------------------
/// GetStartPoint
//----------------------------------------------------------------
//static 
TextPoint* System::GetStartPoint(CodeElement* i_pCE, vsCMPart i_Part, eFileType i_FromFile)
{
	if (i_pCE != 0)
	{
		VCCodeElement* pvcCE = dynamic_cast<VCCodeElement*>(i_pCE);

		if (i_FromFile == Header)
		{
			TextPoint* pTP = pvcCE->get_StartPointOf( i_Part, vsCMWhere::vsCMWhereDeclaration );
			//DevEnvLib::DebugOutput::Message( String::Format(S"StartPoint hpp {0}", __box(pTP->Line)) );
			return pTP;
		}
		else
		{
			TextPoint* pTP = pvcCE->get_StartPointOf( i_Part, vsCMWhere::vsCMWhereDefinition );
			//DevEnvLib::DebugOutput::Message( String::Format(S"StartPoint cpp {0}", __box(pTP->Line)) );
			return pTP;
		}
	}

	return 0;
}


}
