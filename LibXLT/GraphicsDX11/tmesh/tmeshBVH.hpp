/****************************************************************************\
**	tmeshBVH.hpp
**
**  Implementation of BVH construction        
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef TMESH_BVH_HPP
#error tmeshBVH.hpp multiply included
#endif
#define TMESH_BVH_HPP

#ifndef TMESH_SHAPE_HPP
#include "GraphicsDX11/tmesh/tmeshShape.hpp"
#endif

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif

maPoint3d getBoxMin(maAxisBox myBox)
{
	return maPoint3d( myBox.GetMinX(), myBox.GetMinY(), myBox.GetMinZ() );
}

maPoint3d getBoxMax(maAxisBox myBox)
{
	return maPoint3d( myBox.GetMaxX(), myBox.GetMaxY(), myBox.GetMaxZ() );
}

maAxisBox surround(const maAxisBox& b1, const maAxisBox& b2)
{
    return maAxisBox(
         maPoint3d( b1.GetMinX() < b2.GetMinX() ? b1.GetMinX() : b2.GetMinX(),
                    b1.GetMinY() < b2.GetMinY() ? b1.GetMinY() : b2.GetMinY(),
                    b1.GetMinZ() < b2.GetMinZ() ? b1.GetMinZ() : b2.GetMinZ() ),
         maPoint3d( b1.GetMaxX() > b2.GetMaxX() ? b1.GetMaxX() : b2.GetMaxX(),
                    b1.GetMaxY() > b2.GetMaxY() ? b1.GetMaxY() : b2.GetMaxY(),
                    b1.GetMaxZ() > b2.GetMaxZ() ? b1.GetMaxZ() : b2.GetMaxZ() ));
}

maAxisBox surround_wrap(Shape** shapes, int num_shapes)
{
	maAxisBox result = shapes[0]->boundingBox(0,0);
	for ( int i = 1; i < num_shapes; i++ )
		result = surround(result, shapes[i]->boundingBox(0,0));
	return result;
} 

int qsplit(Shape** list, int size, double pivot_val, int axis)
{
   maAxisBox bbox;
   double centroid;
   int ret_val = 0;

   for (int i = 0; i < size; i++)
   {
      bbox = list[i]->boundingBox(0.0f, 0.0f);
      centroid = ((getBoxMin(bbox))[axis] + (getBoxMax(bbox))[axis]) / 2.0f;
      if (centroid < pivot_val)
      {
         Shape* temp = list[i];
         list[i]       = list[ret_val];
         list[ret_val] = temp;
         ret_val++;
      }
   }
   if (ret_val == 0 || ret_val == size) ret_val = size/2;

   return ret_val;
}


class BVH;

class BVHNode
{
public:
    BVHNode(){}
    BVHNode(Shape** shapes, int num_shapes, BVH* bvh, int axis = 0);

    struct Child
    {
        unsigned int flag:2;
        unsigned int index:30;
    };
    
    BVHNode& operator=(const BVHNode& orig)
    {
        bbox = orig.bbox;
        child[0] = orig.child[0];
        child[1] = orig.child[1];
        return *this;
    }
    maAxisBox bbox;              // 24 bytes
    Child child[2];              // 8 bytes
};


class BVH : public Shape
{
public:
    BVH(Shape** shapes, int num_shapes, int axis = 0);

    maAxisBox boundingBox(double time0, double time1) const;

    unsigned int num_shapes;
    unsigned int num_nodes;
    BVHNode*  node_list;
    Shape**   shape_list; 

private:
    BVH(){}
};
