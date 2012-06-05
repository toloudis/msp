/*****************************************************************************
**  emdlStaticTemplate.hpp
**
**      A emdlStaticTemplate adds a member in order to tell how many
**	of the fragments are low-res.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef EMDL_STATICTEMPLATE_HPP
#error emdlStaticTemplate.hpp multiply included
#endif
#define EMDL_STATICTEMPLATE_HPP

#ifndef ENT_MODELTEMPLATE_HPP
#include "Graphics/ent/entModelTemplate.hpp"
#endif


//============================================================================
//============================================================================
class emdlStaticTemplate : public entModelTemplate
{
	public:
		//--------------------------------------------------------------------
		//	Constructor
		//--------------------------------------------------------------------
		emdlStaticTemplate();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~emdlStaticTemplate();

		//--------------------------------------------------------------------
		//	GetNumLowRes returns the number of fragments that are 
		//	low resolution. These will be at the end of the fragment list.
		//--------------------------------------------------------------------
		inline int GetNumLowRes() const;
		void SetNumLowRes( int i_NumLowRes );

		//--------------------------------------------------------------------
		//	GetNumHighRes returns the number of fragments that are 
		//	high resolution. These will be at the end of the fragment list,
		//	after the low resolution fragments.
		//--------------------------------------------------------------------
		inline int GetNumHighRes() const;
		void SetNumHighRes( int i_NumHighRes );

	private:
		int m_NumLowRes;
		int m_NumHighRes;
};


//--------------------------------------------------------------------
//	GetNumLowRes returns the number of fragments that are 
//	low resolution. These will be at the end of the fragment list.
//--------------------------------------------------------------------
inline int emdlStaticTemplate::GetNumLowRes() const
{
	return m_NumLowRes;
}

//--------------------------------------------------------------------
//	GetNumHighRes returns the number of fragments that are 
//	high resolution. These will be at the end of the fragment list,
//	after the low resolution fragments.
//--------------------------------------------------------------------
inline int emdlStaticTemplate::GetNumHighRes() const
{
	return m_NumHighRes;
}