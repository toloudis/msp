/*****************************************************************************
**  ProjLight.sl
**
**      MSP Projected light shader
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support.hsl"

//--------------------------------------------------------------------
// clipSuperellipse()
//--------------------------------------------------------------------
float
clipSuperellipse(point Q; point Pc;
				 float innerAngle; float outerAngle; 
				 float scale; float aspect; float range; float lightLength;
				 float i_bConeLighting; float i_bDirectional; float i_bPureDirectional;
				 float bbox_min_x; float bbox_min_y; float bbox_min_z;
				 float bbox_max_x; float bbox_max_y; float bbox_max_z 
				 )
{
	
	if ( i_bPureDirectional == 1 )
	{
		return 0;
	}

    float result = 0;
	float x = abs(xcomp(Q));
	float y = abs(ycomp(Q));	
	float inner = radians(innerAngle/2);
	float outer = radians(outerAngle/2);

    if ( i_bDirectional == 0 && i_bConeLighting == 0 ) 
	{  	
		if ( zcomp(Pc) > scale && zcomp(Pc) < scale+range )
		{
			float A = tan( outer );
			float B = tan( outer ) * (1/aspect);
			result = 1 - (1-step(A,x)) * (1-step(B,y));
		}
		else result = 1.0;
	} 

	else if ( i_bDirectional == 0 && i_bConeLighting == 1 ) 
	{		
		if ( zcomp(Pc) > scale && zcomp(Pc) < scale+range )
		{
			float a = tan(inner);
			float b = tan(inner) * (1/aspect);
			float A = tan(outer);
			float B = tan(outer) * (1/aspect);
			float q = a*b/sqrt(b*b*x*x + a*a*y*y);
			float r = A*B/sqrt(B*B*x*x + A*A*y*y);
			result = smoothstep(q, r, 1);
		}
		else result = 1.0;
    }

	else if ( i_bDirectional == 1 && i_bConeLighting == 0 ) 
	{
		point shiftedPt = Pc;
		setzcomp(shiftedPt, zcomp(shiftedPt) - scale);
		result = 1 - ContainsPoint(shiftedPt,
								   bbox_min_x,bbox_min_y,bbox_min_z,
								   bbox_max_x,bbox_max_y,bbox_max_z);
	}

	else if ( i_bDirectional == 1 && i_bConeLighting == 1 ) 
	{	
		if ( zcomp(Pc) > scale )
		{
			float currRadius = sqrt( xcomp(Pc)*xcomp(Pc) + ycomp(Pc)*ycomp(Pc)*aspect*aspect );
			result = (currRadius > scale/2) ? 1 : 0;
		}
		else result = 1.0;
	}

    return result;
}

//--------------------------------------------------------------------
//	GetPoissonVal() - PCSS helper function
//--------------------------------------------------------------------
float GetPoissonVal(float idx)
{	
	float poisson_disk[512] = {
		-0.415757954121, 	-0.307007312775,
		-0.999989032745, 	0.123504042625,
		-0.324011802673, 	0.33845436573,
		-0.969086050987, 	-0.997798800468,
		-0.181707203388, 	-0.0354183316231,
		0.9620013237, 		0.746612906456,
		0.634776711464, 	-0.359547495842,
		0.686866641045, 	0.570747971535,
		-0.00195515155792, 	-0.605298280716,
		0.0248702764511, 	0.37431037426,
		-0.0493814945221, 	0.33611869812,
		0.402388095856, 	-0.887425601482,
		0.920067548752, 	0.192143321037,
		-0.360320627689, 	-0.868100583553,
		0.0381577014923, 	-0.111639559269,
		0.539649248123, 	0.668017029762,
		0.0927656888962, 	0.384810805321,
		0.651272773743, 	0.127217888832,
		0.206308484077, 	0.607260704041,
		0.79118001461, 		0.677464842796,
		0.139808416367, 	-0.963199913502,
		-0.22441393137, 	-0.851792991161,
		0.0195223093033, 	-0.482260942459,
		0.342718839645, 	0.930730938911,
		-0.0997076034546, 	0.204538822174,
		-0.378837943077, 	0.659482836723,
		0.00894856452942, 	0.505936384201,
		0.651379942894, 	-0.00585699081421,
		-0.837081551552, 	0.951209783554,
		-0.290645718575, 	-0.701874494553,
		0.0917477607727, 	0.152590155602,
		-0.503503918648, 	-0.72034406662,
		0.703035116196, 	0.0749887228012,
		-0.96193087101, 	0.780337452888,
		-0.770724475384, 	-0.285424351692,
		-0.521964728832, 	0.859279155731,
		-0.264045059681, 	0.0244901180267,
		-0.287628412247, 	-0.221044540405,
		-0.726094305515, 	0.877244234085,
		-0.0579364299774, 	0.575763344765,
		-0.89625442028, 	0.213267683983,
		0.921410679817, 	0.599874258041,
		0.405699491501, 	0.167470216751,
		-0.266678452492, 	-0.486743867397,
		-0.200808227062, 	-0.28875541687,
		-0.407266914845, 	0.879252076149,
		-0.610522627831, 	-0.502714037895,
		0.190292000771, 	-0.375916957855,
		0.432513713837, 	0.337543964386,
		-0.599826455116, 	-0.362500607967,
		0.122844338417, 	-0.674452006817,
		-0.0707055330276, 	-0.804758310318,
		0.561969161034, 	0.534954190254,
		-0.706166863441, 	0.713750720024,
		0.50043463707, 		-0.15829706192,
		-0.931690633297, 	0.597908616066,
		-0.0521476864815, 	0.0349020957947,
		-0.997243881226, 	-0.141711473465,
		-0.962580561638, 	0.935882329941,
		0.4426176548, 		-0.195207297802,
		0.749297738075, 	-0.0625938177109,
		0.908566951752, 	-0.0246162414551,
		-0.637861430645, 	-0.923464179039,
		-0.79311645031, 	0.809995889664,
		0.887435913086, 	0.544613480568,
		0.946038246155, 	0.121113061905,
		-0.593244731426, 	0.739417433739,
		0.46822810173, 		-0.0841932892799,
		0.529536962509, 	-0.901707589626,
		0.92825114727, 		-0.577435731888,
		0.600801944733, 	0.924322366714,
		0.665665864944, 	0.993400335312,
		0.774976491928, 	0.0103375911713,
		0.62262237072, 		-0.705139458179,
		0.493190407753, 	0.288145065308,
		-0.864136457443, 	-0.975541055202,
		0.625094771385, 	-0.61847358942,
		0.324367523193, 	0.309258699417,
		-0.658737182617, 	-0.149027884007,
		-0.476232171059, 	-0.254612147808,
		0.449486494064, 	0.591151356697,
		0.315091848373, 	-0.569881975651,
		0.325016498566, 	-0.0642792582512,
		0.484638929367, 	0.160881996155,
		-0.73237580061, 	0.206281661987,
		0.111445307732, 	0.0115730762482,
		-0.457200288773, 	-0.359537184238,
		0.752270579338, 	0.145184874535,
		-0.26561653614, 	-0.290171980858,
		-0.86328625679, 	-0.506230592728,
		0.908035159111, 	-0.188288152218,
		-0.353170394897, 	-0.747117161751,
		0.189489364624, 	0.399418950081,
		-0.872706890106, 	0.0921521186829,
		-0.727744817734, 	-0.860645651817,
		-0.948987066746, 	-0.441547632217,
		0.401406764984, 	-0.469781160355,
		-0.26779872179, 	-0.0837929844856,
		0.937297105789, 	-0.730389237404,
		0.0744278430939, 	0.0924748182297,
		-0.704310297966, 	0.354593992233,
		0.155883431435, 	0.644702553749,
		-0.636761069298, 	-0.0448909401894,
		0.420086026192, 	0.921164751053,
		-0.251735448837, 	-0.144667387009,
		0.189654707909, 	-0.225541591644,
		0.501892805099, 	0.495574474335,
		0.346584320068, 	0.539728045464,
		0.141563415527, 	0.998720407486,
		-0.506407439709, 	-0.885527908802,
		-0.540439724922, 	0.238392114639,
		-0.305917322636, 	0.130444765091,
		0.17865550518, 		0.905961751938,
		-0.514184236526, 	0.381422758102,
		0.0286765098572, 	0.179683923721,
		-0.712139964104, 	-0.478293836117,
		-0.521578788757, 	-0.358527898788,
		0.323919296265, 	-0.903955638409,
		-0.250787854195, 	-0.774267852306,
		-0.958730041981, 	0.393280744553,
		-0.635471224785, 	-0.309784889221,
		-0.248264908791, 	0.699278116226,
		0.250646710396, 	-0.535455822945,
		0.521033525467, 	0.373806238174,
		0.348887324333, 	-0.270278334618,
		-0.394953966141, 	0.139647841454,
		0.0216785669327, 	0.919688224792,
		0.371087551117, 	0.46440577507,
		0.556820988655, 	-0.339622676373,
		0.738476872444, 	0.477137684822,
		0.771810650826, 	0.93575835228,
		-0.205457806587, 	0.182102441788,
		-0.903442919254, 	-0.576662361622,
		-0.421976387501, 	-0.214109063148,
		-0.3824852705, 		0.41473531723,
		0.88427066803, 		0.845234274864,
		-0.597764253616, 	-0.794075012207,
		0.679982304573, 	-0.802298903465,
		-0.238126754761, 	0.403813362122,
		0.6050812006, 		-0.543026208878,
		-0.145884990692, 	-0.211347818375,
		-0.745605826378, 	-0.999535262585,
		-0.384266376495, 	0.295287370682,
		0.271909594536, 	-0.0186420679092,
		0.0944821834564, 	0.740437626839,
		-0.266018867493, 	-0.962322890759,
		0.0610929727554, 	-0.268881261349,
		-0.697465002537, 	-0.720154285431,
		-0.308974266052, 	0.838204264641,
		0.582984209061, 	0.220297574997,
		0.484161615372, 	0.869464635849,
		0.835884928703, 	-0.217734217644,
		0.866134524345, 	0.962750196457,
		0.359741687775, 	-0.157439529896,
		-0.978330790997, 	0.503557443619,
		0.515821576118, 	0.0977878570557,
		-0.781891405582, 	0.660185813904,
		-0.467448890209, 	0.0832816362381,
		-0.502683162689, 	-0.151012897491,
		0.876625180244, 	-0.895189762115,
		-0.900877118111, 	-0.658530831337,
		-0.367899179459, 	0.79533624649,
		0.32302069664, 		-0.365221679211,
		0.470696091652, 	0.72997879982,
		-0.167205512524, 	-0.500283718109,
		-0.93359130621, 	0.148563742638,
		0.395768165588, 	-0.0961967110634,
		0.395884156227, 	0.74974834919,
		0.080939412117, 	0.599899888039,
		-0.0910460948944, 	-0.67810177803,
		0.524116873741, 	-0.644667506218,
		0.672092199326, 	-0.292624533176,
		0.76543033123, 		0.551385998726,
		0.443885326385, 	-0.527751803398,
		0.301817536354, 	0.707025885582,
		0.107479453087, 	-0.202602922916,
		-0.160126268864, 	-0.582996606827,
		0.402836322784, 	-0.633915960789,
		-0.523296117783, 	-0.95643222332,
		0.452896356583, 	-0.841628909111,
		-0.865230441093, 	0.794190526009,
		0.522953748703, 	-0.397499680519,
		-0.717108011246, 	-0.234159350395,
		0.0981377363205, 	0.841750502586,
		-0.810242652893, 	0.19634604454,
		-0.905021548271, 	0.730925798416,
		0.211004734039, 	0.215678215027,
		-0.441374003887, 	0.262136340141,
		0.0325330495834, 	0.0389763116837,
		-0.812033832073, 	0.536602258682,
		0.0334894657135, 	-0.386205136776,
		0.200131177902, 	0.0127446651459,
		-0.027800142765, 	0.781478524208,
		0.415264368057, 	0.258855342865,
		0.306766033173, 	0.480510234833,
		0.349022507668, 	-0.77649140358,
		0.136532664299, 	0.520867466927,
		0.812962174416, 	-0.559707403183,
		-0.0205968022346, 	0.986451745033,
		-0.509413957596, 	0.560806512833,
		-0.550288438797, 	0.476644039154,
		0.939857363701, 	0.456780433655,
		-0.898642957211, 	0.863899350166,
		0.786417841911, 	-0.890314102173,
		-0.076176404953, 	0.922913551331,
		-0.860582351685, 	-0.122878789902,
		0.600135326385, 	0.861961245537,
		0.698167085648, 	0.89713537693,
		0.492275953293, 	-0.0186278223991,
		-0.841658473015, 	-0.596030235291,
		0.905914902687, 	0.337806820869,
		-0.721285104752, 	-0.0184351801872,
		-0.0693228840828, 	-0.993405759335,
		0.715800404549, 	-0.203863024712,
		0.117646455765, 	-0.843482613564,
		-0.443656802177, 	-0.815229892731,
		0.597274661064, 	-0.766075611115,
		-0.762543559074, 	0.747245788574,
		0.193806886673, 	0.733397245407,
		-0.985407471657, 	-0.356724023819,
		-0.738691449165, 	-0.371147453785,
		-0.537099838257, 	-0.810829877853,
		0.740815758705, 	-0.994248151779,
		0.807778120041, 	-0.664702653885,
		0.582011938095, 	0.792075037956,
		0.136256337166, 	0.264344453812,
		-0.18073785305, 	-0.428484141827,
		0.00223731994629, 	-0.956182062626,
		-0.00850087404251, 	-0.251703977585,
		0.756610989571, 	-0.620826601982,
		-0.911600470543, 	-0.279290378094,
		-0.653744339943, 	0.645545363426,
		0.657974839211, 	0.625242829323,
		0.58411192894, 		0.132586479187,
		0.879118680954, 	0.107514619827,
		-0.133203148842, 	-0.907719731331,
		0.583663225174, 	-0.442635238171,
		0.918936729431, 	-0.316513895988,
		-0.244584262371, 	-0.625973284245,
		-0.808946669102, 	-0.828397154808,
		0.271717905998, 	-0.104681670666,
		-0.229069411755, 	-0.698700726032,
		0.267337203026, 	-0.172531247139,
		0.671711683273, 	-0.569589614868,
		0.477365732193, 	-0.997455060482,
		0.292757153511, 	-0.426412165165,
		0.393433690071, 	-0.725052118301,
		-0.469008982182, 	-0.452587723732,
		-0.594199061394, 	0.0838865041733,
		0.171688318253, 	-0.522022247314,
		0.991615891457, 	0.225111961365,
		0.370167851448, 	0.396544337273,
		-0.218688964844, 	0.797135591507,
		-0.57398968935, 	0.347099304199,
		0.68114900589, 		0.757608890533,
		0.745979785919, 	-0.698154687881
	};

	return poisson_disk[idx];
}

//--------------------------------------------------------------------
//	PenumbraSize() - PCSS helper function
//--------------------------------------------------------------------
float PenumbraSize(float zReceiver; float zBlocker) //Parallel plane estimation
{
	return (zReceiver - zBlocker) / zBlocker;
}

//--------------------------------------------------------------------
//	FindBlocker() - PCSS helper function
//--------------------------------------------------------------------
void FindBlocker(output float avgBlockerDepth;
				 output float numBlockers;
				 float ss; float tt; float zReceiver;
				 string ShadowMap; float scale;
				 float nBlockerSamples; float lightSizeUV)
{

	// This uses similar triangles to compute what area of the shadow map we should search
	float searchWidth = lightSizeUV * (zReceiver - scale) / zReceiver;
	float blockerSum = 0;
	numBlockers = 0;
	float i;
	float j;
	for( i = 0 ; i < nBlockerSamples ; i += 1 )
	{
		for( j = 0 ; j < nBlockerSamples ; j += 1 )
		{	
			float disk_idx = (i*nBlockerSamples+j)*2;
			float offset_x = GetPoissonVal(disk_idx) * searchWidth;
			float offset_y = GetPoissonVal(disk_idx+1) * searchWidth;
			float shadowMapDepth = float texture(ShadowMap[0], ss+offset_x, tt+offset_y );

			if ( shadowMapDepth > 1 )
				Print("shadowMapDepth",shadowMapDepth);

			if ( (1-shadowMapDepth) < zReceiver ) {
				blockerSum += shadowMapDepth;
				numBlockers += 1;
			}
		}
	}
	avgBlockerDepth = blockerSum / numBlockers;

}

//--------------------------------------------------------------------
//	PCF_Filter() - PCSS helper function
//--------------------------------------------------------------------
float PCF_Filter( float ss; float tt; float zReceiver; float filterRadiusUV;
				  string ShadowMap; float nShadowSamples)
{
	float sum = 0;
	float shadowMapDepth;
	float i;
	float j;
	for( i = 0; i < nShadowSamples; i += 1 )
	{	
		for( j = 0; j < nShadowSamples; j += 1 )
		{			
			float disk_idx = (i*nShadowSamples+j)*2;
			float offset_x = GetPoissonVal(disk_idx) * filterRadiusUV;
			float offset_y = GetPoissonVal(disk_idx+1) * filterRadiusUV;
			float shadowMapDepth = float texture(ShadowMap[0], ss+offset_x, tt+offset_y );
			sum += (zReceiver <= shadowMapDepth) ? 1 : 0;
		}
	}
	return sum / (nShadowSamples*nShadowSamples);
}

//--------------------------------------------------------------------
//	PCSS() - percentage-closer soft shadows
//--------------------------------------------------------------------
float PCSS( float Ps_biased_sub0; float Ps_biased_sub1; float Ps_biased_sub2;
			float Ptot_biased_sub0; float Ptot_biased_sub1; float Ptot_biased_sub2; 
			float ss; float tt; string ShadowMap; string ShadowMapRGB; 
			float nBlockerSamples; float scale; float shadowSoftness; float shadowPCSS;
			float near; float far)
{	
	point Ps_biased = point(Ps_biased_sub0,Ps_biased_sub1,Ps_biased_sub2);
	point Ptot_biased = point(Ptot_biased_sub0,Ptot_biased_sub1,Ptot_biased_sub2);

	float zReceiver = (zcomp(Ptot_biased));// - near) / (far-near)  /* /w */;
	//zReceiver = clamp(zReciever,0,1);
	float lightSizeUV = (shadowSoftness) / scale;

	// STEP 1: blocker search
	float avgBlockerDepth = 0;
	float numBlockers = 0;

	float sss = 1 - (Ptot_biased_sub0 * 0.5 + 0.5);
	float ttt = 1 - (Ptot_biased_sub1 * 0.5 + 0.5);

	FindBlocker( avgBlockerDepth, numBlockers, sss, ttt, zReceiver, ShadowMapRGB, scale, nBlockerSamples, lightSizeUV );

	// There are no occluders so early out (this saves filtering)
	if( numBlockers < 1 )		
		return 0.0;

	// STEP 2: penumbra size
	float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);
	float filterRadiusUV = penumbraRatio * lightSizeUV * scale / zReceiver;
	float interpolated = lerp(shadowSoftness,filterRadiusUV,shadowPCSS);
	
	// STEP 3: filtering
	//float shadowed = PCF_Filter( ss, tt, zReceiver, interpolated, ShadowMapRGB, nBlockerSamples );
	float shadowed = shadow( ShadowMap, Ps_biased , "samples", pow(2,6), "blur", interpolated );
	
	return shadowed;
}

