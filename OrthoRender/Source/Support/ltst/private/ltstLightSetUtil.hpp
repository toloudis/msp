/*****************************************************************************
**	ltstLightSetUtil.hpp
**
**	Supporting namespace for finding things by name.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef LTST_LIGHTSETUTIL_HPP
#error ltstLightSetUtil.hpp multiply included
#endif
#define LTST_LIGHTSETUTIL_HPP

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace ltstLightSetUtil
{
	template<class S> 
	class name_search
	{
	public:
		name_search(const nameString& i_Name) : m_Name(i_Name) {};
		bool operator () ( S* i_Set )
		{
			return (m_Name == i_Set->m_Name);
		}
		nameString m_Name;
	};

	template<class S> 
	class nameobj_search
	{
	public:
		nameobj_search(const nameString& i_Name) : m_Name(i_Name) {};
		bool operator () ( S* i_Set )
		{
			return (m_Name == i_Set->m_pNameObj->GetName());
		}
		nameString m_Name;
	};

	template<class S> 
	class nameobjptr_search
	{
	public:
		nameobjptr_search(nameObject* i_pNameObj) : m_pNameObj(i_pNameObj) {};
		bool operator () ( S* i_Set )
		{
			return (m_pNameObj == i_Set->m_pNameObj);
		}
		nameObject* m_pNameObj;
	};

}	// end of namespace
