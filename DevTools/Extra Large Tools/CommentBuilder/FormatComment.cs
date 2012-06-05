//****************************************************************************
/// \file FormatComment.cs
///
///	Format a comment based on type
///
///	Note: 
///	1) improvements would to make constants for:
///		- comment start ("//")
///		- spaces per tab (currently four)
///		- start/end tag for summary
///		- start/end tag for parameter
///		- start/end tag for return value
///	2) after #1, make this switchable between Doxygen version and NDoc
///	standards.
///	3) make these things data driven
///		
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
using System;
using System.Collections;


//============================================================================
///
//============================================================================
namespace CommentBuilder
{
	//------------------------------------------------------------------------
	/// Summary description for FormatComment.
	//------------------------------------------------------------------------
	public class FormatComment
	{
		// TODO: [rjk] is there a way to get these comment lines to calculate
		//	to the correct length (= 79)?

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public static string GetCommentLineForFile(string i_Pad)
		{
			string commentline = "//******************************************************************************"; // 79 chars
			string temp = i_Pad;
			temp = temp.Replace("\t","    ");
			commentline = commentline.Remove(3,temp.Length+2);	// 2 for comment slashes
			commentline = commentline.Insert(0,i_Pad);
			return commentline;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public static string GetCommentLineForFunction(string i_Pad)
		{
			string commentline = "//------------------------------------------------------------------------------"; // 79 chars
			string temp = i_Pad;
			temp = temp.Replace("\t","    ");
			commentline = commentline.Remove(3,temp.Length+2);	// 2 for comment slashes
			commentline = commentline.Insert(0,i_Pad);
			return commentline;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public static string GetCommentLineForClass(string i_Pad)
		{
			string commentline = "//=============================================================================="; // 79 chars
			string temp = i_Pad;
			temp = temp.Replace("\t","    ");
			commentline = commentline.Remove(3,temp.Length+2);	// 2 for comment slashes
			commentline = commentline.Insert(0,i_Pad);
			return commentline;
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public static void ForClass(string line, ref ArrayList al)
		{
			// Get the proper padding for the entered line
			string tLine = line;
			int idx = tLine.IndexOf(tLine.Trim());
			string pad;
			if (idx<0) 
				pad = "";
			else 
				pad = tLine.Substring(0, idx);			

			al.Add(String.Format("{0}\n", GetCommentLineForClass(pad) ));
			al.Add(String.Format("{0}/// \n", pad));
			al.Add(String.Format("{0}\n", GetCommentLineForClass(pad) ));
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public static void ForFile(string line, ref ArrayList al)
		{
			al.Add(String.Format("{0}\n", GetCommentLineForFile("") ));
			al.Add(String.Format("/// \\file {0}.hpp \n", "filename"));
			al.Add(String.Format("/// \n" ));
			al.Add(String.Format("/// \t\n" ));
			al.Add(String.Format("/// \n" ));
			al.Add(String.Format("/// \tExtra Large Technology\n" ));
			al.Add(String.Format("/// \tCopyright (c) 2005 - All Rights Reserved\n" ));
			al.Add(String.Format("{0}\n", GetCommentLineForFile("") ));
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public static void ForFunction(string line, ref ArrayList al)
		{
			// Remove the attributes from the line
			string tLine = "";
			bool inAttribute = false;
			for (int i=0; i<line.Length; i++)
			{
				// Do we see start of attribute line
				if ( (line[i] == '[') && (inAttribute == false) )
				{
					inAttribute = true;
					continue;
				}

				if ( (line[i] == ']') && (inAttribute == true) )
				{
					inAttribute = false;
					continue;
				}

				if (inAttribute == true)
					continue;

				tLine += line[i];
			}

			// Get the proper padding for the entered line
			int idx = tLine.IndexOf(tLine.Trim());
			string pad;
			if (idx<0) 
				pad = "";
			else
			{
				//	if there is no index then the line is made up of purely
				//	padding, so use it.
				if (idx == 0)
				{
					pad = tLine;
				}
				else
				{
					pad = tLine.Substring(0, idx);
				}
			}
			
			// Add the Summary section
			al.Add(String.Format("{0}\n", GetCommentLineForFunction(pad)));
			al.Add(String.Format("{0}/// \n", pad));

			// Add the parameters
			int sIdx = tLine.IndexOf("(")+1;
			int eIdx = tLine.IndexOf(")");
			if (sIdx < eIdx)
			{
				al.Add(String.Format("{0}/// \n", pad));

				string strParams = tLine.Substring(sIdx, eIdx-sIdx).Trim();
				string[] pars = strParams.Split(',');
				int parsAdded = 0;
				for (int i=0; i<pars.Length; i++)				
				{
					// Get the string
					string s = pars[i];

					// Do not pay attention to the void keyword
					if (s == "void") continue;
					if (s.Length == 0) continue;

					// Add an end 
					//if (parsAdded == 0)
					//	al.Add("\n");

					// Split the string into pieces
					string[] subStr = s.Trim().Split(' ');				

					// Get the parameter name
					string param = "";
					if (subStr.Length <=2)
						param = subStr[subStr.Length-1];
					else
						param = subStr[1];

					// Build the string
					string txt = String.Format("{0}/// \\param {1}",pad, param);
					if (i < (pars.Length-1))
						txt += "\n";

					// Add the last item in the list
					al.Add(txt);

					// Indicate parameter has been added
					parsAdded++;
				}

				al.Add(String.Format("\n{0}/// \n", pad));
			}

			// Add a Returns section if needed	
			if ( idx > sIdx )
			{
				string temp = tLine.Substring(idx, sIdx-idx-1);
				string[] rVal = temp.Split(' ','\t');	
		
				if (   (rVal.Length >= 1)
					&& !(rVal[0].StartsWith("void"))
					)
				{
					// Add the return section
					al.Add(String.Format("{0}/// \\return \n", pad));
				}
			}

			// Add the end
			al.Add(String.Format("{0}\n", GetCommentLineForFunction(pad)));
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public static void ForNamespace(string line, ref ArrayList al)
		{
			FormatComment.ForClass(line, ref al);
		}
	}
}
