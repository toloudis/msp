REM 32-bit
@echo #####################################
@echo BUILDING SUPPORT...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS Support.cpp

@echo #####################################
@echo BUILDING AO...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS AO.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:AO.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj AO.obj

@echo #####################################
@echo BUILDING SHADOWSONLY...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS Shadows.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:Shadows.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj Shadows.obj

@echo #####################################
@echo BUILDING ILLUMINATION...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS Illumination.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:Illumination.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj Illumination.obj

@echo #####################################
@echo BUILDING NORMALS...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS Normals.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:Normals.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj Normals.obj

@echo #####################################
@echo BUILDING HDRLIGHTING...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS HDRLighting.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:HDRLighting.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj HDRLighting.obj

@echo #####################################
@echo BUILDING PROJECTEDLIGHT...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS ProjectedLight.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:ProjectedLight.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj ProjectedLight.obj

@echo #####################################
@echo BUILDING POINTLIGHT...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS PointLight.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:PointLight.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj PointLight.obj

@echo #####################################
@echo BUILDING DISPLACEMENT...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS Displacement.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:Displacement.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj Displacement.obj

@echo #####################################
@echo BUILDING PHONG...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS Phong.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:Phong.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj Phong.obj

@echo #####################################
@echo BUILDING PHONGREFLECTION...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS PhongReflection.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:PhongReflection.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj PhongReflection.obj

@echo #####################################
@echo BUILDING SPECULARFRESNEL...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS SpecularFresnel.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:SpecularFresnel.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj SpecularFresnel.obj

@echo #####################################
@echo BUILDING SUBSURFACESCATTER...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS SubSurfaceScatter.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:SubSurfaceScatter.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj SubSurfaceScatter.obj

@echo #####################################
@echo BUILDING SUBSURFACESCATTER w/BLINN...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS SubSurfaceScatter_wBlinnSpecular.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:SubSurfaceScatter_wBlinnSpecular.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj SubSurfaceScatter_wBlinnSpecular.obj

@echo #####################################
@echo BUILDING BLINN...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS Blinn.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:Blinn.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj Blinn.obj

@echo #####################################
@echo BUILDING SIMPLE...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS Simple.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:Simple.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj Simple.obj

@echo #####################################
@echo BUILDING LAMBERT...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS Lambert.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:Lambert.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj Lambert.obj

@echo #####################################
@echo BUILDING CARTOON...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS Cartoon.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:Cartoon.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj Cartoon.obj

@echo #####################################
@echo BUILDING BLINNREFLECTION...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS BlinnReflection.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:BlinnReflection.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj BlinnReflection.obj

@echo #####################################
@echo BUILDING CARPAINT...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS CarPaint.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:CarPaint.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj CarPaint.obj

@echo #####################################
@echo BUILDING ANISOTROPIC...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS Anisotropic.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:Anisotropic.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj Anisotropic.obj

@echo #####################################
@echo BUILDING ENVIRONMENT...
@echo #####################################
cl /c /O2 /MD /W3 /I "C:\Program Files (x86)\mental images\mental ray for Windows\common\include" -DWIN_NT -D_CRT_SECURE_NO_WARNINGS Environment.cpp
link /nodefaultlib:LIBC.LIB /OPT:NOREF /DLL /OUT:Environment.dll "C:\Program Files (x86)\mental images\mental ray for Windows\nt-x86\lib\shader.lib" Support.obj Environment.obj
