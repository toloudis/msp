/*****************************************************************************
**  mnpTranslateInteraction.hpp
**
**      An interaction that translates a set of objects along a vector,
**	mapping mouse motions to that vector.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MNP_TRANSLATEINTERACTION_HPP
#error mnpTranslateInteraction.hpp multiply included
#endif
#define MNP_TRANSLATEINTERACTION_HPP

#ifndef MNP_INTERACTION_HPP
#include "Features/ObjectManip/mnpInteraction.hpp"
#endif 

#ifndef MA_MATRIX4X4_HPP
#include "Core/Ma/maMatrix4x4.hpp"
#endif 

#include <list>

//----------------------------------------------------------------------------
//	forward references
//----------------------------------------------------------------------------
class mnmObject;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class mnpTranslateInteraction : public mnpInteraction
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mnpTranslateInteraction(mnmObject* i_pSelectedObject);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mnpTranslateInteraction() = 0;

protected:
	//------------------------------------------------------------------------
	// Translate the main selected object to the given world position,
	// computing and applying the delta translation to 
	//------------------------------------------------------------------------
	void TranslateToWorldPosition(const maPoint3d& i_WorldPos,
								  bool i_bNewOperation = true);

private:
	//------------------------------------------------------------------------
	// Translate objects in the selected list that aren't the focus
	// of the translation compass.
	//------------------------------------------------------------------------
	void apply_delta_position(const maVector3d& i_Delta, bool i_bNewOperation);

protected:
	mnmObject*				m_pSelectedObject;
	maMatrix4x4				m_ParentXform;
	maMatrix4x4				m_InverseParentXform;

	struct sObjectMatrix
	{
		mnmObject* m_pObject;
		maMatrix4x4	m_InverseParentXform;
	};
	std::list<sObjectMatrix>	m_AdditionalObjects;
};
