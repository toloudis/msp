//****************************************************************************
/// \file SwapCppHppPlugin
///
///	allows user to swap from hpp to cpp
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
using System;
using Microsoft.Office.Core;
using Extensibility;
using System.Runtime.InteropServices;
using EnvDTE;
using System.Collections;
using System.Globalization;


//============================================================================
///
//============================================================================
namespace CodePlugIn
{
	//------------------------------------------------------------------------
	/// 
	//------------------------------------------------------------------------
	public class SwapCppHppPlugin : DevEnvLib.VSPlugIn
	{
		#region public Attributes
		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		public override string cmdName
		{
			get
			{
				return "SwapCppHpp";
			}
		}

		//--------------------------------------------------------------------
		/// used for the button text
		//--------------------------------------------------------------------
		public override string shortDescription{get { return "Swap HPP <-> CPP";} }
		
		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		public override string longDescription { get {return "Swap from the header to source file and back"; }}
		
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
		public SwapCppHppPlugin(_DTE applicationDTE, AddIn addInInstance)
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
			return 	(vsCommandStatus)(vsCommandStatus.vsCommandStatusEnabled | vsCommandStatus.vsCommandStatusSupported);
		}

		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		protected override bool doExec()
		{
			return SwapCppHpp();
		}
		#endregion

		#region private methods
		//--------------------------------------------------------------------
		/// perform the swap
		/// 
		/// \return
		//--------------------------------------------------------------------
		private bool SwapCppHpp()
		{
			DevEnvLib.DocHelper.SwitchToHppOrCpp( m_ApplicationObject,(Document)m_ApplicationObject.ActiveDocument );
			return true;
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
	}
}
