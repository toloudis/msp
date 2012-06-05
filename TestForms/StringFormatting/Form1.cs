using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace StringFormatting
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.Label label_desc;
		private System.Windows.Forms.TabControl tabControl1;
		private System.Windows.Forms.TabPage tabPage_numbers;
		private System.Windows.Forms.GroupBox groupBox_float;
		private System.Windows.Forms.Label label_float1;
		private System.Windows.Forms.Label label_float2;
		private System.Windows.Forms.Label label_float3;
		private System.Windows.Forms.Label label_float4;
		private System.Windows.Forms.Label label_float8;
		private System.Windows.Forms.Label label_float7;
		private System.Windows.Forms.Label label_float5;
		private System.Windows.Forms.Label label_float6;
		private System.Windows.Forms.Label label_float9;
		private System.Windows.Forms.GroupBox groupBox_int;
		private System.Windows.Forms.Label label_integer1;

		const float c_floatnum1 = 12345.1234567f;
		private System.Windows.Forms.Label label_integer2;
		private System.Windows.Forms.Label label_integer3;
		private System.Windows.Forms.Label label_integer4;
		private System.Windows.Forms.Label label_integer5;
		private System.Windows.Forms.TabPage tabPage_date;
		private System.Windows.Forms.TabPage tabPage_color;
		private System.Windows.Forms.TabPage tabPage_strings;
		private System.Windows.Forms.Label label_date01;
		private System.Windows.Forms.Label label_date02;
		private System.Windows.Forms.Label label_date03;
		private System.Windows.Forms.Label label_date04;
		private System.Windows.Forms.Label label_date05;
		private System.Windows.Forms.Label label_date06;
		private System.Windows.Forms.Label label_date07;
		private System.Windows.Forms.Label label_date08;
		private System.Windows.Forms.Label label_date16;
		private System.Windows.Forms.Label label_date15;
		private System.Windows.Forms.Label label_date14;
		private System.Windows.Forms.Label label_date13;
		private System.Windows.Forms.Label label_date12;
		private System.Windows.Forms.Label label_date11;
		private System.Windows.Forms.Label label_date10;
		private System.Windows.Forms.Label label_date09;
		private System.Windows.Forms.TextBox textBox_multiline;
		const int c_integernum1 = 12345;

		public Form1()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//	numbers
			label_float1.Text = string.Format("{0}   unformatted", c_floatnum1);
			label_float2.Text = string.Format("{0:F}   F", c_floatnum1);
			label_float3.Text = string.Format("{0:F4}   F4", c_floatnum1);
			label_float4.Text = string.Format("{0:N}   N", c_floatnum1);
			label_float5.Text = string.Format("{0:#####.#}   #####.#", c_floatnum1);
			label_float6.Text = string.Format("{0:#####.######}   #####.######", c_floatnum1);
			label_float7.Text = string.Format("{0:###.#}   ###.#", c_floatnum1);
			label_float8.Text = string.Format("{0:.###}   .###", c_floatnum1);
			String tempstr = "{0:F" + 3 + "}   F w/variable percision";	// 3 could be a precision variable
			label_float9.Text = string.Format(tempstr, c_floatnum1);
			label_integer1.Text = string.Format("{0} unformatted", c_integernum1);
			label_integer2.Text = string.Format("{0:G} general", c_integernum1);
			label_integer3.Text = string.Format("{0:X} hexadecimal", c_integernum1);
			label_integer4.Text = string.Format("{0:N0} numerical", c_integernum1);
			label_integer5.Text = string.Format("{0:00000000} 00000000", c_integernum1);

			//	Date

//			    DateTime^ thisDate = DateTime::Now;
//    String^ resultString = "";

//    // Format the current date in various ways.
//    Console::WriteLine("Standard DateTime Format Specifiers");
//    resultString = String::Format(CultureInfo::CurrentCulture,
//        "(d) Short date: . . . . . . . {0:d}\n" +
//        "(D) Long date:. . . . . . . . {0:D}\n" +
//        "(t) Short time: . . . . . . . {0:t}\n" +
//        "(T) Long time:. . . . . . . . {0:T}\n" +
//        "(f) Full date/short time: . . {0:f}\n" +
//        "(F) Full date/long time:. . . {0:F}\n" +
//        "(g) General date/short time:. {0:g}\n" +
//        "(G) General date/long time: . {0:G}\n" +
//        "    (default):. . . . . . . . {0} (default = 'G')\n" +
//        "(M) Month:. . . . . . . . . . {0:M}\n" +
//        "(R) RFC1123:. . . . . . . . . {0:R}\n" +
//        "(s) Sortable: . . . . . . . . {0:s}\n" +
//        "(u) Universal sortable: . . . {0:u} (invariant)\n" +
//        "(U) Universal sortable: . . . {0:U}\n" +
//        "(Y) Year: . . . . . . . . . . {0:Y}\n",
//        thisDate);
//    Console::WriteLine(resultString);

			// Color

