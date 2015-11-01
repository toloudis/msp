/*****************************************************************************
**  AnimFuncs.cpp
**
**   Namespace for animation related functions      
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <AnimFuncs.hpp>
#include <AnimUtil.hpp>

#include <maya/MAnimControl.h>
#include <maya/MDagPath.h>
//#include <maya/MFnAnimCurve.h>
#include <maya/MFnBlendShapeDeformer.h>
#include <maya/MFnCamera.h>
#include <maya/MFnTransform.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>


#include <list>
#include <vector>

namespace AnimFuncs
{

	namespace
	{

	bool	bWriteAnimDetails = false;
	bool	bWriteKeyDetails = false;

	//=============================================================================
	//	Chunk types
	//
	//	APKY - Animation Position KeY
	//	ARKY - Animation Rotation KeY
	//	ASKY - Animation Scale KeY
	//	RORD - Rotation ORDer
	//	AVIS - Animation VISibilty 
	//	ASKN - Animation on SKiN surface 
	//	ABLS - Animation Blend Shape
	//	AMKY - Animation Morph target KeY
	//	NNAM - Name
	//	AFCL - Animation FoCal Length for camera
	//	ACOI - Animation Center Of Interest for camera
	//	AFPS - Animation's preferred frames per second playback rate
	//	CASP - Camera film aspect ratio animation
	//	HAPT - Horizontal film aperture
	//	VAPT - Vertical film aperture
	//=============================================================================
	const chDefs::Name c_APKY = chDefs::MakeName('A', 'P', 'K', 'Y');
	const chDefs::Name c_ARKY = chDefs::MakeName('A', 'R', 'K', 'Y');
	const chDefs::Name c_ASKY = chDefs::MakeName('A', 'S', 'K', 'Y');
	const chDefs::Name c_RORD = chDefs::MakeName('R', 'O', 'R', 'D');
	const chDefs::Name c_AVIS = chDefs::MakeName('A', 'V', 'I', 'S');
	const chDefs::Name c_ASKN = chDefs::MakeName('A', 'S', 'K', 'N');

	const chDefs::Name c_ABLS = chDefs::MakeName('A', 'B', 'L', 'S');
	const chDefs::Name c_AMKY = chDefs::MakeName('A', 'M', 'K', 'Y');
	const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
	const chDefs::Name c_WNAM = chDefs::MakeName('W', 'N', 'A', 'M');
	const chDefs::Name c_AFCL = chDefs::MakeName('A', 'F', 'C', 'L');
	const chDefs::Name c_ACOI = chDefs::MakeName('A', 'C', 'O', 'I');
	const chDefs::Name c_AFPS = chDefs::MakeName('A', 'F', 'P', 'S');
	//const chDefs::Name c_CASP = chDefs::MakeName('C', 'A', 'S', 'P');
	const chDefs::Name c_HAPT = chDefs::MakeName('H', 'A', 'P', 'T');
	const chDefs::Name c_VAPT = chDefs::MakeName('V', 'A', 'P', 'T');
	const chDefs::Name c_BGFR = chDefs::MakeName('B', 'G', 'F', 'R');


	struct gsVector3
	{
		float x, y, z;

		gsVector3() { x = y = z = 0; }
		gsVector3(float a, float b, float c) { x = a; y = b; z = c; }

		inline bool operator ==(const gsVector3 &a) const
		{
			return ((a.x == x) && (a.y == y) && (a.z == z));
		}
		inline gsVector3 operator -(const gsVector3 &a)
		{
			return gsVector3(x - a.x, y - a.y, z - a.z);
		}
		inline void operator -=(const gsVector3 &a)
		{
			x -= a.x;
			y -= a.y;
			z -= a.z;
		}
	};

	struct veckey_struct
	{
		gsVector3 vec;
		float	time;

		veckey_struct(const gsVector3 &v, float t)
		{
			vec = v;
			time = t;
		}
	};

	void
	write_veckey(chWriter &o_Writer, veckey_struct &veckey)
	{
		o_Writer.Write(veckey.vec.x);
		o_Writer.Write(veckey.vec.y);
		o_Writer.Write(veckey.vec.z);
		o_Writer.Write(veckey.time);

		if (bWriteKeyDetails)
		{
			cout << "\ttime: " << veckey.time << ", value: " 
					<< veckey.vec.x << " " << veckey.vec.y << " " << veckey.vec.z << endl;
		}
	}

	template<class T>
	struct valkey_struct
	{
		T	val;
		float	time;

		valkey_struct(T v, float t)
		{
			val = v;
			time = t;
		}
	};

	template<class T>
	void write_valkey(chWriter &o_Writer, T &valkey)
	//void write_valkey(chWriter &o_Writer, valkey_struct<T> &valkey)
	{
		o_Writer.Write(valkey.val);
		o_Writer.Write(valkey.time);

		if (bWriteKeyDetails)
		{
			cout << "\ttime: " << valkey.time << ", value: " << valkey.val << endl;
		}
	}


	//========================================================================
	// writes single key to file for current time.
	// If WriteDeltas is true, writes only changes from frame 0.
	//========================================================================
	void write_pose(chWriter &o_Writer, 
					MFnAnimCurve &animx, MFnAnimCurve &animy, MFnAnimCurve &animz, 
					bool i_bWriteDeltas)
	{
		MTime time = MayaUtil::GetCurrentTime();

		gsVector3 vec;
		vec.x = (float) animx.evaluate(time);
		vec.y = (float) animy.evaluate(time);
		vec.z = (float) animz.evaluate(time);

		if (i_bWriteDeltas)
		{
			// Write pose's change from frame 0

			MTime start_time; // default constructor defaults to first frame (1?)
			gsVector3 org;
			org.x = (float) animx.evaluate(start_time);
			org.y = (float) animy.evaluate(start_time);
			org.z = (float) animz.evaluate(start_time);

			o_Writer.Write( envType::Int16(2) ); // two keys same value
			//veckey_struct veckey(vec, (float)time.value());
			gsVector3 delta = vec - org;
			veckey_struct veckey(delta, 0);
			write_veckey(o_Writer, veckey);
			veckey.time = 1;
			write_veckey(o_Writer, veckey);
		}
		else
		{
			// Write position of pose

			o_Writer.Write( envType::Int16(2) ); // two keys same value
			//veckey_struct veckey(vec, (float)time.value());
			veckey_struct veckey(vec, 0);
			write_veckey(o_Writer, veckey);
			veckey.time = 1;
			write_veckey(o_Writer, veckey);
		}
	}

	//========================================================================
	// writes single key to file for current time.
	//========================================================================
	void write_pose(chWriter &o_Writer, 
					MFnAnimCurve &anim)
	{
		MTime time = MayaUtil::GetCurrentTime();

		o_Writer.Write( envType::Int16(2) ); // two keys same value
		//valkey_struct valkey((float) anim.evaluate(time), (float)time.value());
		valkey_struct<float> valkey((float) anim.evaluate(time), 0);
		write_valkey(o_Writer, valkey);
		valkey.time = 1;
		write_valkey(o_Writer, valkey);
	}

	//========================================================================
	// writes keys to file. doesn't write 3 keys with same value in row
	// in order to reduce file size.
	//========================================================================
	void write_channels(chWriter &o_Writer, std::list<float> &keys, float offset,
					MFnAnimCurve &animx, MFnAnimCurve &animy, MFnAnimCurve &animz,
					bool i_bWriteDeltas)
	{
		std::vector<veckey_struct> veckeys;

		gsVector3 org;
		MTime start_time;
		org.x = (float) animx.evaluate(start_time);
		org.y = (float) animy.evaluate(start_time);
		org.z = (float) animz.evaluate(start_time);

		std::list<float>::iterator it;
		gsVector3 vec, lastvec;
		bool	bDelay = false;
		float	delay_time;
		for (it = keys.begin(); it != keys.end(); ++it)
		{
			//MTime time(*it);
			MTime time(*it, MTime::uiUnit());
			vec.x = (float) animx.evaluate(time);
			vec.y = (float) animy.evaluate(time);
			vec.z = (float) animz.evaluate(time);

			if (i_bWriteDeltas)
			{
				vec -= org;
			}

			// Compare vec to previous frame, try not to 
			// write three of the same frame values in a row.
			//
			if ((vec == lastvec) && (it != keys.begin()))
			{
				// delay this frame
				bDelay = true;
				delay_time = (*it);
			}
			else
			{
				if (bDelay)
				{
					// if delaying, write delayed frame now
					veckeys.push_back(veckey_struct(lastvec, delay_time));
					bDelay = false;
				}

				// write current key value
				veckeys.push_back(veckey_struct(vec, (*it)));
				lastvec = vec;
			}
		}

		// Hit end of list, if still delaying frame write it now
		if (bDelay)
		{
			veckeys.push_back(veckey_struct(lastvec, delay_time));
		}

		// Offset the keys by the minimum time
		//MAnimControl query_time;
		//float offset = (float) query_time.animationStartTime().value();

		// Now actually write keys
		//	
		envType::Int16 num_keys = veckeys.size();
		o_Writer.Write( num_keys );
		for (int i=0; i<num_keys; i++)
		{
			veckeys[i].time -= offset;
			write_veckey(o_Writer, veckeys[i]);
		}
	}

	//========================================================================
	// writes keys to file. doesn't write 3 keys with same value in row
	// in order to reduce file size.
	//========================================================================
	template<class T>
	void write_channel(chWriter &o_Writer, 
					std::list<float> &keys, 
					float offset,
					MFnAnimCurve &anim)
	{
		std::vector<valkey_struct<T>> valkeys;

		std::list<float>::iterator it;
		T  val, lastval = 0;
		bool	bDelay = false;
		float	delay_time;
		for (it = keys.begin(); it != keys.end(); ++it)
		{
			//MTime time(*it);
			MTime time(*it, MTime::uiUnit());
			val = (T) anim.evaluate(time);

			// Compare val to previous frame, try not to 
			// write three of the same frame values in a row.
			//
			if ((val == lastval) && (it != keys.begin()))
			{
				// delay this frame
				bDelay = true;
				delay_time = (*it);
			}
			else
			{
				if (bDelay)
				{
					// if delaying, write delayed frame now
					valkeys.push_back(valkey_struct<T>(lastval, delay_time));
					bDelay = false;
				}

				// write current key value
				valkeys.push_back(valkey_struct<T>(val, (*it)));
				lastval = val;
			}
		}

		// Hit end of list, if still delaying frame write it now
		if (bDelay)
		{
			valkeys.push_back(valkey_struct<T>(lastval, delay_time));
		}

		// Offset the keys by the minimum time
		//MAnimControl query_time;
		//float offset = (float) query_time.animationStartTime().value();

		// Now actually write keys
		//	
		envType::Int16 num_keys = valkeys.size();
		if (bWriteKeyDetails)
			cout << "Writing " << num_keys << " keys" << endl;
		o_Writer.Write( num_keys );
		for (int i=0; i<num_keys; i++)
		{
			valkeys[i].time -= offset;
			write_valkey(o_Writer, valkeys[i]);
		}
	}

	//========================================================================
	// find x,y,z components of animation and write channels
	//========================================================================
	void write_animation_group(chWriter &o_Writer,
							MFnDependencyNode &node,
							MString group_name, 
							chDefs::Name name, 
							double i_MinTime, 
							double i_MaxTime, 
							bool i_bSinglePose, 
							bool i_bWriteDeltas = false)
	{
		MStatus status1, status2, status3;

		MFnAnimCurve* animx = AnimFuncs::GetAnimCurve(group_name + "X", node, status1);
		MFnAnimCurve* animy = AnimFuncs::GetAnimCurve(group_name + "Y", node, status2);
		MFnAnimCurve* animz = AnimFuncs::GetAnimCurve(group_name + "Z", node, status3);

		if (status1 == MS::kSuccess && status2 == MS::kSuccess && status3 == MS::kSuccess)
		{
			if (i_bSinglePose)
			{
				o_Writer.WriteChunkHeader(name, 0, false);
				write_pose(o_Writer, *animx, *animy, *animz, i_bWriteDeltas);
				o_Writer.FinishChunk();
				delete animx;
				delete animy;
				delete animz;
				return;
			}
			else
			{
				std::list<float> keys;
				AnimFuncs::GatherKeys(keys, *animx, *animy, *animz, i_MinTime, i_MaxTime);

				if (!keys.empty())
				{
					if (bWriteAnimDetails)
						cout << "found animation on " << group_name << endl;
					o_Writer.WriteChunkHeader(name, 0, false);
					write_channels(o_Writer, keys, i_MinTime, *animx, *animy, *animz, i_bWriteDeltas);
					o_Writer.FinishChunk();
					delete animx;
					delete animy;
					delete animz;
					return;
				}
			}
		}
		
		if (bWriteAnimDetails)
			cout << "missing animation channels on " << group_name << endl;
		
		delete animx;
		delete animy;
		delete animz;
	}

	//========================================================================
	// get rotation order from this node in order to know how to interpret
	//	the animation channels on rotate x,y,z.
	//========================================================================
	void write_rotation_order(chWriter &o_Writer, MFnDependencyNode &node)
	{
		MStatus status;
		MFnTransform transform(node.object(), &status);
		if (status)
		{
			o_Writer.WriteChunkHeader(c_RORD, 0, false);

			envType::UInt8 order = 0;

			MTransformationMatrix::RotationOrder rOrder = transform.rotationOrder();
			switch (rOrder)
			{
			default:
			case MTransformationMatrix::kXYZ:
				order = 0;
				break;
			case MTransformationMatrix::kYZX:
				order = 1;
				break;
			case MTransformationMatrix::kZXY:
				order = 2;
				break;
			case MTransformationMatrix::kXZY:
				order = 3;
				break;
			case MTransformationMatrix::kYXZ:
				order = 4;
				break;
			case MTransformationMatrix::kZYX:
				order = 5;
				break;
			}

			o_Writer.Write(order);

			o_Writer.FinishChunk();
		}
	}

	}	// end of namespace


	//========================================================================
	// Get range of animation times from Maya's playback slider
	//========================================================================
	void GetTimeSliderRange(double &o_Min, double &o_Max)
	{
		MAnimControl query_time;

		//cout << "MinTime: " << query_time.minTime() << endl;
		//cout << "MaxTime: " << query_time.maxTime() << endl;
		//cout << "animationStartTime: " << query_time.animationStartTime() << endl;
		//cout << "animationEndTime: " << query_time.animationEndTime() << endl;

		o_Min = query_time.animationStartTime().value();
		o_Max = query_time.animationEndTime().value();
	}

	//========================================================================
	// gets keys from channel into list
	// If i_bMerge is true, then it adds keys to the existing list
	//========================================================================
	void GatherKeys(std::list<float> &o_Keys, 
					MFnAnimCurve &anim,
					double i_MinTime, 
					double i_MaxTime,
					bool i_bMerge)
	{
		//double min, max;
		//AnimFuncs::GetTimeSliderRange(min, max);

		int numkeys = anim.numKeyframes();

		if (!i_bMerge)
			o_Keys.clear();

		unsigned int k;
		for (k = 0; k < anim.numKeyframes(); k++) 
		{
			MTime time = anim.time(k);
		//	time.setUnit(MTime::uiUnit());

			double time_val = time.value();
			if (time_val >= i_MinTime && time_val <= i_MaxTime)
				o_Keys.push_back( (float) time_val  );
		}

		o_Keys.sort();
		o_Keys.unique();
	}

	//========================================================================
	// gets keys from channel into list
	//========================================================================
	void GatherKeys(std::list<float> &keys, 
					MFnAnimCurve &animx, MFnAnimCurve &animy, MFnAnimCurve &animz,
					double i_MinTime, double i_MaxTime)
	{
		//double min, max;
		//AnimFuncs::GetTimeSliderRange(min, max);

		int numkeysx = animx.numKeyframes();
		int numkeysy = animy.numKeyframes();
		int numkeysz = animz.numKeyframes();

		std::list<float> keysx;
		std::list<float> keysy;
		std::list<float> keysz;

		unsigned int k;
		for (k = 0; k < animx.numKeyframes(); k++) 
		{
			MTime time = animx.time(k);
		//	time.setUnit(MTime::uiUnit());

			double time_val = time.value();
			if (time_val >= i_MinTime && time_val <= i_MaxTime)
				keysx.push_back( (float) time_val  );
		}

		for (k = 0; k < animy.numKeyframes(); k++) 
		{
			MTime time = animy.time(k);
		//	time.setUnit(MTime::uiUnit());

			double time_val = time.value();
			if (time_val >= i_MinTime && time_val <= i_MaxTime)
				keysy.push_back( (float) time_val  );
		}

		for (k = 0; k < animz.numKeyframes(); k++) 
		{
			MTime time = animz.time(k);
		//	time.setUnit(MTime::uiUnit());

			double time_val = time.value();
			if (time_val >= i_MinTime && time_val <= i_MaxTime)
				keysz.push_back( (float) time_val  );
		}

		keys.clear();
		keys.merge(keysx);
		keys.merge(keysy);
		keys.merge(keysz);
		keys.unique();
	}

	//========================================================================
	// Gets animation curve by name using Maya's plug system
	//========================================================================
	MFnAnimCurve* GetAnimCurve(MString name, MFnDependencyNode &node, MStatus &status)
	{
		MPlug plug = node.findPlug(name, &status);
		if(status == MS::kSuccess) {
			return GetAnimCurve(plug, status);
		}
		status = MS::kFailure;
		MFnAnimCurve* dummy = new MFnAnimCurve();  
		return dummy;
	}
	MFnAnimCurve* GetAnimCurve(MPlug plug, MStatus &status)
	{
		MPlugArray connections;
		
		if(plug.connectedTo(connections, true, false, &status)) {
			if(status == MS::kSuccess && connections.length() > 0) {
				for(unsigned int j = 0; j < connections.length(); j++) {
					MObject node = connections[j].node(&status);

					//cout << "Plug connected to type: " << node.apiTypeStr() << endl;

					MFnAnimCurve* anim = new MFnAnimCurve(node, &status);
					if(status == MS::kSuccess) {
						status = MS::kSuccess;
						return anim;
					}
				}
			}
		}

		status = MS::kFailure;
		MFnAnimCurve* dummy = new MFnAnimCurve();
		return dummy;
	}


	//========================================================================
	//	WriteAnimation - get animation channels from given node and 
	//		writes keys out to chunnk writer.
	//========================================================================
	void WriteAnimation(chWriter &o_Writer, 
								MFnDependencyNode &node, 
								double i_MinTime, 
								double i_MaxTime, 
								bool i_bSinglePose, 
								bool i_bWriteDeltas)
	{
		MString name = node.name();

		write_animation_group(o_Writer, node, "translate", c_APKY, i_MinTime, i_MaxTime, i_bSinglePose, i_bWriteDeltas);
		write_rotation_order(o_Writer, node);
		write_animation_group(o_Writer, node, "rotate", c_ARKY, i_MinTime, i_MaxTime, i_bSinglePose, i_bWriteDeltas);
		write_animation_group(o_Writer, node, "scale", c_ASKY, i_MinTime, i_MaxTime, i_bSinglePose, i_bWriteDeltas);

		if (!i_bSinglePose && !i_bWriteDeltas)
		{
			MStatus status;
			MFnAnimCurve* visCurve = AnimFuncs::GetAnimCurve("visibility", node, status);
			if (status == MS::kSuccess)
			{
				if (bWriteAnimDetails)
					cout << "Found animation channel on visibility, node = " << node.name() << endl;
			
				std::list<float> keys;
				AnimFuncs::GatherKeys(keys, *visCurve, i_MinTime, i_MaxTime);

				o_Writer.WriteChunkHeader(c_AVIS, 0, false);
				write_channel<bool>(o_Writer, keys, i_MinTime, *visCurve);
				o_Writer.FinishChunk();
			}
			delete visCurve;
		}
	}
							
	
	//========================================================================
	//	WriteSkinVisibility - write visibility animation from the
	//	transform above the skinned surface in the given dagPath
	//========================================================================
	void WriteSkinVisibility(chWriter &o_Writer, 
							 MDagPath &dagPath, 
							 const MString &skin_name, 
							 double i_MinTime, 
							 double i_MaxTime)
	{
		MStatus status;
		MObject obj = dagPath.transform(&status);
		if (status == MS::kSuccess)
		{
			MFnTransform xform(obj, &status);
			if (status == MS::kSuccess)
			{
				MFnAnimCurve* visCurve = AnimFuncs::GetAnimCurve("visibility", xform, status);
				if (status == MS::kSuccess)
				{
					if (bWriteAnimDetails)
						cout << "Found animation channel on visibility, skin = " << skin_name << endl;
				
					std::list<float> keys;
					AnimFuncs::GatherKeys(keys, *visCurve, i_MinTime, i_MaxTime);

					// Write skin animation, consists of skin name and visibility animation
					o_Writer.WriteChunkHeader(c_ASKN, 0, true);

					o_Writer.WriteChunkHeader(c_NNAM, 0, false);
					o_Writer.Write(MayaUtil::PrepareName(skin_name).asUTF8());
					o_Writer.FinishChunk();

					o_Writer.WriteChunkHeader(c_AVIS, 0, false);
					write_channel<bool>(o_Writer, keys, i_MinTime, *visCurve);
					o_Writer.FinishChunk();

					o_Writer.FinishChunk();
				}
				delete visCurve;
			}
		}
	}

	//========================================================================
	//	WriteWeightAnimation - get blend shape anims from given node and 
	//		writes keys out to chunk writer.
	//========================================================================
	void WriteWeightAnimation(chWriter &o_Writer, 
								MFnBlendShapeDeformer &blend, 
								double i_MinTime, 
								double i_MaxTime, 
								bool i_bSinglePose)
	{
		MStatus status;

		// This temporary code writes the current state of the blend shape
		// if "pose" is turned on. So, it doesn't check to see if there is
		// an animation curve (which I am having trouble with becuase of baking).
		if (i_bSinglePose)
		{
			float weight = blend.weight(0, &status);
			if (status == MS::kSuccess) 
			{
				if (bWriteAnimDetails)
					cout << "Writing blend shape pose: " << weight << endl;

				o_Writer.WriteChunkHeader(c_ABLS, 0, true);	// Blend Shape Animation
				o_Writer.WriteChunkHeader(c_NNAM, 0, false);
				o_Writer.Write(MayaUtil::PrepareName(blend.name()).asUTF8());
				o_Writer.FinishChunk();

				o_Writer.WriteChunkHeader(c_AMKY, 0, false);	
				o_Writer.Write( envType::Int16(2) ); // two keys same value
				valkey_struct<float> valkey(weight, 0);
				write_valkey(o_Writer, valkey);
				valkey.time = 1;
				write_valkey(o_Writer, valkey);
				o_Writer.FinishChunk();
								
				o_Writer.FinishChunk();	// c_ABLS
			}
		}

		// weight is an array plug, one per target object
		MPlug plug = blend.findPlug("weight", &status);
		if (status == MS::kSuccess) 
		{	
			int num_elements = plug.numElements();
			if (bWriteAnimDetails)
				cout << "Num elements in weight plug: " << num_elements << endl;
			for (int i=0; i<num_elements; ++i)
			{
				MPlug elem = plug.elementByPhysicalIndex(i);

				MPlugArray connections;
				if (elem.connectedTo(connections, true, false, &status)) 
				{
					if (status == MS::kSuccess && connections.length() > 0) 
					{
						for(unsigned int j = 0; j < connections.length(); j++) 
						{
							MObject node = connections[j].node(&status);
							MFnAnimCurve anim_curve(node, &status);
							if (status == MS::kSuccess) 
							{
								if (bWriteAnimDetails)
									cout << "found anim curve on " << blend.name() << endl;
								o_Writer.WriteChunkHeader(c_ABLS, 0, true);	// Blend Shape Animation

								o_Writer.WriteChunkHeader(c_NNAM, 0, false);
								o_Writer.Write(MayaUtil::PrepareName(blend.name()).asUTF8());
								o_Writer.FinishChunk();
								

								// Get alias name for this weight
								MString aliasName;
								if (blend.getPlugsAlias(elem, aliasName, &status))
								{
									if (bWriteAnimDetails)
										cout << "Have aliased name " << aliasName << endl;
									o_Writer.WriteChunkHeader(c_WNAM, 0, false);
									o_Writer.Write(aliasName.asUTF8());
									o_Writer.FinishChunk();
								}

								if (i_bSinglePose)
								{
									o_Writer.WriteChunkHeader(c_AMKY, 0, false);
									write_pose(o_Writer, anim_curve);
									o_Writer.FinishChunk();
								}
								else
								{
									std::list<float> keys;
									AnimFuncs::GatherKeys(keys, anim_curve, i_MinTime, i_MaxTime);
					
									o_Writer.WriteChunkHeader(c_AMKY, 0, false);
									write_channel<float>(o_Writer, keys, i_MinTime, anim_curve);
									o_Writer.FinishChunk();
								}

								o_Writer.FinishChunk();	// c_ABLS
							}
						}
					}
				}
			}
		}
	}

	//========================================================================
	//	WriteCameraAnimation - write camera movement to file. Needs parent
	//	node in order to get transformation animation channels.
	//========================================================================
	void WriteCameraAnimation(chWriter &o_Writer, 
										MFnCamera &camera,
										MFnTransform &parent, 
										double i_MinTime, 
										double i_MaxTime)
	{
		// Write out transformation first
		const bool c_SinglePose = false;
		write_animation_group(o_Writer, parent, "translate", c_APKY, i_MinTime, i_MaxTime, c_SinglePose);
		write_rotation_order(o_Writer, parent);
		write_animation_group(o_Writer, parent, "rotate", c_ARKY, i_MinTime, i_MaxTime, c_SinglePose);
		write_animation_group(o_Writer, parent, "scale", c_ASKY, i_MinTime, i_MaxTime, c_SinglePose);

		// then write focalLength curve
		MStatus status;
		MFnAnimCurve* flCurve = AnimFuncs::GetAnimCurve("focalLength", camera, status);
		if (status == MS::kSuccess) 
		{
			if (bWriteAnimDetails)
				cout << "found anim curve on focalLength" << endl;
		
			std::list<float> keys;
			AnimFuncs::GatherKeys(keys, *flCurve, i_MinTime, i_MaxTime);

			o_Writer.WriteChunkHeader(c_AFCL, 0, false);
			write_channel<float>(o_Writer, keys, i_MinTime, *flCurve);
			o_Writer.FinishChunk();
		}
		delete flCurve;

		// write center of interest distance
		MFnAnimCurve* coiCurve = AnimFuncs::GetAnimCurve("centerOfInterest", camera, status);
		if (status == MS::kSuccess) 
		{
			if (bWriteAnimDetails)
				cout << "found anim curve on centerOfInterest" << endl;
		
			std::list<float> keys;
			AnimFuncs::GatherKeys(keys, *coiCurve, i_MinTime, i_MaxTime);

			o_Writer.WriteChunkHeader(c_ACOI, 0, false);
			write_channel<float>(o_Writer, keys, i_MinTime, *coiCurve);
			o_Writer.FinishChunk();
		}
		delete coiCurve;


		// Get anim curves related to film aspect ratio
		MFnAnimCurve* haptCurve = AnimFuncs::GetAnimCurve("horizontalFilmAperture", camera, status);
		if (status == MS::kSuccess) 
		{
			if (bWriteAnimDetails)
				cout << "found anim curve on horizontalFilmAperture" << endl;
		
			std::list<float> keys;
			AnimFuncs::GatherKeys(keys, *haptCurve, i_MinTime, i_MaxTime);

			o_Writer.WriteChunkHeader(c_HAPT, 0, false);
			write_channel<float>(o_Writer, keys, i_MinTime, *haptCurve);
			o_Writer.FinishChunk();
		}
		delete haptCurve;
		MFnAnimCurve* vaptCurve = AnimFuncs::GetAnimCurve("verticalFilmAperture", camera, status);
		if (status == MS::kSuccess) 
		{
			if (bWriteAnimDetails)
				cout << "found anim curve on verticalFilmAperture" << endl;
		
			std::list<float> keys;
			AnimFuncs::GatherKeys(keys, *vaptCurve, i_MinTime, i_MaxTime);

			o_Writer.WriteChunkHeader(c_VAPT, 0, false);
			write_channel<float>(o_Writer, keys, i_MinTime, *vaptCurve);
			o_Writer.FinishChunk();
		}
		delete vaptCurve;

		//// Get anim curves related to film aspect ratio
		//MStatus hstatus, vstatus;
		//MFnAnimCurve haptCurve = AnimFuncs::GetAnimCurve("horizontalFilmAperture", camera, hstatus);
		//MFnAnimCurve vaptCurve = AnimFuncs::GetAnimCurve("verticalFilmAperture", camera, vstatus);
		//if (hstatus == MS::kSuccess && vstatus == MS::kSuccess) 
		//{
		//	cout << "found anim curve on horizontalFilmAperture and verticalFilmAperture" << endl;
		//	std::list<float> keys;
		//	AnimFuncs::GatherKeys(keys, haptCurve);
		//	const bool bMergeKeys = true;
		//	AnimFuncs::GatherKeys(keys, vaptCurve, bMergeKeys);

		//	if (!keys.empty())
		//	{
		//		// Gather up aspect ratio animation
		//		std::map<float, float> aspect_keys;
		//
		//		std::list<float>::iterator it;
		//		float height, width, aspect;
		//		for (it = keys.begin(); it != keys.end(); ++it)
		//		{
		//			MTime time(*it, MTime::uiUnit());
		//			height = vaptCurve.evaluate(time);
		//			width = haptCurve.evaluate(time);

		//			// Aspect Ratio is width/height of the camera aperture
		//			if (height > 0)
		//			{
		//				aspect = width / height;
		//				aspect_keys[*it] = aspect;
		//			}
		//		}

		//		// Offset the keys by the minimum time
		//		MAnimControl query_time;
		//		float time_offset = (float) query_time.animationStartTime().value();

		//		// Write aspect ratio chunk
		//		o_Writer.WriteChunkHeader(c_CASP, 0, false);
		//		AnimUtil::WriteChannel(o_Writer, aspect_keys, time_offset);
		//		o_Writer.FinishChunk();
		//	}
		//}
	}

	//========================================================================
	// Add a chunk with the animation's frame rate to the chunk writer
	//========================================================================
	void WriteCurrentFrameRate(chWriter &o_Writer)
	{
		o_Writer.WriteChunkHeader(c_AFPS, 0, false);
		float fps = MayaUtil::GetCurrentFrameRate();
		o_Writer.Write( fps );
		cout << "Exporting frame rate: " << fps << endl;
		o_Writer.FinishChunk();
	}

	//========================================================================
	// Add a chunk with the animation's start frame number to the chunk writer
	//========================================================================
	void WriteBeginFrame(chWriter &o_Writer, float i_BeginFrame)
	{
		cout << "Exporting time origin: " << i_BeginFrame << endl;
		o_Writer.WriteChunkHeader(c_BGFR, 0, false);
		o_Writer.Write( i_BeginFrame );
		o_Writer.FinishChunk();
	}


}