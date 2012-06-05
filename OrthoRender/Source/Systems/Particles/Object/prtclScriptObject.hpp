/*****************************************************************************
**  prtclScriptObject.hpp
**
**      A prtclScriptObject is a derived class for displaying an object's position.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRTCL_SCRIPTOBJECT_HPP
#error prtclScriptObject.hpp multiply included
#endif
#define PRTCL_SCRIPTOBJECT_HPP

#ifndef PRTCL_SCRIPTDATA_HPP
#include "Systems/Particles/Data/prtclScriptData.hpp"
#endif

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif
#ifndef TMLN_SCRIPTOBJECT_HPP
#include "Support/tmln/tmlnScriptObject.hpp"
#endif
#ifndef LYER_OBJECT_HPP
#include "Support/lyer/lyerObject.hpp"
#endif

#include <string>


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class api3dParticleGenerator;
class prtParticleGenerator;
class prtParticleGeneratorTemplate;
class prtclObject;
class prtclChannelAnimation;
class prtclChannelEmit;
class tmlnChannelBoolean;
class tmlnChannelFileName;
class tmlnChannelFloat;
class tmlnChannelOrientation;
class tmlnChannelPosition;


//============================================================================
//============================================================================
class prtclScriptObject : public pick3dPickObject, 
						  public tmlnScriptObject,
						  public lyerObject
{
	public:
		//--------------------------------------------------------------------
		// Ownership for the generator and the template pass to this object.
		//--------------------------------------------------------------------
		prtclScriptObject(	prtParticleGenerator* i_pGenerator, 
							prtParticleGeneratorTemplate* i_pTemplate );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtclScriptObject();

		//--------------------------------------------------------------------
		// Clear out already created particles
		//--------------------------------------------------------------------
		void ClearParticles();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Pause(bool i_bPause);

	//============================================================================
	//	Overloads
	//============================================================================

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetTmlnName() const;

		//--------------------------------------------------------------------
		//  get a list of resources.  the resources will be appended to the
		//	passed in list.
		//--------------------------------------------------------------------
		virtual void GetResourceList( fsResourceTrackerData& io_List );


	//============================================================================
	//	members
	//============================================================================

		//--------------------------------------------------------------------
		//	Name - simply pass functions to icon object
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);
		const nameString& GetName() const;

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the prtclScriptObject.
		//	If it does, the t value is also returned in o_T.
		//--------------------------------------------------------------------
		virtual bool RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T);

		//--------------------------------------------------------------------
		//	ShowIcons sets whether the driver icons are visible
		//--------------------------------------------------------------------
		void ShowIcons(bool i_bVisible);

		//--------------------------------------------------------------------
		//	Set whether this object is selected in order to control
		//	display of icons or render style, etc.
		//--------------------------------------------------------------------
		void SetSelected(bool i_bSelected);

		//--------------------------------------------------------------------
		//  Changes visible state of character
		//--------------------------------------------------------------------
		void SetEditorVisible(bool i_bVisible);

		//--------------------------------------------------------------------
		//	LayerVisible represents if objects in the layer are visible
		//--------------------------------------------------------------------
		virtual void SetLayerVisible(bool i_bVisible);

		//--------------------------------------------------------------------
		//	LayerPickable represents if objects in the layer can be picked.
		//--------------------------------------------------------------------
		virtual void SetLayerPickable(bool i_bPickable);

		//--------------------------------------------------------------------
		//	LayerWireframe represents if the objects are rendered
		//	in a wireframe style.
		//--------------------------------------------------------------------
		virtual void SetLayerWireframe(bool i_bWireframe);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtclObject* GetPickObject() const;

		//--------------------------------------------------------------------
		// This is called when a driver in this script object is changed,
		//	or if a driver is added or deleted from this object.
		//--------------------------------------------------------------------
		virtual void NotifyDriverChanged();


	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a data structure
		//--------------------------------------------------------------------
		prtclScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetScriptData(const prtclScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a base data structure
		//--------------------------------------------------------------------
		prtclData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const prtclData &i_Data);

		//--------------------------------------------------------------------
		//	Update the data with the generator and template
		//--------------------------------------------------------------------
		void UpdateData();


	//============================================================================
	//	Timeline
	//============================================================================

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		prtclChannelEmit& ChannelEmit();
		prtclChannelAnimation& ChannelAnimation();
		tmlnChannelPosition& ChannelPos();
		tmlnChannelOrientation& OrientationChannel();
		tmlnChannelFloat& RateChannel();		// base generator
		tmlnChannelFloat& MaxParticlesChannel();
		tmlnChannelFloat& LifetimeMinChannel();
		tmlnChannelFloat& LifetimeMaxChannel();
		tmlnChannelFloat& PreSimTimeChannel();
		tmlnChannelFloat& ScaleStartChannel();	// sprite generator
		tmlnChannelFloat& ScaleCoefficientChannel();
		tmlnChannelFloat& StartAngleMinChannel();
		tmlnChannelFloat& StartAngleMaxChannel();
		tmlnChannelFloat& AngularVelocityMaxChannel();
		tmlnChannelFloat& AngularVelocityMinChannel();
		tmlnChannelFloat& AngularAccelerationMaxChannel();
		tmlnChannelFloat& AngularAccelerationMinChannel();
		tmlnChannelFloat& EmitterScaleChannel();
		tmlnChannelFloat& ConeAngleChannel();	// prtConeParticleGenerator
		tmlnChannelFloat& MinSpeedChannel();
		tmlnChannelFloat& MaxSpeedChannel();
		tmlnChannelFloat& AccelerationXChannel();
		tmlnChannelFloat& AccelerationYChannel();
		tmlnChannelFloat& AccelerationZChannel();
		tmlnChannelFloat& MinEmitSpeedChannel();	// prtSpiralParticleGenerator
		tmlnChannelFloat& MaxEmitSpeedChannel();
		tmlnChannelFloat& EmitDirectionXChannel();
		tmlnChannelFloat& EmitDirectionYChannel();
		tmlnChannelFloat& EmitDirectionZChannel();
		tmlnChannelFloat& MinRotStartAngleChannel();
		tmlnChannelFloat& MaxRotStartAngleChannel();
		tmlnChannelFloat& MinRotAngularVelChannel();
		tmlnChannelFloat& MaxRotAngularVelChannel();
		tmlnChannelFloat& RotRadiusChannel();
		tmlnChannelFloat& RotRadiusScaleRateChannel();
		tmlnChannelFloat& TextureAlphaStartChannel();
		tmlnChannelFloat& TextureAlphaMiddleChannel();
		tmlnChannelFloat& TextureAlphaEndChannel();
		tmlnChannelFloat& TextureAlphaMiddlePercentStartChannel();
		tmlnChannelFloat& TextureAlphaMiddlePercentEndChannel();
		tmlnChannelFileName& TextureFileNameChannel();
		tmlnChannelBoolean& RenderStreaksChannel();
		tmlnChannelFloat& StreakLengthChannel();
		tmlnChannelFloat& StreakTaperChannel();
		tmlnChannelFloat& StreakFadeChannel();
		tmlnChannelBoolean&	ShowInCubeReflectionsChannel();
		tmlnChannelBoolean&	ShowInPlanarReflectionsChannel();
		tmlnChannelBoolean&	CastShadowsChannel();
		tmlnChannelBoolean&	UseDitheredShadowsChannel();
		tmlnChannelFloat& ShadowDitherBiasChannel();
		tmlnChannelBoolean&	AdditiveChannel();

	private:
	//--------------------------------------------------------------------
	// Callbacks for when properties change
	//--------------------------------------------------------------------

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtclObject*		m_pIcon;	// Manipulation object for the compass

		prtclChannelEmit*		m_pChannelEmit;
		prtclChannelAnimation*	m_pChannelAnimation;
		tmlnChannelPosition*	m_pChannelPos;
		tmlnChannelOrientation*	m_pOrientationChannel;
		tmlnChannelFloat*		m_pRateChannel;	// base generator
		tmlnChannelFloat*		m_pMaxParticlesChannel;
		tmlnChannelFloat*		m_pLifetimeMinChannel;
		tmlnChannelFloat*		m_pLifetimeMaxChannel;
		tmlnChannelFloat*		m_pScaleStartChannel;	// sprite generator
		tmlnChannelFloat*		m_pScaleCoefficientChannel;
		tmlnChannelFloat*		m_pStartAngleMinChannel;
		tmlnChannelFloat*		m_pStartAngleMaxChannel;
		tmlnChannelFloat*		m_pAngularVelocityMaxChannel;
		tmlnChannelFloat*		m_pAngularVelocityMinChannel;
		tmlnChannelFloat*		m_pAngularAccelerationMaxChannel;
		tmlnChannelFloat*		m_pAngularAccelerationMinChannel;
		tmlnChannelFloat*		m_pEmitterScaleChannel;
		tmlnChannelFloat*		m_pConeAngleChannel;	// prtConeParticleGenerator
		tmlnChannelFloat*		m_pMinSpeedChannel;
		tmlnChannelFloat*		m_pMaxSpeedChannel;
		tmlnChannelFloat*		m_pAccelerationXChannel;
		tmlnChannelFloat*		m_pAccelerationYChannel;
		tmlnChannelFloat*		m_pAccelerationZChannel;
		tmlnChannelFloat*		m_pMinEmitSpeedChannel;	// prtSpiralParticleGenerator
		tmlnChannelFloat*		m_pMaxEmitSpeedChannel;
		tmlnChannelFloat*		m_pEmitDirectionXChannel;
		tmlnChannelFloat*		m_pEmitDirectionYChannel;
		tmlnChannelFloat*		m_pEmitDirectionZChannel;
		tmlnChannelFloat*		m_pMinRotStartAngleChannel;
		tmlnChannelFloat*		m_pMaxRotStartAngleChannel;
		tmlnChannelFloat*		m_pMinRotAngularVelChannel;
		tmlnChannelFloat*		m_pMaxRotAngularVelChannel;
		tmlnChannelFloat*		m_pPreSimTimeChannel;
		tmlnChannelFloat*		m_pRotRadiusChannel;
		tmlnChannelFloat*		m_pRotRadiusScaleRateChannel;
		tmlnChannelFloat*		m_pTextureAlphaStartChannel;
		tmlnChannelFloat*		m_pTextureAlphaMiddleChannel;
		tmlnChannelFloat*		m_pTextureAlphaEndChannel;
		tmlnChannelFloat*		m_pTextureAlphaMiddlePercentStartChannel;
		tmlnChannelFloat*		m_pTextureAlphaMiddlePercentEndChannel;
		tmlnChannelFileName*	m_pTextureFilenameChannel;
		tmlnChannelBoolean*		m_pRenderStreaksChannel;
		tmlnChannelFloat*		m_pStreakLengthChannel;
		tmlnChannelFloat*		m_pStreakTaperChannel;
		tmlnChannelFloat*		m_pStreakFadeChannel;
		tmlnChannelBoolean*		m_pShowInCubeReflectionsChannel;
		tmlnChannelBoolean*		m_pShowInPlanarReflectionsChannel;
		tmlnChannelBoolean*		m_pCastShadowsChannel;
		tmlnChannelBoolean*		m_pUseDitheredShadowsChannel;
		tmlnChannelFloat*		m_pShadowDitherBiasChannel;
		tmlnChannelBoolean*		m_pAdditiveChannel;

		bool				m_bShowDriverIcons;
		bool				m_bSelected;
};

