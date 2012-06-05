//****************************************************************************
/// \file EditLineEvent.cs
///
///	An EditLine event checks for the specific tag string to see if a user 
///	entered it while coding.  If so, the event fires usually inserting a
///	chunk of code in place of the tag.
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
using System;
using EnvDTE;


//============================================================================
///
//============================================================================
namespace DevEnvLib
{
	//------------------------------------------------------------------------
	/// Summary description for Class1.
	//------------------------------------------------------------------------
	public abstract class EditLineEvent
	{
		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public EditLineEvent()
		{
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public bool HandleEvent( ref EditPoint io_EP, ref TextSelection io_TS )
		{
			string sOrigLine = io_EP.GetText(io_EP.LineLength);

			//	check if the tag matches
			//
			string sLine = sOrigLine.Trim();
			if (sLine.StartsWith(GetTag()) != true)
				return false;

			//	it does, so insert the appropriate code
			//
			InsertCode( ref io_EP, ref io_TS, ref sOrigLine );

			return true;
		}

		//--------------------------------------------------------------------
		///	the unique string tag for this event
		//--------------------------------------------------------------------
		protected abstract string GetTag();

		//--------------------------------------------------------------------
		///	the code to insert
		//--------------------------------------------------------------------
		protected abstract void InsertCode( ref EditPoint io_EP, ref TextSelection io_TS, ref string i_sOrigLine );
	}
}
