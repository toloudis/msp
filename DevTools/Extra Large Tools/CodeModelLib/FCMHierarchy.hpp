//****************************************************************************
///  \file FCMHierarchy.hpp
///
///	CodeModel hierarchy used for modifying physical code files.
///
///	Note: operations on this hierarchy are usually performed BOTTOM to TOP
///	because otherwise the line numbers for classes gets messed up.
/// (microsoft FCM issue)
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
#ifdef FCMHIERARCHY_HPP
#error FCMHierarchy.hpp multiply included
#endif
#define FCMHIERARCHY_HPP

#ifndef CMNODE_HPP
#include "CMNode.hpp"
#endif


//============================================================================
//	using
//============================================================================
using namespace EnvDTE;
using namespace System;


//============================================================================
/// CodeModelLib
//============================================================================
namespace CodeModelLib
{
	//------------------------------------------------------------------------
	/// FCMHierarchy
	//------------------------------------------------------------------------
	public __gc class FCMHierarchy
	{
		public:
			__value enum eFileType
			{
				Header = 0,			// .hpp or .h
				Source = 1,			// .cpp
			};

			// convert this to the traversal with node adding
			// helper namespace to traverse and output
			// test set-up

			////----------------------------------------------------------------
			///// Test
			//// ToDo [rjk] remove this function
			////----------------------------------------------------------------
			//static void Test(Object* i_pApp);

			//----------------------------------------------------------------
			///	set up the variables
			//----------------------------------------------------------------
			static void Init();

			////----------------------------------------------------------------
			/////
			////----------------------------------------------------------------
			//static bool IsStringCommentLine(String* i_pString);

			////----------------------------------------------------------------
			/////
			////----------------------------------------------------------------
			//static void BuildTreeForFunction(CodeElement* i_pCodeElement);

			////----------------------------------------------------------------
			////----------------------------------------------------------------
			//static String* GetCommentForClass( CodeClass* i_pCClass );

			////----------------------------------------------------------------
			////----------------------------------------------------------------
			//static void SetCommentForClass( CodeClass* i_pCClass, String* i_pNewComments );

			////----------------------------------------------------------------
			/////
			////----------------------------------------------------------------
			//static void BuildTreeForClass(CodeElement* i_pCodeElement);

			////----------------------------------------------------------------
			////----------------------------------------------------------------
			//static String* GetCommentForNamespace( CodeNamespace* i_pCNamespace );

			////----------------------------------------------------------------
			////----------------------------------------------------------------
			//static void SetCommentForNamespace( CodeNamespace* i_pCNamespace, String* i_pNewComments );

			////----------------------------------------------------------------
			/////
			////----------------------------------------------------------------
			//static void BuildTreeForNamespace(CodeElement* i_pCodeElement);

			//----------------------------------------------------------------
			/// \return the code element node and the rest of the code hierarchy
			//----------------------------------------------------------------
			static void BuildTreeForCodeElements(CodeElements* i_pCEs, CMNode& io_ParentNode);

			//----------------------------------------------------------------
			/// \return the project item node and the rest of the code hierarchy
			//----------------------------------------------------------------
			static CMNode* BuildTreeForProjectItem(ProjectItem* i_pProjItem);

			//----------------------------------------------------------------
			/// \return the project node and the rest of the code hierarchy
			//----------------------------------------------------------------
			static CMNode* BuildTreeForProject(Project* i_pProj);

			//----------------------------------------------------------------
			/// \return the solution node and the rest of the code hierarchy
			//----------------------------------------------------------------
			static CMNode* BuildTreeForActiveSolution();

			//----------------------------------------------------------------
			/// \return root node
			//----------------------------------------------------------------
			static CMNode* GetRootNode()
			{
				return m_pRootNode;
			};

			//----------------------------------------------------------------
			/// \param the parent of the hierarchy
			//----------------------------------------------------------------
			static void DebugTree( TreeNode* i_pRootNode );

		private:
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
			static void SetStartPoints(CodeElement* i_pCE, CMNode& i_pNode);

			//----------------------------------------------------------------
			/// ScrapeComments
			///
			///	Grab the comment block based on the starting line numbers
			/// for the hpp and the cpp.
			///
			///	\param i_pCE the code element to grab the comments from.
			///		cannot be null.
			//----------------------------------------------------------------
			static void ScrapeComments(CMNode& i_pNode);

			//----------------------------------------------------------------
			/// \param the parent of the hierarchy
			//----------------------------------------------------------------
			static void DebugTree( TreeNode* i_pNode, int level );

		private:
			static CMNode* m_pRootNode = 0;
			static bool m_bInitCalled = true;
	};
}
