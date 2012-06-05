/*****************************************************************************
**	cmaCommand.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003-7 - All Rights Reserved
\****************************************************************************/
#include "Tool/cma/cmaCommand.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/prty/prtyKeyComboUIInfo.hpp"
#include "Tool/cma/cmaOperations.hpp"

#include <algorithm>


//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
cmaCommand::cmaCommand( const std::string& i_strTag,
						ExecuteHandler i_handlerExecute,
						UpdateHandler i_handlerUpdate )
:	m_Tag(""),
	m_Category("Shortcut"),
	m_Description(""),
	OnUpdate(NULL),
	OnExecute(NULL),
	m_bEnableHotKey(true)
{
	//	Setting the tag this way allows the hotkey property to get properly set also.
	SetTag(i_strTag);

	//	QUESTIONS:
	//	- Any reason this should be done in each command? [rjk]
	//	- perhaps for setting the category + description?  or just accessors?
	//	- Should this be created somewhere else to allow support for another
	//	control type?
	m_pKeyComboUIInfo = new prtyKeyComboUIInfo(&(m_HotKey), m_Category, m_Description);
	AddProperty( m_pKeyComboUIInfo );

	//	Add name callback in order to notify the layer manager
	m_HotKey.AddCallback(new prtyCallbackWrapper<cmaCommand>(this, &cmaCommand::HotKeyChanged));

	//	Hook up the Handlers
	if ( i_handlerUpdate != NULL )
		OnUpdate = i_handlerUpdate;
	if ( i_handlerExecute != NULL )
		OnExecute = i_handlerExecute;
}


//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
//virtual
cmaCommand::~cmaCommand()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmaCommand::Execute()
{
	if (OnExecute != NULL)
	{
		//DBG_LOG("Executing cmaCommand " << this->m_Tag.c_str() );

		OnExecute(this);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
void cmaCommand::ProcessUpdates()
{
	if (OnUpdate != NULL)
	{
		OnUpdate(this);
	}
}

//--------------------------------------------------------------------
// Callbacks for when properties change, updates member data
//--------------------------------------------------------------------
void cmaCommand::HotKeyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	//	has a new HotKey
	cmaOperations::UpdateHotKey(this->GetTag(), m_HotKey.GetValue());
}

//--------------------------------------------------------------------
//	SetObjectID
//--------------------------------------------------------------------
void cmaCommand::SetObjectID(int m_ID)
{
	m_ObjectID = m_ID;
}

//--------------------------------------------------------------------
//	GetObjectID
//--------------------------------------------------------------------
int	cmaCommand::GetObjectID() const
{
	return m_ObjectID;
}

//--------------------------------------------------------------------
//	Tag
//--------------------------------------------------------------------
void cmaCommand::SetTag( const std::string& i_Tag )
{
	m_Tag = i_Tag;

	//	also set the hot key property name
	m_HotKey.SetPropertyName(i_Tag);
}
const std::string& cmaCommand::GetTag() const
{
	return m_Tag;
}

//--------------------------------------------------------------------
//	Category
//--------------------------------------------------------------------
void cmaCommand::SetCategory( const std::string& i_Category )
{
	m_Category = i_Category;

	m_pKeyComboUIInfo->SetCategory(m_Category);
}
const std::string& cmaCommand::GetCategory() const
{
	return m_Category;
}

//--------------------------------------------------------------------
//	Description
//--------------------------------------------------------------------
void cmaCommand::SetDescription( const std::string& i_Description )
{
	m_Description = i_Description;

	m_pKeyComboUIInfo->SetDescription(m_Description);
}
const std::string& cmaCommand::GetDescription() const
{
	return m_Description;
}


//--------------------------------------------------------------------
//	Enabled
//--------------------------------------------------------------------
void cmaCommand::SetEnabled( bool i_bEnabled )
{
	m_bEnabled = i_bEnabled;
}
bool cmaCommand::GetEnabled() const
{
	return m_bEnabled;
}

//--------------------------------------------------------------------
//	Checked
//--------------------------------------------------------------------
void cmaCommand::SetChecked( bool i_bChecked )
{
	m_bChecked = i_bChecked;
}
bool cmaCommand::GetChecked() const
{
	return m_bChecked;
}

//--------------------------------------------------------------------
//	FocusBits
//--------------------------------------------------------------------
void cmaCommand::SetFocusBits( int i_FocusBits )
{
	m_FocusBits = i_FocusBits;
}
int cmaCommand::GetFocusBits() const
{
	return m_FocusBits;
}

//--------------------------------------------------------------------
//	EnableHotKey
//--------------------------------------------------------------------
void cmaCommand::SetEnableHotKey( bool i_bEnableHotKey )
{
	m_bEnableHotKey = i_bEnableHotKey;
}
bool cmaCommand::GetEnableHotKey() const
{
	return m_bEnableHotKey;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtyHotKey& cmaCommand::GetHotKey()
{
	return m_HotKey;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::string& cmaCommand::GetHotKeyString() const
{
	return m_HotKey.GetValue();
}

