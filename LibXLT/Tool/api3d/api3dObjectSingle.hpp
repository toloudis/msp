/*****************************************************************************
**	api3dObjectSingle.hpp
**
**	Abstract Derived class, provides an interface for accessing the
**	internal scObject of derived classes. Most apps shouldn't use this 
**	interface, this is just for the internal api3d interface.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_OBJECTSINGLE_HPP
#error api3dObjectSingle.hpp multiply included
#endif
#define API3D_OBJECTSINGLE_HPP

#ifndef API3D_OBJECT_HPP
#include "Tool/api3d/api3dObject.hpp"
#endif


//============================================================================
//============================================================================
class scObject;
class g3dLight;
struct g3dAmbientEnvState;
struct g3dRenderState;
class matTexture;


//============================================================================
//============================================================================
class api3dObjectSingle : public api3dObject
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	api3dObjectSingle();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~api3dObjectSingle();

	//--------------------------------------------------------------------
	// Accessor to object
	//--------------------------------------------------------------------
	virtual const scObject* GetObject() const = 0;
	virtual scObject* Object() = 0;

	//--------------------------------------------------------------------
	// Pivot point information
	//--------------------------------------------------------------------
	virtual maPoint3d GetPivotPoint() const;
	virtual void  SetPivotPoint(const maPoint3d &i_Position, 
						   bool i_bPreserveTransformation = false);
	virtual maVector3d GetPivotCompensation() const;
	virtual void  SetPivotCompensation(const maVector3d &i_Compensation);

	//--------------------------------------------------------------------
	// Get transformation for the root node of the given object.
	// This is the matrix for the position, scale, rotation,
	// along with pivots stored in this object.
	//--------------------------------------------------------------------
	virtual void GetTransformation(maMatrix4x4& o_Transformation);

	//--------------------------------------------------------------------
	//	AddLightToRenderState - add this light to the render state
	//	for this object. Creates the g3dRenderState instance if 
	//	necessary.
	//--------------------------------------------------------------------
	void AddLightToRenderState(g3dLight * i_pLight);

	//--------------------------------------------------------------------
	//	RemoveLightFromRenderState
	//--------------------------------------------------------------------
	void RemoveLightFromRenderState(g3dLight * i_pLight);

	//--------------------------------------------------------------------
	//	Set environment map name in the render state for this object
	//--------------------------------------------------------------------
	void SetRenderStateNameEnv(std::string i_Name);

	//--------------------------------------------------------------------
	//	Set diffuse environment map in the render state for this object
	//--------------------------------------------------------------------
	void SetRenderStateDiffuseEnv(matTexture* i_Map, float i_Weight, float i_Angle,
		const maFloatRGBA& i_Color);

	//--------------------------------------------------------------------
	//	Set specular environment map in the render state for this object
	//--------------------------------------------------------------------
	void SetRenderStateSpecularEnv(matTexture* i_Map, float i_Weight, float i_Angle,
		const maFloatRGBA& i_Color);

	//--------------------------------------------------------------------
	//	Set swl data
	//--------------------------------------------------------------------
	void SetSwlData(bool i_bEnable);

	//--------------------------------------------------------------------
	//	Return the ambient state
	//--------------------------------------------------------------------
	g3dAmbientEnvState * GetAmbientState();

	//--------------------------------------------------------------------
	//	Wireframe sets whether the object drawn in wireframe
	//--------------------------------------------------------------------
	virtual void SetWireframe(bool i_Wireframe);
	virtual bool GetWireframe() const;

	//--------------------------------------------------------------------
	//	LowResolution sets whether the object should render its
	//		low resolution model.  Calls to SetLowResolution may
	//		not change anything if there is no low-res model to choose.
	//--------------------------------------------------------------------
	virtual void SetLowResolution(bool i_LowResolution);
	virtual bool GetLowResolution() const;

	//--------------------------------------------------------------------
	//	GPUPickable sets whether the object should be rendered
	//	in pick renders when doing GPU picking
	//--------------------------------------------------------------------
	virtual void SetGPUPickable(bool i_Pickable);
	virtual bool GetGPUPickable() const;

	//--------------------------------------------------------------------
	//	PickHull sets whether the object should be rendered in 
	//	pick renders even if the Renderable flag is false.
	//--------------------------------------------------------------------
	virtual void SetPickHull(bool i_PickHull);
	virtual bool GetPickHull() const;

	//----------------------------------------------------------------------------
	// PickMask is a user defined bit mask that can be used to filter
	// the pickable objects. The default value of "0" means "do not filter".
	//----------------------------------------------------------------------------
	virtual void SetPickMask(envType::UInt32 i_PickMask);

	//--------------------------------------------------------------------
	// Inherits Transform controls if the given object uses the 
	// transformations of its parent nodes.
	//--------------------------------------------------------------------
	virtual void SetInheritsTransform(bool i_bInherits);
	virtual bool GetInheritsTransform() const;

	//----------------------------------------------------------------------------
	// Is the pick code given within the high and low pick codes assigned
	// to this node?
	//----------------------------------------------------------------------------
	virtual bool ContainsPickCode(envType::UInt32 i_PickCode) const;

	//--------------------------------------------------------------------
	//	Returns the node with a fragment with the given pick code
	//--------------------------------------------------------------------
	virtual const g3dSceneNode* GetPickedNode(envType::UInt32 i_PickCode) const;

private:
	g3dRenderState* m_pRenderState;
	g3dAmbientEnvState* m_pEnvironmentState;
};

