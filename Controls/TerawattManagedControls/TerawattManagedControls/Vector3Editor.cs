using System;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Reflection;
using System.Collections.Specialized;
using System.Drawing.Design;
using System.Drawing;
using System.Resources;
using System.Windows.Forms.Design;
using TerawattManagedControls;


namespace TerawattManagedControls
{
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	[System.ComponentModel.TypeConverter(typeof(Vector3EditorConverter))]
	[Editor(typeof(Vector3Editor), typeof(System.Drawing.Design.UITypeEditor))]
	[EditorAttribute(typeof(Vector3Editor), typeof(System.Drawing.Design.UITypeEditor))]
	public class Vector3Value
	{
		public Vector3Value(float i_X, float i_Y, float i_Z )
		{
			m_X = i_X;
			m_Y = i_Y;
			m_Z = i_Z;
		}
		public float m_X;
		public float m_Y;
		public float m_Z;

		public override string ToString()
		{
			int precision = 2; // FIX [rjk] this.precision;
			String tempstr = "{0:F" + precision + "}, " + "{1:F" + precision + "}, " + "{2:F" + precision + "}";
			return String.Format( tempstr, m_X, m_Y, m_Z );
		}
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	/// <summary>
	/// Summary description for Vector3Editor.
	/// </summary>
	public class Vector3Editor : UITypeEditor
	{
		private Vector3Value m_V3Object;

		public Vector3Editor(float i_X, float i_Y, float i_Z )
		{
			m_V3Object = new Vector3Value(i_X, i_Y, i_Z);
		}
		public override UITypeEditorEditStyle GetEditStyle(
			ITypeDescriptorContext context)
		{
			return UITypeEditorEditStyle.DropDown;
		}

		public override object EditValue(ITypeDescriptorContext context,
			IServiceProvider provider, object value)
		{
			IWindowsFormsEditorService wfes = provider.GetService(
				typeof(IWindowsFormsEditorService)) as
				IWindowsFormsEditorService;
			
			if (wfes != null)
			{
				Vector3Edit _frmV3E = new Vector3Edit();

				_frmV3E.ValueX = (float) m_V3Object.m_X;
				_frmV3E.ValueY = (float) m_V3Object.m_Y;
				_frmV3E.ValueZ = (float) m_V3Object.m_Z;
				_frmV3E.m_WFES = wfes;
				
				wfes.DropDownControl(_frmV3E);
				value = m_V3Object;
			}
			return value;
		}
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	public class Vector3EditorConverter : TypeConverter
	{
		public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
		{
			if (sourceType.Equals(typeof(string)))
			{
				return true;
			}
			else
			{
				return base.CanConvertFrom(context, sourceType);
			}
		}

		public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value)
		{
			if (value.GetType() == typeof(string))
			{
				// Parse property string
				//const int lc_NUMPARAMS = 3;
				string[] ss = value.ToString().Split(new char[] {'\''}, 3);
				try
				{
					if (ss.Length!=3)
					{
						// TODO [rjk] what should we do in this case?
					}
					return new Vector3Value( Int32.Parse(ss[0]), Int32.Parse(ss[1]), Int32.Parse(ss[2]) );
				}
				catch
				{
					throw new InvalidCastException(value.ToString());
				}
			}
			else
			{
				return base.ConvertFrom(context, culture, value);
			}
		}

		public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
		{
			if (destinationType.Equals( typeof(string) ))
			{
				return true;
			}
			else
			{
				return base.CanConvertTo(context, destinationType);
			}
		}

		public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
		{
			if (destinationType.Equals(typeof(string)))
			{
				return value.ToString();
			}
			else
			{

				return base.ConvertTo(context, culture, value, destinationType);
			}
		}

		public override bool GetPropertiesSupported(ITypeDescriptorContext context)
		{
			return true;
		}

		public override PropertyDescriptorCollection GetProperties(ITypeDescriptorContext context, object value, Attribute[] attributes)
		{
			return TypeDescriptor.GetProperties(value);
		}

	}
}

//	public class RuleConverter : StringConverter
//	{
//
//		public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
//		{
//			//true means show a combobox
//			return true;
//		}
//
//		public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
//		{
//			//true will limit to list. false will show the list, but allow free-form entry
//			return true;
//		}
//
//		public override
//			System.ComponentModel.TypeConverter.StandardValuesCollection
//			GetStandardValues(ITypeDescriptorContext context)
//		{
//			return new StandardValuesCollection(HE_GlobalVars._ListofRules);
//		}
//
//	}
//
//	public class HE_Task
//	{
//		private string _Rule;
//		private HE_SourceType _SourceType;
//		private int _Contrast;
//
//		[Browsable(true)]
//		[TypeConverter(typeof(RuleConverter))]
//		public string Rule
//		{
//		
//			//When first loaded set property with the first item in the rule list.
//			get	
//			{
//				string S = "";
//				if (_Rule != null)
//				{
//					S = _Rule;
//				}
//				else
//				{
//					if (HE_GlobalVars._ListofRules.Length > 0)
//					{
//						//Sort the list before displaying it
//						Array.Sort(HE_GlobalVars._ListofRules);
//
//						S = HE_GlobalVars._ListofRules[0];
//					}
//				}
//
//				return S;
//			}
//			set{ _Rule = value; }
//
//		}
//
//		[Editor(typeof(SourceTypePropertyGridEditor),
//			 typeof(System.Drawing.Design.UITypeEditor))]
//		public		HE_SourceType		SourceType
//		{
//			get{return _SourceType;}
//			set{_SourceType = value;}
//		}
//
//		[Editor(typeof(ContrastEditor),
//			 typeof(System.Drawing.Design.UITypeEditor))]
//		public		int	Contrast
//		{
//			get{return _Contrast;}
//			set{_Contrast = value;}
//		}
//	}
//
//	
//}
