/*****************************************************************************
**	cmaCommand.hpp
**
**		base command class
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CMA_COMMAND_HPP
#error cmaCommand.hpp multiply included
#endif
#define CMA_COMMAND_HPP

#ifndef PRTY_HOTKEY_HPP
#include "Core/prty/prtyHotKey.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//============================================================================
class prtyKeyComboUIInfo;


//============================================================================
//============================================================================
class cmaCommand : public prtyObject
{
	public:
		//
		// Events
		//

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		typedef void (*UpdateHandler)(cmaCommand *cmd);
		UpdateHandler OnUpdate;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		typedef void (*ExecuteHandler)(cmaCommand *cmd);
		ExecuteHandler OnExecute;

	public:
		//--------------------------------------------------------------------
		// constructor
		//--------------------------------------------------------------------
		cmaCommand( const std::string&	i_strTag,
					ExecuteHandler i_handlerExecute,
					UpdateHandler  i_handlerUpdate );

		//--------------------------------------------------------------------
		// destructor
		//--------------------------------------------------------------------
		virtual ~cmaCommand();

		//--------------------------------------------------------------------
		//	Execute
		//
		//	i_ObjectID execute a specific ObjectID.  If -1 then process all
		//--------------------------------------------------------------------
    	void Execute();

		//--------------------------------------------------------------------
		//	ProcessUpdates
		//
		//	i_ObjectID execute a specific ObjectID.  If -1 then process all
		//--------------------------------------------------------------------
		virtual void ProcessUpdates();

		//--------------------------------------------------------------------
		// Callbacks for when properties change, updates member data
		//--------------------------------------------------------------------
		void HotKeyChanged(prtyProperty *i_pProperty, bool i_bDirty);

		//
		//	accessors
		//

		//--------------------------------------------------------------------
		//	SetObjectID
		//--------------------------------------------------------------------
		void SetObjectID(int m_ID);

		//--------------------------------------------------------------------
		//	GetObjectID
		//--------------------------------------------------------------------
		int	GetObjectID() const;

		//--------------------------------------------------------------------
		//	Tag
		//--------------------------------------------------------------------
		void SetTag( const std::string& i_Tag );
		const std::string& GetTag() const;

		//--------------------------------------------------------------------
		//	Category
		//--------------------------------------------------------------------
		void SetCategory( const std::string& i_Category );
		const std::string& GetCategory() const;

		//--------------------------------------------------------------------
		//	Description
		//--------------------------------------------------------------------
		void SetDescription( const std::string& i_Description );
		const std::string& GetDescription() const;


		//--------------------------------------------------------------------
		//	Enabled
		//--------------------------------------------------------------------
		void SetEnabled( bool i_bEnabled );
		bool GetEnabled() const;

		//--------------------------------------------------------------------
		//	Checked
		//--------------------------------------------------------------------
		void SetChecked( bool i_bChecked );
		bool GetChecked() const;

		//--------------------------------------------------------------------
		//	FocusBits
		//--------------------------------------------------------------------
		void SetFocusBits( int i_FocusBits );
		int GetFocusBits() const;

		//--------------------------------------------------------------------
		//	EnableHotKey
		//--------------------------------------------------------------------
		void SetEnableHotKey( bool i_bEnableHotKey );
		bool GetEnableHotKey() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyHotKey& GetHotKey();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		const std::string& GetHotKeyString() const;

	private:
		int			m_ObjectID;
		std::string	m_Tag;				// what might be display to the user (e.g. menu item)
		std::string	m_Category;			// the group this command belongs to
		std::string	m_Description;
		bool		m_bEnabled;
		bool		m_bChecked;
		int			m_FocusBits;		// app-defined bits to allow for focus in specific windows
		bool		m_bEnableHotKey;	// Don't let this command have a hotkey
		
		prtyHotKey	m_HotKey;
		prtyKeyComboUIInfo* m_pKeyComboUIInfo;
};
