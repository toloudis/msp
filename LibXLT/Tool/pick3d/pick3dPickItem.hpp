/****************************************************************************\
**	pick3dPickItem.hpp
**
**		An item that can be picked
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PICK3D_PICKITEM_HPP
#error pick3dPickItem.hpp multiply included
#endif
#define PICK3D_PICKITEM_HPP

#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
//class pick3dPickObject;


//============================================================================
//============================================================================
class pick3dPickItem
{
	public:

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pick3dPickItem();
		pick3dPickItem(pick3dPickObject* i_pObject, float i_tVal);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~pick3dPickItem();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const float&	GetTVal() const {return m_tVal;}
		void	SetTVal(const float &val)	{m_tVal = val;}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pick3dPickObject*	GetObject() const {return m_pObject;}
		void	SetObject( pick3dPickObject* val )	{m_pObject = val;}

		//--------------------------------------------------------------------
		// operator== checks if the object pointers match
		//--------------------------------------------------------------------
		bool operator == (const pick3dPickItem& i_Item) const;

	private:
		float	m_tVal;
		pick3dPickObject*	m_pObject;
};

//--------------------------------------------------------------------
// operator< compares t-values
//--------------------------------------------------------------------
bool operator<( const pick3dPickItem& a, const pick3dPickItem& b );
