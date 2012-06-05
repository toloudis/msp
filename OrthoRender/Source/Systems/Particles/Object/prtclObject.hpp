/*****************************************************************************
**  prtclObject.hpp
**
**      A prtclObject is the selectable pick/icon object that connects 
**	prtclData to a particle generator and allows movement from the 
**	compass manips.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRTCL_OBJECT_HPP
#error prtclObject.hpp multiply included
#endif
#define PRTCL_OBJECT_HPP

#ifndef PRTCL_DATA_HPP
#include "Systems/Particles/Data/prtclData.hpp"
#endif

#ifndef API3D_SCALE_HPP
#include "Tool/api3d/api3dScale.hpp"
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
#ifndef PRT_PARTICLEGENERATORTEMPLATE_HPP
#include "Graphics/prt/prtParticleGeneratorTemplate.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class api3dParticleGenerator;
class prtParticleGenerator;
class prtyFileName;


//============================================================================
//============================================================================
class prtclObject : public mnmObject, 
					public nameObject, 
					public prtyObject,
					public api3dScaleInterest
{
	public:
		//--------------------------------------------------------------------
		// Ownership for the generator and the template pass to this object.
		//--------------------------------------------------------------------
		prtclObject(prtParticleGenerator* i_pGenerator, 
					prtParticleGeneratorTemplate* i_pTemplate );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtclObject();

		//--------------------------------------------------------------------
		// Get values as a data structure
		//--------------------------------------------------------------------
		const prtclData& GetData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetData(const prtclData &i_Data);

		//--------------------------------------------------------------------
		//	Update the data with the generator and template
		//--------------------------------------------------------------------
		void UpdateData();

		//--------------------------------------------------------------------
		// Clear out already created particles
		//--------------------------------------------------------------------
		void ClearParticles();

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetPick3dName() const;

		//--------------------------------------------------------------------
		//	GlobalScaleChanged - notification that global scale has changed.
		//--------------------------------------------------------------------
		virtual void GlobalScaleChanged( float i_Scale );

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
		// Rate property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyRate();
		const prtyFloat&	GetPropertyRate() const;

		//--------------------------------------------------------------------
		// MaxParticles property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyMaxParticles();
		const prtyFloat&	GetPropertyMaxParticles() const;

		//--------------------------------------------------------------------
		// LifetimeMin property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyLifetimeMin();
		const prtyFloat&	GetPropertyLifetimeMin() const;

		//--------------------------------------------------------------------
		// LifetimeMax property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyLifetimeMax();
		const prtyFloat&	GetPropertyLifetimeMax() const;

		//--------------------------------------------------------------------
		// ScaleStart property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyScaleStart();
		const prtyFloat&	GetPropertyScaleStart() const;

		//--------------------------------------------------------------------
		// ScaleCoefficient property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyScaleCoefficient();
		const prtyFloat&	GetPropertyScaleCoefficient() const;

		//--------------------------------------------------------------------
		// StartAngleMin property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyStartAngleMin();
		const prtyFloat&	GetPropertyStartAngleMin() const;

		//--------------------------------------------------------------------
		// StartAngleMax property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyStartAngleMax();
		const prtyFloat&	GetPropertyStartAngleMax() const;

		//--------------------------------------------------------------------
		// AngularVelocityMin property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyAngularVelocityMin();
		const prtyFloat&	GetPropertyAngularVelocityMin() const;

		//--------------------------------------------------------------------
		// AngularVelocityMax property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyAngularVelocityMax();
		const prtyFloat&	GetPropertyAngularVelocityMax() const;

		//--------------------------------------------------------------------
		// AngularAccelerationMin property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyAngularAccelerationMin();
		const prtyFloat&	GetPropertyAngularAccelerationMin() const;

		//--------------------------------------------------------------------
		// AngularAccelerationMax property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyAngularAccelerationMax();
		const prtyFloat&	GetPropertyAngularAccelerationMax() const;

		//--------------------------------------------------------------------
		// EmitterScale property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyEmitterScale();
		const prtyFloat&	GetPropertyEmitterScale() const;

		//--------------------------------------------------------------------
		// ConeAngle property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyConeAngle();
		const prtyFloat&	GetPropertyConeAngle() const;

		//--------------------------------------------------------------------
		// MinSpeed property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyMinSpeed();
		const prtyFloat&	GetPropertyMinSpeed() const;

		//--------------------------------------------------------------------
		// MaxSpeed property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyMaxSpeed();
		const prtyFloat&	GetPropertyMaxSpeed() const;

		//--------------------------------------------------------------------
		// AccelerationX property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyAccelerationX();
		const prtyFloat&	GetPropertyAccelerationX() const;

		//--------------------------------------------------------------------
		// AccelerationY property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyAccelerationY();
		const prtyFloat&	GetPropertyAccelerationY() const;

		//--------------------------------------------------------------------
		// AccelerationZ property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyAccelerationZ();
		const prtyFloat&	GetPropertyAccelerationZ() const;

		//--------------------------------------------------------------------
		// MinEmitSpeed property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyMinEmitSpeed();
		const prtyFloat&	GetPropertyMinEmitSpeed() const;

		//--------------------------------------------------------------------
		// MaxEmitSpeed property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyMaxEmitSpeed();
		const prtyFloat&	GetPropertyMaxEmitSpeed() const;

		//--------------------------------------------------------------------
		// EmitDirectionX property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyEmitDirectionX();
		const prtyFloat&	GetPropertyEmitDirectionX() const;

		//--------------------------------------------------------------------
		// EmitDirectionY property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyEmitDirectionY();
		const prtyFloat&	GetPropertyEmitDirectionY() const;

		//--------------------------------------------------------------------
		// EmitDirectionZ property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyEmitDirectionZ();
		const prtyFloat&	GetPropertyEmitDirectionZ() const;

		//--------------------------------------------------------------------
		// MinRotStartAngle property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyMinRotStartAngle();
		const prtyFloat&	GetPropertyMinRotStartAngle() const;

		//--------------------------------------------------------------------
		// MaxRotStartAngle property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyMaxRotStartAngle();
		const prtyFloat&	GetPropertyMaxRotStartAngle() const;

		//--------------------------------------------------------------------
		// MinRotAngularVel property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyMinRotAngularVel();
		const prtyFloat&	GetPropertyMinRotAngularVel() const;

		//--------------------------------------------------------------------
		// MaxRotAngularVel property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyMaxRotAngularVel();
		const prtyFloat&	GetPropertyMaxRotAngularVel() const;

		//--------------------------------------------------------------------
		// PreSimTime property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyPreSimTime();
		const prtyFloat&	GetPropertyPreSimTime() const;

		//--------------------------------------------------------------------
		// RotRadius property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyRotRadius();
		const prtyFloat&	GetPropertyRotRadius() const;

		//--------------------------------------------------------------------
		// RotRadiusScaleRate property access
		//--------------------------------------------------------------------
		prtyFloat&	PropertyRotRadiusScaleRate();
		const prtyFloat&	GetPropertyRotRadiusScaleRate() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFloat& PropertyTextureAlphaStart();
		const prtyFloat& GetPropertyTextureAlphaStart() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFloat& PropertyTextureAlphaMiddle();
		const prtyFloat& GetPropertyTextureAlphaMiddle() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFloat& PropertyTextureAlphaEnd();
		const prtyFloat& GetPropertyTextureAlphaEnd() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFloat& PropertyTextureAlphaMiddlePercentStart();
		const prtyFloat& GetPropertyTextureAlphaMiddlePercentStart() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFloat& PropertyTextureAlphaMiddlePercentEnd();
		const prtyFloat& GetPropertyTextureAlphaMiddlePercentEnd() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFileName& PropertyTextureFilename();
		const prtyFileName& GetPropertyTextureFilename() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyInt8& PropertyTextureRows();
		const prtyInt8& GetPropertyTextureRows() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyInt8& PropertyTextureCols();
		const prtyInt8& GetPropertyTextureCols() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyBoolean& PropertyTextureLooping();
		const prtyBoolean& GetPropertyTextureLooping() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyBoolean& PropertyTextureReverse();
		const prtyBoolean& GetPropertyTextureReverse() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFloat& PropertyTextureRate();
		const prtyFloat& GetPropertyTextureRate() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyBoolean& PropertyRenderStreaks();
		const prtyBoolean& GetPropertyRenderStreaks() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFloat& PropertyStreakLength();
		const prtyFloat& GetPropertyStreakLength() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFloat& PropertyStreakTaper();
		const prtyFloat& GetPropertyStreakTaper() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFloat& PropertyStreakFade();
		const prtyFloat& GetPropertyStreakFade() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyBoolean& PropertyShowInCubeReflections();
		const prtyBoolean& GetPropertyShowInCubeReflections() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyBoolean& PropertyShowInPlanarReflections();
		const prtyBoolean& GetPropertyShowInPlanarReflections() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyBoolean& PropertyCastShadows();
		const prtyBoolean& GetPropertyCastShadows() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyBoolean& PropertyUseDitheredShadows();
		const prtyBoolean& GetPropertyUseDitheredShadows() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFloat& PropertyShadowDitherBias();
		const prtyFloat& GetPropertyShadowDitherBias() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyBoolean& PropertyAdditive();
		const prtyBoolean& GetPropertyAdditive() const;

	//============================================================================
	//	members
	//============================================================================

		//--------------------------------------------------------------------
		//	GetWorldBox returns a box which completely encloses the
		//	prtclObject.
		//--------------------------------------------------------------------
		virtual const maAxisBox& GetWorldBox() const;

		//--------------------------------------------------------------------
		//	GetLocalBox returns a box which would enclose the prtclObject
		//	if its transformations were identity
		//--------------------------------------------------------------------
		virtual const maAxisBox& GetLocalBox() const;

		//--------------------------------------------------------------------
		// UpdateName() is called when the gui sets the name of the object,
		//	derived classes can set dirty bits and do "undo" operations, etc.
		// The default behavior calls SetName()
		//--------------------------------------------------------------------
		virtual void UpdateName(const std::string& i_Name);
		void SetName(const nameString& i_Name);

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
		//	RayPick returns true if the given ray intersects the ptltScriptObject.
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
		//  Changes visible state of particle based on GUI
		//--------------------------------------------------------------------
		void SetEditorVisible(bool i_bVisible);
		bool GetEditorVisible() const;

		//--------------------------------------------------------------------
		//	LayerVisible represents if objects in the layer are visible
		//--------------------------------------------------------------------
		void SetLayerVisible(bool i_bVisible);

		//--------------------------------------------------------------------
		//	GPUPickable sets whether the icons should be rendered
		//	in pick renders when doing GPU picking
		//--------------------------------------------------------------------
		void SetGPUPickable(bool i_Pickable);

		//--------------------------------------------------------------------
		//	Wireframe represents if the objects are rendered
		//	in a wireframe style.
		//--------------------------------------------------------------------
		void SetWireframe(bool i_bWireframe);

		//--------------------------------------------------------------------
		//	ShowIcons sets whether the debug shape is visible
		//--------------------------------------------------------------------
		void ShowIcons(bool i_bVisible);

		//--------------------------------------------------------------------
		//	GetDefaultTerrainOffset is the desired offset from
		//	the terrain for this object.  This can be altered
		//	by the user during placement
		//--------------------------------------------------------------------
		virtual float GetDefaultTerrainOffset() const;

		//--------------------------------------------------------------------
		// Flags for how object can be modified
		//--------------------------------------------------------------------
		virtual RotateFlags	GetRotateFlags();
		virtual ScaleFlags	GetScaleFlags();
		virtual TranslateFlags GetTranslateFlags();

		//--------------------------------------------------------------------
		//	Get reference for given name.  The returned pointer is owned
		//	by this object.  The object should be retained by the caller
		//	to avoid repeated string searches.
		//--------------------------------------------------------------------
		//api3dReference* GetReference(const char* i_Name);

		//--------------------------------------------------------------------
		// Get list of references for possible attachment within this object.
		//--------------------------------------------------------------------
		void GetReferenceList(std::vector<std::string> &o_List);

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		virtual pick3dPickObject* GetParentObject() const;
		virtual void SetParentObject(pick3dPickObject* i_pParent);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Pause(bool i_bPause);

		//--------------------------------------------------------------------
		//	Type of Generator
		//--------------------------------------------------------------------
		prtParticleGeneratorTemplate::Type GetGeneratorType();
		prtParticleGeneratorTemplate::EmitterType GetEmitterType();

	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void PositionChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void OrientationChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);

		void EmitterDataChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void StreakDataChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void BaseGeneratorDataChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void SpriteGeneratorDataChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void ConeGeneratorDataChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void SpiralGeneratorDataChanged(prtyProperty *i_pProperty, bool i_bDirty);

		void TextureAlphaDataChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void TextureFilenameDataChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void TextureDataChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void RenderDataChanged(prtyProperty *i_pProperty, bool i_bDirty);

		api3dParticleGenerator*			m_pObject;
		prtParticleGenerator*			m_pGenerator;
		prtParticleGeneratorTemplate*	m_pGeneratorTemplate;
		pick3dPickObject*				m_pParent;

		api3dObject*	m_pDebugObject;
		bool			m_bShowIcons;
		bool			m_bLayerVisible;

		mutable maAxisBox	m_WorldBox;
		//fsLocator		m_Directory;

		prtclData		m_Data;
};
