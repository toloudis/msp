//****************************************************************************
/// \file CommentEditLineEvent.cs
///
///	the EditLine Event to create a for loop.
///
/// FIX: [rjk] the chunk of code to determine the type of comment (function,
///	class, etc) I took from someone's code is crap.  It needs to be rewritten.
///	
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
namespace CommentBuilder
{
	//------------------------------------------------------------------------
	/// Summary description for CommentEditLineEvent.
	//------------------------------------------------------------------------
	public class CommentEditLineEvent : DevEnvLib.EditLineEvent
	{
		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public CommentEditLineEvent()
		{
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		protected override string GetTag()
		{
			return "///";
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		protected override void InsertCode( ref EditPoint io_EP, ref TextSelection io_TS, ref string i_sOrigLine )
		{
			//	add the new lines
			io_EP.StartOfLine();
			io_EP.Delete(i_sOrigLine.Length);

			string sLine = i_sOrigLine.Trim();

			// Find the next line			
			int count;			
			string nextLine;
			if (DevEnvLib.EditPointHelpers.FindNextLine(io_EP, out count, out nextLine) == false) return;

			// Reposition the target
			io_EP.LineUp(count);
			io_EP.StartOfLine();

			//Find the Previous Line
			string prevLine;
			if (DevEnvLib.EditPointHelpers.FindPreviousLine(io_EP, out count, out prevLine) == false) return;

			// Reposition the target
			io_EP.LineUp(count);
			io_EP.StartOfLine();

			// Determine the Type of the line
			DevEnvLib.EditPointHelpers.LineType lt = DevEnvLib.EditPointHelpers.GetLineType(nextLine);

			// If the line type is a code element, see what type of line the previous one is
			switch (lt)
			{
				case DevEnvLib.EditPointHelpers.LineType.FUNCTION:
				case DevEnvLib.EditPointHelpers.LineType.VARIABLE:				
					DevEnvLib.EditPointHelpers.LineType plt = DevEnvLib.EditPointHelpers.GetLineType(prevLine);

					// if a comment already exists, do nothing
					// FIX: [rjk] - handle if comments already exist
					//if (plt == DevEnvLib.EditPointHelpers.LineType.COMMENT) return;
					break;

				default:

					//	if the comment is at the top of the file it must be a file comment
					//
					if ( io_TS.CurrentLine <= 2 )
					{
						lt = DevEnvLib.EditPointHelpers.LineType.FILE;
					}
					break;
			}

			// Build array of strings to add
			ArrayList strAdd = new ArrayList();

			// Process The Line
			switch (lt)
			{
				case DevEnvLib.EditPointHelpers.LineType.UNKNOWN:
					return;

				case DevEnvLib.EditPointHelpers.LineType.NAMESPACE:
					CommentBuilder.FormatComment.ForNamespace( nextLine, ref strAdd );
					return;

				case DevEnvLib.EditPointHelpers.LineType.CLASS:
				case DevEnvLib.EditPointHelpers.LineType.INTERFACE:
					CommentBuilder.FormatComment.ForClass( nextLine, ref strAdd );
					//GetClassListing(nextLine, ref strAdd);
					break;

				case DevEnvLib.EditPointHelpers.LineType.FUNCTION:
					CommentBuilder.FormatComment.ForFunction(nextLine, ref strAdd);
					//GetFunctionListing(nextLine, ref strAdd);
					break;

				case DevEnvLib.EditPointHelpers.LineType.FILE:
					CommentBuilder.FormatComment.ForFile(nextLine, ref strAdd);
					break;

				case DevEnvLib.EditPointHelpers.LineType.VARIABLE:
					GetVariableListing(nextLine, ref strAdd);
					break;

				case DevEnvLib.EditPointHelpers.LineType.COMMENT:
					GetCommentListing(nextLine, ref strAdd);
					break;
				default:
					int i = 8;	//junk for testing
					int x = i;
					break;
			}

			// Convert Strings to text
			string[] sa = new string[strAdd.Count];
			strAdd.CopyTo(0, sa, 0, strAdd.Count);

			// Add the lines
			io_EP.EndOfLine();
			foreach (string s in sa)
			{
				io_EP.Insert(s);
			}

			io_EP.LineDown(1);
			io_EP.Delete(1);			
			io_EP.EndOfLine();

			io_TS.EndOfLine(false);
		}
	
		
		//--------------------------------------------------------------------
		/// Get the listing for a comment line
		/// 
		/// \param lineThe next line
		/// \param alThe array of strings to place
		//--------------------------------------------------------------------
		private void GetCommentListing(string line, ref ArrayList al)
		{
			string pad = DevEnvLib.EditPointHelpers.GetPad( line );
			
			al.Add("\n");
			al.Add(String.Format("{0}/// ", pad));
		}

		//--------------------------------------------------------------------
		/// Get the listing for a variable line
		/// 
		/// \param lineThe next line
		/// \param alThe array of strings to place
		//--------------------------------------------------------------------
		private void GetVariableListing(string line, ref ArrayList al)
		{
			string pad = DevEnvLib.EditPointHelpers.GetPad( line );
			
			// Add the Summary section
			al.Add("<summary>\n");
			al.Add(String.Format("{0}/// \n", pad));

			// Close the Summary Section
			al.Add(String.Format("{0}///</summary>", pad));
		}

		//--------------------------------------------------------------------
		/// Get the listing for a class definition line
		/// 
		/// \param lineThe next line
		/// \param alThe array of strings to place
		//--------------------------------------------------------------------
		private void GetClassListing(string line, ref ArrayList al)
		{
			string pad = DevEnvLib.EditPointHelpers.GetPad( line );
			
			// Add the Summary section
			al.Add("<summary>\n");		
			al.Add(String.Format("{0}/// \n", pad));

			// Close the Summary Section
			al.Add(String.Format("{0}///</summary>", pad));
		}
		
	}
}
