//****************************************************************************
/// \file GetSetPlugin
///
///	automatically create the Get/Set functions for a variable
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
using EnvDTE;
using Extensibility;
using Microsoft.Office.Core;
using System;
using System.Collections;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Text;


//============================================================================
///
//============================================================================
namespace CodePlugIn
{
	//------------------------------------------------------------------------
	/// 
	//------------------------------------------------------------------------
	public class GetSetPlugin : DevEnvLib.VSPlugIn
	{
		#region public Attributes
		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		public override string cmdName
		{
			get
			{
				return "GetSet";
			}
		}

		//--------------------------------------------------------------------
		/// used for the button text
		//--------------------------------------------------------------------
		public override string shortDescription{get { return "Create Get/Set";} }
		
		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		public override string longDescription { get {return "Create Get + Set functions for a variable"; }}
		
		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		public override int position { get {return  1;} }
		
		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		public override int iconId { get {return 54;} }
		#endregion

		#region construction
		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		public GetSetPlugin(_DTE applicationDTE, AddIn addInInstance)
			: base(applicationDTE, addInInstance)
		{
		}
		#endregion

		#region public methods
		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		protected override EnvDTE.vsCommandStatus doQueryStatus()
		{
			m_CV = (CodeVariable)this.selectedElement(vsCMElement.vsCMElementVariable);
			if (m_CV != null)
			{
				return vsCommandStatus.vsCommandStatusEnabled | vsCommandStatus.vsCommandStatusSupported;
			}
			return vsCommandStatus.vsCommandStatusUnsupported;
		}

		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		protected override bool doExec()
		{
			return doGetSet();
		}
		#endregion

		#region private methods
		//--------------------------------------------------------------------
		/// perform the swap
		/// 
		/// \return
		//--------------------------------------------------------------------
		private bool doGetSet()
		{
			// Only Process C/C++ Applications
			if (m_ApplicationObject.ActiveDocument == null) return false;

			DevEnvLib.DebugOutput.Message("language {0}", m_ApplicationObject.ActiveDocument.Language);

			if (m_ApplicationObject.ActiveDocument.Language != "C/C++")
			{
				return false;
			}

			int linenum = m_CV.GetStartPoint(vsCMPart.vsCMPartHeader).Line;
			TextSelection ts = (TextSelection) m_CV.DTE.ActiveWindow.Selection;
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

			//	Check to see if the get and set functions already exist.
			string varname = m_CV.Name;
			if (   (varname.StartsWith("m_"))
				|| (varname.StartsWith("c_")) )
			{
				varname = varname.Remove(0,2);
			}

			string funcname;
			string paramtype;
			ArrayList functionlisting = new ArrayList();
			paramtype = m_CV.Type.AsString;

			//	Get
			funcname = "Get" + varname;

			//	get a comment
			// TODO: [rjk] put a generic comment line in for these like "Get/Set for variable m_Whatever"
			int ndx = origvarline.IndexOf(origvarline.Trim());
			string cpad;
			if (ndx<0) 
				cpad = "";
			else 
			{
				cpad = origvarline.Substring(0, ndx);
			}
			CommentBuilder.FormatComment.ForFunction(cpad,ref functionlisting);

			//	build the get
			functionlisting.Add( string.Format("{0}{1} {2}()\n", pad, paramtype, funcname) );
			functionlisting.Add( string.Format("{0}{{\n", pad ) );
			functionlisting.Add( string.Format("{0}\treturn {1};\n", pad, m_CV.Name) );
			functionlisting.Add( string.Format("{0}}}\n", pad ) );

			//	build the set
			functionlisting.Add( string.Format("{0}void {1}({2} i_{3})\n", pad, funcname, paramtype, varname) );
			functionlisting.Add( string.Format("{0}{{\n", pad ) );
			functionlisting.Add( string.Format("{0}\t{1} = i_{2};\n", pad, m_CV.Name, varname) );
			functionlisting.Add( string.Format("{0}}}\n", pad ) );

			//	add the code
			foreach (string str in functionlisting)
			{
				ep.Insert(str);
			}

			return false;
		}

		//--------------------------------------------------------------------
		/// info function
		/// 
		/// \param element
		//--------------------------------------------------------------------
		private void showInfo (CodeElement element)
		{
		}
		#endregion

		private CodeVariable m_CV;
	}
}
