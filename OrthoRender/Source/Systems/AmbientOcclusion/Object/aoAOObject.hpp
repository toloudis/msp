/*****************************************************************************
**  aoAOObject.hpp
**
**      A aoAOObject is a derived class for a storyboard, which is a textured
**	polygon that always faces the camera.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef AO_AOOBJECT_HPP
#error aoAOObject.hpp multiply included
#endif
#define AO_AOOBJECT_HPP

#ifndef AO_AODATA_HPP
#include "Systems/AmbientOcclusion/Data/aoAOData.hpp"
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


//============================================================================
//	forward references
//============================================================================
class prtyName;
class prtyEnum;
class prtyColor;
class prtyFloat;

//============================================================================
//============================================================================
class aoAOObject : public pick3dPickObject, public prtyObject, public nameObject
{
	public:
		//--------------------------------------------------------------------
		// This object takes ownership of the arguments passed in
		//--------------------------------------------------------------------
		aoAOObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~aoAOObject();


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
		// Get values as a data structure
		//--------------------------------------------------------------------
		const aoAOData& GetData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetData(const aoAOData &i_Data);

	//============================================================================
	//	properties
	//============================================================================

		//--------------------------------------------------------------------
		// SetName
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);

private:
		aoAOData		m_Data;

		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void UpdateSSAO(prtyProperty *i_pProperty, bool i_bDirty);
};

