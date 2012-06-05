/*****************************************************************************
**	cmraAdapterReference.hpp
**
**	 Adapter for getting target position from attachment reference to object
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef CMRA_ADAPTERREFERENCE_HPP
#error cmraAdapterReference.hpp multiply included
#endif
#define CMRA_ADAPTERREFERENCE_HPP


#ifndef TMLN_ADAPTERPOSITION_HPP
#include "Support/tmln/tmlnAdapterPosition.hpp"
#endif
#ifndef ENV_AUDITOR_HPP
#include "Core/env/envAuditor.hpp"
#endif

class api3dReference;

class cmraAdapterReference : public tmlnAdapterGetPosition, public envAuditor
{

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraAdapterReference();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~cmraAdapterReference();

	//--------------------------------------------------------------------
	// Attach to given named reference point
	//--------------------------------------------------------------------
	void AttachTo(api3dReference *i_pReference, const maPoint3d &i_TargetOffset);

	//--------------------------------------------------------------------
	//  Get position of object, for target of camera
	//--------------------------------------------------------------------
	virtual maPoint3d  GetPosition() const;

	//--------------------------------------------------------------------
	// From envAuditor, this callback is called when the reference 
	//	is deleted, so that we can break the connection.
	//--------------------------------------------------------------------
	virtual void AuditorNotify(envAuditable* i_pReference);

private:
	api3dReference *m_pReference;
	maPoint3d m_Offset;
};
