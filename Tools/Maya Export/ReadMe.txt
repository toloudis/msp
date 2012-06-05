Maya Engine Export

Commands for outputing geometry and animation:

writeBRep	: writes selected meshes to .mx file (GFRG chunks)
writeJoints -geom : writes base pose to .jnx file (MSSK chunk)
writeJoints -anim : writes joint animation to .jna file (ANIH chunk)
writeXforms -geom : writes hierarchy to .mhx file (MHIE chunk)
writeXforms -anim : writes hier. animation to .mha file (ANIH chunk)

Chunk writing code is in files:
SceneFuncs : writes geometry and material info
JointFuncs : write joint bose and influence info
AnimFuncs : writes animation chunks for hier. or joint animation


