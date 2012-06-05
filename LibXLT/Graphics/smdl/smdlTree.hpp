/****************************************************************************\
**	smdlTree.hpp
**
**		smdlTree is a container which provides a tree structure.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_TREE_HPP
#error smdlTree.hpp multiply included
#endif
#define SMDL_TREE_HPP

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
template <class T>
class smdlTree
{
	private:
		//============================================================================
		//============================================================================
		struct Node
		{
			//----------------------------------------------------------------
			//	for some reason this foolish compiler refuses to accept
			//	these function definitions outside of the class declaration
			//----------------------------------------------------------------
			Node() : m_Parent(NULL) {}

			//----------------------------------------------------------------
			//----------------------------------------------------------------
			Node(const T& i_Val) : m_Parent(NULL), m_Data(i_Val) {}

			//----------------------------------------------------------------
			//----------------------------------------------------------------
			~Node()
			{
				std::vector< Node* >::iterator it = m_Children.begin();
				std::vector< Node* >::iterator end = m_Children.end();

				for( ; it != end ; it++ )
					delete *it;
			}

			T m_Data;
			Node* m_Parent;
			std::vector<Node*> m_Children;
		};

		Node* m_Head;
		int m_Size;

	public:
		//============================================================================
		//============================================================================
		class const_iterator
		{
			public:
				//------------------------------------------------------------
				//------------------------------------------------------------
				bool HasParent() const { return m_Node->m_Parent != NULL; }

				//------------------------------------------------------------
				//------------------------------------------------------------
				int NumChildren() const { return m_Node->m_Children.size(); }

				//------------------------------------------------------------
				//------------------------------------------------------------
				void MoveToParent()
				{
					DBG_ASSERT(m_Node, "Invalid node");
					if (!m_Node)
						return;
					DBG_ASSERT(m_Node->m_Parent != NULL, "Can't move iterator to empty parent");
					if (m_Node->m_Parent == NULL)
						return;
					m_Node = m_Node->m_Parent;
				}

				//------------------------------------------------------------
				//------------------------------------------------------------
				void MoveToChild(int i_Num)
				{
					DBG_ASSERT(m_Node, "Invalid node");
					if (!m_Node)
						return;
					DBG_ASSERT(m_Node->m_Children.size() > i_Num, "Can't move iterator to non-existent child");

					if ( m_Node->m_Children.size() > i_Num )
						m_Node = m_Node->m_Children[i_Num];
					else
						m_Node = NULL;
				}

				//------------------------------------------------------------
				//------------------------------------------------------------
				const T& GetData() const
				{
					DBG_ASSERT(m_Node, "Invalid node");
					return m_Node->m_Data;
				}

			private:
				friend smdlTree<T>;

				//------------------------------------------------------------
				//------------------------------------------------------------
				const_iterator(Node* i_Node) : m_Node(i_Node) {}

				const Node* m_Node;
		};

		//============================================================================
		//============================================================================
		class iterator
		{
			public:
				//------------------------------------------------------------
				//------------------------------------------------------------
				bool HasParent() const { return m_Node->m_Parent != NULL; }

				//------------------------------------------------------------
				//------------------------------------------------------------
				int NumChildren() const { return m_Node->m_Children.size(); }

				//------------------------------------------------------------
				//------------------------------------------------------------
				void MoveToParent()
				{
					DBG_ASSERT(m_Node, "Invalid node");
					if (!m_Node)
						return;
					DBG_ASSERT(m_Node->m_Parent != NULL, "Can't move iterator to empty parent");
					if (m_Node->m_Parent == NULL)
						return;
					m_Node = m_Node->m_Parent;
				}

				//------------------------------------------------------------
				//------------------------------------------------------------
				void MoveToChild(int i_Num)
				{
					DBG_ASSERT(m_Node, "Invalid node");
					if (!m_Node)
						return;
					DBG_ASSERT(m_Node->m_Children.size() > i_Num, "Can't move iterator to non-existent child");

					if ( m_Node->m_Children.size() > i_Num )
						m_Node = m_Node->m_Children[i_Num];
					else
						m_Node = NULL;
				}

				//------------------------------------------------------------
				//------------------------------------------------------------
				T& GetData()
				{
					DBG_ASSERT(m_Node, "Invalid node");
					return m_Node->m_Data;
				}

				//------------------------------------------------------------
				//------------------------------------------------------------
				void AddChild()
				{
					Node* new_node = new Node;
					m_Node->m_Children.push_back(new_node);
					new_node->m_Parent = m_Node;
				}

				//------------------------------------------------------------
				//------------------------------------------------------------
				void AddChild(const T& i_Data)
				{
					Node* new_node = new Node;
					m_Node->m_Children.push_back(new_node);
					new_node->m_Data = i_Data;
					new_node->m_Parent = m_Node;
				}

			private:
				friend smdlTree<T>;

				//------------------------------------------------------------
				//------------------------------------------------------------
				iterator(Node* i_Node) : m_Node(i_Node) {}

				Node* m_Node;
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		smdlTree();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~smdlTree();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetHead(const T& i_Data);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int GetSize() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		iterator GetIterator();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const_iterator GetIterator() const;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
smdlTree<T>::smdlTree()
:	m_Head(NULL),
	m_Size(0)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
smdlTree<T>::~smdlTree()
{
	delete m_Head;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
void smdlTree<T>::SetHead(const T& i_Data)
{
	if ( m_Head == NULL )
	{
		m_Size++;
		m_Head = new Node;
	}

	m_Head->m_Data = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
int smdlTree<T>::GetSize() const
{
	return m_Size;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
typename smdlTree<T>::iterator smdlTree<T>::GetIterator()
{
	return iterator(m_Head);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
typename smdlTree<T>::const_iterator smdlTree<T>::GetIterator() const
{
	return const_iterator(m_Head);
}
