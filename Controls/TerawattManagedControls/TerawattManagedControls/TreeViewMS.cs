using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace TerawattManagedControls
{
    public partial class TreeViewMS : TreeView
    {
        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        public TreeViewMS()
        {
            m_coll = new ArrayList();
        }

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        public void Clear()
        {
            // First set the selectedNode(s) to null as they are cleared
            //treeViewMS1.SelectedNode = null;
            this.SelectedNodes = null;

            // Clear the treeview
            this.Nodes.Clear();
        }

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        public Int32 NumSelectNodes()
        {
            return m_coll.Count;
        }

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        public ArrayList SelectedNodes
        {
            get
            {
                return m_coll;
            }
            set
            {
                removePaintFromNodes();
                if (m_coll != null)
                {
                    m_coll.Clear();
                }
                m_coll = value;

                if (value == null)
                    this.SelectedNode = null;   // does this make sense to have here? when select nodes set to null, node also should be cleared. [rjk]
                paintSelectedNodes();
            }
        }

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        public void AddToSelectedNodes(TreeNode i_TreeNode)
        {
            if (!m_coll.Contains(i_TreeNode)) // new node ?
            {
                m_coll.Add(i_TreeNode);
            }
            paintSelectedNodes();
        }

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        public void RemoveFromSelectedNodes(TreeNode i_TreeNode)
        {
            if (m_coll.Contains(i_TreeNode)) // new node ?
            {
                m_coll.Remove(i_TreeNode);
            }
            paintSelectedNodes();
        }

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        protected override void OnPaint(PaintEventArgs pe)
        {
            // TODO: Add custom paint code here

            // Calling the base class OnPaint
            base.OnPaint(pe);
        }

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        protected void selected_node(TreeNode i_TreeNode, bool i_bSimulateControlPressed)
        {
            bool bControl = ((ModifierKeys == Keys.Control) || (i_bSimulateControlPressed));
            bool bShift = (ModifierKeys == Keys.Shift);

            if (bControl)
            {
                if (!m_coll.Contains(i_TreeNode)) // new node ?
                {
                    m_coll.Add(i_TreeNode);
                }
                else  // not new, remove it from the collection
                {
                    removePaintFromNodes();
                    m_coll.Remove(i_TreeNode);
                }
                paintSelectedNodes();
            }
            else
            {
                // SHIFT is pressed
                if (bShift)
                {
                    Queue myQueue = new Queue();

                    TreeNode uppernode = m_firstNode;
                    TreeNode bottomnode = i_TreeNode;
                    // case 1 : begin and end nodes are parent
                    bool bParent = isParent(m_firstNode, i_TreeNode); // is m_firstNode parent (direct or not) of i_TreeNode
                    if (!bParent)
                    {
                        bParent = isParent(bottomnode, uppernode);
                        if (bParent) // swap nodes
                        {
                            TreeNode t = uppernode;
                            uppernode = bottomnode;
                            bottomnode = t;
                        }
                    }
                    if (bParent)
                    {
                        TreeNode n = bottomnode;
                        while (n != uppernode.Parent)
                        {
                            if (!m_coll.Contains(n)) // new node ?
                                myQueue.Enqueue(n);

                            n = n.Parent;
                        }
                    }
                    // case 2 : nor the begin nor the end node are descendant one another
                    else
                    {
                        if ((uppernode.Parent == null && bottomnode.Parent == null) || (uppernode.Parent != null && uppernode.Parent.Nodes.Contains(bottomnode))) // are they siblings ?
                        {
                            int nIndexUpper = uppernode.Index;
                            int nIndexBottom = bottomnode.Index;
                            if (nIndexBottom < nIndexUpper) // reversed?
                            {
                                TreeNode t = uppernode;
                                uppernode = bottomnode;
                                bottomnode = t;
                                nIndexUpper = uppernode.Index;
                                nIndexBottom = bottomnode.Index;
                            }

                            TreeNode n = uppernode;
                            while (nIndexUpper <= nIndexBottom)
                            {
                                if (!m_coll.Contains(n)) // new node ?
                                    myQueue.Enqueue(n);

                                n = n.NextNode;

                                nIndexUpper++;
                            } // end while

                        }
                        else
                        {
                            if (!m_coll.Contains(uppernode)) myQueue.Enqueue(uppernode);
                            if (!m_coll.Contains(bottomnode)) myQueue.Enqueue(bottomnode);
                        }
                    }

                    m_coll.AddRange(myQueue);

                    paintSelectedNodes();
                    m_firstNode = i_TreeNode; // let us chain several SHIFTs if we like it
                } // end if m_bShift
                else
                {
                    // in the case of a simple click, just add this item
                    if (m_coll != null && m_coll.Count > 0)
                    {
                        removePaintFromNodes();
                        m_coll.Clear();
                    }
                    m_coll.Add(i_TreeNode);
                }
            }
        }

        
    //====================================================================
    // Triggers
    //
    // (overriden method, and base class called to ensure events are triggered)
    //====================================================================

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        protected override void OnBeforeSelect(TreeViewCancelEventArgs e)
        {
            bool bControl = (ModifierKeys == Keys.Control);
            bool bShift = (ModifierKeys == Keys.Shift);

            // selecting twice the node while pressing CTRL ?
            if (bControl && m_coll.Contains(e.Node))
            {
                // unselect it (let framework know we don't want selection this time)
                e.Cancel = true;

                // update nodes
                removePaintFromNodes();
                m_coll.Remove(e.Node);
                paintSelectedNodes();
                return;
            }

            m_lastNode = e.Node;
            if (!bShift) m_firstNode = e.Node; // store begin of shift sequence

            base.OnBeforeSelect(e);
        }

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        protected override void OnAfterSelect(TreeViewEventArgs e)
        {
            selected_node(e.Node,false);

            // let the base class do its thing
            //
            base.OnAfterSelect(e);
        }


    //====================================================================
    // Helpers
    //====================================================================

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        protected bool isParent(TreeNode parentNode, TreeNode childNode)
        {
            if (parentNode == childNode)
                return true;

            TreeNode n = childNode;
            bool bFound = false;
            while (!bFound && n != null)
            {
                n = n.Parent;
                bFound = (n == parentNode);
            }
            return bFound;
        }

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        protected void paintNode(TreeNode i_Node)
        {
            if (i_Node == null) return;

            //  only update the node if the color is different.
            //
            i_Node.BackColor = SystemColors.Highlight;
            i_Node.ForeColor = SystemColors.HighlightText;
        }

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        protected void paintSelectedNodes()
        {
            if (m_coll == null) return;
            if (m_coll.Count == 0) return;

            foreach (TreeNode n in m_coll)
            {
                paintNode(n);
            }
        }

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        protected void removePaintFromNode(TreeNode i_Node)
        {
            if (i_Node == null) return;
            i_Node.BackColor = this.BackColor;
            i_Node.ForeColor = this.ForeColor;
        }

        //--------------------------------------------------------------------
        //--------------------------------------------------------------------
        protected void removePaintFromNodes()
        {
            if (m_coll == null) return;
            if (m_coll.Count == 0) return;

            TreeNode n0 = (TreeNode)m_coll[0];

            foreach (TreeNode n in m_coll)
            {
                removePaintFromNode(n);
            }
        }

        protected ArrayList m_coll;
        protected TreeNode m_lastNode, m_firstNode;
    }
}
