//****************************************************************************
/// \file FCMHelper.cs
///
///	Helper functions for the FileCodeModel system
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
using System;
using EnvDTE;
using Microsoft.Office.Core;


//============================================================================
///
//============================================================================
namespace DevEnvLib
{
	//------------------------------------------------------------------------
	/// Summary description for FCMHelper.
	//------------------------------------------------------------------------
	public class FCMHelper
	{
		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		static public CodeElement GetElementFromPoint( FileCodeModel i_FCM, TextPoint i_StartTP )
		{
			Type tElements = typeof(vsCMElement);
			foreach (vsCMElement kind in Enum.GetValues(tElements))
			{
				try
				{
					CodeElement element = i_FCM.CodeElementFromPoint(i_StartTP, kind);
					if (element != null)
					{
						if (element.Kind == kind)
							return element;
					}
				}
				catch (Exception ex)
				{
					string sex = ex.Message;
				}
			}

			return null;
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
//		static public CodeElement DebugRecurseElements( FileCodeModel i_FCM, int level )
//		{
//			Type tElements = typeof(vsCMElement);
//			foreach (vsCMElement kind in Enum.GetValues(tElements))
//			{
//				try
//				{
//					CodeElement element = i_FCM.CodeElementFromPoint(i_StartTP, kind);
//					if (element != null)
//					{
//						if (element.Kind == kind)
//							return element;
//					}
//				}
//				catch (Exception ex)
//				{
//					string sex = ex.Message;
//				}
//			}
//
//			return null;
//		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		static public void DebugRecurseElements( FileCodeModel i_FCM )
		{
			DevEnvLib.DebugOutput.Message("Code Parent = {0}", i_FCM.Parent.Name);

			if ( i_FCM != null )
			{
				RecurseCodeElements( i_FCM.CodeElements, 0 );
			}
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		static private CodeElements GetMembers( ref CodeElement i_CE )
		{
			if (i_CE != null)
			{
				switch (i_CE.Kind)
				{
					case vsCMElement.vsCMElementNamespace:
					{
						CodeNamespace elem = (CodeNamespace)i_CE;
						return elem.Members;
					}
					case vsCMElement.vsCMElementClass:
					{
						CodeClass elem = (CodeClass)i_CE;
						return elem.Members;
					}
					case vsCMElement.vsCMElementStruct:
					{
						CodeStruct elem = (CodeStruct)i_CE;
						return elem.Members;
					}
					case vsCMElement.vsCMElementDelegate:
					{
						CodeDelegate elem = (CodeDelegate)i_CE;
						return elem.Members;
					}
				}
			}

			return null;
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		static private void RecurseCodeElements( CodeElements i_CEs, int level )
		{
			string indent = new String(' ',level*4);
			CodeElement ce;
			CodeElements CEMembers;

			for ( int i=1; i <= i_CEs.Count; i++ )
			{
				ce = i_CEs.Item(i);
				
				DevEnvLib.DebugOutput.Message("{0}{1}", indent, ce.Name);

				CEMembers = GetMembers( ref ce );

				if (CEMembers != null)
				{
					RecurseCodeElements( CEMembers, level+1 );
				}
			}
		}

		//		static public Array GetProjects(_DTE i_App)
		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
//		static public Array GetProjects(_DTE i_App)
//		{
//			m_Application = i_App;
//
//			return (Array)m_Application.ActiveSolutionProjects;
//		}

		static private _DTE m_Application;
	}
}
