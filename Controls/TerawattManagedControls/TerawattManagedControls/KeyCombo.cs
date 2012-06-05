using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace TerawattManagedControls
{
	/// <summary>
	/// Summary description for KeyCombo.
	/// </summary>
	public class KeyCombo : System.Windows.Forms.UserControl
	{
		private System.Windows.Forms.TextBox textBox_keycombo;

        private Keys m_Hotkey = Keys.None;
        private Keys m_Modifiers = Keys.None;
		private bool m_bUseOnlyValidMenuItemKeyCombos;

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		/// <summary>
		/// Callback when key changes
		/// </summary>
		[Category("Key Changed"), 
		Description("Callback when key changes")]
		public event KeyEventHandler KeyChanged;
		
		public KeyCombo()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			m_bUseOnlyValidMenuItemKeyCombos = false;
		}
		
		/// <summary>
		/// Specifies if this control accepts only valid MenuItem KeyCombo combos
		/// </summary>
		[Bindable(true), Category("Control"), 
		Description("Specifies if this control accepts only valid MenuItem KeyCombo combos")]
		public bool UseMenuItemKeyCombos
		{
			get { return this.m_bUseOnlyValidMenuItemKeyCombos; }
			set
			{
				this.m_bUseOnlyValidMenuItemKeyCombos = value;
			}
		}

        /// <summary>
        /// Hotkey value
        /// </summary>
        [Bindable(true), Category("Control"),
        Description("The value of the keycombo")]
        public int HotkeyValue
        {
            get { return (int)(this.m_Hotkey); }
            set
            {
                this.m_Hotkey = (Keys)value;
            }
        }

        /// <summary>
        /// Modifiers value
        /// </summary>
        [Bindable(true), Category("Control"),
        Description("The value of the Modifiers")]
        public int ModifiersValue
        {
            get { return (int)(this.m_Modifiers); }
            set
            {
                this.m_Modifiers = (Keys)value;
            }
        }

        /// <summary>
        /// Hotkey string
        /// </summary>
        [Bindable(true), Category("Control"),
        Description("The string of the keycombo")]
        public String KeyComboString
        {
            get 
            {
                return textBox_keycombo.Text; 
            }
            set
            {
                // TODO set the key + modifier values also
                textBox_keycombo.Text = value;
            }
        }

        /// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.textBox_keycombo = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// textBox_keycombo
			// 
			this.textBox_keycombo.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.textBox_keycombo.Location = new System.Drawing.Point(0, 0);
			this.textBox_keycombo.MaxLength = 32;
			this.textBox_keycombo.Name = "textBox_keycombo";
			this.textBox_keycombo.ReadOnly = true;
			this.textBox_keycombo.Size = new System.Drawing.Size(136, 20);
			this.textBox_keycombo.TabIndex = 1;
			this.textBox_keycombo.Text = "";
			this.textBox_keycombo.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_keycombo_KeyDown);
			// 
			// KeyCombo
			// 
			this.Controls.Add(this.textBox_keycombo);
			this.Name = "KeyCombo";
			this.Size = new System.Drawing.Size(136, 20);
			this.ResumeLayout(false);

		}
		#endregion

		private void textBox_keycombo_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
            //  if backspace is pressed, clear everything out.
            //
            if (e.KeyCode == System.Windows.Forms.Keys.Back)
            {
                m_Modifiers = 0;
                m_Hotkey = System.Windows.Forms.Keys.None;
                textBox_keycombo.Clear();
                textBox_keycombo.Text = "";
                e.Handled = true;

                //	let others know about the event
                //
                if (this.KeyChanged != null)
                    this.KeyChanged(this, e);
                return;
            }

            m_Modifiers = e.Modifiers;

            //  check for the standard modifier keys
            //
            if ((e.KeyCode != System.Windows.Forms.Keys.Menu)		// better known as "ALT"
                && (e.KeyCode != System.Windows.Forms.Keys.ControlKey)
                && (e.KeyCode != System.Windows.Forms.Keys.ShiftKey))
            {
                //	lock in the key code
                m_Hotkey = e.KeyCode;
            }
            else
            {
                m_Hotkey = Keys.None;
            }

			//	build the text box
			//
			String keycombo;
            KeysConverter kc = new KeysConverter();

            keycombo = kc.ConvertToString(m_Hotkey | e.Modifiers);
            //keycombo = String.Format("{0} + ", m_Modifiers.ToString());
            //keycombo = String.Concat(keycombo, m_Hotkey.ToString());

			//	if this flag is set, only display VALID (in conjunction with
			//	MenuItems) hot key combos
			//
			if (m_bUseOnlyValidMenuItemKeyCombos)
			{
				//	Get the keys.
				try
				{
					//	try to convert it and see if it throws an exception
					//
					TypeConverter keyconv = TypeDescriptor.GetConverter(typeof(Keys));
					Shortcut scut = (Shortcut)keyconv.ConvertFromString(keycombo);
				}
				catch (ArgumentNullException)
				{
					// if not a valid key combo, show nothing.
					keycombo = "";
				}
				catch (ArgumentException)
				{
					// if not a valid key combo, show nothing.
					keycombo = "";
				}
			}

			textBox_keycombo.Text = keycombo;
			e.Handled = true;

			//	let others know about the event
			//
			if (this.KeyChanged != null)
				this.KeyChanged(this, e);
		}
	}
}