color CalcLight( point PL; point Pc; point Pcp; float falloffStart; float falloff_x; float falloff_y; float falloff_z; float falloff_w;
				 float innerAngle; float outerAngle; float aspect; float bConeLighting; float bDirectional;
				 float bPureDirectional; float scale; float range; float lightLength; float shadowSource;
				 float shadowType; string shadowMapName; point Ps_biased; float shadowQuality;
				 float shadowSoftness; float moddedShadowIntensity; point from; float shadowsOnly;
				 string gobo; float intensity; color shadowColor; color lightColor;				 
				 float bbox_min_x; float bbox_min_y; float bbox_min_z;
				 float bbox_max_x; float bbox_max_y; float bbox_max_z )
{

	color lc = 1;
	float unoccluded = 1;
	color lcol = lightColor;

	color c_out = 0;

	float bReceivesShadow = 1;
	surface("bReceivesShadow",bReceivesShadow);

	// Light falloff
	float atten = attenuation( length(L),falloffStart,falloff_x,falloff_y,falloff_z,falloff_w);
	
	if ( bPureDirectional == 0 )
	{
		// Clip cone-shaped light into rectangle
		atten *= 1 - clipSuperellipse(PL/zcomp(PL), Pc, innerAngle, outerAngle, 
									scale, aspect, range, lightLength, 
									bConeLighting, bDirectional, bPureDirectional,
									bbox_min_x, bbox_min_y, bbox_min_z, 
									bbox_max_x, bbox_max_y, bbox_max_z  );

		// Control light range
		atten *= 1 - step(scale+range,zcomp(PL));

	}
	
	// Shadows
	if ( shadowSource == 1 )
	{
		if ( bReceivesShadow == 1 )
		{
			// Map-based
			if ( shadowType == 0 )
			{	
				//point Pw_biased = transform(worldMat,Ps_biased);
				//point Ptot_biased = transform(camProjMat,Pw_biased);
				//unoccluded = 1 - PCSS( xcomp(Ps_biased), ycomp(Ps_biased), zcomp(Ps_biased),
				//					   xcomp(Ptot_biased), ycomp(Ptot_biased), zcomp(Ptot_biased),
				//					   ss, tt, shadowMapName, shadowMapNameRGB, 
				//					   shadowQuality, scale, shadowSoftness, shadowPCSS,
				//					   scale, scale+range) * moddedShadowIntensity;
				unoccluded = 1 - shadow( shadowMapName, Ps_biased , "samples", shadowQuality, "blur", shadowSoftness ) * moddedShadowIntensity;
			}

			// Ray-traced
			else if ( shadowType == 1 )
			{
				lc = transmission( Ps_biased, from,
								   "samples", shadowQuality,
								   "minsamples", shadowQuality,
								   "samplecone", radians(10) * shadowSoftness/0.02, // 0.02 fudge factor undo
								   "hitmode", "shader");
				
				unoccluded = 1 - ( 1 - (lc[0]+lc[1]+lc[2]) / 3 ) * moddedShadowIntensity;
			
			}
		}
	}

	if ( shadowsOnly == 1 )
	{
		c_out = lcol * (1-unoccluded);
	}
	else
	{
		// calc final light color
		lcol = mix(shadowColor, lcol, unoccluded);

		// Apply gobo	
		float ss = 1 - ( ( xcomp(Pcp) + 1 ) / 2 );
		float tt = 1 - ( ( ycomp(Pcp) + 1 ) / 2 );
		color goboCol = tex2D(gobo,ss,tt);

		// Final color
		c_out = atten * intensity * lcol * goboCol;
	}

	return c_out;
}


