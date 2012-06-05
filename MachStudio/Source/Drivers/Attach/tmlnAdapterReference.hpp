/*****************************************************************************
**	tmlnAdapterReference.hpp
**
**	 Adapter for getting target position from attachment reference to object
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef TMLN_ADAPTERREFERENCE_HPP
#error tmlnAdapterReference.hpp multiply included
#endif
#define TMLN_ADAPTERREFERENCE_HPP


#ifndef TMLN_ADAPTERPOSITION_HPP
#include "Support/tmln/tmlnAdapterPosition.hpp"
#endif
#ifndef ENV_AUDITOR_HPP
#include "Core/env/envAuditor.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

class api3dReference;

class tmlnAdapterReference : public tmlnAdapterGetPosition, public envAuditor
{

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnAdapterReference();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~tmlnAdapterReference();

	//--------------------------------------------------------------------
	// Attach to given named reference point
	//--------------------------------------------------------------------
	void AttachTo(api3dReference *i_pReference); 

	//--------------------------------------------------------------------
	// If the object we attached to is deleted, the reference will become
	//	NULL.  In this case, this function will return false and 
	//	the driver can look for the reference again.
	//--------------------------------------------------------------------
	bool IsAttached() const;

	//--------------------------------------------------------------------
	// Set offset to position within attachment matrix
	//--------------------------------------------------------------------
	void SetAttachOffset(const maPoint3d &i_AttachOffset); 

	//--------------------------------------------------------------------
	// Set offset to rotation within attachment matrix
	//--------------------------------------------------------------------
	void SetAttachOrientation(const maRotation &i_AttachOrient);

	//--------------------------------------------------------------------
	//  Get position of object based on attachment matrix
	//--------------------------------------------------------------------
	virtual maPoint3d  GetPosition() const;

	//--------------------------------------------------------------------
	//  Get rotation of object based on attachment matrix
	//--------------------------------------------------------------------
	maRotation  GetOrientation() const;

	//--------------------------------------------------------------------
	// Set whether to attach to the center of the bounding box at a
	//	given reference point instead of using world transform.
	//--------------------------------------------------------------------
	void SetUseBoundingBox(bool i_bUseBBox);

	//--------------------------------------------------------------------
	// From envAuditor, this callback is called when the reference 
	//	is deleted, so that we can break the connection.
	//--------------------------------------------------------------------
	virtual void AuditorNotify(envAuditable* i_pReference);

private:
	api3dReference *m_pReference;
	maPoint3d m_AttachOffset;
	maRotation m_AttachOrient;
};
