#include "FCMHierarchy.hpp"
#include "System.hpp"


//============================================================================
//============================================================================
using namespace Microsoft::VisualStudio::VCCodeModel;
using namespace System::IO;


//============================================================================
//============================================================================
namespace CodeModelLib
{
////----------------------------------------------------------------
///// 
////---------------------------------------------------------------
////static 
//void FCMHierarchy::Test(Object* i_pApp)
//{
//	BuildTreeForActiveSolution(i_pApp);
//}

//--------------------------------------------------------------------
///	set up the variables
//--------------------------------------------------------------------
//static 
void FCMHierarchy::Init()
{
	m_bInitCalled = true;
	m_pRootNode = 0;
}

////--------------------------------------------------------------------
/////
////--------------------------------------------------------------------
////static 
//bool FCMHierarchy::IsStringCommentLine(String* i_pString)
//{
//	if (   (i_pString->StartsWith("==="))
//		|| (i_pString->StartsWith("---")))
//	{
//		return true;
//	}
//	return false;
//}
//
////--------------------------------------------------------------------
/////
////--------------------------------------------------------------------
////static
//void FCMHierarchy::BuildTreeForFunction(CodeElement* i_pCodeElement)
//{
//	CodeFunction* pCFunc = dynamic_cast<CodeFunction*>(i_pCodeElement);
//
//	TextPoint*	htp;
//	TextPoint*	stp;
//	try
//	{
//		htp = GetStartPoint(i_pCodeElement, vsCMPart::vsCMPartHeader, CodeModelLib::FCMHierarchy::Header);
//	}
//	catch (System::Exception* ex)
//	{
//		DevEnvLib::DebugOutput::Message( String::Format(S"  exception header {0}", ex->ToString() ));
//	}
//	try
//	{
//		stp = GetStartPoint(i_pCodeElement, vsCMPart::vsCMPartHeader, CodeModelLib::FCMHierarchy::Source);
//	}
//	catch (System::Exception* ex)
//	{
//		DevEnvLib::DebugOutput::Message( String::Format(S"  exception header {0}", ex->ToString() ));
//	}
//
//	DevEnvLib::DebugOutput::Message( String::Format(S"  line hpp{0} cpp{1}", __box(htp->Line), __box(stp->Line)) );
//
////	int linenum = i_CodeElement::GetStartPoint(vsCMPart::vsCMPartHeader)::Line;
////
////	string outstring;
////	outstring = String::Format("function {0} -  line#{1}\n", cfunc::Name, linenum);
////	m_OWP::OutputString( outstring );
////
////	string origcomments;
////	string newcomments;
////	origcomments = cfunc::Comment;
////	newcomments  = "//-----------------------------------------------------------------------------\n";
////
////	try
////	{
////		cfunc::DTE::UndoContext::Open("Insert Function Comments",false);
////
////		// grab the old comment block
////		//
////		string substring;
////		int lastfound = 1;
////		int found;
////		while ((found = origcomments::IndexOf("\r",lastfound)) != -1)
////		{
////			found++;
////			substring = origcomments::Substring(lastfound, (found-lastfound));
////			if (!IsStringCommentLine(substring))
////			{
////				newcomments += "//";	// the Comment command removes two slashes
////				newcomments += substring;
////			}
////			lastfound = found; // + 1;
////		}
////
////		substring = origcomments::Substring(lastfound, ((origcomments::Length-lastfound)));
////		if (!IsStringCommentLine(substring))
////		{
////			newcomments += "//";	// the Comment command removes two slashes
////			newcomments += substring;
////		}
////
////		//	parameters
////		//
////		//ToDo [rjk] need to PRESERVE the old parameter comments!
////		CodeElements cparams = cfunc::Parameters;
////		if ( cparams::Count > 0 )
////		{
////			foreach (CodeElement cparam in cparams)
////			{
////				newcomments += "/// \\param " + cparam::Name + " \n";
////			}
////		}
////
////		//	return value
////		//
////		//ToDo [rjk] need to PRESERVE the old parameter comments!
////		if (cfunc::Type::ToString() != "void")
////		{
////			newcomments += "/// \\return \n";
////		}
////
////		newcomments += "//-----------------------------------------------------------------------------\n";
////
////		TextSelection ts = (TextSelection) i_CodeElement::DTE::ActiveWindow::Selection;
////		EditPoint ep = ts::ActivePoint::CreateEditPoint();
////		ep::MoveToLineAndOffset(linenum,1);
////
////		//cfunc::Comment = string::Empty;
////
////		// Add the lines
////		ep::Insert(newcomments);
////	}
////	finally
////	{
////		cfunc::DTE::UndoContext::Close();
////	}
//}
//
//
////--------------------------------------------------------------------
////--------------------------------------------------------------------
////static
//String* FCMHierarchy::GetCommentForClass( CodeClass* i_pCClass )
//{
//	return i_pCClass->Comment;
//}
//
////--------------------------------------------------------------------
////--------------------------------------------------------------------
////static
//void FCMHierarchy::SetCommentForClass( CodeClass* i_pCClass, String* i_pNewComments )
//{
//	CodeModelLib::System::GetApplication()->get_UndoContext()->Open("Insert Class Comments",false);
//
//	int linenum = i_pCClass->GetStartPoint(vsCMPart::vsCMPartHeader)->Line;
//
//	TextSelection* ts = dynamic_cast<TextSelection*>(CodeModelLib::System::GetApplication()->ActiveWindow->Selection);
//	EditPoint* ep = ts->ActivePoint->CreateEditPoint();
//	ep->MoveToLineAndOffset(linenum,1);
//	i_pCClass->Comment = String::Empty;
//
//	ep->LineUp(1);
//	ep->Delete(__box(ep->LineLength+1));
//
//	// Add the lines
//	ep->Insert(i_pNewComments);
//
//	CodeModelLib::System::GetApplication()->get_UndoContext()->Close();
//}
//
////--------------------------------------------------------------------
/////
////--------------------------------------------------------------------
////static
//void FCMHierarchy::BuildTreeForClass(CodeElement* i_pCodeElement)
//{
//	CodeClass* pCClass = dynamic_cast<CodeClass*>(i_pCodeElement);
//
//	String* origcomments;
//	String* newcomments;
//
//	origcomments = GetCommentForClass( pCClass );
//
//	newcomments  = "//=============================================================================\n";
//
//	if (origcomments->Length > 0)
//	{
//		try
//		{
//			// grab the old comment block
//			//
//			int lastfound = 1;
//			int found;
//			while ((found = origcomments->IndexOf("\r",lastfound)) != -1)
//			{
//				found++;
//				if (!IsStringCommentLine(origcomments->Substring(lastfound, (found-lastfound))))
//				{
//					newcomments = String::Concat(newcomments,S"//");	// the Comment command removes two slashes
//					newcomments = String::Concat(newcomments,origcomments->Substring(lastfound, (found-lastfound)));
//				}
//				lastfound = found; // + 1;
//			}
//
//			if (!IsStringCommentLine(origcomments->Substring(lastfound, ((origcomments->Length-lastfound)))))
//			{
//				newcomments = String::Concat(newcomments,S"//");	// the Comment command removes two slashes
//				newcomments = String::Concat(newcomments,origcomments->Substring(lastfound, ((origcomments->Length-lastfound))));
//			}
//		}
//		catch (System::Exception* /*e*/)
//		{
//		}
//	}
//
//	newcomments = String::Concat(newcomments,S"//=============================================================================\n");
//
//// TODO:	SetCommentForClass( pCClass, newcomments );
//}
//
////--------------------------------------------------------------------
////--------------------------------------------------------------------
////static
//String* FCMHierarchy::GetCommentForNamespace( CodeNamespace* i_pCNamespace )
//{
//	return i_pCNamespace->Comment;
//}
//
////--------------------------------------------------------------------
////--------------------------------------------------------------------
////static
//void FCMHierarchy::SetCommentForNamespace( CodeNamespace* i_pCNamespace, String* i_pNewComments )
//{
//	CodeModelLib::System::GetApplication()->get_UndoContext()->Open("Insert Namespace Comments",false);
//
//	int linenum = i_pCNamespace->GetStartPoint(vsCMPart::vsCMPartHeader)->Line;
//
//	TextSelection* ts = dynamic_cast<TextSelection*>(CodeModelLib::System::GetApplication()->ActiveWindow->Selection);
//	EditPoint* ep = ts->ActivePoint->CreateEditPoint();
//	ep->MoveToLineAndOffset(linenum,1);
//	i_pCNamespace->Comment = String::Empty;
//
//	ep->LineUp(1);
//	ep->Delete(__box(ep->LineLength+1));
//
//	// Add the lines
//	ep->Insert(i_pNewComments);
//
//	CodeModelLib::System::GetApplication()->get_UndoContext()->Close();
//}
//
////--------------------------------------------------------------------
/////
////--------------------------------------------------------------------
////static
//void FCMHierarchy::BuildTreeForNamespace(CodeElement* i_pCodeElement)
//{
//	CodeNamespace* pCNamespace = dynamic_cast<CodeNamespace*>(i_pCodeElement);
//
//	String* origcomments;
//	String* newcomments;
//
//	origcomments = GetCommentForNamespace( pCNamespace );
//
//	newcomments  = "//=============================================================================\n";
//
//	if (origcomments->Length > 0)
//	{
//		try
//		{
//			// grab the old comment block
//			//
//			int lastfound = 1;
//			int found;
//			while ((found = origcomments->IndexOf("\r",lastfound)) != -1)
//			{
//				found++;
//				if (!IsStringCommentLine(origcomments->Substring(lastfound, (found-lastfound))))
//				{
//					newcomments = String::Concat(newcomments,S"//");	// the Comment command removes two slashes
//					newcomments = String::Concat(newcomments,origcomments->Substring(lastfound, (found-lastfound)));
//				}
//				lastfound = found; // + 1;
//			}
//
//			if (!IsStringCommentLine(origcomments->Substring(lastfound, ((origcomments->Length-lastfound)))))
//			{
//				newcomments = String::Concat(newcomments,S"//");	// the Comment command removes two slashes
//				newcomments = String::Concat(newcomments,origcomments->Substring(lastfound, ((origcomments->Length-lastfound))));
//			}
//		}
//		catch (System::Exception* /*e*/)
//		{
//		}
//	}
//
//	newcomments = String::Concat(newcomments,S"//=============================================================================\n");
//
//// TODO:	SetCommentForNamespace( pCNamespace, newcomments );
//}

//--------------------------------------------------------------------
/// \return the code element node and the rest of the code hierarchy
//--------------------------------------------------------------------
//static 
void FCMHierarchy::BuildTreeForCodeElements(CodeElements* i_pCEs, CMNode& io_ParentNode)
{
	if (!m_bInitCalled)
	{
		//ToDo [rjk] assert here
		return;
	}

	//
	CodeElement*	pCElmt;
	for ( int i = 1; i <= i_pCEs->Count ; ++i )
	{
		pCElmt = i_pCEs->Item(__box(i));
		
		//	set-up the node for the project item
		//
		CMNode* pNode = new CMNode();
		pNode->Tag = pCElmt;
		pNode->Text = pCElmt->FullName;
		pNode->SetKind(CMNode::CMNKindValue::CodeElement);

		SetStartPoints( pCElmt, *pNode );

		ScrapeComments( *pNode );

		io_ParentNode.Nodes->Add( pNode );

		//
		String* kindstring = __box(pCElmt->Kind)->ToString();
		DevEnvLib::DebugOutput::Message( String::Format(S"      {0} {1}", kindstring, pCElmt->Name) );

		if (pCElmt->Kind == vsCMElement::vsCMElementClass)
		{
			//BuildTreeForClass( pCElmt );

			CodeClass* pCClass = dynamic_cast<CodeClass*>(pCElmt);
			BuildTreeForCodeElements( pCClass->Members, *pNode );
		}
		else if (pCElmt->Kind == vsCMElement::vsCMElementFunction)
		{
			//BuildTreeForFunction( pCElmt );
		}
		else if (pCElmt->Kind == vsCMElement::vsCMElementNamespace)
		{
			//BuildTreeForNamespace( pCElmt );

			CodeNamespace* pCNamespace = dynamic_cast<CodeNamespace*>(pCElmt);
			BuildTreeForCodeElements( pCNamespace->Members, *pNode );
		}
		else
		{
			i = i;	// for testing only
		}
	}

	return;
}


//--------------------------------------------------------------------
/// \return the project item node and the rest of the code hierarchy
//--------------------------------------------------------------------
//static 
CMNode* FCMHierarchy::BuildTreeForProjectItem(ProjectItem* i_pProjItem)
{
	if (!m_bInitCalled)
	{
		//ToDo [rjk] assert here
		return 0;
	}

	//	set-up the node for the project item
	//
	CMNode* pNode = new CMNode();
	pNode->Tag = i_pProjItem;
	pNode->Text = i_pProjItem->Name;
	pNode->SetKind(CMNode::CMNKindValue::ProjectItem);
	//SetStartPoints( i_pProjItem, *pNode );

	//Todo [rjk] remove this -- it's for testing only
	String* filename = i_pProjItem->get_FileNames(0);
	if (filename->IndexOf("appEvent.") == -1)
	{
		return pNode;
	}

	// debugging
	bool bPIOpen = (i_pProjItem->get_IsOpen(Constants::vsViewKindAny));
	DevEnvLib::DebugOutput::Message( String::Format(S"\n  {0}[open={1}]", filename, __box(bPIOpen)) );

	//	
	FileCodeModel* pFCM = i_pProjItem->FileCodeModel;
	if ( pFCM )
	{
		// debug
		//DevEnvLib::FCMHelper::DebugRecurseElements( pFCM->CodeElements );

		BuildTreeForCodeElements( pFCM->CodeElements, *pNode );
	}
	else
	{
		DevEnvLib::DebugOutput::Message( String::Format(S"  {0}[NO FCMHierarchyMODEL]\n", filename ) );
	}

	return pNode;
}


//--------------------------------------------------------------------
/// \return the project node and the rest of the code hierarchy
//--------------------------------------------------------------------
//static 
CMNode* FCMHierarchy::BuildTreeForProject(Project* i_pProj)
{
	if (!m_bInitCalled)
	{
		//ToDo [rjk] assert here
		return 0;
	}

	//	set-up the node for the project
	//
	CMNode* pNode = new CMNode();
	pNode->Tag = i_pProj;
	pNode->Text = Path::GetFileName( i_pProj->FullName);
	pNode->SetKind(CMNode::CMNKindValue::Project);
	//SetStartPoints( i_pProj, *pNode );

	//	traverse the project items
	//
	ProjectItems* pis = i_pProj->ProjectItems;
	ProjectItem* pi;

	for (int i = 1; i <= pis->Count; i++)
	{
		pi = dynamic_cast<ProjectItem*>(pis->Item( __box(i) ));

		TreeNode* pChildNode = BuildTreeForProjectItem( pi );
		if ( pChildNode != 0 )
			pNode->Nodes->Add( pChildNode );
	}

	return pNode;
}

//--------------------------------------------------------------------
/// \return the solution node and the rest of the code hierarchy
//--------------------------------------------------------------------
//static 
CMNode* FCMHierarchy::BuildTreeForActiveSolution()
{
	if (!m_bInitCalled)
	{
		//ToDo [rjk] assert here
		return 0;
	}

	//	set-up the node for the solution
	//
	CMNode* pNode = new CMNode();
	pNode->Tag = CodeModelLib::System::GetApplication()->get_Solution();
	pNode->Text = Path::GetFileName( CodeModelLib::System::GetApplication()->get_Solution()->FullName);
	pNode->SetIsChild( false );
	pNode->SetKind(CMNode::CMNKindValue::Solution);
	//SetStartPoints( CodeModelLib::System::GetApplication()->get_Solution(), *pNode );
	m_pRootNode = pNode;

	//	traverse through the projects
	//
	Array* pProjects	= dynamic_cast<Array*>(CodeModelLib::System::GetApplication()->get_ActiveSolutionProjects());
	Project* pProject;
	for (int i = 0; i < pProjects->Count; i++)
	{
		pProject = dynamic_cast<Project*>(pProjects->get_Item(i));

		DevEnvLib::DebugOutput::Message( String::Format(S"Project {0}\n", pProject->Name) );

		TreeNode* pChildNode = BuildTreeForProject( pProject );
		if ( pChildNode != 0 )
			m_pRootNode->Nodes->Add( pChildNode );
	}

	return m_pRootNode;
}

//--------------------------------------------------------------------
/// \param the parent of the hierarchy
//--------------------------------------------------------------------
//static 
void FCMHierarchy::DebugTree( TreeNode* i_pRootNode )
{
	TreeView* pTV = i_pRootNode->get_TreeView();
	if (pTV != 0)
	{
		pTV->BeginUpdate();
	}
	DebugTree( i_pRootNode, 0 );
	if (pTV != 0)
	{
		pTV->EndUpdate();
	}
}

//----------------------------------------------------------------
/// SetStartPoints
///
///	set the start points for this CodeElement and put them in
///	the node.
///
///	\param i_pCE the code element to grab the start points from.
///		cannot be null.
/// \param i_pNode the node to put the start points into.
//----------------------------------------------------------------
//static 
void FCMHierarchy::SetStartPoints(CodeElement* i_pCE, CMNode& i_pNode)
{
	//ToDo [rjk] assert these pointers

	if (i_pCE != 0)
	{
		TextPoint* pTP;
		pTP = System::GetStartPoint( i_pCE, vsCMPart::vsCMPartHeader, CodeModelLib::System::eFileType::Header );
		i_pNode.SetStartPointHPP( *pTP );
		pTP = i_pNode.GetStartPointHPP();
		DevEnvLib::DebugOutput::Message( String::Format(S"StartPoint hpp {0}", __box(pTP->Line)) );

		pTP = System::GetStartPoint( i_pCE, vsCMPart::vsCMPartHeader, CodeModelLib::System::eFileType::Source );
		i_pNode.SetStartPointCPP( *pTP );
		pTP = i_pNode.GetStartPointCPP();
		DevEnvLib::DebugOutput::Message( String::Format(S"StartPoint cpp {0}", __box(pTP->Line)) );
	}
}

//----------------------------------------------------------------
/// ScrapeComments
///
///	Grab the comment block based on the starting line numbers
/// for the hpp and the cpp.
///
///	\param i_pCE the code element to grab the comments from.
///		cannot be null.
//----------------------------------------------------------------
//static 
void FCMHierarchy::ScrapeComments(CMNode& i_pNode)
{
	//ToDo [rjk] assert these pointers

	CodeElement* pCE = dynamic_cast<CodeElement*>(i_pNode.Tag);
	if (pCE != 0)
	{
		//CodeType* pCType = dynamic_cast<CodeType*>(pCE);
		EditPoint* pEP;

		TextPoint* pTPH = i_pNode.GetStartPointHPP();
		TextPoint* pTPC = i_pNode.GetStartPointCPP();
		DevEnvLib::DebugOutput::Message( String::Format(S"LINE - hpp {0} cpp {1}", __box(pTPH->Line), __box(pTPC->Line)));

		pEP = i_pNode.GetStartPointHPP()->CreateEditPoint();
		//pEP->MoveToPoint( i_pNode.GetStartPointHPP() );
		pEP->Insert(S"BLAH");

		pEP = i_pNode.GetStartPointCPP()->CreateEditPoint();
		//pEP->MoveToPoint( i_pNode.GetStartPointCPP() );
		pEP->Insert(S"BLOOP");
	}
}

//--------------------------------------------------------------------
/// \param the parent of the hierarchy
//--------------------------------------------------------------------
//static 
void FCMHierarchy::DebugTree( TreeNode* i_pNode, int level )
{
	String* pad = new String(' ', level*4);
	CMNode* pCMNode = dynamic_cast<CMNode*>(i_pNode);
	DevEnvLib::DebugOutput::Message( String::Format(S"{0}{1}", pad, 
															i_pNode->Text ));

	TextPoint* pTPH = pCMNode->GetStartPointHPP();
	TextPoint* pTPC = pCMNode->GetStartPointCPP();
	if (pTPH != 0)
	{
		if (pTPC != 0)
		{
			DevEnvLib::DebugOutput::Message( String::Format(S"{0}    hpp {1} cpp {2}", pad, __box(pTPH->Line), __box(pTPC->Line)));
		}
		else
		{
			DevEnvLib::DebugOutput::Message( String::Format(S"{0}    hpp {1}", pad,__box(pTPH->Line) ));
		}
	}

	if ( i_pNode->Nodes->Count > 0 )
	{
		int count = i_pNode->Nodes->Count;
		for ( int i = 0; i < count; ++i )
		{
			try
			{
				FCMHierarchy::DebugTree( i_pNode->Nodes->Item[i], level+1 );
			}
			catch (Exception* ex)
			{
				DevEnvLib::DebugOutput::Message( String::Format(S"exception! {0}", ex->ToString() ));
				int x;
				x = 2;
			}
		}
	}
}

}