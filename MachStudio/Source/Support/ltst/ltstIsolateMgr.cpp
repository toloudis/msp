/*****************************************************************************
**	ltstIsolateMgr.cpp
**
**	Keeps track of lights and objects that can be grouped so that certain
**	lights affect only certain objects.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/ltst/ltstIsolateMgr.hpp"
#include "Support/ltst/ltstIsolatable.hpp"

#include "Support/mnm/mnmThinkInterest.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"

#include "Core/App/appTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/name/nameObject.hpp"

#include <boost/bind.hpp>
#include <list>


namespace ltstIsolateMgr
{

	namespace
	{
		bool l_bHasIsolation = false;
		
		const bool c_DoFadeAnimation = true;
		const float c_AnimationLength = 0.20f;
		bool l_bIsAnimating = false;
		float l_BeginTime = 0.0f;

		struct sLight
		{
			nameObject*		m_pNameObject;
			ltstIsolatable*	m_pLight;
			bool			m_bEnabled;
		};

		// Note: I would prefer a map here, but the nameObject class might change its
		// nameString during its lifetime and the map would have to be based on the
		// nameString, not on the nameObject
		std::list<sLight> l_Lights;
	
		//============================================================================
		//============================================================================
		template<class S> 
		class nameobj_search
		{
		public:
			nameobj_search(const nameString& i_Name) : m_Name(i_Name) {};
			bool operator () ( S i_Set )
			{
				return (m_Name == i_Set.m_pNameObject->GetName());
			}
			nameString m_Name;
		};

		//============================================================================
		// Animation code for fade in/out
		//============================================================================
		class FadeThinkInterest : public mnmThinkInterest
		{
		public:
			//--------------------------------------------------------------------
			//	Think
			//--------------------------------------------------------------------
			virtual void Think()
			{
				if (l_bIsAnimating)
				{
					float time = appTime::GetTime() - l_BeginTime;

					if (time >= c_AnimationLength)
					{
						l_bIsAnimating = false;
						
						std::list<sLight>::iterator it, end = l_Lights.end();
						for (it = l_Lights.begin(); it != end; ++it)
						{
							it->m_pLight->FinishAnimationState();
						}
					}
					else
					{
						float alpha = time / c_AnimationLength;
						maFunctions::Clamp(alpha, 0.0f, 1.0f);

						std::list<sLight>::iterator it, end = l_Lights.end();
						for (it = l_Lights.begin(); it != end; ++it)
						{
							it->m_pLight->AnimateState(alpha);
						}
					}
				}
			}
		};
		FadeThinkInterest l_ThinkInterest;

		//--------------------------------------------------------------------
		// Set whether light is isolated on or off. Begins animation.
		//--------------------------------------------------------------------
		void set_light_isolation(std::list<sLight>::iterator& i_Light, bool i_bIsolated)
		{
			if (i_bIsolated)
			{
				// In isolation set, keep the light enabled
				//  visible==true and alpha to 1
				if (c_DoFadeAnimation)
					i_Light->m_pLight->AnimateIsolationState(true);
				else
					i_Light->m_pLight->SetIsolationState(true, 1.0f);
			}
			else
			{
				// Outside the isolation set means disable, visible==false and alpha to 0
				if (c_DoFadeAnimation)
					i_Light->m_pLight->AnimateIsolationState(false);
				else
					i_Light->m_pLight->SetIsolationState(false, 0.0f);
			}

			i_Light->m_bEnabled = i_bIsolated;
		}


	}	// end of namespace

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Initialize()
	{
		if (c_DoFadeAnimation)
		{
			// register the think interest
			mnmThinkMgr::RegisterThinkInterest( &l_ThinkInterest );
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void DeInitialize()
	{
		if (c_DoFadeAnimation)
		{
			// unregister the think interest
			mnmThinkMgr::UnRegisterThinkInterest( &l_ThinkInterest );
		}
	}

	//--------------------------------------------------------------------
	//  Add named light to list of things that can be isolated
	//--------------------------------------------------------------------
	void  AddLight(nameObject* i_pNameObj, 
				   ltstIsolatable* i_pLight)
	{
		sLight new_light = { i_pNameObj, i_pLight };
		l_Lights.push_back( new_light );
	}

	//--------------------------------------------------------------------
	//	Remove light from manager 
	//--------------------------------------------------------------------
	void RemoveLight(nameObject* i_pNameObj, 
				     ltstIsolatable* i_pLight)
	{
		std::list<sLight>::iterator it, end = l_Lights.end();
		for (it = l_Lights.begin(); it != end; ++it)
		{
			if (it->m_pNameObject == i_pNameObj)
			{
				l_Lights.erase(it);
				break;
			}
		}
	}

	//--------------------------------------------------------------------
	//	Test if name is component that can be added to light sets
	//--------------------------------------------------------------------
	bool  IsLight(const nameString& i_Name)
	{
		std::list<sLight>::iterator it, end = l_Lights.end();
		for (it = l_Lights.begin(); it != end; ++it)
		{
			if (it->m_pNameObject->GetName() == i_Name)
				return true;
		}
		return false;
	}

	//--------------------------------------------------------------------
	// Remove all light sets (preparing for a new scene)
	//--------------------------------------------------------------------
	void ClearIsolation()
	{
		l_bHasIsolation = false;

		std::list<sLight>::iterator it, end = l_Lights.end();
		for (it = l_Lights.begin(); it != end; ++it)
		{
			// Clearing the isolation sets visible==true and alpha to 1
			set_light_isolation(it, true);
		}

		if (c_DoFadeAnimation)
		{
			l_bIsAnimating = true;
			l_BeginTime = appTime::GetTime();
		}
	}

	//--------------------------------------------------------------------
	//	Turn off all lights except the ones in this named set.
	//--------------------------------------------------------------------
	void  IsolateLights(const std::set<nameString>& i_IsolatedLights)
	{
		l_bHasIsolation = false;

		std::list<sLight>::iterator it, end = l_Lights.end();
		for (it = l_Lights.begin(); it != end; ++it)
		{
			if (i_IsolatedLights.find(it->m_pNameObject->GetName()) != i_IsolatedLights.end())
			{
				// In isolation set, keep the light enabled
				set_light_isolation(it, true);
			}
			else
			{
				// Outside the isolation set means disable
				set_light_isolation(it, false);
				l_bHasIsolation = true;
			}
		}

		if (c_DoFadeAnimation)
		{
			l_bIsAnimating = true;
			l_BeginTime = appTime::GetTime();
		}
	}

	//--------------------------------------------------------------------
	// Add or remove a single light to the isolation set by name
	//--------------------------------------------------------------------
	void  SetIsolated(const nameString& i_LightName, bool i_bIsolated)
	{
		// Find named light
		std::list<sLight>::iterator light_it = 
			std::find_if(l_Lights.begin(), l_Lights.end(), nameobj_search<sLight>(i_LightName));
		if (light_it != l_Lights.end())
		{
			set_light_isolation(light_it, i_bIsolated);
		}

		if (c_DoFadeAnimation)
		{
			l_bIsAnimating = true;
			l_BeginTime = appTime::GetTime();
		}
	}

	//--------------------------------------------------------------------
	// Return true if there is a current isolated set of lights.
	//--------------------------------------------------------------------
	bool HasIsolatedSet()
	{
		return l_bHasIsolation;
	}

	//--------------------------------------------------------------------
	// Return true if light is isolated. If there is no isolated set,
	// then this will return true for all lights (because all are enabled)
	//--------------------------------------------------------------------
	bool  IsLightEnabled(const nameString& i_LightName)
	{
		if (l_bHasIsolation)
		{
			// Find named light
			std::list<sLight>::iterator light_it = 
				std::find_if(l_Lights.begin(), l_Lights.end(), nameobj_search<sLight>(i_LightName));
			if (light_it != l_Lights.end())
			{
				return light_it->m_bEnabled;
			}
		}
		return true;
	}

}	// end of namespace
