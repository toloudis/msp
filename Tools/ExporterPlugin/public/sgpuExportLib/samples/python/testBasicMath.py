# Copyright Studio GPU. 
# test script for sgpuVector3 and sgpuMatrix




# Using the doctest module here to ensure that the results are as expected.
r'''>>> from sgpuExportLib import *
    >>> from math import *
    >>> # vector3 construction
    >>> v1 = sgpuExportLib.sgpuVector3( 1.0, 2.0, 3.0)
    >>> v2 = sgpuExportLib.sgpuVector3( 4.0, 5.0, 6.0)
    >>> # vector3 construction, default construction, zero vector
    >>> v3 = sgpuExportLib.sgpuVector3()
    >>> # vector3 addition
    >>> v3 = v1 + v2
    >>> v3[0]
    5.0
    >>> v3[1]
    7.0
    >>> v3[2]
    9.0
    >>> v3 += v1
    >>> v3[0]
    6.0
    >>> v3[1]
    9.0
    >>> v3[2]
    12.0
    >>> # dot product
    >>> d = v1 * v2
    >>> d
    32.0
    >>> d = v2 * v1
    >>> d
    32.0
    >>> # vector3 assignment
    >>> v3 = v1
    >>> v3[0]
    1.0
    >>> v3[1]
    2.0
    >>> v3[2]
    3.0
    >>> # vector3 substraction
    >>> v3 =  v2 - v1
    >>> v3[0]
    3.0
    >>> v3[1]
    3.0
    >>> v3[2]
    3.0
    >>> v3 = v2
    >>> v3 -= v1
    >>> v3[0]
    3.0
    >>> v3[1]
    3.0
    >>> v3[2]
    3.0
    >>> #vector3 scaling
    >>> v3 = v1 * 2.0
    >>> v3[0]
    2.0
    >>> v3[1]
    4.0
    >>> v3[2]
    6.0
    >>> v3 = v1
    >>> v3 *= 2.0
    >>> v3[0]
    2.0
    >>> v3[1]
    4.0
    >>> v3[2]
    6.0
    >>> v3 = v1
    >>> #vector3 equality
    >>> v3 == v1
    True
    >>> #vector3 epsilon equality, epsilon = 1.0e-5
    >>> v2 = sgpuExportLib.sgpuVector3(1.0e-8, 1.0e-8, 1.0e-8)
    >>> v3 += v2
    >>> v1.EpsilonEqual( v3 )
    True
    >>> # Matrix tests
    >>> # Matrix construction, default construction, identity matrix
    >>> m1 = sgpuExportLib.sgpuMatrix()
    >>> v1 = sgpuExportLib.sgpuVector3(1.0, 0, 0)
    >>> v2 = sgpuExportLib.sgpuVector3(0, 1.0, 0)
    >>> v3 = sgpuExportLib.sgpuVector3(0, 0, 1.0)
    >>> v4 = sgpuExportLib.sgpuVector3()
    >>> #Matrix construction, using vector3-s
    >>> m2 = sgpuExportLib.sgpuMatrix( v1, v2, v3, v4 )
    >>> m2[0][0]
    1.0
    >>> m2[0][1]
    0.0
    >>> m2[0][2]
    0.0
    >>> m2[1][0]
    0.0
    >>> m2[1][1]
    1.0
    >>> m2[1][2]
    0.0
    >>> m2[2][0]
    0.0
    >>> m2[2][1]
    0.0
    >>> m2[2][2]
    1.0
    >>> m2[3][0]
    0.0
    >>> m2[3][1]
    0.0
    >>> m2[3][2]
    0.0
    >>> #matrix, reference to row access, 0 <= row_idx <= 3
    >>> v1 = m1[0]
    >>> #matrix, element access, 0 <= row_idx <= 3, 0 <= col_idx <= 2
    >>> m1[0][0] += 7
    >>> # matrix, row access
    >>> v1[0]
    8.0
    >>> m1[0][0]
    8.0
    >>> v2 = m1[1]
    >>> v2[1] += 8
    >>> m1[1][1]
    9.0
    >>> m1 = m2
    >>> #matrix, EpsilonEqual, epsilon 1.0e-7
    >>> m1[0][0] += 1.0e-8
    >>> m1[1][1] += 1.0e-8
    >>> m2.EpsilonEqual( m1 )
    True
    >>> #matrix, MakeTranslate
    >>> m1 = sgpuExportLib.sgpuMatrix()
    >>> m1.MakeTranslate( 1.0, 2.0, 3.0 )
    >>> v3 = m1[3]
    >>> v1 = sgpuExportLib.sgpuVector3( 1.0, 2.0, 3.0 )
    >>> v1 == v3
    True
    >>> m1.MakeTranslate( 0.0, 0.0, 0.0)
    >>> m3 = sgpuExportLib.sgpuMatrix()
    >>> m1 == m2
    True
    >>> # matrix, MakeRotate
    >>> m1 = m2
    >>> theta = pi * 30/180 # 30 degrees
    >>> m1.MakeRotate( theta, 0, 0, 1)
    >>> v1 = sgpuExportLib.sgpuVector3( cos(theta), sin(theta), 0)
    >>> v2 = sgpuExportLib.sgpuVector3( -sin(theta), cos(theta), 0)
    >>> v3 = sgpuExportLib.sgpuVector3(0,0, 1)
    >>> v4 = sgpuExportLib.sgpuVector3()
    >>> m2 = sgpuExportLib.sgpuMatrix( v1, v2, v3 , v4 )
    >>> m2.EpsilonEqual( m1 )
    True
    >>> # matrix, MakeScale
    >>> m2 = sgpuExportLib.sgpuMatrix()
    >>> m2.MakeScale( 1.0, 2.0, 3.0 )
    >>> v1 = sgpuExportLib.sgpuVector3(1, 0, 0)
    >>> v2 = sgpuExportLib.sgpuVector3(0, 2, 0)
    >>> v3 = sgpuExportLib.sgpuVector3(0, 0, 3)
    >>> m2[0] == v1
    True
    >>> m2[1] == v2
    True
    >>> m2[2] == v3
    True
    >>> # matrix, Identity
    True
'''

def run(args = None):
    if args is not None:
        import sys
        sys.argv = args
    import doctest, testBasicMath
    return doctest.testmod(testBasicMath, verbose=True)

if __name__ == '__main__':
    import sys
    sys.exit(run()[0])

