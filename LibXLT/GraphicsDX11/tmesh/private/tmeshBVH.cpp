/****************************************************************************\
**	tmeshBVH.cpp
**
**  Implementation of BVH construction        
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/tmesh/tmeshBVH.hpp"

const unsigned int RIGHT_CHILD_MASK = 0x0001;
const unsigned int LEFT_CHILD_MASK  = 0x0002;

BVHNode::BVHNode(Shape** shapes, int num_shapes, BVH* bvh, int axis)
{
    // check for bad cases
    if ( num_shapes < 2 )
    {
       // GRError("BVHNodes shouldn't be constructed with %i shapes",num_shapes);
        return;
    }

    // if num_shapes is 2, point both children to the two shapes
    if ( num_shapes == 2 )
    {

        child[0].flag = 0;
        child[0].flag |= RIGHT_CHILD_MASK;
        child[0].flag |= LEFT_CHILD_MASK;

        child[1].flag = axis;

        maAxisBox box0 = shapes[0]->boundingBox(0, 0);
        maAxisBox box1 = shapes[1]->boundingBox(0, 0);
        bbox = surround(box0, box1);

        if ( (getBoxMin(box0))[axis] < (getBoxMin(box1))[axis]) // 
        {
            bvh->shape_list[bvh->num_shapes+0] = shapes[0];
            bvh->shape_list[bvh->num_shapes+1] = shapes[1];
            child[0].index = bvh->num_shapes+0;
            child[1].index = bvh->num_shapes+1;
            bvh->num_shapes+= 2;
        }
        else //
        {
            bvh->shape_list[bvh->num_shapes+0] = shapes[1];
            bvh->shape_list[bvh->num_shapes+1] = shapes[0];
            child[0].index = bvh->num_shapes+0;
            child[1].index = bvh->num_shapes+1;
            bvh->num_shapes += 2;
        }

        return;
    }

    // split the list and create children appropriately
    bbox          = surround_wrap(shapes, num_shapes);
    maPoint3d pivot = (getBoxMax(bbox) + getBoxMin(bbox)) * 0.5;

    int midpoint  = qsplit(shapes, num_shapes, pivot[axis], axis);
    child[1].flag = axis;
    child[0].flag = 0;

    int len = midpoint;
// {* * * * * | * }
    if (len == 1) // RIGHT HALF - SINGLE CHILD (LEAF)
    {
        child[0].flag |= RIGHT_CHILD_MASK;
        bvh->shape_list[bvh->num_shapes] = shapes[0];
        child[0].index = bvh->num_shapes++;
    }
    else // RIGHT HALF - DUAL CHILD
    { 
        int current_node = bvh->num_nodes++;
        child[0].index   = current_node;

        BVHNode tempnode( shapes, len, bvh, (axis+1)%3 );
        bvh->node_list[current_node] = tempnode;
    }

    len = num_shapes - midpoint;
    if (len == 1) // LEFT HALF - SINGLE CHILD (LEAF)
    {
        child[0].flag |= LEFT_CHILD_MASK;
        bvh->shape_list[bvh->num_shapes] = shapes[midpoint];
        child[1].index = bvh->num_shapes++;

    }
    else // LEFT HALF - DUAL CHILD
    {
        int current_node = bvh->num_nodes++;
        child[1].index   = current_node;

        BVHNode tempnode( &shapes[midpoint], len, bvh, (axis+1)%3 );
        bvh->node_list[current_node] = tempnode;
    }
}

BVH::BVH(Shape** shapes, int numShapes, int axis)
{
    shape_list   = new Shape*[numShapes];
    node_list    = new BVHNode[numShapes-1];
    num_shapes   = 0;
    num_nodes    = 1;

    node_list[0] = BVHNode(shapes, numShapes, this, axis);
}

maAxisBox BVH::boundingBox(double time0, double time1) const
{
    return node_list[0].bbox;
}

