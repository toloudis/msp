//****************************************************************************
/// \file EditPointHelpers.cs
///
///	Helper functions for EditPoints.
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
using EnvDTE;
using System;
using System.Text.RegularExpressions;


//============================================================================
///
//============================================================================
namespace DevEnvLib
{
	//------------------------------------------------------------------------
	/// Summary description for EditPointHelpers.
	//------------------------------------------------------------------------
	public class EditPointHelpers
	{
		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public EditPointHelpers()
		{
		}

		//--------------------------------------------------------------------
		/// Find the next line in the sequence
		///
		/// \param epThe edit point to start at
		/// \param countThe number of lines from the start point to find the line
		/// \param nextLineThe next line text
		/// \return Was a line found
		//--------------------------------------------------------------------
		static public bool FindNextLine(EditPoint ep, out int count, out string nextLine) 
		{
			// Initialize Variables
			count = 0;
			nextLine = "";

			// Mark current position
			int startLine = ep.Line;

			// Determine total Number of lines in the text
			ep.EndOfDocument();
			int max = ep.Line;

			// Restore starting line
			ep.LineUp(max-startLine);

			// Loop through the lines
			do
			{
				// Move to next line
				ep.LineDown(1);
				count++;

				// Get the text
				ep.StartOfLine();
				nextLine = ep.GetText(ep.LineLength);

				// If text is on the line, see what kind of line it is
				if (nextLine.Length>0) 
				{
					// Get the type
					LineType lt = GetLineType(nextLine);

					// If unknown or attribute, continue looking
					if ( (lt != LineType.UNKNOWN) && (lt != LineType.ATTRIBUTE) )
						return true;

				}

				// If we reached max
				if (ep.Line >= max) return false;
				
			} while (true);
		}

		//--------------------------------------------------------------------
		/// Find the previous line in the sequence
		///
		/// \param epThe edit point to start at
		/// \param countThe number of lines from the start point to find the line
		/// \param prevLineThe previous line text
		/// 
		/// \return 
		//--------------------------------------------------------------------
		static public bool FindPreviousLine(EditPoint ep, out int count, out string prevLine)
		{
			// Initialize Variables
			count = 0;
			prevLine = "";

			bool ok = false;
			do
			{
				// Move to next line
				ep.LineUp(1);
				count--;

				// Get the text
				ep.StartOfLine();
				prevLine = ep.GetText(ep.LineLength);

				if (prevLine.Length>0) return true;

			} while (ok == false);

			return false;
		}
		
		
		//--------------------------------------------------------------------
		/// Return the type of line
		/// 
		/// \param textThe line to check agains
		/// 
		/// \return The type of line
		//--------------------------------------------------------------------
		static public LineType GetLineType(string text)
		{			
			// Look for an attribute
			//			if (Regex.IsMatch(text, "[0-9a-zA-Z]*[\\([0-9a-zA-Z\\* _,&=:]*\\)]") == true) return LineType.ATTRIBUTE;						
			//			if (Regex.IsMatch(text, "\\[\\([0-9a-zA-Z\\* _,&=:]*\\)\\]") == true) return LineType.ATTRIBUTE;						
			if (Regex.IsMatch(text, "\\[(.)*\\]") == true) return LineType.ATTRIBUTE;						

			// Look for function 
			if (Regex.IsMatch(text, "\\([0-9a-zA-Z\\* _,&=:]*\\)") == true) return LineType.FUNCTION;						

			// Look for Class Definition
			if (Regex.IsMatch(text, "[0-9a-zA-Z _]*class [0-9a-zA-Z _]+") == true) return LineType.CLASS;

			// Look for Interface Definition
			if (Regex.IsMatch(text, "[0-9a-zA-Z _]*interface [0-9a-zA-Z _]+") == true) return LineType.INTERFACE;

			// Look for Namespace Definition
			if (Regex.IsMatch(text, "[0-9a-zA-Z _]*namespace [0-9a-zA-Z _]+") == true) return LineType.NAMESPACE;

			// Look for Comment
			if (text.Trim().StartsWith("///") == true) return LineType.COMMENT;	
			if (text.Trim().StartsWith("//") == true) return LineType.COMMENT;	
		
			// Look for variable declaration
			if (text.IndexOf(";") >= 0) 
			{
				string txt = text.Substring(0, text.IndexOf(";")).Trim();			
				string[] vals = txt.Split(' ');
				if (vals.Length>=2) return  LineType.VARIABLE;				
			}

			// Unknown type
			return LineType.UNKNOWN;
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		static public string GetPad(string i_Line)
		{
			// Get the proper padding for the entered line
			int idx = i_Line.IndexOf(i_Line.Trim());
			string pad;

			if (idx < 0) 
			{
				pad = "";
			}
			else 
			{
				pad = i_Line.Substring(0, idx);			
			}

			return pad;
		}

		//--------------------------------------------------------------------
		/// Types of lines in C++ file to look for
		//--------------------------------------------------------------------
		public enum LineType
		{
			CLASS = 1,			/// Line represents a class definition
			FUNCTION,			/// Line represents a function
			VARIABLE,			/// Line represents a vairiable
			UNKNOWN,			/// Line is unknown
			COMMENT,			/// Within the bounds of a comment
			INTERFACE,			/// Interface definition
			ATTRIBUTE,			/// Line represents an attribute definition
			NAMESPACE,			/// Line represents a namespace
			FILE,				/// Line represents a file
		}
	}
}
