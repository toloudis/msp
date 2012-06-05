/*****************************************************************************
**  sbrdBillboardObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/Object/sbrdBillboardObject.hpp"

#include "Systems/Storyboards/GUI/sbrdDialogDataUtil.hpp"
#include "Systems/Storyboards/Data/sbrdDocumentChunk.hpp"
#include "Systems/Storyboards/GUI/sbrdGeomList.hpp"
#include "Systems/Storyboards/Undo/sbrdOperations.hpp"

#include "Support/cmps/cmpsReference.hpp"
#include "Support/mnm/mnmReferencePosChannel.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"

// library
#include "Tool/api3d/api3dBillboard.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"


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
	const float l_fInitialScale = 16.0f;
}


//----------------------------------------------------------------------------
// This object takes ownership of the arguments passed in
//----------------------------------------------------------------------------
sbrdBillboardObject::sbrdBillboardObject(	api3dBillboard * i_pBillboard,
											pick3dPickObject* i_pParent )
:	m_pBillboard(i_pBillboard),
	m_pTexture(NULL),
	m_WorldBox(l_SphereBox),
	m_pParent( i_pParent ),
	m_bLayerVisible(false)
{
	m_Data.m_bEditorVisible = false;

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Position), "Transform", "Position of the object");
	AddProperty( pPUII );
	//pPUII  = new prtyVector3dEditUpDownUIInfo(&(m_Data.m_Orientation), "category2", "Orientation of the object");
	//AddProperty( pPUII );
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Name), "Asset", "Name of the object");
	AddProperty( pPUII );
	pPUII = new prtyTextBoxUIInfo(&(m_Data.m_Filename), "Asset", "File Name of the object");
	pPUII->SetReadOnly(true);
	AddProperty( pPUII );
	// Editor visible should not be exposed in regular properties GUI
	//AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bEditorVisible), "Editor", "Is the object visible in the editor") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bOrientToCamera), "General", "Is the object oriented to the camera") );
	AddProperty( new prtyCheckBoxUIInfo(&(m_Data.m_bAdditiveMaterial), "General", "Does the object have additive material") );
	pPUII = new prtyNumericUpDownUIInfo(&(m_Data.m_Scale), "Transform", "Scale of the object");
	AddProperty( pPUII );
	pPUII = new prtyColorRGBAEditUIInfo(&(m_Data.m_Color), "General", "Color of the object");
	AddProperty( pPUII );

	// Register callbacks to update when properties change
	m_Data.m_Filename.AddCallback(new prtyCallbackWrapper<sbrdBillboardObject>(this, &sbrdBillboardObject::TextureChanged));
	m_Data.m_Position.AddCallback(new prtyCallbackWrapper<sbrdBillboardObject>(this, &sbrdBillboardObject::TransformChanged));
	m_Data.m_Orientation.AddCallback(new prtyCallbackWrapper<sbrdBillboardObject>(this, &sbrdBillboardObject::TransformChanged));
	m_Data.m_Scale.AddCallback(new prtyCallbackWrapper<sbrdBillboardObject>(this, &sbrdBillboardObject::TransformChanged));
	m_Data.m_Name.AddCallback(new prtyCallbackWrapper<sbrdBillboardObject>(this, &sbrdBillboardObject::NameChanged));
	m_Data.m_bOrientToCamera.AddCallback(new prtyCallbackWrapper<sbrdBillboardObject>(this, &sbrdBillboardObject::BillboardChanged));
	m_Data.m_bAdditiveMaterial.AddCallback(new prtyCallbackWrapper<sbrdBillboardObject>(this, &sbrdBillboardObject::BillboardChanged));
	m_Data.m_Color.AddCallback(new prtyCallbackWrapper<sbrdBillboardObject>(this, &sbrdBillboardObject::BillboardChanged));
	m_Data.m_bVisible.AddCallback(new prtyCallbackWrapper<sbrdBillboardObject>(this, &sbrdBillboardObject::VisibleChanged));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdBillboardObject::~sbrdBillboardObject()
{
	if ( m_pBillboard )
	{
		api3dScene::RemoveObject(m_pBillboard);
		delete m_pBillboard;
	}
	if ( m_pTexture )
	{
		matTextureMgr::ReleaseTexture(m_pTexture);
	}
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string sbrdBillboardObject::GetPick3dName() const
{
	return GetName().GetString();
}


//============================================================================
//	Data
//============================================================================


//--------------------------------------------------------------------
// Get values as a data structure
//--------------------------------------------------------------------
const sbrdObjectData& sbrdBillboardObject::GetData() const
{
	return this->m_Data;
}

//--------------------------------------------------------------------
// Set from data structure
//--------------------------------------------------------------------
void sbrdBillboardObject::SetData(const sbrdObjectData &i_Data)
{
	m_Data = i_Data;
}

//--------------------------------------------------------------------
// Name property access
//--------------------------------------------------------------------
prtyName&	sbrdBillboardObject::PropertyName()
{
	return m_Data.m_Name;
}
const prtyName&	sbrdBillboardObject::GetPropertyName() const
{
	return m_Data.m_Name;
}

//--------------------------------------------------------------------
// Position property access
//--------------------------------------------------------------------
prtyPoint3d&	sbrdBillboardObject::PropertyPosition()
{
	return m_Data.m_Position;
}
const prtyPoint3d&	sbrdBillboardObject::GetPropertyPosition() const
{
	return m_Data.m_Position;
}

//--------------------------------------------------------------------
// Orientation property access
//--------------------------------------------------------------------
prtyRotation&	sbrdBillboardObject::PropertyOrientation()
{
	return m_Data.m_Orientation;
}
const prtyRotation&	sbrdBillboardObject::GetPropertyOrientation() const
{
	return m_Data.m_Orientation;
}

//--------------------------------------------------------------------
// Color property access
//--------------------------------------------------------------------
prtyColor&	sbrdBillboardObject::PropertyColor()
{
	return m_Data.m_Color;
}
const prtyColor&	sbrdBillboardObject::GetPropertyColor() const
{
	return m_Data.m_Color;
}

//--------------------------------------------------------------------
// Scale property access
//--------------------------------------------------------------------
prtyFloat& sbrdBillboardObject::PropertyScale()
{
	return m_Data.m_Scale;
}
const prtyFloat& sbrdBillboardObject::GetPropertyScale() const
{
	return m_Data.m_Scale;
}

//--------------------------------------------------------------------
// Visible property access
//--------------------------------------------------------------------
prtyBoolean&	sbrdBillboardObject::PropertyVisible()
{
	return m_Data.m_bVisible;
}
const prtyBoolean&	sbrdBillboardObject::GetPropertyVisible() const
{
	return m_Data.m_bVisible;
}

//--------------------------------------------------------------------
// FileName property access
//--------------------------------------------------------------------
prtyFileName&	sbrdBillboardObject::PropertyFileName()
{
	return m_Data.m_Filename;
}
const prtyFileName&	sbrdBillboardObject::GetPropertyFileName() const
{
	return m_Data.m_Filename;
}

////--------------------------------------------------------------------
//// Maintains texture pointer based on filename
////--------------------------------------------------------------------
//void sbrdBillboardObject::SetTextureFilename( const itString& i_Filename )
//{
//	if (m_Data.m_Filename.GetValue() != i_Filename)
//	{
//		if (m_pTexture)
//		{
//			matTextureMgr::ReleaseTexture(m_pTexture);
//			m_pTexture = NULL;
//		}
//
//		m_Data.m_Filename.SetValue( i_Filename );
//
//		// Expand filename into full path
//		fsLocator tex_loc;
//		if (sbrdGeomList::FindFile(i_Filename, tex_loc))
//		{
//			matTexture *pTexture = matTextureMgr::LoadTexture(tex_loc);
//			m_pTexture = pTexture;
//			m_pBillboard->SetTexture(m_pTexture);
//		}
//	}
//}
//
////--------------------------------------------------------------------
//// Returns the texture pointer from the filename that was loaded.
//// This is the texture that is owned by this object (m_pTexture)
//// but may not be the texture currently on the billboard.
////--------------------------------------------------------------------
//matTexture* sbrdBillboardObject::GetBaseTexture()
//{
//	return m_pTexture;
//}
//
////--------------------------------------------------------------------
//// Changes the texture on the billboard without altering this
//// object's texture filename value or its "owned" base texture.
////--------------------------------------------------------------------
//void sbrdBillboardObject::SetBillboardTexture( matTexture* i_pTexture )
//{
//	if (m_pTexture != i_pTexture)
//	{
//		m_pTexture = i_pTexture;
//		this->CalculateScale( this->PropertyScale().GetValue() );
//	}
//	else
//	{
//		m_pTexture = i_pTexture;
//	}
//
//	m_pBillboard->SetTexture(i_pTexture);
//}

//----------------------------------------------------------------------------
//	GetWorldBox returns a box which completely encloses the sbrdBillboardObject.
//----------------------------------------------------------------------------
//virtual 
const maAxisBox& sbrdBillboardObject::GetWorldBox() const
{
	return m_WorldBox;
}

//----------------------------------------------------------------------------
//	GetLocalBox returns a box which would enclose the sbrdBillboardObject
//	if its transformations were identity
//----------------------------------------------------------------------------
//virtual 
const maAxisBox& sbrdBillboardObject::GetLocalBox() const
{
	return l_SphereBox;
}

//----------------------------------------------------------------------------
//	Position
//----------------------------------------------------------------------------
//virtual 
maPoint3d sbrdBillboardObject::GetPosition() const
{
	return m_Data.m_Position.GetValue();
}
//virtual 
void sbrdBillboardObject::UpdatePosition(const maPoint3d& i_Position, 
										bool i_bNewOperation)
{
	//sbrdOperations::ChangePosition( i_Position );
	m_Data.m_Position.SetValue(i_Position, 
		i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
}


//----------------------------------------------------------------------------
//	Orientation
//----------------------------------------------------------------------------
//virtual 
maRotation sbrdBillboardObject::GetOrientation() const
{
	return m_pBillboard->GetOrientation();
}
//virtual 
void sbrdBillboardObject::UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation)
{
	//sbrdOperations::ChangeOrientation( i_Orientation );
	m_Data.m_Orientation.SetQuaternion(i_Orientation, 
		i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
}

//----------------------------------------------------------------------------
//	Scale
//----------------------------------------------------------------------------
//virtual 
maPoint3d sbrdBillboardObject::GetScale() const
{
	return maPoint3d(m_Data.m_Scale.GetValue(),m_Data.m_Scale.GetValue(),m_Data.m_Scale.GetValue());
}
//virtual 
void sbrdBillboardObject::UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation)
{
	//sbrdOperations::ChangeScale( i_Scale );
	m_Data.m_Scale.SetValue(i_Scale.GetX(), 
		i_bNewOperation ? prtyProperty::eNewUndo : prtyProperty::eContinueUndo);
}

//--------------------------------------------------------------------
//	Name
//--------------------------------------------------------------------
//virtual 
void sbrdBillboardObject::SetName(const nameString& i_Name)
{
    // Set name first, which may change the ID number
    nameObject::SetName(i_Name);

    // Make sure that the data matches our true name (including
    // ID number)
    m_Data.m_Name = this->GetName();
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the sbrdBillboardObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool sbrdBillboardObject::RayPick(	const maPoint3d& i_RayStart,
									const maPoint3d& i_RayEnd,
									float& o_T)
{
	if (!m_pBillboard->GetRenderable())
		return false;

	return geoRayIntersection::IntersectLineSphere(	i_RayStart,
								i_RayEnd - i_RayStart,
								m_Data.m_Position.GetValue(),
								l_PickRadius,
								o_T);
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool sbrdBillboardObject::MatchPickCode(envType::UInt32 i_PickCode) const
{
	if (!m_pBillboard->GetRenderable())
		return false;

	return (m_pBillboard->ContainsPickCode(i_PickCode));
}

//--------------------------------------------------------------------
//  Changes visible state of prop based on GUI
//--------------------------------------------------------------------
void  sbrdBillboardObject::SetEditorVisible(bool i_bVisible)
{
	m_Data.m_bEditorVisible = i_bVisible;

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() 
						&& m_Data.m_bVisible.GetValue() 
						&& m_bLayerVisible;
	m_pBillboard->SetRenderable( bRenderable );
}
bool sbrdBillboardObject::GetEditorVisible() const
{
	return m_Data.m_bEditorVisible.GetValue();
}
 
//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
void sbrdBillboardObject::SetLayerVisible(bool i_bVisible)
{
	m_bLayerVisible = i_bVisible;

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	bool bRenderable = m_Data.m_bEditorVisible.GetValue() 
						&& m_Data.m_bVisible.GetValue() 
						&& m_bLayerVisible;
	m_pBillboard->SetRenderable(bRenderable);
}

//--------------------------------------------------------------------
//	Wireframe represents if the objects are rendered
//	in a wireframe style.
//--------------------------------------------------------------------
void sbrdBillboardObject::SetWireframe(bool i_bWireframe)
{
	m_pBillboard->SetWireframe(i_bWireframe);
}

//----------------------------------------------------------------------------
// Flags for how object can be modified
//----------------------------------------------------------------------------
mnmObject::RotateFlags	sbrdBillboardObject::GetRotateFlags()
{
	return (m_Data.m_bOrientToCamera.GetValue()) ? mnmObject::e_RotateNone : mnmObject::e_RotateAll;
}
mnmObject::ScaleFlags	sbrdBillboardObject::GetScaleFlags()
{
	return mnmObject::e_ScaleUniform;
}
mnmObject::TranslateFlags	sbrdBillboardObject::GetTranslateFlags()
{
	return mnmObject::e_TranslateAll;
}

//--------------------------------------------------------------------
//	using the ratio of the texture, set the scale of x+y
//--------------------------------------------------------------------
void sbrdBillboardObject::CalculateScale(float i_fScale)
{
	this->PropertyScale().SetValue(i_fScale);
}

//--------------------------------------------------------------------
//	Access to billboard object
//--------------------------------------------------------------------
api3dBillboard * sbrdBillboardObject::GetBillboardObject()
{
	return m_pBillboard;
}

//--------------------------------------------------------------------
// Get parent object of this object in order to define relationships
//	between icons and their affected objects.
//--------------------------------------------------------------------
//virtual 
pick3dPickObject* sbrdBillboardObject::GetParentObject() const
{
	return m_pParent;
}

//--------------------------------------------------------------------
//	Get reference for given name.  The returned pointer is owned
//	by this object.  The object should be retained by the caller
//	to avoid repeated string searches.
//--------------------------------------------------------------------
api3dReference* sbrdBillboardObject::GetReference(const char* i_Name)
{
	// Attachment
	api3dReference* pRef = new cmpsReference(*this);
	m_References.push_back(pRef);
	return pRef;
}

//--------------------------------------------------------------------
// Get list of references for possible attachment within this object.
//--------------------------------------------------------------------
void sbrdBillboardObject::GetReferenceList(std::vector<std::string> &o_List)
{
//	m_p3DObject->GetReferenceList(o_List);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void sbrdBillboardObject::update_world_box()
{
	float radius = l_SphereRadius * GetScale().m_X;
	m_WorldBox = maAxisBox(-radius, radius,
							-radius, radius,
							-radius, radius);
	m_WorldBox.Translate(this->GetPosition());
}

//--------------------------------------------------------------------
//	using the ratio of the texture, set the scale of x+y
//--------------------------------------------------------------------
void sbrdBillboardObject::calculate_scale()
{
	float scale = m_Data.m_Scale.GetValue();
	if ((m_pTexture != 0) && (m_pBillboard != 0))
	{
		float sy = (float)m_pTexture->GetHeight() / (float)m_pTexture->GetWidth();
		m_pBillboard->SetScale( maVector3d(scale, sy * scale, 1.0f) );
		//m_pBillboard->SetUniformScale( scale );
		update_world_box();
	}
	else
	{
		m_pBillboard->SetUniformScale( scale );
		update_world_box();
	}
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void sbrdBillboardObject::TextureChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	if ( m_pTexture )
	{
		matTextureMgr::ReleaseTexture(m_pTexture);
		m_pTexture = NULL;
	}

	if (m_Data.m_Filename.GetValue().GetLength() > 0)
	{
		fsLocator tex_loc;
		if (sbrdGeomList::FindFile(m_Data.m_Filename.GetValue(), tex_loc))
		{
			const bool c_bMIPMAP = false;
			m_pTexture = matTextureMgr::LoadTexture(tex_loc, TEXTURE_TYPE_2D, c_bMIPMAP);

			// give the texture to our billboard
			m_pBillboard->SetTexture(m_pTexture);
			this->calculate_scale();
		}
	}
}
void sbrdBillboardObject::TransformChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// This callback is triggered from position, orientation or scale changes

	m_pBillboard->SetPosition( m_Data.m_Position.GetValue() );
	m_pBillboard->SetOrientation(m_Data.m_Orientation.GetQuaternion());
	//m_pBillboard->SetUniformScale( m_Data.m_Scale.GetValue() );
	this->calculate_scale();

	update_world_box();

	if (i_bDirty)
		sbrdDocumentChunk::ActiveDataChanged();
}
void sbrdBillboardObject::NameChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	// The property's check will prevent an infinite
	// loop here.
	this->SetName( m_Data.m_Name.GetValue() );

	if (i_bDirty)
	{
		sbrdDialogDataUtil::UpdateListDialog();
		sbrdDocumentChunk::ActiveDataChanged();
	}
}
void sbrdBillboardObject::BillboardChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	m_pBillboard->SetOrientToCamera(m_Data.m_bOrientToCamera.GetValue());
	m_pBillboard->SetAdditiveMaterial(m_Data.m_bAdditiveMaterial.GetValue());
	m_pBillboard->SetColor(m_Data.m_Color.GetValue());

	if (i_bDirty)
		sbrdDocumentChunk::ActiveDataChanged();
}
void sbrdBillboardObject::VisibleChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	m_pBillboard->SetRenderable(m_Data.m_bVisible.GetValue() 
								&& m_Data.m_bEditorVisible.GetValue() 
								&& m_bLayerVisible);
}
