/*****************************************************************************
**	api3dParticleGenerator.hpp
**
**	Base class for all particle generators that can be placed into the scene
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_PARTICLEGENERATOR_HPP
#error api3dParticleGenerator.hpp multiply included
#endif
#define API3D_PARTICLEGENERATOR_HPP

#ifndef API3D_OBJECTSINGLE_HPP
#include "Tool/api3d/api3dObjectSingle.hpp"
#endif
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
#include <string>


//============================================================================
//	forward references
//============================================================================
class api3dReference;
class scParticleGenerator;
class scParticleGeneratorTemplate;


//============================================================================
//============================================================================
class api3dParticleGenerator : public api3dObjectSingle
{
public:
	//--------------------------------------------------------------------
	// This class takes ownership of the generator and the template
	//--------------------------------------------------------------------
	api3dParticleGenerator( scParticleGenerator* i_pPG, 
							scParticleGeneratorTemplate* i_pPGT );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~api3dParticleGenerator();

	//--------------------------------------------------------------------
	// Position
	//--------------------------------------------------------------------
	virtual maPoint3d  GetPosition() const;
	virtual void  SetPosition(const maPoint3d &i_Position);

	//--------------------------------------------------------------------
	// Orientation
	//--------------------------------------------------------------------
	virtual maRotation GetOrientation() const;
	virtual void  SetOrientation(const maRotation &i_Rotation);

	//--------------------------------------------------------------------
	// Scale
	//--------------------------------------------------------------------
	virtual maVector3d GetScale() const;
	virtual void  SetScale(const maVector3d& i_Scale);
	virtual void  SetUniformScale(const float i_fScale);

	//----------------------------------------------------------------------------
	//	Renderable sets whether the object is visible
	//----------------------------------------------------------------------------
	void SetRenderable(bool i_Renderable);
	bool GetRenderable() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	scParticleGenerator* GetParticleGenerator();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void SetParticleGeneratorTemplate(scParticleGeneratorTemplate* i_pPGT);

	//--------------------------------------------------------------------
	// Accessor to object
	//--------------------------------------------------------------------
	virtual const scObject* GetObject() const;
	virtual scObject* Object();

	//--------------------------------------------------------------------
	//	Get reference for given name.  The returned pointer is owned
	//	by this object.  The object should be retained by the caller
	//	to avoid repeated string searches.
	//--------------------------------------------------------------------
	virtual api3dReference* GetReference(const char* i_Name);

	//--------------------------------------------------------------------
	// Get list of references
	//--------------------------------------------------------------------
	virtual void GetReferenceList(std::vector<std::string> &o_List);

	//--------------------------------------------------------------------
	//	GetWorldBox returns the bounding box
	//--------------------------------------------------------------------
	virtual const maAxisBox& GetWorldBox() const;

	//--------------------------------------------------------------------
	// Set color (emissive, not diffuse) for fragments
	//--------------------------------------------------------------------
	virtual void SetColor(const maFloatRGBA &i_Color);

private:
	scParticleGenerator* m_pPG;
	scParticleGeneratorTemplate* m_pPGT;

	bool m_bTemplateOwner;

	std::vector<api3dReference*> m_References;
	maAxisBox m_Bounds;
};
