//****************************************************************************
/// \file SwitchEditLineEvent
///
///	the EditLine Event for creating a switch statement
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
	/// Summary description for SwitchEditLineEvent.
	//------------------------------------------------------------------------
	public class SwitchEditLineEvent : EditLineEvent
	{
		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public SwitchEditLineEvent()
		{
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		protected override string GetTag()
		{
			return "!switch";
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		protected override void InsertCode( ref EditPoint io_EP, ref TextSelection io_TS, ref string i_sOrigLine )
		{
			string pad = EditPointHelpers.GetPad( i_sOrigLine );
			
			// Build array of strings to add
			ArrayList strAdd = new ArrayList();

			strAdd.Add(String.Format("{0}// switch\n", pad));
			strAdd.Add(String.Format("{0}//\n", pad));
			strAdd.Add(String.Format("{0}switch (x)\n", pad));
			strAdd.Add(String.Format("{0}{{\n", pad));
			strAdd.Add(String.Format("{0}\tcase 0:\n", pad));
			strAdd.Add(String.Format("{0}\t{{\n", pad));
			strAdd.Add(String.Format("{0}\t\tbreak;\n", pad));
			strAdd.Add(String.Format("{0}\t}}\n", pad));
			strAdd.Add(String.Format("{0}\tcase 1:\n", pad));
			strAdd.Add(String.Format("{0}\t{{\n", pad));
			strAdd.Add(String.Format("{0}\t\tbreak;\n", pad));
			strAdd.Add(String.Format("{0}\t}}\n", pad));
			strAdd.Add(String.Format("{0}\tcase 2:\n", pad));
			strAdd.Add(String.Format("{0}\t{{\n", pad));
			strAdd.Add(String.Format("{0}\t\tbreak;\n", pad));
			strAdd.Add(String.Format("{0}\t}}\n", pad));
			strAdd.Add(String.Format("{0}\tdefault:\n", pad));
			strAdd.Add(String.Format("{0}\t{{\n", pad));
			strAdd.Add(String.Format("{0}\t\tbreak;\n", pad));
			strAdd.Add(String.Format("{0}\t}}\n", pad));
			strAdd.Add(String.Format("{0}}}", pad));

			// Convert Strings to text
			string[] sa = new string[strAdd.Count];
			strAdd.CopyTo(0, sa, 0, strAdd.Count);

			//	remove the tag line
			io_EP.StartOfLine();
			//ep.LineUp(1);
			io_EP.Delete(i_sOrigLine.Length);

			io_EP.StartOfLine();
			foreach (string s in sa)
			{
				io_EP.Insert(s);
			}

			io_EP.StartOfLine();
			io_EP.LineDown(5);

			io_TS.EndOfLine(false);
		}
	}
}
