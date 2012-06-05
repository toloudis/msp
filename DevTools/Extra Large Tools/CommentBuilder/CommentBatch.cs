//****************************************************************************
/// \file CommentBatch.cs
///
///	Comment batch handling functionality
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
using System;
using EnvDTE;


//============================================================================
///
//============================================================================
namespace CommentBuilder
{
	//------------------------------------------------------------------------
	/// Summary description for CommentBatch.
	//------------------------------------------------------------------------
	public class CommentBatch
	{
		//--------------------------------------------------------------------
		// FIX: [rjk] these things shouldn't be here.  they are for testing.
		//--------------------------------------------------------------------
		static private _DTE m_Application;

		//--------------------------------------------------------------------
		///	set up the variables
		//--------------------------------------------------------------------
		public static void Init(object application)
		{
			m_Application = (_DTE)application;
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public static void RebuildCommentsForClassFunction(CodeElement i_CodeElement)
		{
			CodeFunction cfunc = (CodeFunction)i_CodeElement;

			int linenum = i_CodeElement.GetStartPoint(vsCMPart.vsCMPartHeader).Line;
			TextSelection ts = (TextSelection) i_CodeElement.DTE.ActiveWindow.Selection;
			EditPoint ep = ts.ActivePoint.CreateEditPoint();
			ep.MoveToLineAndOffset(linenum,1);

			string origvarline = ep.GetText(ep.LineLength);
			string varline = origvarline;
			int idx = varline.IndexOf(varline.Trim());
			string pad;
			if (idx<0) 
				pad = "";
			else 
				pad = origvarline.Substring(0, idx);			

			DevEnvLib.DebugOutput.Message(String.Format("function {0} -  line#{1}\n", cfunc.Name, linenum));

			string origcomments;
			string newcomments;
			origcomments = cfunc.Comment;
			newcomments  = FormatComment.GetCommentLineForFunction(pad);

			try
			{
				cfunc.DTE.UndoContext.Open("Insert Function Comments",false);

				// grab the old comment block
				//
				string substring;
				int lastfound = 1;
				int found;
				while ((found = origcomments.IndexOf("\r",lastfound)) != -1)
				{
					found++;
					substring = origcomments.Substring(lastfound, (found-lastfound));
					if (!IsStringCommentLine(substring))
					{
						newcomments += pad;
						newcomments += "//";	// the Comment command removes two slashes
						newcomments += substring;
					}
					lastfound = found; // + 1;
				}

				substring = origcomments.Substring(lastfound, ((origcomments.Length-lastfound)));
				if (!IsStringCommentLine(substring))
				{
					newcomments += pad;
					newcomments += "//";	// the Comment command removes two slashes
					newcomments += substring;
				}

				//	parameters
				//
				//ToDo [rjk] need to PRESERVE the old parameter comments!
				CodeElements cparams = cfunc.Parameters;
				if ( cparams.Count > 0 )
				{
					foreach (CodeElement cparam in cparams)
					{
						newcomments += pad + "/// \\param " + cparam.Name + " \n";
					}
				}

				//	return value
				//
				//ToDo [rjk] need to PRESERVE the old parameter comments!
				if (cfunc.Type.ToString() != "void")
				{
					newcomments += pad + "/// \\return \n";
				}

				newcomments += FormatComment.GetCommentLineForFunction(pad);

				//cfunc.Comment = string.Empty;

				// Add the lines
				ep.Insert(newcomments);
			}
			finally
			{
				cfunc.DTE.UndoContext.Close();
			}
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public static void RebuildCommentsForClassFunctions(CodeClass i_CodeClass)
		{
			DevEnvLib.DebugOutput.Message(String.Format("functions = {0}\n", i_CodeClass.Members.Count));

			foreach (CodeElement CElmt in i_CodeClass.Members)
			{
				if (CElmt.Kind == vsCMElement.vsCMElementFunction)
				{
					CodeFunction cfunc = (CodeFunction)CElmt;
					RebuildCommentsForClassFunction( CElmt );
				}
			}
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		private static bool IsStringCommentLine(string i_String)
		{
			if (   (i_String.StartsWith("==="))
				|| (i_String.StartsWith("***"))
				|| (i_String.StartsWith("---")))
			{
				return true;
			}
			return false;
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public static void RebuildCommentsForClass(CodeElement i_CodeElement)
		{
			CodeClass cclass = (CodeClass)i_CodeElement;

			int linenum = i_CodeElement.GetStartPoint(vsCMPart.vsCMPartHeader).Line;
			TextSelection ts = (TextSelection) i_CodeElement.DTE.ActiveWindow.Selection;
			EditPoint ep = ts.ActivePoint.CreateEditPoint();
			ep.MoveToLineAndOffset(linenum,1);

			string origvarline = ep.GetText(ep.LineLength);
			string varline = origvarline;
			int idx = varline.IndexOf(varline.Trim());
			string pad;
			if (idx<0) 
				pad = "";
			else 
				pad = origvarline.Substring(0, idx);			

			DevEnvLib.DebugOutput.Message(String.Format("class {0} -  line#{1}\n", cclass.Name, linenum));

			string origcomments;
			string newcomments;
			origcomments = cclass.Comment;
			newcomments  = FormatComment.GetCommentLineForClass(pad);

			try
			{
				cclass.DTE.UndoContext.Open("Insert Class Comments",false);

				// grab the old comment block
				//
				int lastfound = 1;
				int found;
				while ((found = origcomments.IndexOf("\r",lastfound)) != -1)
				{
					found++;
					if (!IsStringCommentLine(origcomments.Substring(lastfound, (found-lastfound))))
					{
						newcomments += pad;
						newcomments += "//";	// the Comment command removes two slashes
						newcomments += origcomments.Substring(lastfound, (found-lastfound));
					}
					lastfound = found; // + 1;
				}

				if (!IsStringCommentLine(origcomments.Substring(lastfound, ((origcomments.Length-lastfound)))))
				{
					newcomments += pad;
					newcomments += "//";	// the Comment command removes two slashes
					newcomments += origcomments.Substring(lastfound, ((origcomments.Length-lastfound)));
				}

				newcomments  += FormatComment.GetCommentLineForClass(pad);

				ep.MoveToLineAndOffset(linenum,1);

				cclass.Comment = string.Empty;

				// Add the lines
				ep.Insert(newcomments);
			}
			finally
			{
				cclass.DTE.UndoContext.Close();
			}

			//	Now, do all the functions for the class
			//
			RebuildCommentsForClassFunctions( cclass );
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public static void RebuildCommentsForDocument(Document i_Doc)
		{
			ProjectItem projitem = i_Doc.ProjectItem;
			FileCodeModel fcModel = projitem.FileCodeModel;
			CodeElements CElmts = fcModel.CodeElements;
			CodeElement CElmt;

			for ( int i = 1; i <= CElmts.Count ; ++i )
			{
				CElmt = CElmts.Item(i);
				
				string kindstring = CElmt.Kind.ToString();

				if (CElmt.Kind == vsCMElement.vsCMElementAttribute)
				{
					i = i;
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementClass)
				{
					RebuildCommentsForClass( CElmt );
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementDelegate)
				{
					i = i;
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementEnum)
				{
					i = i;
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementFunction)
				{
					RebuildCommentsForClassFunction( CElmt );
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementImportStmt)
				{
					i = i;
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementIncludeStmt)
				{
					i = i;
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementInterface)
				{
					i = i;
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementMacro)
				{
					i = i;
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementModule)
				{
					i = i;
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementNamespace)
				{
					i = i;
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementParameter)
				{
					i = i;
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementProperty)
				{
					i = i;
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementStruct)
				{
					i = i;
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementTypeDef)
				{
					i = i;
				}
				else if (CElmt.Kind == vsCMElement.vsCMElementVariable)
				{
					i = i;
				}
				else
				{
					i = i;
				}
			}
		}							

		//--------------------------------------------------------------------
		///	Functions for perusing the entire active solution 
		//--------------------------------------------------------------------
		public static void RebuildCommentsForProjectItem(ProjectItem i_ProjItem)
		{
			string filename;
			filename = i_ProjItem.get_FileNames(0);
			
			//ToDo [rjk] need to remove this if statement at some point
			// FOT TESTING ONLY
			if (filename.IndexOf("appEvent.") == -1)
			{
				return;
			}

			bool bPIOpen = (i_ProjItem.get_IsOpen(Constants.vsViewKindAny));
			DevEnvLib.DebugOutput.Message(String.Format("  {0} [{1}]\n", filename, bPIOpen ));

			if ( !bPIOpen )
			{
				EnvDTE.Window win = i_ProjItem.Open(Constants.vsViewKindCode);
			}
			
			//	ignore the .cpp because the code will load the .cpp based on the .hpp
			if (!DevEnvLib.DocHelper.IsHeader(i_ProjItem.Document))
			{
//				Document doc;
//				doc.FullName = i_ProjItem.Document.FullName;
//				doc.FullName.Replace(".h",".c");
//
//				doc.Activate();

				RebuildCommentsForDocument( i_ProjItem.Document );
			}
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public static void RebuildCommentsForProject(Project i_Proj)
		{
			ProjectItems pis = i_Proj.ProjectItems;
			DevEnvLib.DebugOutput.Message(String.Format("projects = {0}\n", pis.Count));

			foreach (ProjectItem pi in pis)
			{
				RebuildCommentsForProjectItem( pi );
			}
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public static void RebuildCommentsForActiveSolution(object application)
		{
			Init(application);	// must be here for now.

			Array activeProjects = (Array)m_Application.ActiveSolutionProjects;
			foreach (Project proj in m_Application.Solution.Projects)
			{
				RebuildCommentsForProject( proj );
			}
		}
	}
}
