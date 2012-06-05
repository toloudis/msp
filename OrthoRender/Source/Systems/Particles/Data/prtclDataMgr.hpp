/*****************************************************************************
**	prtclScriptDataMgr.hpp
**
**	Keeps track of the current displayed set Particles
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PRTCL_DATAMGR_HPP
#error prtclScriptDataMgr.hpp multiply included
#endif
#define PRTCL_DATAMGR_HPP

#ifndef MA_POINT3D_HPP
#include "maPoint3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "maRotation.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class prtclScriptData;
class prtclParticlesData;
class fsLocator;
class geoPickRay;
class api3dObject;


//============================================================================
//============================================================================
namespace prtclScriptDataMgr
{
	// EventCallback when any data in this manager changes
	class DataChangedCallback
	{
		public:
			virtual void DataListChanged() = 0;
			virtual void DataObjectChanged(int i_Index) = 0;
	};

	typedef int ParticleID;

	//--------------------------------------------------------------------
	// Set directory to look for geometry files within
	//--------------------------------------------------------------------
	void SetDirectory(const fsLocator &i_Dir);

	//--------------------------------------------------------------------
	// Clear all prtcls
	//--------------------------------------------------------------------
	void  Clear();

	//--------------------------------------------------------------------
	//  Add new prtcl
	//--------------------------------------------------------------------
	ParticleID  AddParticle(const prtclScriptData &i_Particle);

	//--------------------------------------------------------------------
	//  Removes an Particle from the set
	//--------------------------------------------------------------------
	void  RemoveParticle(const prtclScriptData &i_Particle);
	void  RemoveParticle(int i_Index);

	//--------------------------------------------------------------------
	//  Get a Particle
	//--------------------------------------------------------------------
	api3dObject*  GetParticle(int i_Index);

	//--------------------------------------------------------------------
	//  Return number of set Particles
	//--------------------------------------------------------------------
	int  GetNumParticles();

	//--------------------------------------------------------------------
	// Update individual Particle data
	//--------------------------------------------------------------------
	void SetParticleData(int i_Index, const prtclScriptData& i_Data);
	prtclScriptData GetParticleData(ParticleID i_Index);

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	const prtclParticlesData& GetData();
	void SetData(const prtclParticlesData &i_Data);

	////--------------------------------------------------------------------
	////  Access to whole data as one structure for easy display and
	//// parsing
	////--------------------------------------------------------------------
	//const prtclScriptData& GetParticleData();
	//void SetParticleData(const prtclScriptData& i_Data);

	//--------------------------------------------------------------------
	// Update individual Particle position
	//--------------------------------------------------------------------
	void SetParticlePosition( ParticleID i_Index, const maPoint3d& i_Pos);
	maPoint3d GetParticlePosition( ParticleID i_Index);

	//--------------------------------------------------------------------
	// Update individual Particle Orientation
	//--------------------------------------------------------------------
	void SetParticleOrientation( ParticleID i_Index, const maRotation& i_Rot);
	maRotation GetParticleOrientation( ParticleID i_Index);

	//--------------------------------------------------------------------
	// This callback will be called when the data changes
	//--------------------------------------------------------------------
	void AddDataChangedCallback(DataChangedCallback *i_Func);
	void RemoveDataChangedCallback(DataChangedCallback *i_Func);

	//--------------------------------------------------------------------
	//	Ray pick occluders returning a modified tVal
	//--------------------------------------------------------------------
	bool OccluderPick(geoPickRay& i_Ray, float &o_tVal);

}	// end of namespace
