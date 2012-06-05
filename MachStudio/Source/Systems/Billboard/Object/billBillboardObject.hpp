/*****************************************************************************
**	billBillboardObject.hpp
**
**		A billBillboardObject is a derived class for a billboard, which is a textured
**	polygon that always faces the camera.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef BILL_BILLBOARDOBJECT_HPP
#error billBillboardObject.hpp multiply included
#endif
#define BILL_BILLBOARDOBJECT_HPP

#ifndef BILL_DATA_HPP
#include "Systems/Billboard/Data/billData.hpp"
#endif
#ifndef MNM_OBJECT_HPP
#include "Support/mnm/mnmObject.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef CMM_NAMEDPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmNamedPropertyObject.hpp"
#endif 
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class api3dBillboard;
class api3dObject;
class matTexture;
class gpxBillboard;
class prtyComboBoxUIInfo;
class camCamera;


//============================================================================
//============================================================================
class billBillboardObject : public mnmObject, public cmmNamedPropertyObject
{
	public:
		//--------------------------------------------------------------------
		// This object takes ownership of the arguments passed in
		//--------------------------------------------------------------------
		billBillboardObject( api3dBillboard* i_pBillboard,
						std::vector<shared_ptr<camCamera>>& i_CamList);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~billBillboardObject();


	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetDisplayName() const;


	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a data structure
		//--------------------------------------------------------------------
		const billData& GetData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetData(const billData &i_Data);


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
		// Brightness property access
		//--------------------------------------------------------------------
		prtyFloat& PropertyBrightness();
		const prtyFloat& GetPropertyBrightness() const;

		//--------------------------------------------------------------------
		// NonUniform Scale property access
		//--------------------------------------------------------------------
		prtyPoint3d& PropertyNonUniformScale();
		const prtyPoint3d& GetPropertyNonUniformScale() const;

		//--------------------------------------------------------------------
		// FileName property access
		//--------------------------------------------------------------------
		prtyFilePath&	PropertyFileName();
		const prtyFilePath&	GetPropertyFileName() const;

		//--------------------------------------------------------------------
		// Video property access
		//--------------------------------------------------------------------
		prtyVideo&	PropertyVideo();
		const prtyVideo&	GetPropertyVideo() const;

		//--------------------------------------------------------------------
		// Visible property access
		//--------------------------------------------------------------------
		prtyBoolean&	PropertyVisible();
		const prtyBoolean&	GetPropertyVisible() const;


	//============================================================================
	//	members
	//============================================================================

		//--------------------------------------------------------------------
		// Maintains texture pointer based on filename
		//--------------------------------------------------------------------
		void SetTextureFilename( const fsLocator& i_Filename );

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
		//	billBillboardObject.
		//--------------------------------------------------------------------
		virtual maAxisBox GetWorldBox(int i_IconLayerIndex = 0) const;

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
		//	NonUniform Scale
		//--------------------------------------------------------------------
		virtual maPoint3d GetNonUniformScale() const;
		virtual void UpdateNonUniformScale(const maPoint3d& i_Scale, 
											bool i_bNewOperation);

		//--------------------------------------------------------------------
		// SetName
		//--------------------------------------------------------------------
		virtual void SetName(const nameString& i_Name);

		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual bool MatchPickCode(envType::UInt32 i_PickCode) const;

		//--------------------------------------------------------------------
		// Set object active state in the render layer
		//--------------------------------------------------------------------
		void SetActiveRenderLayer(bool i_bActive);

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
		virtual ScaleFlags	GetNonUniformScaleFlags();
		virtual TranslateFlags GetTranslateFlags();

		//--------------------------------------------------------------------
		//	Access to billboard object
		//--------------------------------------------------------------------
		api3dBillboard * GetBillboardObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetTextureDimensions( int i_Width, int i_Height ){ m_Width = i_Width; m_Height = i_Height; }
		void GetTextureDimensions( int& o_Width, int& o_Height ){ o_Width = m_Width; o_Height = m_Height; }

		//--------------------------------------------------------------------
		// Update List of camera
		//--------------------------------------------------------------------
		void UpdateCameraList(std::vector<shared_ptr<camCamera>>& i_DataList,
							std::vector<std::string>& i_NameList);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void DeleteCameraIndex(int i_Index);
	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_world_box();

		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void TextureChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void TransformChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ScaleChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void BillboardChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void VideoChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void VisibleChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void SnapToCameraChanged(prtyProperty *i_pProperty, bool i_bDirty);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetCamera(int i_Index);

	private:
		api3dBillboard* m_pBillboard;
		gpxBillboard*	m_pBillboardProxy;
		matTexture*		m_pTexture;
		matTexture*		m_pVideoTexture;
		matTexture*		m_pVideoTextureTemp;
		fsLocator		m_TextureNameLoaded;
		shared_ptr<prtyComboBoxUIInfo> m_pCameraListUI;
		std::vector<shared_ptr<camCamera>>& m_CameraList;

		int				m_Width, m_Height;	//dimensions for video texture
		int				m_Frame;			//frame for video

		maAxisBox		m_WorldBox;
		bool			m_bLayerVisible;

		billData		m_Data;
};

