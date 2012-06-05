/*****************************************************************************
**	billBillboardObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/Object/billBillboardObject.hpp"

#include "Systems/Billboard/GUI/billDialogDataUtil.hpp"
#include "Systems/Billboard/Data/billDocumentChunk.hpp"
#include "Systems/Billboard/GUI/billGeomList.hpp"
#include "Systems/Billboard/Undo/billOperations.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/mnm/mnmAviUtil.hpp"
#include "Support/mnm/mnmReference.hpp"
#include "Support/mnm/mnmReferencePosChannel.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

// library
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFileChooserUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Graphics/Cam/camCamera.hpp"
#include "Graphics/G2d/g2dPFD.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/api3d/api3dBillboard.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/gpx/gpxBillboard.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"


//============================================================================
//============================================================================
namespace
{
	const float l_SphereRadius = 0.8f;
	const float l_PickRadius = 2.0f;	// pick larger than icon
	const maAxisBox l_SphereBox(-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius,
								-l_SphereRadius, l_SphereRadius);
	const maVector3d l_Zero(0,0,0);
	const maVector3d l_One(1,1,1);
	const maRotation l_NoRot;

//	DXGI_FORMAT_B8G8R8X8_UNORM
	g2dPFD l_RGB_PFD(g2dPFD::e_RColor, 8*4 );
	g2dPFD l_RGBA_PFD(g2dPFD::e_Color, 8 * 4);
}


//----------------------------------------------------------------------------
// This object takes ownership of the arguments passed in
//----------------------------------------------------------------------------
billBillboardObject::billBillboardObject( api3dBillboard * i_pBillboard,
										 std::vector<shared_ptr<camCamera>>& i_CamList)
:	m_pBillboard(i_pBillboard),
	m_pTexture(NULL),
	m_pVideoTexture(NULL),
	m_pVideoTextureTemp(NULL),
	m_WorldBox(l_SphereBox),
//	m_pParent( i_pParent ),
	m_bLayerVisible(true),
	m_Width(0), m_Height(0), m_Frame(-1),
	m_CameraList(i_CamList)
{
	evmtEnvironmentMgr::AddObject(this, i_pBillboard);
	rlyrRenderLayerMgr::AddObject(this, m_pBillboard);

//	m_pBillboard->SetUniformScale(m_Data.m_Scale.GetValue());	// Scale defaults to larger value
	m_pBillboard->SetScale(m_Data.m_NonUniformScale.GetValue());
	update_world_box();

	// create thread-safe proxy
	m_pBillboardProxy = new gpxBillboard(*i_pBillboard);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Position), "Transform", "Position of the object");
	AddProperty( pPUII );
	//pPUII  = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Orientation), "category2", "Orientation of the object");
	//AddProperty( pPUII );
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	AddProperty( pPUII );

	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_NonUniformScale), "Transform", "Non-Uniform Scale of the Object");
	AddProperty( pPUII );
	
	//pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Filename), "Asset", "File Name of the object");
	//pPUII->SetReadOnly(true);
	//AddProperty( pPUII );
	prtyFileChooserUIInfo* pFCI;
	pFCI = new prtyFileChooserUIInfo(&(m_Data.m_Filename), "Asset", "File Name of the billboard texture");
	pFCI->SetDirectoryCategory("Billboards");
	AddProperty( pFCI );

	pPUII = new prtyColorRGBAEditUIInfo(&(m_Data.m_Color), "Surface Color", "Color of the object");
	AddProperty( pPUII );
	prtyRangedFloatUIInfo* pRFUII;
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_Brightness), "Surface Color", "Brightness");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(10.0f);
	pRFUII->SetNumTicks(1000);
	AddProperty( pRFUII );

	// Editor visible should not be exposed in regular properties GUI
	//AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bEditorVisible), "Editor", "Is the object visible in the editor") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bOrientToCamera), "General", "Is the object oriented to the camera") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bAdditiveMaterial), "General", "Does the object have additive material") );
//	pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_Scale), "Transform", "Scale of the object");
//	AddProperty( pPUII );

	pPUII = new prtyCheckBoxUIInfo(&(m_Data.m_bVisible), "Asset", "Animatable Visibility");
	AddProperty( pPUII );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bSnapToCamera), "General", "Is the object snapped to the camera") );
	prtyNumericUpDownUIInfo* pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_DistToCamera), "General", "Distance to camera");
	pNUDUII->SetMinimum(0);
	pNUDUII->SetIncrement(1.0f);
	pNUDUII->SetDecimalPlaces(2);
	AddProperty( pNUDUII );

	// ChromaKey property
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bCKActive), "Chroma Key", "Is chroma key active") );
	pPUII = new prtyColorRGBAEditUIInfo(&(m_Data.m_CKColor), "Chroma Key", "Chroma key color");
	AddProperty( pPUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_CKTolerance), "Chroma Key", "Tolerance");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetNumTicks(1000);
	AddProperty( pRFUII );

	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bCKRemoveSpill), "Chroma Key", "Remove spill") );
	prtyComboBoxUIInfo* pCBUII = new prtyComboBoxUIInfo(&(m_Data.m_CKSpillType), "Chroma Key", "Spill Type");
	AddProperty( pCBUII );
	pRFUII  = new prtyRangedFloatUIInfo(&(m_Data.m_CKSpillBias), "Chroma Key", "Spill Bias");
	pRFUII->SetMinimum(0.0f);
	pRFUII->SetMaximum(1.0f);
	pRFUII->SetNumTicks(1000);
	AddProperty( pRFUII );
	pNUDUII = new prtyNumericUpDownUIInfo(&(m_Data.m_CKEdgeBlur), "Chroma Key", "Blur Radius");
	pNUDUII->SetMinimum(0);
	pNUDUII->SetIncrement(1);
	pNUDUII->SetDecimalPlaces(0);
	AddProperty( pNUDUII );


	// Register callbacks to update when properties change
	m_Data.m_Filename.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::TextureChanged));
	m_Data.m_Position.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::TransformChanged));
	m_Data.m_Orientation.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::TransformChanged));
//	m_Data.m_Scale.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::ScaleChanged));
	m_Data.m_NonUniformScale.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::ScaleChanged));
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::NameChanged));
	m_Data.m_bOrientToCamera.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::BillboardChanged));
	m_Data.m_bAdditiveMaterial.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::BillboardChanged));
	m_Data.m_Color.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::BillboardChanged));
	m_Data.m_Brightness.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::BillboardChanged));
	m_Data.m_Video.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::VideoChanged));
	m_Data.m_bVisible.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::VisibleChanged));
	m_Data.m_bCKActive.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::BillboardChanged));
	m_Data.m_CKColor.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::BillboardChanged));
	m_Data.m_CKTolerance.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::BillboardChanged));
	m_Data.m_bCKRemoveSpill.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::BillboardChanged));
	m_Data.m_CKSpillType.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::BillboardChanged));
	m_Data.m_CKSpillBias.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::BillboardChanged));
	m_Data.m_CKEdgeBlur.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::BillboardChanged));
	m_Data.m_bSnapToCamera.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::SnapToCameraChanged));
	m_Data.m_DistToCamera.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::SnapToCameraChanged));
	m_Data.m_CameraIndex.AddCallback(new prtyCallbackWrapper<billBillboardObject>(this, &billBillboardObject::SnapToCameraChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
billBillboardObject::~billBillboardObject()
{
	// delete proxy
	delete m_pBillboardProxy;

	if ( m_pBillboard )
	{
		evmtEnvironmentMgr::RemoveObject(this, m_pBillboard);
		rlyrRenderLayerMgr::RemoveObject(this, m_pBillboard);
		api3dScene::RemoveObject(m_pBillboard);
		delete m_pBillboard;
	}

	if ( m_pTexture )
	{
		matTextureMgr::ReleaseTexture(m_pTexture);
	}

	if (m_pVideoTextureTemp)
	{
		matTextureMgr::ReleaseTexture(m_pVideoTextureTemp);
		m_pVideoTextureTemp = NULL;
	}
	// m_pVideoTexture is owned by billboard and does not need to be released here
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string billBillboardObject::GetDisplayName() const
{
	return GetName().GetString();
}


//============================================================================
//	Data
//============================================================================


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
const billData& billBillboardObject::GetData() const
{
	return this->m_Data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void billBillboardObject::SetData(const billData &i_Data)
{
	m_Data = i_Data;
}

//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName& billBillboardObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName& billBillboardObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
// Position property access
//--------------------------------------------------------------------
prtyPoint3d& billBillboardObject::PropertyPosition()
{
	return m_Data.m_Position;
}
const prtyPoint3d& billBillboardObject::GetPropertyPosition() const
{
	return m_Data.m_Position;
}

//--------------------------------------------------------------------
// Orientation property access
//--------------------------------------------------------------------
prtyRotation& billBillboardObject::PropertyOrientation()
{
	return m_Data.m_Orientation;
}
const prtyRotation& billBillboardObject::GetPropertyOrientation() const
{
	return m_Data.m_Orientation;
}

//--------------------------------------------------------------------
// Color property access
//--------------------------------------------------------------------
prtyColor& billBillboardObject::PropertyColor()
{
	return m_Data.m_Color;
}
const prtyColor& billBillboardObject::GetPropertyColor() const
{
	return m_Data.m_Color;
}

//--------------------------------------------------------------------
// Scale property access
//--------------------------------------------------------------------
prtyFloat& billBillboardObject::PropertyScale()
{
	return m_Data.m_Scale;
}
const prtyFloat& billBillboardObject::GetPropertyScale() const
{
	return m_Data.m_Scale;
}

//--------------------------------------------------------------------
// Scale property access
//--------------------------------------------------------------------
prtyFloat& billBillboardObject::PropertyBrightness()
{
	return m_Data.m_Brightness;
}
const prtyFloat& billBillboardObject::GetPropertyBrightness() const
{
	return m_Data.m_Brightness;
}

//--------------------------------------------------------------------
// NonUniform Scale property access
//--------------------------------------------------------------------
prtyPoint3d& billBillboardObject::PropertyNonUniformScale()
{
	return m_Data.m_NonUniformScale;
}
const prtyPoint3d& billBillboardObject::GetPropertyNonUniformScale() const
{
	return m_Data.m_NonUniformScale;
}


//--------------------------------------------------------------------
// FileName property access
//--------------------------------------------------------------------
prtyFilePath& billBillboardObject::PropertyFileName()
{
	return m_Data.m_Filename;
}
const prtyFilePath& billBillboardObject::GetPropertyFileName() const
{
	return m_Data.m_Filename;
}

//--------------------------------------------------------------------
// Video property access
//--------------------------------------------------------------------
prtyVideo& billBillboardObject::PropertyVideo()
{
	return m_Data.m_Video;
}
const prtyVideo& billBillboardObject::GetPropertyVideo() const
{
	return m_Data.m_Video;
}

//--------------------------------------------------------------------
// Visible property access
//--------------------------------------------------------------------
prtyBoolean& billBillboardObject::PropertyVisible()
{
	return m_Data.m_bVisible;
}
const prtyBoolean&	billBillboardObject::GetPropertyVisible() const
{
	return m_Data.m_bVisible;
}

//--------------------------------------------------------------------
//Maintains texture pointer based on filename
//--------------------------------------------------------------------
void billBillboardObject::SetTextureFilename( const fsLocator& i_Filename )
{
	//if (m_Data.m_Filename.GetValue() != i_Filename)
	//{
	//	if (m_pTexture)
	//	{
	//		matTextureMgr::ReleaseTexture(m_pTexture);
	//		m_pTexture = NULL;
	//	}

		m_Data.m_Filename.SetValue( i_Filename );

	//	// Expand filename into full path
	//	fsLocator tex_loc;
	//	if (billGeomList::FindFile(i_Filename, tex_loc))
	//	{
			matTexture *pTexture = matTextureMgr::LoadTexture(i_Filename);
			m_pBillboard->SetTexture(m_pTexture);
			m_pBillboardProxy->SetTexture(m_pTexture);
	//	}
	//}
	//fsLocator tex_loc(i_Filename);
	//matTextureMgr::ReloadTexture(i_Filename);
}

//--------------------------------------------------------------------
// Returns the texture pointer from the filename that was loaded.
// This is the texture that is owned by this object (m_pTexture)
// but may not be the texture currently on the billboard.
//--------------------------------------------------------------------
//matTexture* billBillboardObject::GetBaseTexture()
//{
//	return m_pTexture;
//}

//--------------------------------------------------------------------
// Changes the texture on the billboard without altering this
// object's texture filename value or its "owned" base texture.
//--------------------------------------------------------------------
//void billBillboardObject::SetBillboardTexture( matTexture* i_pTexture )
//{
//	m_pBillboard->SetTexture(i_pTexture);
//}

//----------------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the billBillboardObject.
//----------------------------------------------------------------------------
//virtual 
maAxisBox billBillboardObject::GetWorldBox(int i_IconLayerIndex) const
{
	return m_WorldBox;
}

//----------------------------------------------------------------------------
//	Position
//----------------------------------------------------------------------------
//virtual 
maPoint3d billBillboardObject::GetPosition() const
{
	return m_Data.m_Position.GetValue();
}
//virtual 
void billBillboardObject::UpdatePosition(const maPoint3d& i_Position, 
										bool i_bNewOperation)
{
	if(i_bNewOperation)
		this->CreateUndoForProperty(m_Data.m_Position);

	const bool bSetDirty = true;
	m_Data.m_Position.SetValue(i_Position, bSetDirty);
}


//----------------------------------------------------------------------------
//	Orientation
//----------------------------------------------------------------------------
//virtual 
maRotation billBillboardObject::GetOrientation() const
{
	return m_pBillboard->GetOrientation();
}
//virtual 
void billBillboardObject::UpdateOrientation(const maRotation& i_Orientation, 
											bool i_bNewOperation)
{
	if (i_bNewOperation)
		this->CreateUndoForProperty(m_Data.m_Orientation);

	const bool bSetDirty = true;
	m_Data.m_Orientation.SetQuaternion(i_Orientation, bSetDirty);
}

//----------------------------------------------------------------------------
//	Scale
//----------------------------------------------------------------------------
//virtual 
maPoint3d billBillboardObject::GetScale() const
{
	return maPoint3d(m_Data.m_Scale.GetValue(),
					m_Data.m_Scale.GetValue(),
					m_Data.m_Scale.GetValue());
}

//virtual 
void billBillboardObject::UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation)
{
	if (i_bNewOperation)
		this->CreateUndoForProperty(m_Data.m_Scale);

	const bool bSetDirty = true;
	m_Data.m_Scale.SetValue(i_Scale.GetX(), bSetDirty);
}

//----------------------------------------------------------------------------
//	Scale
//----------------------------------------------------------------------------
//virtual 
maPoint3d billBillboardObject::GetNonUniformScale() const
{
	return m_Data.m_NonUniformScale.GetValue();
}
//virtual 
void billBillboardObject::UpdateNonUniformScale(const maPoint3d& i_Scale, 
												bool i_bNewOperation)
{
	if (i_bNewOperation)
		this->CreateUndoForProperty(m_Data.m_NonUniformScale);

	const bool bSetDirty = true;
	m_Data.m_NonUniformScale.SetValue(i_Scale, bSetDirty);
}


//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
//virtual 
void billBillboardObject::SetName(const nameString& i_Name)
{
	// Set name first, which may change the ID number
	nameObject::SetName(i_Name);

	// Make sure that the data matches our true name (including
	// ID number)
	m_Data.m_Name = this->GetName();
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool billBillboardObject::MatchPickCode(envType::UInt32 i_PickCode) const
{
	// No proxies for GPU pick
	if (!m_pBillboard->GetRenderable())
		return false;

	return (m_pBillboard->ContainsPickCode(i_PickCode));
}

//--------------------------------------------------------------------
// Set object active state in the render layer
//--------------------------------------------------------------------
void billBillboardObject::SetActiveRenderLayer(bool i_bActive)
{
	m_bLayerVisible = i_bActive;
	//m_p3DObjectProxy->SetActiveFromRenderLayer(i_bActive);
	m_pBillboard->SetActiveInRenderLayer(i_bActive);
}

//--------------------------------------------------------------------
//  Changes visible state of prop based on GUI
//--------------------------------------------------------------------
void billBillboardObject::SetEditorVisible(bool i_bVisible)
{
	m_Data.m_bEditorVisible = i_bVisible;

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() 
		&& m_Data.m_bVisible.GetValue() 
		&& m_bLayerVisible;

	m_pBillboardProxy->SetRenderable( bRenderable );
}
bool billBillboardObject::GetEditorVisible() const
{
	return m_Data.m_bEditorVisible.GetValue();
}
 
//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
void billBillboardObject::SetLayerVisible(bool i_bVisible)
{
	m_bLayerVisible = i_bVisible;

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() 
		&& m_Data.m_bVisible.GetValue() 
		&& m_bLayerVisible;
	m_pBillboardProxy->SetRenderable(bRenderable);
}

//--------------------------------------------------------------------
//	Wireframe represents if the objects are rendered
//	in a wireframe style.
//--------------------------------------------------------------------
void billBillboardObject::SetWireframe(bool i_bWireframe)
{
	m_pBillboardProxy->SetWireframe(i_bWireframe);
}

//----------------------------------------------------------------------------
// Flags for how object can be modified
//----------------------------------------------------------------------------
mnmObject::RotateFlags	billBillboardObject::GetRotateFlags()
{
	return (m_Data.m_bOrientToCamera.GetValue()) ? mnmObject::e_RotateNone : mnmObject::e_RotateAll;
}
mnmObject::ScaleFlags	billBillboardObject::GetScaleFlags()
{
	return mnmObject::e_ScaleUniform;
}
mnmObject::ScaleFlags	billBillboardObject::GetNonUniformScaleFlags()
{
	return mnmObject::e_ScaleNonUniform;
}
mnmObject::TranslateFlags	billBillboardObject::GetTranslateFlags()
{
	return mnmObject::e_TranslateAll;
}

//--------------------------------------------------------------------
//	Access to billboard object
//--------------------------------------------------------------------
api3dBillboard * billBillboardObject::GetBillboardObject()
{
	return m_pBillboard;
}

//--------------------------------------------------------------------
// Update List of camera
//--------------------------------------------------------------------
void billBillboardObject::UpdateCameraList(std::vector<shared_ptr<camCamera>>& i_DataList,
										   std::vector<std::string>& i_NameList)
{
	m_CameraList = i_DataList;
	m_Data.m_CameraIndex.ClearTags();
	if (m_pCameraListUI.get())
		GetListContainer().Remove(shared_ptr<prtyPropertyUIInfo>(m_pCameraListUI));

	for(int i = 0; i < i_NameList.size(); i++)
	{
		m_Data.m_CameraIndex.SetEnumTag(i, i_NameList[i]);
	}
	m_pCameraListUI.reset(new prtyComboBoxUIInfo(&(m_Data.m_CameraIndex), "General", "Snap to Camera"));
	AddProperty( m_pCameraListUI );

	SetCamera(m_Data.m_CameraIndex.GetValue());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void billBillboardObject::DeleteCameraIndex(int i_Index)
{
	if (((int)(m_Data.m_CameraIndex.GetValue())) == i_Index + 1)
		m_Data.m_CameraIndex.SetValue(0);
}

//--------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//--------------------------------------------------------------------
//virtual 
//sel3dObject* billBillboardObject::GetParentObject() const
//{
//	return m_pParent;
//}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void billBillboardObject::update_world_box()
{
	float radius = l_SphereRadius * this->GetNonUniformScale().m_X;
	m_WorldBox = maAxisBox(-radius, radius,
						   -radius, radius,
						   -radius, radius);
	m_WorldBox.Translate(this->GetPosition());
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void billBillboardObject::TextureChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// stop any render threads for texture manager changes
	gpxRenderControl::ConfirmSingleThread();

	if (!m_pTexture || (m_Data.m_Filename.GetValue() != m_TextureNameLoaded))
	{
		if (m_pTexture)
		{
			matTextureMgr::ReleaseTexture(m_pTexture);
			m_pTexture = NULL;
		}

		fsLocator tex_loc = m_Data.m_Filename.GetValue();
		if (tex_loc.GetNumNames() > 0)
		{
			m_pTexture = matTextureMgr::LoadTexture(tex_loc);	
		}

		//	set the name
		m_TextureNameLoaded = m_Data.m_Filename.GetValue();

		// give the texture to our billboard
		m_pBillboardProxy->SetTexture(m_pTexture);
	}

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void billBillboardObject::VideoChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// stop any render threads for texture manager changes
	gpxRenderControl::ConfirmSingleThread();

	CVideoData data = m_Data.m_Video.GetValue();
	if (data.m_Width != m_Width || data.m_Height != m_Height)
	{
		m_Width = data.m_Width;
		m_Height = data.m_Height;

		if ((m_Width > 0) && (m_Height > 0) && data.m_Filename.GetNumNames() != 0)
		{
			m_pVideoTexture = matTextureMgr::CreateRenderTargetTexture( m_Width, m_Height, false, &l_RGBA_PFD, false, false );
			m_pVideoTextureTemp = matTextureMgr::CreateRenderTargetTexture( m_Width, m_Height, false, &l_RGB_PFD, false, false );

			// give the texture to our billboard.
			// With video textures, we let the billboard own and release the texture
			// because render target textures are not reference counted.
			m_pBillboardProxy->SetTexture(m_pVideoTexture);
		}
		else
		{
			m_pVideoTexture = NULL;
			if (m_pVideoTextureTemp)
			{
				matTextureMgr::ReleaseTexture(m_pVideoTextureTemp);
			}
			m_pVideoTextureTemp = NULL;
		}
	}

	if (m_pVideoTexture && m_pVideoTextureTemp && (data.m_Frame != m_Frame))
	{
		m_Frame = data.m_Frame;
		unsigned char* pData = NULL;
		int dSize = 0;
		mnmAviUtil::GetAviFrame( (mnmAviUtil::HAVI)data.m_pVideo, m_Frame, pData, dSize );
		//matTextureMgr::FillTexture( m_pVideoTexture, pData, dSize );
		matTextureMgr::FillTexture( m_pVideoTextureTemp, pData, dSize );
		matTextureMgr::CopyTexture( m_pVideoTextureTemp, m_pVideoTexture);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void billBillboardObject::VisibleChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	m_pBillboardProxy->SetRenderable(m_Data.m_bVisible.GetValue() 
		&& m_Data.m_bEditorVisible.GetValue() 
		&& m_bLayerVisible);
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void billBillboardObject::TransformChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// This callback is triggered from position, orientation or scale changes

	m_pBillboardProxy->SetPosition( m_Data.m_Position.GetValue() );
	m_pBillboardProxy->SetOrientation(m_Data.m_Orientation.GetQuaternion());

	m_pBillboardProxy->SetScale( m_Data.m_NonUniformScale.GetValue() );

	update_world_box();

	if (i_bDirty)
		billDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void billBillboardObject::ScaleChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// This callback is triggered from position, orientation or scale changes

	m_pBillboardProxy->SetPosition( m_Data.m_Position.GetValue() );
	m_pBillboardProxy->SetOrientation(m_Data.m_Orientation.GetQuaternion());

	m_pBillboardProxy->SetScale( m_Data.m_NonUniformScale.GetValue() );

	update_world_box();

	if (i_bDirty)
		billDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void billBillboardObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );

	if (i_bDirty)
	{
		billDialogDataUtil::UpdateListDialog();
		billDocumentChunk::ActiveDataChanged();
	}
}
void billBillboardObject::BillboardChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	m_pBillboardProxy->SetOrientToCamera(m_Data.m_bOrientToCamera.GetValue());
	m_pBillboardProxy->SetAdditiveMaterial(m_Data.m_bAdditiveMaterial.GetValue());
	m_pBillboardProxy->SetColor(m_Data.m_Color.GetValue());
	m_pBillboardProxy->SetBrightness(m_Data.m_Brightness.GetValue());

	m_pBillboardProxy->SetCKActive(m_Data.m_bCKActive.GetValue());
	m_pBillboardProxy->SetCKColor(m_Data.m_CKColor.GetValue());
	m_pBillboardProxy->SetCKTolerance(m_Data.m_CKTolerance.GetValue());
	m_pBillboardProxy->SetCKRemoveSpill(m_Data.m_bCKRemoveSpill.GetValue());
	m_pBillboardProxy->SetCKSpillType(m_Data.m_CKSpillType.GetValue());
	m_pBillboardProxy->SetCKSpillBias(m_Data.m_CKSpillBias.GetValue());
	m_pBillboardProxy->SetCKEdgeBlur(m_Data.m_CKEdgeBlur.GetValue());
	

	if (i_bDirty)
		billDocumentChunk::ActiveDataChanged();
}

void billBillboardObject::SnapToCameraChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	m_pBillboardProxy->SetSnapToCamera(m_Data.m_bSnapToCamera.GetValue());
	m_pBillboardProxy->SetDistToCamera(m_Data.m_DistToCamera.GetValue());
	SetCamera(m_Data.m_CameraIndex.GetValue());

	if (i_bDirty)
		billDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void billBillboardObject::SetCamera(int i_Index)
{
	// select the camera to pass in
	if (m_CameraList.size() == 0)
		return;

	//DBG_ASSERT(i_Index >= 0 && i_Index < m_CameraList.size(), "billboard camera object out of list");
	if (i_Index < 0 || i_Index >= m_CameraList.size())
		return;

	m_pBillboardProxy->SetCamera(m_CameraList[i_Index]);
}