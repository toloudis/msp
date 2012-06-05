/*****************************************************************************
**	prtConeParticleGenerator.hpp
**
**		prtConeParticleGenerator is the base class for Terawatt particle
**	generators.  It stores a lot of different parameters which hopefully
**	apply to all Terawatt particle generators.
**	
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/prt/prtConeParticleGenerator.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/eff/effParticleData.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matUVATexture.hpp"
#include "Graphics/prt/private/prtAnimationUtil.hpp"
#include "Graphics/prt/prtEmitter.hpp"
#include "Graphics/prt/prtVertexAnimation.hpp"
#include "Graphics/sprt/sprtSpriteData.hpp"
#include "Graphics/sprt/sprtSpriteGroupFrag.hpp"


//============================================================================
//============================================================================
class ConeParticle : public sprtSpriteData
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		ConeParticle(	envType::Int32 i_ID,
						const maPoint3d& i_Position,
						const maVector3d& i_Velocity,
						const maVector3d& i_Acceleration,
						float i_SimulationStartTime,
						float i_StartTimeDelta, 
						float i_LifeTime, 
						float i_Scale,
						float i_InitialRotation,
						float i_AngVel,
						float i_AngAcc,
						int i_InitialFrame);

		//--------------------------------------------------------------------
		//	Linked list accessors
		//--------------------------------------------------------------------
		ConeParticle* GetNext() { return m_pNext; }
		const ConeParticle* GetNext() const { return m_pNext; }
		ConeParticle* GetPrev() { return m_pPrev; }
		const ConeParticle* GetPrev() const { return m_pPrev; }
		
		//--------------------------------------------------------------------
		//	Linked list mutators
		//--------------------------------------------------------------------
		void SetNext(ConeParticle* i_Next) {	m_pNext = i_Next;  
												sprtSpriteData::SetNext(static_cast<sprtSpriteData*>(i_Next)); }
		void SetPrev(ConeParticle* i_Prev) {	m_pPrev = i_Prev;
												sprtSpriteData::SetPrev(static_cast<sprtSpriteData*>(i_Prev)); }

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void* operator new(size_t size);
		void operator delete(void*);

		//--------------------------------------------------------------------
		// accessors
		//--------------------------------------------------------------------
		envType::Int32 GetID() { return m_ID; }

		//--------------------------------------------------------------------
		//	Think thinks each particle.
		//--------------------------------------------------------------------
		void Think(float i_SimulationTime, float i_SimulationTimeDelta);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetAge(float i_StartTime, float i_Age, float i_Lifespan);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		bool IsExpired() const { return m_Expired; }

		//--------------------------------------------------------------------
		//	Static functions to provide information to all of the particles
		//	at once.  This allows us to only iterate over all of them
		//	once per frame.
		//--------------------------------------------------------------------
		static void SetAlphaAnimation(const anTypedAnimation<float>* i_Anim) { m_pAlphaAnim = i_Anim; }
		static void SetExpansionBox(maAxisBox& o_ToExpand) { m_pBoxToExpand = &o_ToExpand; }
		static void SetScaleLinear(bool i_Linear) { m_LinearScale = i_Linear; }
		static void SetScaleFactor(float i_Factor) { m_ScaleFactor = i_Factor; }
		static void SetNumUVAFrames(int i_Num) { m_NumUVAFrames = i_Num; }
		static void SetUVAMode(prtSpriteGroupParticleGenerator::UVAMode i_Mode) {m_UVAMode = i_Mode;}
		static void SetUVATexture(const matUVATexture* i_Texture) {m_Texture = i_Texture;}
		static void SetStreakLength(float i_Time) { m_StreakLength = i_Time; }

	private:
		ConeParticle* m_pNext;
		ConeParticle* m_pPrev;

		envType::Int32 m_ID;
		maVector3d m_StartVelocity;
		maVector3d m_Velocity;
		maVector3d m_Acceleration;
		float m_StartTime;
		float m_Elapsed;
		float m_LifeTime;
		float m_StartAngVel;
		float m_AngVel;
		float m_AngAcc;
		float m_InitScale;
		bool m_Expired;

		static const anTypedAnimation<float>* m_pAlphaAnim;
		static maAxisBox* m_pBoxToExpand;
		static bool m_LinearScale;
		static float m_ScaleFactor;
		static int m_NumUVAFrames;
		static prtSpriteGroupParticleGenerator::UVAMode m_UVAMode;
		static const matUVATexture* m_Texture;
		static float m_StreakLength;
};


//============================================================================
//============================================================================
const anTypedAnimation<float>* ConeParticle::m_pAlphaAnim = NULL;
maAxisBox* ConeParticle::m_pBoxToExpand = NULL;
bool ConeParticle::m_LinearScale = false;
float ConeParticle::m_ScaleFactor = 1.0f;
int ConeParticle::m_NumUVAFrames = 0;
prtSpriteGroupParticleGenerator::UVAMode ConeParticle::m_UVAMode = prtSpriteGroupParticleGenerator::e_RandomFrame;
const matUVATexture* ConeParticle::m_Texture = NULL;
float ConeParticle::m_StreakLength = 0;

envPool l_Pool(sizeof(ConeParticle));


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ConeParticle::ConeParticle(	envType::Int32 i_ID,
							const maPoint3d& i_Position, 
							const maVector3d& i_Velocity, 
							const maVector3d& i_Acceleration, 
							float i_SimulationStartTime,
							float i_StartTimeDelta, 
							float i_LifeTime,
							float i_Scale,
							float i_InitialRotation,
							float i_AngVel,
							float i_AngAcc,
							int i_InitialFrame )
:	m_ID(i_ID),
	m_Velocity(i_Velocity),
	m_StartVelocity(i_Velocity),
	m_Acceleration(i_Acceleration),
	m_Elapsed(0.0f),
	m_LifeTime(i_LifeTime),
	m_StartAngVel(i_AngVel),
	m_AngVel(i_AngVel),
	m_AngAcc(i_AngAcc),
	m_InitScale(i_Scale),
	m_Expired(false),
	m_pNext(NULL),
	m_pPrev(NULL),
	m_StartTime(i_SimulationStartTime)
{
	SetStartPosition(i_Position);
	SetPosition(i_Position);
	SetScale(i_Scale);
	SetRotation(i_InitialRotation);
	SetStartRotation(i_InitialRotation);
	SetUVAFrame(i_InitialFrame);

	if ( m_pAlphaAnim )
		SetAlpha(m_pAlphaAnim->GetValue(0.0f));
	else
		SetAlpha(1.0f);	

	Think( i_SimulationStartTime, i_StartTimeDelta );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void* ConeParticle::operator new(size_t iSize)
{
	return l_Pool.Allocate();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ConeParticle::operator delete(void* iPtr)
{
	l_Pool.Deallocate(iPtr);
}

//--------------------------------------------------------------------
//	Think thinks each particle.
//--------------------------------------------------------------------
void ConeParticle::Think(float i_SimulationTime, float i_SimulationTimeDelta)
{
//	//	check if the particle is out of it's time
//	if (   (i_SimulationTime < m_StartTime)
////		|| (i_SimulationTime > (m_StartTime + m_Elapsed)))
//		|| (m_Elapsed >= m_LifeTime))
//	{
//		m_Expired = true;
//		return;
//	}
//	else
//	{
//		m_Expired = false;
//	}
//	//if ( m_Elapsed > m_LifeTime ) return;

	m_Elapsed = i_SimulationTime - m_StartTime;
	float lifetime_param = m_Elapsed / m_LifeTime;

	//	UVA frame
	int frame = 0;
	if (m_UVAMode == prtSpriteGroupParticleGenerator::e_TUV)
	{
		if (m_Texture != NULL)
			frame = m_Texture->GetFrameAtTime(m_Elapsed);
	}
	else if (m_UVAMode == prtSpriteGroupParticleGenerator::e_ScaleToLifetime)
	{
		frame = (int)(lifetime_param * (float)m_NumUVAFrames);
		if ( frame >= m_NumUVAFrames )
			frame = m_NumUVAFrames - 1;
		else if ( frame < 0 )
			frame = 0;
	}
	SetUVAFrame( frame );

	//	motion
	m_Velocity = m_StartVelocity + m_Acceleration * m_Elapsed;

	SetPosition(GetStartPosition() + m_Velocity * m_Elapsed * 0.5f);

	// "last" position computation, position at some time in the past
	float last_elapsed = m_Elapsed - m_StreakLength;
	if (last_elapsed > 0)
	{
		maVector3d last_velocity = m_StartVelocity + m_Acceleration * last_elapsed;
		SetLastPosition( GetStartPosition() + last_velocity * last_elapsed * 0.5f );
	}
	else SetLastPosition( GetStartPosition() );

	float rotation = GetStartRotation();
	m_AngVel = m_StartAngVel + m_AngAcc * m_Elapsed;
	rotation += m_AngVel * m_Elapsed;
	SetRotation(rotation); 

	//	alpha
	if ( m_pAlphaAnim != 0 )
	{
		SetAlpha(m_pAlphaAnim->GetValue(lifetime_param));
	}

	//	scale
	float factor = m_ScaleFactor * m_Elapsed;
	float scale = m_InitScale;
	if ( m_LinearScale )
		scale += factor;
	else
		scale *= 1.0f + factor * (1.0f + factor * 0.5f);

	SetScale(scale);

	//	bounding box
	maPoint3d pos( GetPosition() );
	maAxisBox ab( pos.GetX() - scale, pos.GetX() + scale,
				  pos.GetY() - scale, pos.GetY() + scale,
				  pos.GetZ() - scale, pos.GetZ() + scale );
	m_pBoxToExpand->Union(ab);

	// If end of time then expire it.
	//
	if ( m_Elapsed > m_LifeTime )
		m_Expired = true;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ConeParticle::SetAge(float i_StartTime, float i_Age, float i_Lifespan)
{
	m_Expired = false;
	m_StartTime = i_StartTime;
	m_Elapsed = i_Age;
	m_LifeTime = i_Lifespan;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
struct prtConeParticleGeneratorImp
{
	prtConeParticleGeneratorImp();

	prtParticleManager<ConeParticle> m_Particles;
	float m_RateAccum;
	maPoint3d m_LastPos;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtConeParticleGeneratorImp::prtConeParticleGeneratorImp()
:	m_RateAccum(0.0f),
	m_LastPos(0, 0, 0)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
struct prtConeParticleCreator
{
	float min_lifetime, max_lifetime;
	float start_scale;
	float min_ang, max_ang;
	float min_ang_vel, max_ang_vel;
	float min_ang_acc, max_ang_acc;
	int num_uva_frames;
	prtSpriteGroupParticleGenerator::UVAMode uva_mode;

	ConeParticle* CreateParticle(envType::Int32 i_ID, const maPoint3d& i_Position) const;
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ConeParticle* prtConeParticleCreator::CreateParticle(envType::Int32 i_ID, const maPoint3d& i_Position) const
{
	//	calculate the frame
	int init_frame = 0;
	if ( num_uva_frames && uva_mode == prtSpriteGroupParticleGenerator::e_RandomFrame )
		init_frame = rand() % num_uva_frames;

	ConeParticle* new_particle = new ConeParticle(	i_ID,
													i_Position,
													maPoint3d(0,0,0),
													maPoint3d(0,0,0),
													0,
													0, 
													maFunctions::FloatRand(min_lifetime, max_lifetime),
													start_scale,
													maFunctions::FloatRand(min_ang, max_ang),
													maFunctions::FloatRand(min_ang_vel, max_ang_vel),
													maFunctions::FloatRand(min_ang_acc, max_ang_acc),
													init_frame);
	return new_particle;
}

//--------------------------------------------------------------------
//	The i_CreationTime is the simulation time at which the particle
//	generator was created.
//--------------------------------------------------------------------
prtConeParticleGenerator::prtConeParticleGenerator(float i_CreationTime)
:	prtSpriteGroupParticleGenerator(i_CreationTime)
{
	m_pImp = new prtConeParticleGeneratorImp;
	SetParameter(e_ConeAngle, maConstants::c_fPI_Div_2);
	SetParameter(e_MinSpeed, 5.0f);
	SetParameter(e_MaxSpeed, 5.0f);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtConeParticleGenerator::~prtConeParticleGenerator()
{
	delete m_pImp;
}

//--------------------------------------------------------------------
//	GetNumParameters returns the number of parameters used by the
//	particle generator.  This function should be overridden by 
//	child classes which add parameters.
//--------------------------------------------------------------------
int prtConeParticleGenerator::GetNumParameters() const
{
	return e_NextParameter;
}


//--------------------------------------------------------------------
//	Animate is called by the scScene for each object before it
//	is rendered. 
//--------------------------------------------------------------------
void prtConeParticleGenerator::Animate(float i_SimulationTime)
{
	prtParticleManager<ConeParticle>& manager = m_pImp->m_Particles;

	bool bDirty = m_bAnimDirty;

	prtSpriteGroupParticleGenerator::Animate(i_SimulationTime);

	if ( IsExpired() && (manager.GetNumParticles() == 0) )
	{
		SetFinished();
		return;
	}

	// everything is delta based, so we might 
	// be able to optimize when delta is 0
	if (!bDirty && (GetTimeDelta() == 0.0f)) 
		return;

	maPoint3d cur_model_pos(0.0f, 0.0f, 0.0f);
	const maMatrix4x4* model_transform;
	if ( GetBase()->GetIgnoreParentTransform() )
		model_transform = &(GetBase()->GetTransform());
	else
		model_transform = &(GetBase()->GetTotalTransform());

	model_transform->Transform(cur_model_pos);	

	if ( m_pImp->m_LastPos == maPoint3d(0, 0, 0) )
		m_pImp->m_LastPos = cur_model_pos;

	maAxisBox new_box;
	ConeParticle::SetExpansionBox(new_box);
	ConeParticle::SetAlphaAnimation(GetAlphaProfile());

	bool linear = GetScaleMode() == prtSpriteGroupParticleGenerator::e_Linear;
	float scale_factor = GetParameter(e_ScaleCoeff);
	
	if ( !linear )
		scale_factor = ::log(scale_factor);

	ConeParticle::SetScaleLinear(linear);
	ConeParticle::SetScaleFactor(scale_factor);
	ConeParticle::SetNumUVAFrames(GetNumUVAFrames());
	ConeParticle::SetStreakLength( GetParameter(prtSpriteGroupParticleGenerator::e_StreakLength) );

	DBG_ASSERT(GetMaterial() != NULL, "NULL material for spiral particle generator");
	if (!GetMaterial())
		return;
	DBG_ASSERT(GetMaterial()->TypedData<effParticleData>() != NULL, "Bad effect data type for spiral particle generator");
	if (!GetMaterial()->TypedData<effParticleData>())
		return;
	matTexture* pTex = GetMaterial()->TypedData<effParticleData>()->m_TextureDiffuse;
	ConeParticle::SetUVATexture(dynamic_cast<matUVATexture*>(pTex));
	ConeParticle::SetUVAMode(GetUVAMode());

	float min_lifetime	= GetParameter(prtParticleGenerator::e_MinParticleLifetime);
	float max_lifetime	= GetParameter(prtParticleGenerator::e_MaxParticleLifetime);
	float start_scale	= GetParameter(prtSpriteGroupParticleGenerator::e_InitialScale);
	float min_ang		= GetParameter(prtSpriteGroupParticleGenerator::e_MinStartAngle);
	float max_ang		= GetParameter(prtSpriteGroupParticleGenerator::e_MaxStartAngle);
	float min_ang_vel	= GetParameter(prtSpriteGroupParticleGenerator::e_MinAngularVelocity);
	float max_ang_vel	= GetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularVelocity);
	float min_ang_acc	= GetParameter(prtSpriteGroupParticleGenerator::e_MinAngularAcceleration);
	float max_ang_acc	= GetParameter(prtSpriteGroupParticleGenerator::e_MaxAngularAcceleration);

	if (this->m_pAnimInstance)
	{
		// Create an instance of a class that can create particles for the animation utility function
		prtConeParticleCreator creator = {
		 min_lifetime,	 max_lifetime,	 start_scale,	 min_ang,	 max_ang,
		 min_ang_vel,	 max_ang_vel,	 min_ang_acc,	 max_ang_acc,
		 GetNumUVAFrames(), 	 GetUVAMode() };

		float streak_length = (GetRenderStreaks()) ? GetParameter(prtSpriteGroupParticleGenerator::e_StreakLength) : 0.0f;
		maAxisBox anim_box = prtAnimationUtil::Animate(*m_pAnimInstance, manager, creator, i_SimulationTime, streak_length);

		// We want to use our baked positions as the bounding box,
		// not the simulation positions computed in Think()
		// that used the Expansion Box
		new_box = anim_box;
	}
	else if (!GetPaused())
	{
		// Traditional simulated method
		manager.Think(i_SimulationTime, GetTimeDelta());

		// create new particles, if necessary
		if ( !IsExpired() )
		{
			// create new particles at given rate
			// and interpolate their positions between the new and old generator positions
			float& rate_accum = m_pImp->m_RateAccum;
			float accum_delta = GetTimeDelta();
			float particle_rate = GetParameter(prtParticleGenerator::e_ParticleRate);
			float rate_accum_delta = accum_delta * particle_rate;
			float max_calculated_num_particles = particle_rate * (i_SimulationTime - GetCreationTime());
			rate_accum += rate_accum_delta;

			//	if going backwards and the time is "early" in the simulation, there won't have
			//	been the "max" number of particles yet, so cap the accum so too many
			//	particles don't show up.
			if (accum_delta < 0.0f)
			{
				float est_num_particles = (-rate_accum) + manager.GetNumParticles();
				if (est_num_particles > max_calculated_num_particles)
				{
					float new_rate_accum = manager.GetNumParticles() - max_calculated_num_particles;
					//DBG_LOG4("-----new %6.3f rate_accum %6.3f prtcls=%d max=%6.3f", new_rate_accum, rate_accum, manager.GetNumParticles(), max_calculated_num_particles);
					if (new_rate_accum < 0.0f)
					{
						rate_accum = new_rate_accum; // + rate_accum;	// calc the diff
					}
					else
					{
						rate_accum = 0.0f;
					}
				}
			}

			maRotation offset_rotation;
			offset_rotation.SetValue(maVector3d(0, 0, 1), GetOffsetDirection());
			maMatrix3x3 offset_rotation_matrix = offset_rotation.GetMatrix3x3();

			if (   ( rate_accum > 1.0f )
				|| ( rate_accum < -1.0f ))
			{
				//DBG_LOG6("time %8.3f   rate accum %8.3f   (max accum %8.3f)   particles (%d) time delta(%6.3f) particle rate(%6.3f)", i_SimulationTime, rate_accum, max_calculated_num_particles, manager.GetNumParticles(), accum_delta, particle_rate );
				maVector3d move_delta;
				maVector3d cur_pos;
				move_delta = cur_model_pos - m_pImp->m_LastPos;
				float recip_num_generated = 1.0f / float(int(rate_accum));
				move_delta *= recip_num_generated;
				maVector3d delta_offset(0.0f, 0.0f, 0.0f);
				cur_pos = m_pImp->m_LastPos;

				float max_particles	= GetParameter(prtParticleGenerator::e_MaxParticles);
				float cone_angle	= GetParameter(e_ConeAngle);
				float min_speed		= GetParameter(e_MinSpeed);
				float max_speed		= GetParameter(e_MaxSpeed);
				maVector3d acceleration(GetParameter(e_AccelerationX),
										GetParameter(e_AccelerationY),
										GetParameter(e_AccelerationZ));

				float current_particle_time_delta = GetTimeDelta();
				float particle_time_delta = current_particle_time_delta * recip_num_generated;

				while (   ( rate_accum > 1.0f )
					   || ( rate_accum < -1.0f ))
				{
					//	if too many particles, bail out
					if (    (max_particles > 0) 
						&& (manager.GetNumParticles() >= int(max_particles)) )
						break;

					//	determine the position
					maPoint3d new_pos = GetEmitter()->GetPosition();
					offset_rotation_matrix.Transform(new_pos);
					new_pos += GetOffsetPosition();
					model_transform->Transform(new_pos);
					new_pos += delta_offset;
					delta_offset += move_delta;

					//	calculate the frame
					int init_frame = 0;
					if ( GetNumUVAFrames() && GetUVAMode() == prtSpriteGroupParticleGenerator::e_RandomFrame )
						init_frame = rand() % GetNumUVAFrames();

					//	calculate the velocity
					float theta = maFunctions::FloatRand(-cone_angle, cone_angle);
					float phi = maFunctions::FloatRand(0, maConstants::c_fPI_Times_2);
					float speed = maFunctions::FloatRand(min_speed, max_speed);
					float sin_theta = float(sin(theta));
					float cos_theta = float(cos(theta));
					float sin_phi = float(sin(phi));
					float cos_phi = float(cos(phi));

					maVector3d velocity(speed * sin_theta * cos_phi,
										speed * sin_theta * sin_phi,
										speed * cos_theta);

					offset_rotation_matrix.Transform(velocity);
					model_transform->GetSubMatrix(3, 3).Transform(velocity);

					//	if the time is moving backwards then when generating particles, set the start time
					//	to the current time - life time so the particle will be at the "end" when starting.
					//
					float start_time;
					float life_time;
					life_time = maFunctions::FloatRand(min_lifetime, max_lifetime);
					if ( rate_accum > 1.0f )
						start_time = i_SimulationTime;
					else
					{
						start_time = i_SimulationTime - life_time;
						if (start_time < 0.0f)
							start_time = 0.0f;
					}

					//	create then add the particle
					//
					ConeParticle* new_particle = new ConeParticle(	0, // ids only used for animation
																	new_pos,
																	velocity,
																	acceleration,
																	start_time,
																	current_particle_time_delta, 
																	maFunctions::FloatRand(min_lifetime, max_lifetime),
																	start_scale,
																	maFunctions::FloatRand(min_ang, max_ang),
																	maFunctions::FloatRand(min_ang_vel, max_ang_vel),
																	maFunctions::FloatRand(min_ang_acc, max_ang_acc),
																	init_frame);
					manager.AddParticle(new_particle);
					new_box.Union(new_pos);

					//DBG_LOG3("new particle (%6.3f, %6.3f, %6.3f)", new_pos.GetX(), new_pos.GetY(), new_pos.GetZ() );
			
					if (rate_accum > 1.0f)
						--rate_accum;
					else
						++rate_accum;
					current_particle_time_delta -= particle_time_delta;
				}
			}
		}
	}

	GetFragment()->SetHead( manager.GetHead() );
	GetFragment()->SetBoundingBox(new_box);
	GetFragment()->SetModelSpaceBox( this->m_pAnimInstance != NULL ); // Use model space if animating
	m_pImp->m_LastPos = cur_model_pos;
}

//--------------------------------------------------------------------
//	DeleteAllParticles just take a guess what this one does. go ahead, guess
//--------------------------------------------------------------------
void prtConeParticleGenerator::DeleteAllParticles()
{
	prtParticleManager<ConeParticle>& manager = m_pImp->m_Particles;
	manager.DeleteAllParticles();
}

//--------------------------------------------------------------------
//	ClearParticleAccumulation clears the variable that tracks the particle
//	accumulation, essentially giving particle generation a clean slate
//	to work with. Good for clearing out undesired accumulation that may
//	occur during long pausessuch as mode changes
//--------------------------------------------------------------------
void prtConeParticleGenerator::ClearParticleAccumulation()
{
	m_pImp->m_RateAccum = 0;
}

