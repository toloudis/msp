//****************************************************************************
///  \file CMNode.hpp
///
///	CodeModel Node for building a tree hierarchy of the code model for a 
///	solution, project, etc.
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
#ifdef CMNODE_HPP
#error CMNode.hpp multiply included
#endif
#define CMNODE_HPP


//============================================================================
//	usings
//============================================================================
using namespace EnvDTE;
using namespace System;
using namespace System::Windows::Forms;


//============================================================================
/// CodeModelLib
//============================================================================
namespace CodeModelLib
{
	//------------------------------------------------------------------------
	/// CodeModel Node
	//------------------------------------------------------------------------
	public __gc class CMNode : public TreeNode
	{
		public:
			__value enum CMNKindValue
			{
				Undefined,
				Solution,
				Project,
				Folder,
				ProjectItem,
				CodeElement,
			};
		
		public:
			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			CMNode::CMNode()
			:	m_bIsChild(false),
				m_Kind(Undefined)
			{
			}

			//--------------------------------------------------------------------
			// Kind
			//--------------------------------------------------------------------
			CMNKindValue GetKind()
			{
				return m_Kind;
			}
			void SetKind( CMNKindValue i_Kind )
			{
				m_Kind = i_Kind;
			}
			//--------------------------------------------------------------------
			// IsChild
			//--------------------------------------------------------------------
			bool GetIsChild()
			{
				return m_bIsChild;
			}
			void SetIsChild( bool i_bIsChild )
			{
				m_bIsChild = i_bIsChild;
			}
			//--------------------------------------------------------------------
			// StartPointCPP
			//--------------------------------------------------------------------
			TextPoint* GetStartPointCPP()
			{
				return m_pStartPointCPP;
			}
			void SetStartPointCPP( TextPoint& i_Point )
			{
				m_pStartPointCPP = &i_Point;
			}
			//--------------------------------------------------------------------
			// StartPointHPP
			//--------------------------------------------------------------------
			TextPoint* GetStartPointHPP()
			{
				return m_pStartPointHPP;
			}
			void SetStartPointHPP( TextPoint& i_Point )
			{
				m_pStartPointHPP = &i_Point;
			}

		private:
			//--------------------------------------------------------------------
			//--------------------------------------------------------------------
			bool			m_bIsChild;
			CMNKindValue	m_Kind;
			TextPoint*		m_pStartPointHPP;
			TextPoint*		m_pStartPointCPP;
	};
}
