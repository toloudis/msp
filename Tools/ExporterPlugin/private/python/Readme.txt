0)
Make sure you have boost jam installed.
Make sure bjam is in the path

1)Edit Jamroot script, 
  entry for the line 'use-project boost' 
  should be changed to your current boost installation

2)Edit boost-build.py file also
  so that the entry for 'boost-build' points
  to your current boost installation

3)Edit bjam_bat.bat file,
  set CONFIG to 'build' or 'release'
 depending upon what you want.

4)
invoke bjam_bat.bat
5)