//    // Format a Color enumeration value in various ways.
//    Console::WriteLine("Standard Enumeration Format Specifiers");
//    resultString = String::Format(CultureInfo::CurrentCulture,
//        "(G) General:. . . . . . . . . {0:G}\n" +
//        "    (default):. . . . . . . . {0} (default = 'G')\n" +
//        "(F) Flags:. . . . . . . . . . {0:F} (flags or integer)\n" +
//        "(D) Decimal number: . . . . . {0:D}\n" +
//        "(X) Hexadecimal:. . . . . . . {0:X}\n",
//        Color::Green);
//    Console::WriteLine(resultString);

			// String
			this.textBox_multiline.Text = "One" + Environment.NewLine + "Two" + Environment.NewLine + "three";
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.label_float1 = new System.Windows.Forms.Label();
			this.label_float2 = new System.Windows.Forms.Label();
			this.label_desc = new System.Windows.Forms.Label();
			this.groupBox_float = new System.Windows.Forms.GroupBox();
			this.label_float9 = new System.Windows.Forms.Label();
			this.label_float8 = new System.Windows.Forms.Label();
			this.label_float7 = new System.Windows.Forms.Label();
			this.label_float5 = new System.Windows.Forms.Label();
			this.label_float6 = new System.Windows.Forms.Label();
			this.label_float4 = new System.Windows.Forms.Label();
			this.label_float3 = new System.Windows.Forms.Label();
			this.tabControl1 = new System.Windows.Forms.TabControl();
			this.tabPage_numbers = new System.Windows.Forms.TabPage();
			this.groupBox_int = new System.Windows.Forms.GroupBox();
			this.label_integer5 = new System.Windows.Forms.Label();
			this.label_integer4 = new System.Windows.Forms.Label();
			this.label_integer3 = new System.Windows.Forms.Label();
			this.label_integer2 = new System.Windows.Forms.Label();
			this.label_integer1 = new System.Windows.Forms.Label();
			this.tabPage_date = new System.Windows.Forms.TabPage();
			this.tabPage_color = new System.Windows.Forms.TabPage();
			this.tabPage_strings = new System.Windows.Forms.TabPage();
			this.label_date01 = new System.Windows.Forms.Label();
			this.label_date02 = new System.Windows.Forms.Label();
			this.label_date03 = new System.Windows.Forms.Label();
			this.label_date04 = new System.Windows.Forms.Label();
			this.label_date05 = new System.Windows.Forms.Label();
			this.label_date06 = new System.Windows.Forms.Label();
			this.label_date07 = new System.Windows.Forms.Label();
			this.label_date08 = new System.Windows.Forms.Label();
			this.label_date16 = new System.Windows.Forms.Label();
			this.label_date15 = new System.Windows.Forms.Label();
			this.label_date14 = new System.Windows.Forms.Label();
			this.label_date13 = new System.Windows.Forms.Label();
			this.label_date12 = new System.Windows.Forms.Label();
			this.label_date11 = new System.Windows.Forms.Label();
			this.label_date10 = new System.Windows.Forms.Label();
			this.label_date09 = new System.Windows.Forms.Label();
			this.textBox_multiline = new System.Windows.Forms.TextBox();
			this.groupBox_float.SuspendLayout();
			this.tabControl1.SuspendLayout();
			this.tabPage_numbers.SuspendLayout();
			this.groupBox_int.SuspendLayout();
			this.tabPage_date.SuspendLayout();
			this.tabPage_strings.SuspendLayout();
			this.SuspendLayout();
			// 
			// label_float1
			// 
			this.label_float1.Location = new System.Drawing.Point(8, 24);
			this.label_float1.Name = "label_float1";
			this.label_float1.Size = new System.Drawing.Size(136, 23);
			this.label_float1.TabIndex = 0;
			this.label_float1.Text = "float1";
			// 
			// label_float2
			// 
			this.label_float2.Location = new System.Drawing.Point(8, 48);
			this.label_float2.Name = "label_float2";
			this.label_float2.Size = new System.Drawing.Size(128, 23);
			this.label_float2.TabIndex = 1;
			this.label_float2.Text = "float2";
			// 
			// label_desc
			// 
			this.label_desc.Location = new System.Drawing.Point(24, 8);
			this.label_desc.Name = "label_desc";
			this.label_desc.Size = new System.Drawing.Size(296, 23);
			this.label_desc.TabIndex = 2;
			this.label_desc.Text = "Different Types of Formatting using string.Format()";
			// 
			// groupBox_float
			// 
			this.groupBox_float.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.groupBox_float.Controls.Add(this.label_float9);
			this.groupBox_float.Controls.Add(this.label_float8);
			this.groupBox_float.Controls.Add(this.label_float7);
			this.groupBox_float.Controls.Add(this.label_float5);
			this.groupBox_float.Controls.Add(this.label_float6);
			this.groupBox_float.Controls.Add(this.label_float4);
			this.groupBox_float.Controls.Add(this.label_float3);
			this.groupBox_float.Controls.Add(this.label_float1);
			this.groupBox_float.Controls.Add(this.label_float2);
			this.groupBox_float.Location = new System.Drawing.Point(8, 8);
			this.groupBox_float.Name = "groupBox_float";
			this.groupBox_float.Size = new System.Drawing.Size(368, 152);
			this.groupBox_float.TabIndex = 3;
			this.groupBox_float.TabStop = false;
			this.groupBox_float.Text = "Float";
			// 
			// label_float9
			// 
			this.label_float9.Location = new System.Drawing.Point(8, 120);
			this.label_float9.Name = "label_float9";
			this.label_float9.Size = new System.Drawing.Size(240, 23);
			this.label_float9.TabIndex = 8;
			this.label_float9.Text = "float9";
			// 
			// label_float8
			// 
			this.label_float8.Location = new System.Drawing.Point(192, 96);
			this.label_float8.Name = "label_float8";
			this.label_float8.Size = new System.Drawing.Size(168, 23);
			this.label_float8.TabIndex = 7;
			this.label_float8.Text = "float8";
			// 
			// label_float7
			// 
			this.label_float7.Location = new System.Drawing.Point(192, 72);
			this.label_float7.Name = "label_float7";
			this.label_float7.Size = new System.Drawing.Size(168, 23);
			this.label_float7.TabIndex = 6;
			this.label_float7.Text = "float7";
			// 
			// label_float5
			// 
			this.label_float5.Location = new System.Drawing.Point(192, 24);
			this.label_float5.Name = "label_float5";
			this.label_float5.Size = new System.Drawing.Size(168, 23);
			this.label_float5.TabIndex = 4;
			this.label_float5.Text = "float5";
			// 
			// label_float6
			// 
			this.label_float6.Location = new System.Drawing.Point(192, 48);
			this.label_float6.Name = "label_float6";
			this.label_float6.Size = new System.Drawing.Size(168, 23);
			this.label_float6.TabIndex = 5;
			this.label_float6.Text = "float6";
			// 
			// label_float4
			// 
			this.label_float4.Location = new System.Drawing.Point(8, 96);
			this.label_float4.Name = "label_float4";
			this.label_float4.Size = new System.Drawing.Size(128, 23);
			this.label_float4.TabIndex = 3;
			this.label_float4.Text = "float4";
			// 
			// label_float3
			// 
			this.label_float3.Location = new System.Drawing.Point(8, 72);
			this.label_float3.Name = "label_float3";
			this.label_float3.Size = new System.Drawing.Size(128, 23);
			this.label_float3.TabIndex = 2;
			this.label_float3.Text = "float3";
			// 
			// tabControl1
			// 
			this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.tabControl1.Controls.Add(this.tabPage_numbers);
			this.tabControl1.Controls.Add(this.tabPage_date);
			this.tabControl1.Controls.Add(this.tabPage_color);
			this.tabControl1.Controls.Add(this.tabPage_strings);
			this.tabControl1.Location = new System.Drawing.Point(8, 32);
			this.tabControl1.Name = "tabControl1";
			this.tabControl1.SelectedIndex = 0;
			this.tabControl1.Size = new System.Drawing.Size(392, 296);
			this.tabControl1.TabIndex = 4;
			// 
			// tabPage_numbers
			// 
			this.tabPage_numbers.Controls.Add(this.groupBox_int);
			this.tabPage_numbers.Controls.Add(this.groupBox_float);
			this.tabPage_numbers.Location = new System.Drawing.Point(4, 22);
			this.tabPage_numbers.Name = "tabPage_numbers";
			this.tabPage_numbers.Size = new System.Drawing.Size(384, 270);
			this.tabPage_numbers.TabIndex = 0;
			this.tabPage_numbers.Text = "Numbers";
			// 
			// groupBox_int
			// 
			this.groupBox_int.Controls.Add(this.label_integer5);
			this.groupBox_int.Controls.Add(this.label_integer4);
			this.groupBox_int.Controls.Add(this.label_integer3);
			this.groupBox_int.Controls.Add(this.label_integer2);
			this.groupBox_int.Controls.Add(this.label_integer1);
			this.groupBox_int.Location = new System.Drawing.Point(8, 160);
			this.groupBox_int.Name = "groupBox_int";
			this.groupBox_int.Size = new System.Drawing.Size(368, 104);
			this.groupBox_int.TabIndex = 4;
			this.groupBox_int.TabStop = false;
			this.groupBox_int.Text = "integer";
			// 
			// label_integer5
			// 
			this.label_integer5.Location = new System.Drawing.Point(192, 16);
			this.label_integer5.Name = "label_integer5";
			this.label_integer5.Size = new System.Drawing.Size(136, 23);
			this.label_integer5.TabIndex = 13;
			this.label_integer5.Text = "integer5";
			// 
			// label_integer4
			// 
			this.label_integer4.Location = new System.Drawing.Point(8, 64);
			this.label_integer4.Name = "label_integer4";
			this.label_integer4.Size = new System.Drawing.Size(136, 23);
			this.label_integer4.TabIndex = 12;
			this.label_integer4.Text = "integer4";
			// 
			// label_integer3
			// 
			this.label_integer3.Location = new System.Drawing.Point(8, 48);
			this.label_integer3.Name = "label_integer3";
			this.label_integer3.Size = new System.Drawing.Size(136, 23);
			this.label_integer3.TabIndex = 11;
			this.label_integer3.Text = "integer3";
			// 
			// label_integer2
			// 
			this.label_integer2.Location = new System.Drawing.Point(8, 32);
			this.label_integer2.Name = "label_integer2";
			this.label_integer2.Size = new System.Drawing.Size(136, 23);
			this.label_integer2.TabIndex = 10;
			this.label_integer2.Text = "integer2";
			// 
			// label_integer1
			// 
			this.label_integer1.Location = new System.Drawing.Point(8, 16);
			this.label_integer1.Name = "label_integer1";
			this.label_integer1.Size = new System.Drawing.Size(136, 23);
			this.label_integer1.TabIndex = 9;
			this.label_integer1.Text = "integer1";
			// 
			// tabPage_date
			// 
			this.tabPage_date.Controls.Add(this.label_date16);
			this.tabPage_date.Controls.Add(this.label_date15);
			this.tabPage_date.Controls.Add(this.label_date14);
			this.tabPage_date.Controls.Add(this.label_date13);
			this.tabPage_date.Controls.Add(this.label_date12);
			this.tabPage_date.Controls.Add(this.label_date11);
			this.tabPage_date.Controls.Add(this.label_date10);
			this.tabPage_date.Controls.Add(this.label_date09);
			this.tabPage_date.Controls.Add(this.label_date08);
			this.tabPage_date.Controls.Add(this.label_date07);
			this.tabPage_date.Controls.Add(this.label_date06);
			this.tabPage_date.Controls.Add(this.label_date05);
			this.tabPage_date.Controls.Add(this.label_date04);
			this.tabPage_date.Controls.Add(this.label_date03);
			this.tabPage_date.Controls.Add(this.label_date02);
			this.tabPage_date.Controls.Add(this.label_date01);
			this.tabPage_date.Location = new System.Drawing.Point(4, 22);
			this.tabPage_date.Name = "tabPage_date";
			this.tabPage_date.Size = new System.Drawing.Size(384, 270);
			this.tabPage_date.TabIndex = 1;
			this.tabPage_date.Text = "Date";
			// 
			// tabPage_color
			// 
			this.tabPage_color.Location = new System.Drawing.Point(4, 22);
			this.tabPage_color.Name = "tabPage_color";
			this.tabPage_color.Size = new System.Drawing.Size(384, 270);
			this.tabPage_color.TabIndex = 2;
			this.tabPage_color.Text = "Color";
			// 
			// tabPage_strings
			// 
			this.tabPage_strings.Controls.Add(this.textBox_multiline);
			this.tabPage_strings.Location = new System.Drawing.Point(4, 22);
			this.tabPage_strings.Name = "tabPage_strings";
			this.tabPage_strings.Size = new System.Drawing.Size(384, 270);
			this.tabPage_strings.TabIndex = 3;
			this.tabPage_strings.Text = "Strings";
			// 
			// label_date01
			// 
			this.label_date01.Location = new System.Drawing.Point(8, 8);
			this.label_date01.Name = "label_date01";
			this.label_date01.TabIndex = 0;
			this.label_date01.Text = "label1";
			// 
			// label_date02
			// 
			this.label_date02.Location = new System.Drawing.Point(8, 34);
			this.label_date02.Name = "label_date02";
			this.label_date02.TabIndex = 1;
			this.label_date02.Text = "label1";
			// 
			// label_date03
			// 
			this.label_date03.Location = new System.Drawing.Point(8, 60);
			this.label_date03.Name = "label_date03";
			this.label_date03.TabIndex = 2;
			this.label_date03.Text = "label2";
			// 
			// label_date04
			// 
			this.label_date04.Location = new System.Drawing.Point(8, 86);
			this.label_date04.Name = "label_date04";
			this.label_date04.TabIndex = 3;
			this.label_date04.Text = "label3";
			// 
			// label_date05
			// 
			this.label_date05.Location = new System.Drawing.Point(8, 112);
			this.label_date05.Name = "label_date05";
			this.label_date05.TabIndex = 4;
			this.label_date05.Text = "label4";
			// 
			// label_date06
			// 
			this.label_date06.Location = new System.Drawing.Point(8, 138);
			this.label_date06.Name = "label_date06";
			this.label_date06.TabIndex = 5;
			this.label_date06.Text = "label5";
			// 
			// label_date07
			// 
			this.label_date07.Location = new System.Drawing.Point(8, 164);
			this.label_date07.Name = "label_date07";
			this.label_date07.TabIndex = 6;
			this.label_date07.Text = "label6";
			// 
			// label_date08
			// 
			this.label_date08.Location = new System.Drawing.Point(8, 190);
			this.label_date08.Name = "label_date08";
			this.label_date08.TabIndex = 7;
			this.label_date08.Text = "label7";
			// 
			// label_date16
			// 
			this.label_date16.Location = new System.Drawing.Point(176, 190);
			this.label_date16.Name = "label_date16";
			this.label_date16.TabIndex = 15;
			this.label_date16.Text = "label8";
			// 
			// label_date15
			// 
			this.label_date15.Location = new System.Drawing.Point(176, 164);
			this.label_date15.Name = "label_date15";
			this.label_date15.TabIndex = 14;
			this.label_date15.Text = "label9";
			// 
			// label_date14
			// 
			this.label_date14.Location = new System.Drawing.Point(176, 138);
			this.label_date14.Name = "label_date14";
			this.label_date14.TabIndex = 13;
			this.label_date14.Text = "label10";
			// 
			// label_date13
			// 
			this.label_date13.Location = new System.Drawing.Point(176, 112);
			this.label_date13.Name = "label_date13";
			this.label_date13.TabIndex = 12;
			this.label_date13.Text = "label11";
			// 
			// label_date12
			// 
			this.label_date12.Location = new System.Drawing.Point(176, 86);
			this.label_date12.Name = "label_date12";
			this.label_date12.TabIndex = 11;
			this.label_date12.Text = "label12";
			// 
			// label_date11
			// 
			this.label_date11.Location = new System.Drawing.Point(176, 60);
			this.label_date11.Name = "label_date11";
			this.label_date11.TabIndex = 10;
			this.label_date11.Text = "label13";
			// 
			// label_date10
			// 
			this.label_date10.Location = new System.Drawing.Point(176, 34);
			this.label_date10.Name = "label_date10";
			this.label_date10.TabIndex = 9;
			this.label_date10.Text = "label14";
			// 
			// label_date09
			// 
			this.label_date09.Location = new System.Drawing.Point(176, 8);
			this.label_date09.Name = "label_date09";
			this.label_date09.TabIndex = 8;
			this.label_date09.Text = "label1";
			// 
			// textBox_multiline
			// 
			this.textBox_multiline.AcceptsReturn = true;
			this.textBox_multiline.AcceptsTab = true;
			this.textBox_multiline.Location = new System.Drawing.Point(8, 8);
			this.textBox_multiline.Multiline = true;
			this.textBox_multiline.Name = "textBox_multiline";
			this.textBox_multiline.Size = new System.Drawing.Size(200, 176);
			this.textBox_multiline.TabIndex = 0;
			this.textBox_multiline.Text = "";
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(408, 334);
			this.Controls.Add(this.tabControl1);
			this.Controls.Add(this.label_desc);
			this.Name = "Form1";
			this.Text = "Form1";
			this.groupBox_float.ResumeLayout(false);
			this.tabControl1.ResumeLayout(false);
			this.tabPage_numbers.ResumeLayout(false);
			this.groupBox_int.ResumeLayout(false);
			this.tabPage_date.ResumeLayout(false);
			this.tabPage_strings.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}
	}
}
