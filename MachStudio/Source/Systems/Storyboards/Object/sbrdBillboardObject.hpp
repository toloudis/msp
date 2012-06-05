/*****************************************************************************
**  sbrdBillboardObject.hpp
**
**      A sbrdBillboardObject is a derived class for a storyboard, which is a textured
**	polygon that always faces the camera.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SBRD_SBRDBOARDOBJECT_HPP
#error sbrdBillboardObject.hpp multiply included
#endif
#define SBRD_SBRDBOARDOBJECT_HPP

#ifndef SBRD_OBJECTDATA_HPP
#include "Systems/Storyboards/Data/sbrdObjectData.hpp"
#endif
#ifndef MNM_OBJECT_HPP
#include "Support/mnm/mnmObject.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
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
class api3dBillboard;
class api3dObject;
class matTexture;


//============================================================================
//============================================================================
class sbrdBillboardObject : public mnmObject, public nameObject, public prtyObject
{
	public:
		//--------------------------------------------------------------------
		// This object takes ownership of the arguments passed in
		//--------------------------------------------------------------------
		sbrdBillboardObject( api3dBillboard* i_pBillboard,
							 pick3dPickObject* i_pParent);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~sbrdBillboardObject();


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
		const sbrdObjectData& GetData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetData(const sbrdObjectData &i_Data);


	//============================================================================
	//	properties
	//============================================================================

		//--------------------------------------------------------------------
		// Name property access
		//--------------------------------------------------------------------
		prtyName&	PropertyName();
		const prtyName&	GetPropertyName() const;

		//--------------------------------------------------------------------
		// Position property access
		//--------------------------------------------------------------------
		prtyPoint3d&	PropertyPosition();
		const prtyPoint3d&	GetPropertyPosition() const;

		//--------------------------------------------------------------------
		// Orientation property access
		//--------------------------------------------------------------------
		prtyRotation&	PropertyOrientation();
		const prtyRotation&	GetPropertyOrientation() const;

		//--------------------------------------------------------------------
		// Color property access
		//--------------------------------------------------------------------
		prtyColor&	PropertyColor();
		const prtyColor&	GetPropertyColor() const;

		//--------------------------------------------------------------------
		// Scale property access
		//--------------------------------------------------------------------
		prtyFloat& PropertyScale();
		const prtyFloat& GetPropertyScale() const;

		//--------------------------------------------------------------------
		// Visible property access
		//--------------------------------------------------------------------
		prtyBoolean&	PropertyVisible();
		const prtyBoolean&	GetPropertyVisible() const;

		//--------------------------------------------------------------------
		// FileName property access
		//--------------------------------------------------------------------
		prtyFileName&	PropertyFileName();
		const prtyFileName&	GetPropertyFileName() const;

	//============================================================================
	//	members
	//============================================================================

		//--------------------------------------------------------------------
		// Maintains texture pointer based on filename
		//--------------------------------------------------------------------
		//void SetTextureFilename( const itString& i_Filename );
		
		//--------------------------------------------------------------------
		// Returns the texture pointer from the filename that was loaded.
		// This is the texture that is owned by this object (m_pTexture)
		// but may not be the texture currently on the billboard.
		//--------------------------------------------------------------------
		//matTexture* GetBaseTexture();

		//--------------------------------------------------------------------
		// Changes the texture on the billboard without altering this
		// object's texture filename value or its "owned" base texture.
		//--------------------------------------------------------------------
		//void SetBillboardTexture( matTexture* i_pTexture );

		//--------------------------------------------------------------------
		//	GetWorldBox returns a box which completely encloses the
		//	sbrdBillboardObject.
		//--------------------------------------------------------------------
		virtual const maAxisBox& GetWorldBox() const;

		//--------------------------------------------------------------------
		//	GetLocalBox returns a box which would enclose the sbrdBillboardObject
		//	if its transformations were identity
		//--------------------------------------------------------------------
		virtual const maAxisBox& GetLocalBox() const;

		//--------------------------------------------------------------------
		//	Position
		//--------------------------------------------------------------------
		virtual maPoint3d GetPosition() const;
		virtual void UpdatePosition(const maPoint3d& i_Position, 
										bool i_bNewOperation);

		//--------------------------------------------------------------------
		//	Orientation
		//--------------------------------------------------------------------
		virtual maRotation GetOrientation() const;
		virtual void UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation);

		//--------------------------------------------------------------------
		//	Scale
		//--------------------------------------------------------------------
		virtual maPoint3d GetScale() const;
		virtual void UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation);

		//--------------------------------------------------------------------
		// SetName
		//--------------------------------------------------------------------
		virtual void SetName(const nameString& i_Name);

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the sbrdBillboardObject.
		//	If it does, the t value is also returned in o_T.
		//--------------------------------------------------------------------
		virtual bool RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T);

		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual bool MatchPickCode(envType::UInt32 i_PickCode) const;

		//--------------------------------------------------------------------
		//  Changes visible state of prop based on GUI
		//--------------------------------------------------------------------
		void SetEditorVisible(bool i_bVisible);
		bool GetEditorVisible() const;

		//--------------------------------------------------------------------
		//	LayerVisible represents if objects in the layer are visible
		//--------------------------------------------------------------------
		void SetLayerVisible(bool i_bVisible);

		//--------------------------------------------------------------------
		//	Set/GetVisible comes from channel controlling visibility 
		//	of geometry
		//--------------------------------------------------------------------
		//void SetVisible(bool i_bVisible);
		//bool GetVisible() const;

		//--------------------------------------------------------------------
		//	Wireframe represents if the objects are rendered
		//	in a wireframe style.
		//--------------------------------------------------------------------
		void SetWireframe(bool i_bWireframe);

		//--------------------------------------------------------------------
		// Flags for how object can be modified
		//--------------------------------------------------------------------
		virtual RotateFlags	GetRotateFlags();
		virtual ScaleFlags	GetScaleFlags();
		virtual TranslateFlags GetTranslateFlags();

		//--------------------------------------------------------------------
		//	using the ratio of the texture, set the scale of x+y
		//--------------------------------------------------------------------
		void CalculateScale(float i_fScale);

		//--------------------------------------------------------------------
		//	Access to billboard object
		//--------------------------------------------------------------------
		api3dBillboard * GetBillboardObject();

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		virtual pick3dPickObject* GetParentObject() const;

		//--------------------------------------------------------------------
		//	Get reference for given name.  The returned pointer is owned
		//	by this object.  The object should be retained by the caller
		//	to avoid repeated string searches.
		//--------------------------------------------------------------------
		api3dReference* GetReference(const char* i_Name);

		//--------------------------------------------------------------------
		// Get list of references for possible attachment within this object.
		//--------------------------------------------------------------------
		void GetReferenceList(std::vector<std::string> &o_List);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_world_box();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void calculate_scale();

		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void TextureChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void TransformChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void BillboardChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void VisibleChanged(prtyProperty *i_pProperty, bool i_bDirty);

	//
	private:
		api3dBillboard*		m_pBillboard;
		matTexture*			m_pTexture;
		pick3dPickObject*	m_pParent;

		maAxisBox			m_WorldBox;
		bool				m_bLayerVisible;

		sbrdObjectData		m_Data;
};

