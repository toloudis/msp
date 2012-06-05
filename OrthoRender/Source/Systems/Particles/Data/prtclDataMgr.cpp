/*****************************************************************************
**	prtclScriptDataMgr.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "prtclScriptDataMgr.hpp"

#include "prtclScriptData.hpp"

#include "api3dImport.hpp"
#include "api3dObject.hpp"
#include "api3dScene.hpp"
#include "dbgLog.hpp"
#include "envSTLHelpers.hpp"
#include "geoRayIntersection.hpp"
#include "geoPickRay.hpp"
#include "gfPaths.hpp"


namespace prtclScriptDataMgr
{
//	namespace
//	{
		fsLocator l_Directory;

		prtclParticlesData l_Data;
		std::vector<api3dObject*> l_Objects; // owns objects pointed to

		std::vector<DataChangedCallback*> l_DataCallbacks;

		//--------------------------------------------------------------------
		void notify_callbacks_datalist()
		{
			static bool notifying = false;
			if (notifying) return;

			notifying = true;
			for (int i=0; i < l_DataCallbacks.size(); i++)
				l_DataCallbacks[i]->DataListChanged();

			notifying = false;
		}

		void notify_callbacks_dataobject( int i_Index )
		{
			static bool notifying = false;
			if (notifying) return;

			notifying = true;

			l_DataCallbacks[i_Index]->DataObjectChanged( i_Index );

			notifying = false;
		}

		//----------------------------------------------------------------------------
		void set_prtcl_data(api3dObject *io_Particle, const prtclScriptData &i_Data)
		{
			io_Particle->SetPosition( i_Data.m_Position );
			io_Particle->SetOrientation( i_Data.m_Orientation );
		}

		//--------------------------------------------------------------------
		void create_prtcl( const prtclScriptData &i_Data )
		{
			//	grab the filename + load the object
			fsLocator file_loc = l_Directory;
			file_loc.Push(i_Data.m_Filename);

			api3dObject* Particle = api3dImport::LoadObject(file_loc);

			//	set the data and add it to the object list
			set_prtcl_data(Particle, i_Data);

			l_Objects.push_back(Particle);

			//	add it to the scene
			api3dScene::AddObject(Particle);

			//	add the data to the datat list
			l_Data.m_ParticleItems.push_back(i_Data);
		}

		//--------------------------------------------------------------------
		void remove_all_objects()
		{
			for (int i=0; i<l_Objects.size(); i++)
			{
				api3dScene::RemoveObject(l_Objects[i]);
				delete l_Objects[i];
			}
			l_Objects.clear();
		}

		//--------------------------------------------------------------------
		void remove_object(int index)
		{
			api3dScene::RemoveObject(l_Objects[index]);
			delete l_Objects[index];
			l_Objects.erase(l_Objects.begin() + index);
		}

//	}	// end of namespace

	//--------------------------------------------------------------------
	// Set directory to look for geometry files within
	//--------------------------------------------------------------------
	void SetDirectory(const fsLocator &i_Dir)
	{
		l_Directory = i_Dir;
	}

	//--------------------------------------------------------------------
	// Clear all items
	//--------------------------------------------------------------------
	void  Clear()
	{
		l_Data.Clear();

		remove_all_objects();

		notify_callbacks_datalist();
	}

	//--------------------------------------------------------------------
	//  Add new prtcl
	//--------------------------------------------------------------------
	ParticleID  AddParticle(const prtclScriptData &i_Item)
	{
		create_prtcl( i_Item );

		notify_callbacks_datalist();

		return (l_Objects.size() - 1);
	}

	//--------------------------------------------------------------------
	//  Removes a prtcl
	//--------------------------------------------------------------------
	void  RemoveParticle(const prtclScriptData &i_Item)
	{
		for (int i=0; i<l_Data.m_ParticleItems.size(); i++)
		{
			if (l_Data.m_ParticleItems[i] == i_Item)
			{
				RemoveParticle(i);
				return;
			}
		}
	}
	void  RemoveParticle(int i_Index)
	{
		remove_object(i_Index);
		l_Data.m_ParticleItems.erase(l_Data.m_ParticleItems.begin() + i_Index);

		notify_callbacks_datalist();
	}

	//--------------------------------------------------------------------
	//  Get a Particle
	//--------------------------------------------------------------------
	api3dObject*  GetParticle(int i_Index)
	{
		return l_Objects[ i_Index ];
	}

	//--------------------------------------------------------------------
	//  Return number of set items
	//--------------------------------------------------------------------
	int  GetNumParticles()
	{
		return l_Data.m_ParticleItems.size();
	}

	//--------------------------------------------------------------------
	// Update individual Particle position
	//--------------------------------------------------------------------
	void SetParticlePosition( ParticleID i_Index, const maPoint3d& i_Pos)
	{
		l_Data.m_ParticleItems[i_Index].m_Position = i_Pos;
		l_Objects[i_Index]->SetPosition( i_Pos );

		//DBG_LOG3( "SetParticlePosition (%6.3f, %6.3f, %6.3f)", i_Pos.GetX(), i_Pos.GetY(), i_Pos.GetZ() );

		prtclScriptData data = GetParticleData( i_Index );

		notify_callbacks_dataobject( i_Index );
	}

	maPoint3d GetParticlePosition( ParticleID i_Index)
	{
		return l_Objects[i_Index]->GetPosition();
	}

	//--------------------------------------------------------------------
	// Update individual Particle Orientation
	//--------------------------------------------------------------------
	void SetParticleOrientation( ParticleID i_Index, const maRotation& i_Rot)
	{
		l_Data.m_ParticleItems[i_Index].m_Orientation = i_Rot;
		l_Objects[i_Index]->SetOrientation( i_Rot );

		notify_callbacks_dataobject( i_Index );
	}

	maRotation GetParticleOrientation( ParticleID i_Index)
	{
		return l_Objects[i_Index]->GetOrientation();
	}

	//--------------------------------------------------------------------
	// Update individual Particle data
	//--------------------------------------------------------------------
	void SetParticleData(ParticleID i_Index, const prtclScriptData& i_Data)
	{
		set_prtcl_data(l_Objects[i_Index], i_Data);

		notify_callbacks_dataobject( i_Index );
	}
	prtclScriptData GetParticleData(ParticleID i_Index)
	{
		return l_Data.m_ParticleItems[ i_Index ];
	}

	//--------------------------------------------------------------------
	//  Access to whole data as one structure for easy display and
	// parsing
	//--------------------------------------------------------------------
	const prtclParticlesData& GetData()
	{
		return l_Data;
	}

	//--------------------------------------------------------------------
	// Set data as a whole
	//--------------------------------------------------------------------
	void SetData(const prtclParticlesData& i_Data)
	{
		Clear();

		for (int i=0; i<i_Data.m_ParticleItems.size(); i++)
		{
			// could call add item here, but i want to control the number of
			// dirty callbacks executed
			//
			create_prtcl( i_Data.m_ParticleItems[i] );
		}

		notify_callbacks_datalist();
		//notify_callbacks_dataobject();
	}

	//--------------------------------------------------------------------
	// This callback will be called when the data changes
	//--------------------------------------------------------------------
	void AddDataChangedCallback(DataChangedCallback *i_Func)
	{
		l_DataCallbacks.push_back(i_Func);

	}
	void RemoveDataChangedCallback(DataChangedCallback *i_Func)
	{
		envSTLHelpers::RemoveOneValue(l_DataCallbacks, i_Func);
	}

	//--------------------------------------------------------------------
	//	Ray pick occluders returning a modified tVal
	//--------------------------------------------------------------------
	bool OccluderPick( geoPickRay& i_Ray, float &o_tVal )
	{
		bool bIntersection = false;
		for ( int i=0; i < l_Objects.size(); ++i )
		{
			bIntersection |= \
			geoRayIntersection::IntersectLineBBox(  i_Ray.GetRayStart(),
													i_Ray.GetRayDir(),
													l_Objects[i]->GetPosition(),
													l_Objects[i]->GetPosition(),
													o_tVal );
		}

		return bIntersection;
	}

}	// end of namespace
