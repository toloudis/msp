/*****************************************************************************
**  dcutDirectorsCutObject.hpp
**
**      A dcutDirectorsCutObject is a derived class for tracking the edit cuts
**	between cameras.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef DCUT_DIRECTORSCUTOBJECT_HPP
#error dcutDirectorsCutObject.hpp multiply included
#endif
#define DCUT_DIRECTORSCUTOBJECT_HPP

#ifndef DCUT_DATA_HPP
#include "Systems/DirectorsCut/Data/dcutData.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef CAMS_DIRECTORSCUT_HPP
#include "Support/cams/camsDirectorsCut.hpp"
#endif


//============================================================================
//	forward references
//============================================================================


//============================================================================
//============================================================================
class dcutDirectorsCutObject : public pick3dPickObject, 
						 public nameObject, 
						 public prtyObject,
						 public camsDirectorsCut
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		dcutDirectorsCutObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~dcutDirectorsCutObject();

		//--------------------------------------------------------------------
		//  Return index for which camera to use at this moment 
		//  for this director's cut.
		//--------------------------------------------------------------------
		virtual int GetCameraIndex();

		//--------------------------------------------------------------------
		//  Set the index of the camera to use. This is set from the
		//	drivers to animate the cuts.
		//--------------------------------------------------------------------
		void SetCameraIndex(int i_Index);

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetPick3dName() const;


	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a camera data structure
		//--------------------------------------------------------------------
		const dcutCueData& GetData() const;

		//--------------------------------------------------------------------
		// Set from camera data structure
		//--------------------------------------------------------------------
		void SetData(const dcutCueData &i_Data);


	//============================================================================
	//	properties
	//============================================================================

		//--------------------------------------------------------------------
		// Name property access
		//--------------------------------------------------------------------
		prtyName&	PropertyName();
		const prtyName&	GetPropertyName() const;

		//--------------------------------------------------------------------
		// Description property access
		//--------------------------------------------------------------------
		const prtyText&	GetPropertyDescription() const;

		//--------------------------------------------------------------------
		//	Name
		//--------------------------------------------------------------------
		virtual void SetName(const nameString& i_Name);

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		virtual pick3dPickObject* GetParentObject() const;
		virtual void SetParentObject(pick3dPickObject* i_pParent);
		
	private:

		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void DescriptionChanged(prtyProperty *i_pProperty, bool i_bDirty);

	private:
		dcutCueData	m_Data;
		pick3dPickObject* m_pParent;
		int m_CameraIndex;
};

