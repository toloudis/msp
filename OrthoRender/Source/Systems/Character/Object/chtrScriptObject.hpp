/*****************************************************************************
**  chtrScriptObject.hpp
**
**      A chtrScriptObject is a derived class for displaying an object's position.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_SCRIPTOBJECT_HPP
#error chtrScriptObject.hpp multiply included
#endif
#define CHTR_SCRIPTOBJECT_HPP

#ifndef CHTR_SCRIPTDATA_HPP
#include "Systems/Character/Data/chtrScriptData.hpp"
#endif
#ifndef CHTR_OBJECT_HPP
#include "Systems/Character/Object/chtrObject.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef DYN_SCRIPTOBJECT_HPP
#include "Support/dyn/dynScriptObject.hpp"
#endif
#ifndef FGMT_SCRIPTOBJECT_HPP
#include "Support/fgmt/fgmtScriptObject.hpp"
#endif
#ifndef MTRL_SCRIPTOBJECT_HPP
#include "Support/mtrl/mtrlScriptObject.hpp"
#endif
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef TMLN_SCRIPTOBJECT_HPP
#include "Support/tmln/tmlnScriptObject.hpp"
#endif
#ifndef LYER_OBJECT_HPP
#include "Support/lyer/lyerObject.hpp"
#endif

#include <map>


//============================================================================
//	forward references
//============================================================================
class api3dObject;
class api3dObjectEntity;
class tmlnChannelAnimationFull;
class chtrChannelAnimationSub;
class chtrExpressionObject;
class chtrAdapterGetPosition;
class smdlSubdivCharacter;
class tmlnChannel;
class tmlnChannelBoolean;
class tmlnChannelFloat;
class tmlnChannelOrientation;
class tmlnChannelPosition;
class tmlnChannelRangedFloat;


//============================================================================
//============================================================================
class chtrScriptObject : public pick3dPickObject, 
						 public dynScriptObject,
						 public mtrlScriptObject,
						 public fgmtScriptObject,
						 public lyerObject
{
	public:
		//--------------------------------------------------------------------
		// ownership for the api3dObjectEntity passes to this object
		//--------------------------------------------------------------------
		chtrScriptObject(	api3dObjectEntity* i_pObject, 
							chtrExpressionObject* i_pExpressions, 
							const fsLocator& i_ModelFile, 
							const fsLocator& i_AssetDir );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~chtrScriptObject();

		
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

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		//virtual std::string GetPick3dName() const;


	//============================================================================
	//	members
	//============================================================================

		//--------------------------------------------------------------------
		//	Name - simply pass functions to icon object
		//--------------------------------------------------------------------
		void SetName(const nameString& i_Name);
		const nameString& GetName() const;

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the chtrScriptObject.
		//	If it does, the t value is also returned in o_T.
		//--------------------------------------------------------------------
		bool RayPick(	const maPoint3d& i_RayStart,
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
		void  SetEditorVisible(bool i_bVisible);

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
		//	LayerLowRes represents if the objects are rendered
		//	using a low resolution model.
		//--------------------------------------------------------------------
		virtual void SetLayerLowRes(bool i_bLowRes);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chtrObject*	GetPickObject() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const chtrExpressionObject*	GetExpressionObject() const;
		chtrExpressionObject* ExpressionObject();

		//--------------------------------------------------------------------
		//	Add an expression
		//--------------------------------------------------------------------
		void AddSingleExpression( const std::string& i_ExpressionName, const itString& i_ExpressionAnimFileName );
		void AddDualExpression( const std::string& i_ExpressionName, const itString& i_ExpressionAnimFileNameLeft, const itString& i_ExpressionAnimFileNameRight );
		void AddQuadExpression( const std::string& i_ExpressionName, const itString& i_ExpressionAnimFileNameLeft, const itString& i_ExpressionAnimFileNameRight, const itString& i_ExpressionAnimFileNameUp, const itString& i_ExpressionAnimFileNameDown );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void DeleteExpression( const std::string& i_ExpressionName );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int GetNumExpressions() const;

		//--------------------------------------------------------------------
		//	filename
		//--------------------------------------------------------------------
		void GetFilename( itString& o_Filename ) const;

		//--------------------------------------------------------------------
		//	Directory
		//--------------------------------------------------------------------
		void SetDirectory( const fsLocator& i_Dir );
		const fsLocator& GetDirectory() const;

		//--------------------------------------------------------------------
		// Set the weight for the expression with the given name
		//--------------------------------------------------------------------
		//void SetExpressionWeight(const std::string& i_Name, float i_Weight);

		//--------------------------------------------------------------------
		// Return the channel for the expression with the given name
		//--------------------------------------------------------------------
		tmlnChannelRangedFloat* GetExpressionChannel(const std::string& i_Name);

		//--------------------------------------------------------------------
		// If this channel is an expression channel, return true and
		//	set o_Name to the name of the expression.
		//--------------------------------------------------------------------
		bool GetNameFromExpressionChannel(tmlnChannelRangedFloat* i_pChannel,
								  std::string& o_Name);

		//--------------------------------------------------------------------
		// This is called when a driver in this script object is changed,
		//	or if a driver is added or deleted from this object.
		//--------------------------------------------------------------------
		virtual void NotifyDriverChanged();

		//--------------------------------------------------------------------
		// OverrideMaterials is used by the user interface. It just calls 
		// GatherMaterials; but since it is virtual, it can also set a 
		// dirty bit on the chunk and update the object part interface.
		//--------------------------------------------------------------------
		virtual void OverrideMaterials();

		//--------------------------------------------------------------------
		//	Lock the materials
		//--------------------------------------------------------------------
		virtual void LockMaterials(const bool i_bLock);

		//--------------------------------------------------------------------
		// Set subdivision level being used.
		//--------------------------------------------------------------------
		void SetSubdivLevel(int i_SubdivLevel);


	//============================================================================
	//	Data
	//============================================================================

		//--------------------------------------------------------------------
		// Get values as a data structure
		//--------------------------------------------------------------------
		chtrScriptData GetScriptData() const;

		//--------------------------------------------------------------------
		// Set from data structure
		//--------------------------------------------------------------------
		void SetScriptData(const chtrScriptData &i_Data);

		//--------------------------------------------------------------------
		// Get values as a base data structure
		//--------------------------------------------------------------------
		chtrData GetBaseData() const;

		//--------------------------------------------------------------------
		// Set from base data structure
		//--------------------------------------------------------------------
		void SetBaseData(const chtrData &i_Data);


	//============================================================================
	//	Parts
	//============================================================================

		//--------------------------------------------------------------------
		//	Select object part, like surface or material
		//--------------------------------------------------------------------
		void SelectObjectPart(const std::string i_PartName, 
							  const std::string i_CategoryName, 
							  bool i_bAppend);	

		//--------------------------------------------------------------------
		//	ActivateObjectPart from Placed, represents a double-click
		//		on a part item in the placed menu.
		//--------------------------------------------------------------------
		void ActivateObjectPart(const std::string i_PartName, 
								const std::string i_CategoryName);

		//--------------------------------------------------------------------
		//	DeleteObjectPart from Placed, represents DELETE key or button
		//		when a part item is selected in the placed menu.
		//--------------------------------------------------------------------
		void DeleteObjectPart(const std::string i_PartName, 
							  const std::string i_CategoryName);

	//============================================================================
	//	Timeline
	//============================================================================

		//--------------------------------------------------------------------
		// Return adapter to access point light's position
		//--------------------------------------------------------------------
		chtrAdapterGetPosition& AdapterGetPosition();

		//--------------------------------------------------------------------
		// Accessors to channels
		//--------------------------------------------------------------------
		tmlnChannelAnimationFull& ChannelAnimationFull();
		chtrChannelAnimationSub& ChannelAnimationSub();
		tmlnChannelPosition& ChannelPosition();
		tmlnChannelOrientation& ChannelOrientation();
		tmlnChannelFloat& ChannelScale();
		tmlnChannelBoolean& ChannelVisible();
		tmlnChannel& ChannelSound();

		// TODO: add more channels

	protected:
		//--------------------------------------------------------------------
		// This virtual function is called when the materials are saved 
		//	to a new filename. The locator representing this geometry
		//	should now point to the new filename.
		//--------------------------------------------------------------------
		virtual void NotifyLocatorChanged(const fsLocator& i_NewLocator);

	private:
		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void NameChanged(prtyProperty *i_pProperty, bool i_bDirty);

	private:
		chtrObject*					m_pIcon;	// Manipulation object for the compass
		chtrExpressionObject*		m_pExpressions;

		fsLocator					m_Directory;

		bool						m_bShowDriverIcons;
		bool						m_bSelected;

		tmlnChannelAnimationFull*	m_pChannelAnimationFull;
		chtrChannelAnimationSub*	m_pChannelAnimationSub;
		tmlnChannelOrientation*		m_pChannelOrientation;
		tmlnChannelPosition*		m_pChannelPosition;
		tmlnChannelFloat*			m_pChannelScale;
		tmlnChannelBoolean*			m_pChannelVisible;
		tmlnChannel*				m_pChannelSound;

		chtrAdapterGetPosition*		m_pAdapterGetPosition;
};

