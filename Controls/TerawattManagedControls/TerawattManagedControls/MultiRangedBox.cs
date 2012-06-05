using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace TerawattManagedControls
{
    /// <summary>
    /// Represents a slider for two ranged values by displaying a circle within a box.
    /// </summary>
    [ToolboxItem(true)]
    [ToolboxBitmap(typeof(MultiRangedBox))]
    //public class MultiRangedBox : System.Windows.Forms.PictureBox
    public class MultiRangedBox : System.Windows.Forms.UserControl
    {
        // Private members for properties
        private System.Double valueX = 0, valueY = 0;
        private const System.Double minX = -1, minY = -1;
        private const System.Double maxX = 1, maxY = 1;
        private const int radius = 3;

        /// <summary>
        /// Callback when value changes
        /// </summary>
        [Category("Property Changed"),
        Description("Callback when either value changes")]
        public event EventHandler ValueChanged;

        [Bindable(true), Category("Properties"), DefaultValue(0.0),
        Description("Value in X direction")]
        public double ValueX
        {
            get { return valueX; }
            set
            {
                if (valueX != value)
                {
                    valueX = value;
                    this.Invalidate();

                    //if (this.ValueChanged != null)
                    //    this.ValueChanged(this, e);
                }
            }
        }

        [Bindable(true), Category("Properties"), DefaultValue(0.0),
        Description("Value in Y direction")]
        public double ValueY
        {
            get { return valueY; }
            set
            {
                if (valueY != value)
                {
                    valueY = value;
                    this.Invalidate();
                }
            }
        }

        public MultiRangedBox()
        {
            this.InitializeComponent();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            // Draw circle where value is
            Pen line_pen = new Pen(System.Drawing.Color.Black);

            double x = (this.valueX - minX) * this.Width / (maxX - minX);
            double y = (this.valueY - minY) * this.Height / (maxY - minY);
            e.Graphics.DrawEllipse(line_pen, (int)x - radius, this.Height - (int)y - radius, radius * 2, radius*2);
        }

        private void InitializeComponent()
        {
            this.SuspendLayout();
            // 
            // MultiRangedBox
            // 
            this.Name = "MultiRangedBox";
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.MultiRangedBox_MouseDown);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.MultiRangedBox_MouseMove);
            this.ResumeLayout(false);

        }

        private bool set_from_pick(int x, int y)
        {
            double valx = (x / (double)(this.Width)) * (maxX - minX) + minX;
            double valy = ((this.Height - y) / (double)(this.Height)) * (maxY - minY) + minY;

            // Clamp values to range
            valx = Math.Max(valx, minX);
            valx = Math.Min(valx, maxX);
            valy = Math.Max(valy, minY);
            valy = Math.Min(valy, maxY);

            if (this.ValueX != valx || this.ValueY != valy)
            {
                this.ValueX = valx;
                this.ValueY = valy;
                return true;
            }
            return false;
        }

        private void MultiRangedBox_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (set_from_pick(e.X, e.Y))
                {
                    if (this.ValueChanged != null)
                        this.ValueChanged(this, e);
                }
            }
        }

        private void MultiRangedBox_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                if (set_from_pick(e.X, e.Y))
                {
                    if (this.ValueChanged != null)
                        this.ValueChanged(this, e);
                }
            }
        }

    }
}
