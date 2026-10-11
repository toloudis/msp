/*****************************************************************************
**	rmpDialogMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/rmp/rmpDialogMgr.hpp"

#include "Support/rmp/rmpObject.hpp"
#include "Support/rmp/private/rmpDataParserUtil.hpp"
#include "Support/pyth/pythProperty.hpp"

#include "Core/prty/prtyTextureDataParser.hpp"

#ifdef USE_WXWIDGETS
#include <wx/tooltip.h>
#endif

#undef CreateDirectory


//============================================================================
//============================================================================
namespace rmpDialogMgr
{
	namespace
	{
		static rmpObject* l_pRmpObject = 0;
		
		//====================================================================
		//====================================================================
		class rmpNameResolver : public pythProperty::NameResolver
		{
			//------------------------------------------------------------
			// Resolve the string "RampEditor" into our property object
			// in order to be accessible from python.
			//------------------------------------------------------------
			virtual prtyObject* ResolveName(std::string &i_PropertyObjectName)
			{
				if (i_PropertyObjectName == std::string("RampEditor"))
				{
					return l_pRmpObject;
				}
				return NULL;
			}
		};

		shared_ptr<rmpNameResolver> l_NameResolver(new rmpNameResolver);
		rmpData l_ClipboardData;
	}

	//------------------------------------------------------------------------
	//  Initialize
	//------------------------------------------------------------------------
	void Initialize()
	{
		l_pRmpObject = new rmpObject();
		pythProperty::AddNameResolver( l_NameResolver ); 

		// set read/write callbacks for prtyTextureFileName
		prtyTextureDataParser::SetRampReadFunction( &rmpDataParserUtil::ReadDataCore );
		prtyTextureDataParser::SetRampWriteFunction( &rmpDataParserUtil::WriteDataCore );
	}

	//------------------------------------------------------------------------
	//  CleanUp
	//------------------------------------------------------------------------
	void  CleanUp()
	{
		if (l_pRmpObject)
		{
			// Remove name resolver 
			pythProperty::RemoveNameResolver( l_NameResolver ); 

			delete l_pRmpObject;
			l_pRmpObject = NULL;
		}
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	rmpData& Data()
	{
		DBG_ASSERT(l_pRmpObject, "Ramp object hasn't been initiated yet");

		return l_pRmpObject->m_Data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetData(const rmpData& i_Data)
	{
		DBG_ASSERT(l_pRmpObject, "Ramp object hasn't been initiated yet");

		l_pRmpObject->m_Data = i_Data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	rmpData& GetDataSimple()
	{
		DBG_ASSERT(l_pRmpObject, "Ramp object hasn't been initiated yet");

		return l_pRmpObject->m_Data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CopyData()
	{
		DBG_ASSERT(l_pRmpObject, "Ramp object hasn't been initiated yet");

		l_ClipboardData = l_pRmpObject->m_Data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PasteData()
	{
		DBG_ASSERT(l_pRmpObject, "Ramp object hasn't been initiated yet");

		l_pRmpObject->m_Data = l_ClipboardData;
		l_ClipboardData = rmpData(); //clear clipboard
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	prtyObject* GetDataObject()
	{
		if (!l_pRmpObject)
		{
			l_pRmpObject = new rmpObject();
			pythProperty::AddNameResolver( l_NameResolver ); 

		}
		return l_pRmpObject;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetDataObject(rmpObject* i_pObject)
	{
		if(!l_pRmpObject)
			pythProperty::AddNameResolver( l_NameResolver ); 

		l_pRmpObject = i_pObject;
	}

	//------------------------------------------------------------------------
	// Set callback for property changes
	//------------------------------------------------------------------------
	void SetRampChangedCallback(const RampChangedFunction& i_FuncPtr)
	{
		DBG_ASSERT(l_pRmpObject, "Ramp object hasn't been initiated yet");

		if (l_pRmpObject)
		{
			l_pRmpObject->SetRampChangedCallback(i_FuncPtr);
		}
	}

	//--------------------------------------------------------------------
	//  Get texture size from enum index
	//--------------------------------------------------------------------
	int GetTexSizeFromEnumIndex(const int i_index)
	{
		int tex = 512;
		switch (i_index)
		{
		case rmpData::RT_LOW:
			tex = 512;
			break;
		case rmpData::RT_MED:
			tex = 1024;
			break;
		case rmpData::RT_HIGH:
			tex = 2048;
			break;
		case rmpData::RT_VERY_HIGH:
			tex = 4096;
			break;
		case rmpData::RT_VERY_VERY_HIGH:
			tex = 8192;
			break;
		}

		return tex;
	}
}
