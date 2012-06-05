/*****************************************************************************
**	scObject.hpp
**
**		scObject defines a base class scObject, which represents an object
**	in a 3d scene.  The scObjects provide a bounding box and a "Renderable"
**	flag.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef SC_OBJECT_HPP
#error scObject.hpp multiply included
#endif
#define SC_OBJECT_HPP

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class g3dFragment;
class g3dSceneNode;
class scControlAnim;


//============================================================================
//============================================================================
class scObject
{
	public:
		//--------------------------------------------------------------------
		//  Constructor - default
		//--------------------------------------------------------------------
		scObject();

		//--------------------------------------------------------------------
		//  Constructor - sets the fragment of the base scene node
		//	Does not own fragment
		//--------------------------------------------------------------------
		scObject( g3dFragment* i_pFragment );

		//--------------------------------------------------------------------
		//  Constructor - uses given node as root node, takes ownership.
		// The given node should not have any custom settings yet, the 
		// scObject will overwrite its transform.
		//--------------------------------------------------------------------
		scObject( g3dSceneNode* i_pRootNode );

		//--------------------------------------------------------------------
		//	Destructor - removes its scene node from its parent before
		//	destroying scene node
		//--------------------------------------------------------------------
		virtual ~scObject();

		//--------------------------------------------------------------------
		//	GetBase returns the scene node representing the root of the
		//	object.  This is also the node which affects visibility of the
		//	object, and positioning of the object.
		//--------------------------------------------------------------------
		virtual inline const g3dSceneNode* GetBase() const;
		virtual inline g3dSceneNode* GetBase();

		//--------------------------------------------------------------------
		//  SetFragment - sets the fragment of the base scene node
		//	Does not own fragment
		//--------------------------------------------------------------------
		void SetFragment( g3dFragment* i_pFragment );

		//--------------------------------------------------------------------
		//	GetPosition returns the position of the object.
		//--------------------------------------------------------------------
		inline const maPoint3d& GetPosition() const;

		//--------------------------------------------------------------------
		//	GetOrientation returns the orientation of the object.
		//--------------------------------------------------------------------
		inline const maRotation& GetOrientation() const;

		//--------------------------------------------------------------------
		//	GetScale returns the scale of the object.
		//--------------------------------------------------------------------
		inline const maVector3d& GetScale() const;

		//--------------------------------------------------------------------
		//	GetPivotPoint returns the point around which the object
		//		rotates and scales
		//--------------------------------------------------------------------
		inline const maPoint3d& GetPivotPoint() const;

		//--------------------------------------------------------------------
		//	GetPivotCompensation returns the translation accumulated in 
		//	order to preserve transformations when moving the pivot point.
		//--------------------------------------------------------------------
		inline const maVector3d& GetPivotCompensation() const;

		//--------------------------------------------------------------------
		//	SetPosition changes the position of the object.
		//--------------------------------------------------------------------
		void SetPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	SetOrientation changes the orientation of the object.
		//--------------------------------------------------------------------
		void SetOrientation(const maRotation& i_Orientation);

		//--------------------------------------------------------------------
		//  SetScale
		//--------------------------------------------------------------------
		void SetScale(const maVector3d& i_Scale);

		//--------------------------------------------------------------------
		//	SetPositionAndOrientation has the same effect as calling
		//	SetPosition, then SetOrientation, but is a little more
		//	efficient.
		//--------------------------------------------------------------------
		void SetPositionAndOrientation(	const maPoint3d& i_Position,
										const maRotation& i_Orientation);

		//--------------------------------------------------------------------
		//	SetPivotPoint changes the point around which the object rotates.
		//		If i_bPreserveTransformation is true, then a compensating
		//		translation is added to m_PivotCompensation in order to
		//		make the total transformation matrix stay the same.
		//--------------------------------------------------------------------
		void SetPivotPoint(const maPoint3d& i_Position, 
						   bool i_bPreserveTransformation = false);

		//--------------------------------------------------------------------
		//	The pivot compensation is usually only set through calls to 
		//	SetPivotPoint in which it is computed. But sometimes, you need
		//	to set it directly in order to restore a transformation.
		//--------------------------------------------------------------------
		void SetPivotCompensation(const maPoint3d& i_Compensation);

		//--------------------------------------------------------------------
		// Get transformation for the root node of the given object.
		// This is the matrix for the position, scale, rotation,
		// along with pivots stored in this object.
		//--------------------------------------------------------------------
		void GetTransformation(maMatrix4x4& o_Transformation);

		//--------------------------------------------------------------------
		//	GetWorldBox returns the bounding box
		//--------------------------------------------------------------------
		const maAxisBox& GetWorldBox() const;

		//--------------------------------------------------------------------
		//	PreRender is called by the scScene for each object before it
		//	is rendered.  Objects can use this function to animate.
		//--------------------------------------------------------------------
		virtual void Animate(float i_SimulationTime);

		//--------------------------------------------------------------------
		//	Animate only the scene graph nodes, 
		//  fragments should not be animated yet.
		//	This is for preparation for attachments that need the
		//	transformations in the nodes, but not the bounding boxes.
		//--------------------------------------------------------------------
		virtual void AnimateMatrices(float i_SimulationTime);

		//--------------------------------------------------------------------
		//	Call SetCastsShadow to make the scObject cast a shadow or not.
		//	Some scObjects may not cast shadows anyway (particles).
		//--------------------------------------------------------------------
		virtual void SetCastsShadow(bool i_Cast);

		//--------------------------------------------------------------------
		// GetPeriod returns the think period in frames when an object is
		// being culled.
		//--------------------------------------------------------------------
		inline int GetPeriod() const;

		//--------------------------------------------------------------------
		// SetPeriod sets the think period in frames when an object is
		// being culled.  This way we don't have to think culled objects
		// constantly but we still give them some thinks
		//	Period works best when it is a prime number
		//--------------------------------------------------------------------
		inline void SetPeriod(int i_nPeriod);

		//--------------------------------------------------------------------
		//	GetDelayToThink returns how many frames left until a think is
		//	necessary.
		//--------------------------------------------------------------------
		inline int GetDelayToThink() const;

		//--------------------------------------------------------------------
		//	SetDelayToThink sets how many frames are left until there is a
		//	think.  This should be updated every frame by the scene even if
		//	the object is not being culled.
		//--------------------------------------------------------------------
		inline void SetDelayToThink(int i_nActivationDelay);

		//--------------------------------------------------------------------
		//	IsExpired is used by the pscObjectMgr to tell if the object
		//	should be destroyed.
		//--------------------------------------------------------------------
		inline bool IsExpired() const;

		//--------------------------------------------------------------------
		//	SetExpired will cause IsExpired to return true, which will in
		//	turn cause the object to be destroyed by the scObjectMgr.
		//--------------------------------------------------------------------
		void SetExpired();

		//--------------------------------------------------------------------
		//	SetRenderable is really just a helper that calls
		//	the base node's SetRenderable.
		//--------------------------------------------------------------------
		void SetRenderable( bool i_bRender );
		bool GetRenderable() const;

		//--------------------------------------------------------------------
		//	SetActiveInRenderLayer is just a helper that calls
		// the base node's SetActiveInRenderLayer
		//--------------------------------------------------------------------
		void SetActiveInRenderLayer(bool i_bRenderable);
		bool GetActiveInRenderLayer() const;

		//--------------------------------------------------------------------
		//	SetActiveInRenderLayer is just a helper that calls
		// the base node's SetActiveInSceneMgr
		//--------------------------------------------------------------------
		void SetActiveInSceneMgr(bool i_bRenderable);
		bool GetActiveInSceneMgr() const;

		//--------------------------------------------------------------------
		//	SetFogged is really just a helper that sets all the nodes of this
		//	object to render without fog
		//--------------------------------------------------------------------
		void SetFogged( bool i_bFog );

		//--------------------------------------------------------------------
		//  AddControlAnimation - adds animation controller, this object
		//		will own the control animation
		//--------------------------------------------------------------------
		void AddControlAnimation( scControlAnim* i_pAnim );

		//--------------------------------------------------------------------
		//  RemoveControlAnimation - removes animation controller,
		//		the caller is responsible for deleting the control animation
		//--------------------------------------------------------------------
		void RemoveControlAnimation( scControlAnim* i_pAnim );

		//--------------------------------------------------------------------
		//	Inserts node into hierarchy to allow control animation
		//	to alter matrix
		//--------------------------------------------------------------------
		virtual g3dSceneNode* InsertControlNode(const char* i_Name);

		//--------------------------------------------------------------------
		// Remove Control Node that was added earlier
		//--------------------------------------------------------------------
		virtual void RemoveControlNode(g3dSceneNode* i_pNode);

		//--------------------------------------------------------------------
		// If implementations have low resolution models, return true here.
		//	Default returns false.
		//--------------------------------------------------------------------
		virtual bool HasLowResolutionModel() const;

		//--------------------------------------------------------------------
		// Static access to the algorithm used to compute 
		//	full transformation.
		//--------------------------------------------------------------------
		static void ComputeFullTransformation(const maPoint3d& i_Pivot,
												 const maVector3d& i_PivotCompensation,
												 const maPoint3d& i_Position,
												 const maPoint3d& i_Scale,
												 const maRotation& i_Orientation,
												 maMatrix4x4 &o_Transformation);

	private:
		//--------------------------------------------------------------------
		// Recompute the transformation using all info including pivot.
		//--------------------------------------------------------------------
		void update_full_transformation();

		g3dSceneNode* m_pBase;
		maPoint3d m_Position;
		maPoint3d m_Scale;
		maRotation m_Orientation;
		bool m_bExpired;
		int m_nPeriod;				// execution period in frames (30 = every 30 frames)
		int m_nDelayToThink;		// how many frames tell the next think

		// pivot point information
		maPoint3d m_Pivot;
		maVector3d m_PivotCompensation;	// used to preserve transformations when moving pivot point
		bool m_bHasPivotPoint;

	protected:
		std::vector<scControlAnim*>	m_ControlAnims;
};

//--------------------------------------------------------------------
//	GetBase returns the scene node representing the root of the
//	object.  This is also the node which affects visibility of the
//	object, and positioning of the object.
//--------------------------------------------------------------------
inline const g3dSceneNode* scObject::GetBase() const
{
	return m_pBase;
}

inline g3dSceneNode* scObject::GetBase()
{
	return m_pBase;
}

//--------------------------------------------------------------------
//	GetPosition returns the position of the object.
//--------------------------------------------------------------------
inline const maPoint3d& scObject::GetPosition() const
{
	return m_Position;
}

//--------------------------------------------------------------------
//	GetOrientation returns the orientation of the object.
//--------------------------------------------------------------------
inline const maRotation& scObject::GetOrientation() const
{
	return m_Orientation;
}

//--------------------------------------------------------------------
//	GetScale returns the scale of the object.
//--------------------------------------------------------------------
inline const maVector3d& scObject::GetScale() const
{
	return m_Scale;
}

//--------------------------------------------------------------------
//	GetPivotPoint returns the point around which the object
//		rotates and scales
//--------------------------------------------------------------------
inline const maPoint3d& scObject::GetPivotPoint() const
{
	return m_Pivot;
}

//--------------------------------------------------------------------
//	GetPivotCompensation returns the translation accumulated in 
//	order to preserve transformations when moving the pivot point.
//--------------------------------------------------------------------
inline const maVector3d& scObject::GetPivotCompensation() const
{
	return m_PivotCompensation;
}

//--------------------------------------------------------------------
// GetPeriod returns the think period in frames when an object is
// being culled.
//--------------------------------------------------------------------
inline int scObject::GetPeriod() const
{
	return m_nPeriod;
}

//--------------------------------------------------------------------
// SetPeriod sets the think period in frames when an object is
// being culled.  This way we don't have to think culled objects
// constantly but we still give them some thinks
//	Period works best when it is a prime number
//--------------------------------------------------------------------
inline void scObject::SetPeriod(int i_nPeriod)
{
	m_nPeriod = i_nPeriod;
}

//--------------------------------------------------------------------
//	GetDelayToThink returns how many frames left until a think is
//	necessary.
//--------------------------------------------------------------------
inline int scObject::GetDelayToThink() const
{
	return m_nDelayToThink;
}

//--------------------------------------------------------------------
//	SetDelayToThink sets how many frames are left until there is a
//	think.  This should be updated every frame by the scene even if
//	the object is not being culled.
//--------------------------------------------------------------------
inline void scObject::SetDelayToThink(int i_nDelay)
{
	m_nDelayToThink = i_nDelay;
}

//--------------------------------------------------------------------
//  IsExpired
//--------------------------------------------------------------------
inline bool scObject::IsExpired() const
{
	return m_bExpired;
}
