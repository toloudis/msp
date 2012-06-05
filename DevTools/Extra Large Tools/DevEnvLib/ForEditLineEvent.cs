//****************************************************************************
/// \file ForEditLineEvent.cs
///
///	the EditLine Event to create a for loop.
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
using System;
using EnvDTE;
using System.Collections;


//============================================================================
///
//============================================================================
namespace DevEnvLib
{
	//------------------------------------------------------------------------
	/// Summary description for ForEditLineEvent.
	//------------------------------------------------------------------------
	public class ForEditLineEvent : EditLineEvent
	{
		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public ForEditLineEvent()
		{
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		protected override string GetTag()
		{
			return "!for";
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		protected override void InsertCode( ref EditPoint io_EP, ref TextSelection io_TS, ref string i_sOrigLine )
		{
			string pad = DevEnvLib.EditPointHelpers.GetPad( i_sOrigLine );
		
			// Build array of strings to add
			ArrayList strAdd = new ArrayList();

			strAdd.Add(String.Format("{0}// loop\n", pad));
			strAdd.Add(String.Format("{0}//\n", pad));
			strAdd.Add(String.Format("{0}for (int i = 0; i < max; ++i)\n", pad));
			strAdd.Add(String.Format("{0}{{\n", pad));
			strAdd.Add(String.Format("{0}}}", pad));

			// Convert Strings to text
			string[] sa = new string[strAdd.Count];
			strAdd.CopyTo(0, sa, 0, strAdd.Count);

			//	remove the tag line
			io_EP.StartOfLine();
			//io_EP.LineUp(1);
			io_EP.Delete(i_sOrigLine.Length);

			//	add the new lines
			io_EP.StartOfLine();
			foreach (string s in sa)
			{
				io_EP.Insert(s);
			}

			io_EP.StartOfLine();
			io_EP.LineDown(4);

			io_TS.EndOfLine(false);
		}
	}
}
