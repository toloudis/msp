/*****************************************************************************
**	tmlnDriverUserScriptTemplate.hpp
**
**		UserScript driver template class
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERUSERSCRIPTTEMPLATE_HPP
#error tmlnDriverUserScriptTemplate.hpp multiply included
#endif
#define TMLN_DRIVERUSERSCRIPTTEMPLATE_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef TMLN_DRIVERUSERSCRIPTINFO_HPP
#include "Drivers/UserScript/tmlnDriverUserScriptInfo.hpp"
#endif 

#ifndef MNM_CONSTANTS_HPP
#include "Support/mnm/mnmConstants.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef PRTY_TEXTBOXUIINFO_HPP
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif


//============================================================================
//============================================================================
class tmlnChannel;
class tmlnDriverUserScriptInfo;


//============================================================================
//============================================================================
template<class xxxChannel>
class tmlnDriverUserScriptTemplate : public tmlnDriver
{
public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tmlnDriverUserScriptTemplate(	xxxChannel &i_Channel, 
										chDefs::Name i_ChunkName,
										const char* i_InitialScript)
		:	m_ChunkName(i_ChunkName),
			m_Channel(i_Channel),
			m_PythonScript("Python Expression", i_InitialScript),
			m_ErrorMessage("Error Message")
		{
			this->SetBlendType(tmlnDriver::GetDefaultBlendType());

			// Register the properties so they can be displayed to the user
			//
			prtyTextBoxUIInfo* pTBUII = new prtyTextBoxUIInfo(&(m_PythonScript), "Script", "Python expression to execute");
			// because these are interpreted as expressions, the script has to be single line
			pTBUII->SetMultiline( true ); 
		#if( SGPU_APP == MS_CORE )
			pTBUII->SetReadOnly(true);
		#endif
			AddProperty( pTBUII );
			pTBUII = new prtyTextBoxUIInfo(&(m_ErrorMessage), "Script", "Error messages from script");
			pTBUII->SetMultiline( true );
		#if( SGPU_APP == MS_CORE )
			pTBUII->SetReadOnly(true);
		#endif
			//pTBUII->SetReadOnly( true ); // read-only made it so you couldn't scroll the test
			AddProperty( pTBUII );

			// Register callbacks to update dirty bit when properties change
			m_PythonScript.AddCallback(new prtyCallbackWrapper<tmlnDriverUserScriptTemplate>(this, &tmlnDriverUserScriptTemplate::PythonScriptChanged));
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~tmlnDriverUserScriptTemplate()
		{
		}

		//--------------------------------------------------------------------
		// Return string for description of driver (include its current state)
		// to display in the gui when the mouse hovers over the clip.
		//--------------------------------------------------------------------
		std::string GetHoverDescription()
		{
			std::string desc;
			desc = tmlnDriver::GetHoverDescription();
			desc += "User Script";
			return desc;
		}

		//--------------------------------------------------------------------
		//  GetDriverInfo - return data structure representing state of
		//		this driver suitable for writing to a file.
		//	The returned value should be created with "new" and will
		//		be deleted by the caller.
		//--------------------------------------------------------------------
		tmlnDriverInfo*  GetDriverInfo() const
		{
			tmlnDriverUserScriptInfo *pInfo = new tmlnDriverUserScriptInfo(m_ChunkName);

			this->GetBaseDriverInfo(*pInfo);

			pInfo->m_Value = this->m_PythonScript.GetValue();

			return pInfo;
		}

		//--------------------------------------------------------------------
		// Set internal variables from data structure
		//--------------------------------------------------------------------
		void SetDriverInfo(	const tmlnDriverUserScriptInfo& i_Info )
		{
			// Set base info
			this->SetBaseDriverInfo(i_Info);

			// Store local info
			this->m_PythonScript.SetValue(i_Info.m_Value);
		}

		//--------------------------------------------------------------------
		//	return the fill  to be used for this driver type
		//--------------------------------------------------------------------
		//virtual
		maFloatRGBA GetClipFill() const
		{
			return maFloatRGBA( 0.35f, 0.65f, 0.95f, 1.0f );
		};

		//--------------------------------------------------------------------
		//	Override base class to have this driver's Operate called at end.
		//--------------------------------------------------------------------
		virtual bool NeedsDelayedOperate()
		{
			return true;
		}

private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void PythonScriptChanged(prtyProperty* i_pProperty, bool i_bDirty)
		{
			this->MarkDirty();
		}

protected:
	prtyText			m_PythonScript;
	xxxChannel&			m_Channel;
	chDefs::Name		m_ChunkName;
	prtyText			m_ErrorMessage;
};
