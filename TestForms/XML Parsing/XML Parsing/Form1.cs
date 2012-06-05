using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace XML_Parsing
{
    public partial class Form_Xml : Form
    {
        public Form_Xml()
        {
            InitializeComponent();
        }

        private void button_basic_write_Click(object sender, EventArgs e)
        {
            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;

            XmlWriter wtr = XmlTextWriter.Create("basic.xml", settings);

            wtr.WriteStartElement("Test");

            wtr.WriteStartElement("Test-inner1");
            wtr.WriteElementString("e-one", "100");
            wtr.WriteEndElement();

            wtr.WriteStartElement("Test-inner2");
            wtr.WriteElementString("e-two", "abc");

            wtr.WriteStartElement("Test-inner2b");
            wtr.WriteElementString("e-three", "hello");
            wtr.WriteEndElement();

            wtr.WriteEndElement();
            wtr.WriteEndElement();

            wtr.Close();
        }

        private void button_basic_read_Click(object sender, EventArgs e)
        {
            this.textBox_basic_output.Clear();

            XmlReaderSettings settings = new XmlReaderSettings();
            settings.IgnoreWhitespace = true;
            settings.IgnoreComments = true;
            settings.IgnoreProcessingInstructions = true;
            settings.ProhibitDtd = true;

            XmlReader rdr = XmlTextReader.Create("basic.xml", settings);
            while (rdr.Read())
            {
                String str;
                this.textBox_basic_output.AppendText("\r\n");
                str = rdr.Name;
                this.textBox_basic_output.AppendText(str);
                this.textBox_basic_output.AppendText(" - ");
                str = rdr.Value;
                this.textBox_basic_output.AppendText(str);
            }
            rdr.Close();
        }

        private void button_prefscfg_Click(object sender, EventArgs e)
        {
            this.textBox_prefscfg.Clear();

            //  settings
            XmlReaderSettings settings = new XmlReaderSettings();
            settings.IgnoreWhitespace = true;
            settings.IgnoreComments = true;
            //settings.IgnoreProcessingInstructions = true;

            //	quick and dirty XML reader
            XmlReader rdr = XmlReader.Create("..\\..\\Prefs.cfg", settings);

            //	skip the XML header
            rdr.MoveToContent();

            //	read in the preferences
            //
            //rdr.ReadStartElement("Preferences");
            String name;
            String value;
            while (rdr.Read())
            {
			    if (rdr.NodeType == System.Xml.XmlNodeType.Element)
			    {
				    name = rdr.Name.ToString();
				    name = name.Replace('_',' ');	// elements cannot have spaces, so remove the underscores

                    this.textBox_prefscfg.AppendText(name);
                    this.textBox_prefscfg.AppendText("-");
                }
			    else if (rdr.NodeType == System.Xml.XmlNodeType.Text)
			    {
				    value = rdr.Value.ToString();
                    this.textBox_prefscfg.AppendText(value);
                }
			    else if (rdr.NodeType == System.Xml.XmlNodeType.EndElement)
			    {
                    this.textBox_prefscfg.AppendText("\r\n");
			    }
            }

            rdr.Close();
        }

        private void button_readprefs_text_Click(object sender, EventArgs e)
        {
            this.textBox_prefscfg.Clear();

            //  settings
            XmlReaderSettings settings = new XmlReaderSettings();
            settings.IgnoreWhitespace = true;
            settings.IgnoreComments = true;
            //settings.IgnoreProcessingInstructions = true;

            //	quick and dirty XML reader
            XmlTextReader rdr = new XmlTextReader("..\\..\\Prefs.cfg");

            //	skip the XML header
            rdr.Read(); // MoveToContent();

            //	read in the preferences
            //
            rdr.ReadStartElement("Preferences");
            while (rdr.Read())
            {
                this.textBox_prefscfg.AppendText(rdr.Name);
                this.textBox_prefscfg.AppendText("-");
                this.textBox_prefscfg.AppendText(rdr.Value);
                this.textBox_prefscfg.AppendText("- {");
                this.textBox_prefscfg.AppendText(rdr.NodeType.ToString());
                this.textBox_prefscfg.AppendText("}\r\n");
            }

            rdr.Close();
        }

        private void button_prefsout_Click(object sender, EventArgs e)
        {
		    //	quick and dirty XML Writer

		    //	Write out the XML config file
		    //
		    XmlWriterSettings wtrset = new XmlWriterSettings();
		    wtrset.Indent = true;//Formatting.Indented;

            XmlWriter wtr = XmlTextWriter.Create("..\\..\\PrefsOut.cfg", wtrset);

		    //	write the preferences
		    wtr.WriteStartElement("Preferences");
            wtr.WriteElementString("One","1.0");
            wtr.WriteElementString("Two","2.0");
            wtr.WriteElementString("Three","3.0");
            wtr.WriteElementString("Four","4.0");
            wtr.WriteElementString("Five","5.0");
		    wtr.WriteEndElement();

		    wtr.Close();
        }

        private void button_res_reader_Click(object sender, EventArgs e)
        {
            this.textBox_res_output.Clear();

            //  settings
            XmlReaderSettings settings = new XmlReaderSettings();
            settings.IgnoreWhitespace = true;
            settings.IgnoreComments = true;
            //settings.IgnoreProcessingInstructions = true;

            //	quick and dirty XML reader
            XmlReader rdr = XmlReader.Create("..\\..\\Resolutions.cfg", settings);

            //	skip the XML header
            rdr.MoveToContent();

            //	read in the preferences
            //
            //rdr.ReadStartElement("Preferences");
            String name;
            String value;
            while (rdr.Read())
            {
                if (rdr.NodeType == System.Xml.XmlNodeType.Element)
                {
                    name = rdr.Name.ToString();
                    name = name.Replace('_', ' ');	// elements cannot have spaces, so remove the underscores

                    this.textBox_res_output.AppendText(name);
                    this.textBox_res_output.AppendText("-");
                }
                else if (rdr.NodeType == System.Xml.XmlNodeType.Text)
                {
                    value = rdr.Value.ToString();
                    this.textBox_res_output.AppendText(value);
                }
                else if (rdr.NodeType == System.Xml.XmlNodeType.EndElement)
                {
                    this.textBox_res_output.AppendText("\r\n");
                }
            }

            rdr.Close();
        }
    }
}