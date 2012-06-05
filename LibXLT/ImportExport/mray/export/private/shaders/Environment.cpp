/******************************************************************************
 * Exports:
 *
 *      sgpu_environment()
 *
 * Description:
 *      Perform vertex displacement function
 *****************************************************************************/

#include "shader.h"
#include "mi/math.h"

#include "Support.h"

struct sgpu_environment {
	miColor	diffColor;
	miTag diffTexture;
	miScalar diffFactor;
	miScalar diffAngle;
};

void EvalParams(miState *state,
				const sgpu_environment* paras,
				sgpu_environment& o_Params)
{ 
	// shader params
	o_Params.diffColor = *mi_eval_color(&paras->diffColor);
	o_Params.diffTexture = *mi_eval_tag(&paras->diffTexture);
	o_Params.diffFactor = *mi_eval_scalar(&paras->diffFactor);
	o_Params.diffAngle = *mi_eval_scalar(&paras->diffAngle);
}

extern "C" DLLEXPORT int sgpu_environment_version(void) {return(1);}

extern "C" DLLEXPORT miBoolean sgpu_environment( 
		miColor         *result,
        miState         *state,
        struct sgpu_environment *paras)
{
	struct sgpu_environment o_Params;
	EvalParams(state, paras, o_Params);

	miVector dir = (state->dir);
	mi_vector_to_world(state,&dir,&dir);

	miColor diff = make_color(0);
	diff = o_Params.diffColor * o_Params.diffFactor;
	if (o_Params.diffTexture)
		diff *= SampleEnvDiffuse(state, dir, o_Params.diffTexture, o_Params.diffAngle);
	
	*result = diff;
	
	return (miTRUE);
}