//--------------------------------------------------------------------
// ProjLight()
//--------------------------------------------------------------------
light 
ProjLight(
        float intensity = 1;
        color lightColor = 0.5;
		float falloff_x = 0;
		float falloff_y = 0;
		float falloff_z = 0;
		float falloff_w = 0;
        float falloffStart = 0;
		float innerAngle = 180;
        float outerAngle = 90;
        float scale = 0;
        float range = 0;
        float aspect = 0;
		output float bEnableLight = 1;
		output float bEnableDiffuse = 1;
		output float bEnableSpecular = 1;
		output float bAffectsGlow = 1;
		float shadowSource = 0;
		string shadowMapName = "";
		string shadowMapNameRGB = "";
		float shadowMapRes = 1024;
		float shadowSoftness = 0.001;
		float shadowBias = 20;
		float shadowIntensity = 1;
		float shadowPCSS = 0;
		color shadowColor = 1;
		float shadowQuality = 4;
		float shadowBlur = 0;
		float shadowType = 0;
		float shadowMinSamples = 1;
		float shadowSamples = 16;
		float shadowCptrBias = 0.01;
		float shadowCptrSoftness = 0.01;
		float shadowsOnly = 0;
		string gobo = "";
		float bDirectional = 0;
		float bPureDirectional = 0;
		float bConeLighting = 0;
		float camProjMat_Sub0 = 1;
		float camProjMat_Sub1 = 0;
		float camProjMat_Sub2 = 0;
		float camProjMat_Sub3 = 0;
		float camProjMat_Sub4 = 0;
		float camProjMat_Sub5 = 1;
		float camProjMat_Sub6 = 0;
		float camProjMat_Sub7 = 0;
		float camProjMat_Sub8 = 0;
		float camProjMat_Sub9 = 0;
		float camProjMat_Sub10 = 1;
		float camProjMat_Sub11 = 0;
		float camProjMat_Sub12 = 0;
		float camProjMat_Sub13 = 0;
		float camProjMat_Sub14 = 0;
		float camProjMat_Sub15 = 1;
		float camMat_Sub0 = 1;
		float camMat_Sub1 = 0;
		float camMat_Sub2 = 0;
		float camMat_Sub3 = 0;
		float camMat_Sub4 = 0;
		float camMat_Sub5 = 1;
		float camMat_Sub6 = 0;
		float camMat_Sub7 = 0;
		float camMat_Sub8 = 0;
		float camMat_Sub9 = 0;
		float camMat_Sub10 = 1;
		float camMat_Sub11 = 0;
		float camMat_Sub12 = 0;
		float camMat_Sub13 = 0;
		float camMat_Sub14 = 0;
		float camMat_Sub15 = 1;
		float pos_x = 0;
		float pos_y = 0;
		float pos_z = 0;
		float tar_x = 0;
		float tar_y = 0;
		float tar_z = 0;
		float bbox_min_x = -1; 
		float bbox_min_y = -1;
		float bbox_min_z = -1;
		float bbox_max_x = 1;
		float bbox_max_y = 1;
		float bbox_max_z = 1; 
        )
{   

	matrix camProjMat = matrix( camProjMat_Sub0, camProjMat_Sub1, camProjMat_Sub2, camProjMat_Sub3,
								camProjMat_Sub4, camProjMat_Sub5, camProjMat_Sub6, camProjMat_Sub7,
								camProjMat_Sub8, camProjMat_Sub9, camProjMat_Sub10, camProjMat_Sub11,
								camProjMat_Sub12, camProjMat_Sub13, camProjMat_Sub14, camProjMat_Sub15);

	matrix camMat = matrix( camMat_Sub0, camMat_Sub1, camMat_Sub2, camMat_Sub3,
							  camMat_Sub4, camMat_Sub5, camMat_Sub6, camMat_Sub7,
							  camMat_Sub8, camMat_Sub9, camMat_Sub10, camMat_Sub11,
							  camMat_Sub12, camMat_Sub13, camMat_Sub14, camMat_Sub15);

	point from = point "shader" (0,0,0);
	vector axis = normalize(vector "shader" (0,0,1));
    point PL = transform ("shader", Ps);

	Cl = 1;
	color lc = 1;
	color lcol = lightColor;

	// If only rendering shadows, make lit areas white
	if ( shadowsOnly == 1 )
	{
		lcol = color(1,1,1);
	}	

	matrix identity = matrix(1,0,0,0,
							 0,1,0,0,
							 0,0,1,0,
							 0,0,0,1);	
	matrix worldMat = transform("world",identity);

	point Pw = transform(worldMat,Ps);
	point Pcp = transform(camProjMat,Pw);
	setzcomp( Pcp , zcomp(Pcp) - shadowBias );
	point Pcp_biased = transform((1/camProjMat),Pcp);
	point Ps_biased = transform((1/worldMat),Pcp_biased);

	float moddedShadowIntensity = (shadowsOnly == 1) ? 1 : shadowIntensity;

	// Not directional, so use illuminate()
	if ( bDirectional == 0 ) {

		float coneAngle = PI/2;
		if ( bConeLighting == 1 )
		{
			coneAngle = radians(outerAngle/2);
		}
		else
		{
			coneAngle = radians(outerAngle/2) * sqrt(2);
		}

		illuminate(from, axis, coneAngle) {

			point Pc = transform(camMat,Pw);
			Cl = CalcLight( PL, Pc, Pcp, falloffStart, falloff_x, falloff_y, falloff_z, falloff_w,
							innerAngle, outerAngle, aspect, bConeLighting, bDirectional, 
							bPureDirectional, scale, range, zcomp(Pc), shadowSource,
							shadowType, shadowMapName, Ps_biased, pow(2,shadowQuality+2),
							shadowSoftness, moddedShadowIntensity, from, shadowsOnly,
							gobo, intensity, shadowColor, lcol,
							bbox_min_x, bbox_min_y, bbox_min_z, 
							bbox_max_x, bbox_max_y, bbox_max_z );
		}
	}

	// Directional, so use solar()
	else
	{
		point to = point "shader"(0, 0, 1);
		vector A = to - from;

		solar( A , 0.0 ){

			float shadowDist = 1000;
			point moddedFrom = Ps + (shadowDist*normalize(-A));
			point Pc = transform(camMat,Pw);
			Cl = CalcLight( PL, Pc, Pcp, falloffStart, falloff_x, falloff_y, falloff_z, falloff_w,
							innerAngle, outerAngle, aspect, bConeLighting, bDirectional,
							bPureDirectional, scale, range, zcomp(Pc), shadowSource,
							shadowType, shadowMapName, Ps_biased, pow(2,shadowQuality+2),
							shadowSoftness, moddedShadowIntensity, moddedFrom, shadowsOnly,
							gobo, intensity, shadowColor, lcol,
							bbox_min_x, bbox_min_y, bbox_min_z, 
							bbox_max_x, bbox_max_y, bbox_max_z );
		}
	}

}
