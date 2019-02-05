/* HEADER *********************************************************************************/
/*!
\file   CharacterController.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "CharacterController.h"
#include "System/Input/InputSystem.h"
#include "System/Input/InputMap.h"
#include "System/Driver/Driver.h"
#include "System/Behaviour/BehaviourSystem.h"  

#include "System/Scene/SceneSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Collision/CollisionSystem.h"    
#include "System/Time/TimeSystem.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Editor/EGUI.h"
#include "Utility/DebugAssert.h"

// custom includes
#include "System/Physics/PhysicsSystem.h"
#include "Math/MathLib.h"
#include "Component/RigidBody.h"
#include "Component/SpriteRenderer.h"
#include "Component/Transform.h"
#include "Component/Convex.h"
#include "Component/Collider.h"
#include "Component/AudioSource.h"
#include "System/Database/DatabaseSystem.h"

/* AUDIO DEFINES
*/

#define A_WILORUN_1 "WiloStepOne.wav"
#define A_WILORUN_2 "WiloStepTwo.wav"
#define A_WILORUN_3 "WiloStepThree.wav"
#define A_WILORUN_4 "WiloStepFour.wav"
#define A_WILORUN_5 "WiloStepFive.wav"

#define A_WILOSTRIKE_1 "wilo_strike01.wav"
#define A_WILOSTRIKE_2 "wilo_strike02.wav"
#define A_WILOSTRIKE_3 "wilo_strike03.wav"

#define A_WILOATTACK_1 "Wilo_Attack1.wav"
#define A_WILOATTACK_2 "Wilo_Attack2.wav"
#define A_WILOATTACK_3 "Wilo_Attack3.wav"

#define A_WILOLANDING "Wilo_Landing.wav"
#define A_WILOLANDING_2 "WiloStepThree.wav"

#define A_WILOJUMP "Wilo_Jump.wav"
#define A_WILOHIT "Wilo_Hit.wav"

#define A_WILOROLL "EnemyHit.wav"

#define A_WILO_BLAST "ForceT2_Explosion.wav"
#define A_WILLO_FLAME "ForceT1_Flame.wav"
#define A_WILO_SPIKES "FormT2_FlingOnly.wav"


namespace Dystopia
{
    namespace CharacterController_MSG
    {
        template<typename ... Ts>
        void SendInternalMessage(Behaviour * _ptr, const char * _FuncName, Ts ... _Params)
        { 
            EngineCore::GetInstance()->Get<BehaviourSystem>()->SendInternalMessage(_ptr, _FuncName, _Params...);
        }
        
        template<typename ... Ts>
        void SendExternalMessage(uint64_t _ObjectID, const char * _FuncName, Ts ... _Params)
        {
            EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ObjectID, _FuncName, _Params...);       
        }
        template<typename ... Ts>
        void SendExternalMessage(const GameObject * _ptr, const char * _FuncName, Ts ... _Params)
        {
            EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ptr, _FuncName, _Params...);
        }
        
        template<typename ... Ts>
        void SendExternalMessage(GameObject * _ptr, const char * _FuncName, Ts ... _Params)
        {
            EngineCore::GetInstance()->Get<BehaviourSystem>()->SendExternalMessage(_ptr, _FuncName, _Params...);
        }
        
        template<typename ... Ts>
        void SendAllMessage(const char * _FuncName, Ts ... _Params)
        {
            EngineCore::GetInstance()->Get<BehaviourSystem>()->SendAllMessage(_FuncName, _Params...);
        }
    }
    CharacterController::CharacterController()
        : mpInput{nullptr}, 
        mpBody{nullptr}, 
        mpAnim{nullptr},
		mpCollider{nullptr},
		mpMyHitBox{nullptr},
        mpCastPivot{nullptr},
        mpAudioSrc{nullptr},
		mbDisableControls{false},
        mStatus{Entering},
        mbIsController{false}, 
        mbFaceRight{true}, 
        mbGotHit{false}, 
        mbLanded{false}, 
        mbIsJumping{false}, 
        mfAttackDelay{0}, 
        mfCastingDelay{0}, 
        mfRollDelay{0}, 
        mfFlinchDelay{0}, 
        mfLandDelay{0}, 
        mfTimer{ 0 }, 
        mbDying{ false }, 
        mCurrentState{ FStateTypes::INVALID }, 
        mNextState{ FStateTypes::INVALID },
        mfMaxSpeed{ 0 }, 
        mfCurrentSpeed{ 0 }, 
        mfJumpStrength{ 0 }, 
        mfAirMovementScalerForward{ 0 }, 
        mfAirMovementScalerBackward{ 0 }, 
        mnPriorityDir{ 0 }, 
        //mfAccumulatedTime{ 0 },
        mfAnimSpdIdle{0.016f},
        mfAnimSpdRunning{0.048f},
        mfAnimSpdJumping{0.016f},
        mfAnimSpdFalling{0.016f},
        mnCurrentAttackChain{0},
        mbSpammedAttack{false},
        mbIsInForce{true},
        mbSpammedCast{false},
		mfAttackPower{1.f},
		mfAttackKnockback{0.f},
		mfRollForce{0},
		mbAppliedAtk{false},
		mfInternalForceMultiplier{1000.f},
		mnStateDebug{-1},
		mfAttackPushfoward{50.f},
		mbChainedInto{false},
		mnManualSkill{0},
		mChainSkillName{"",""},
		mManualSkillName{{"",""},{"",""}}
    {
        for (int i = FStateTypes::INVALID + 1; i < FStateTypes::OUT_OF_RANGE; i ++)
        {
            //<LoadState>
            mStates[0] = new Idling{this};
            mStates[1] = new Running{this};
            mStates[2] = new Jumping{this};
            mStates[3] = new Attacking{this};
            mStates[4] = new Casting{this};
            mStates[5] = new Rolling{this};
            mStates[6] = new Flinching{this};
            mStates[7] = new Falling{this};
            mStates[8] = new Landing{this};
            mStates[9] = new Death{this};
            mStates[10] = new Duck{this};
            //</LoadState>
        }
        mCurrentState = mNextState = static_cast<FStateTypes>(0);
    }
    CharacterController::CharacterController(const CharacterController& _rhs)
        : mpInput{ nullptr }, 
        mpBody{ nullptr },  
        mpAnim{nullptr},
		mpCollider{nullptr},
		mpMyHitBox{nullptr},
        mpCastPivot{nullptr},
        mpAudioSrc{nullptr},
        mpHitAudioSrc{nullptr},
		mbDisableControls{_rhs.mbDisableControls},
        mStatus{ _rhs.mStatus },
        mbIsController{ _rhs.mbIsController }, 
        mbFaceRight{ _rhs.mbFaceRight }, 
        mbGotHit{ _rhs.mbGotHit },
        mbLanded{ _rhs.mbLanded }, 
        mbIsJumping{ _rhs.mbIsJumping }, 
        mfAttackDelay{ _rhs.mfAttackDelay },
        mfCastingDelay{ _rhs.mfCastingDelay }, 
        mfRollDelay{ _rhs.mfRollDelay }, 
        mfFlinchDelay{ _rhs.mfFlinchDelay }, 
        mfLandDelay{ _rhs.mfLandDelay },
        mfTimer{ _rhs.mfTimer }, 
        mbDying{ _rhs.mbDying }, 
        mCurrentState{ _rhs.mCurrentState }, 
        mNextState{ _rhs.mNextState },
        mfMaxSpeed{ _rhs.mfMaxSpeed }, 
        mfCurrentSpeed{ _rhs.mfCurrentSpeed }, 
        mfJumpStrength{_rhs.mfJumpStrength}, 
        mfAirMovementScalerForward{ _rhs.mfAirMovementScalerForward }, 
        mfAirMovementScalerBackward{_rhs.mfAirMovementScalerBackward }, 
        mnPriorityDir{_rhs.mnPriorityDir},
        //mfAccumulatedTime{ _rhs.mfAccumulatedTime },
        mfAnimSpdIdle{ _rhs.mfAnimSpdIdle },
        mfAnimSpdRunning{ _rhs.mfAnimSpdRunning },
        mfAnimSpdJumping{ _rhs.mfAnimSpdJumping },
        mfAnimSpdFalling{_rhs.mfAnimSpdFalling },
        mnCurrentAttackChain{_rhs.mnCurrentAttackChain}, 
        mbSpammedAttack{_rhs.mbSpammedAttack},
        mbIsInForce{_rhs.mbIsInForce},
        mbSpammedCast{_rhs.mbSpammedCast},
		mfAttackPower{_rhs.mfAttackPower},
		mfAttackKnockback{_rhs.mfAttackKnockback},
		mfRollForce{_rhs.mfRollForce},
		mbAppliedAtk{_rhs.mbAppliedAtk},
		mfInternalForceMultiplier{_rhs.mfInternalForceMultiplier},
		mnStateDebug{_rhs.mnStateDebug},
		mfAttackPushfoward{_rhs.mfAttackPushfoward},
		mbChainedInto{_rhs.mbChainedInto},
		mnManualSkill{_rhs.mnManualSkill},
		mChainSkillName{"",""},
		mManualSkillName{{"",""},{"",""}},
        mfMaxHealth{_rhs.mfMaxHealth},
        mfMaxMana{_rhs.mfMaxMana},
        mfCurrHealth{_rhs.mfCurrHealth},
        mfCurrMana{_rhs.mfCurrMana},
        mfRollCost{_rhs.mfRollCost},
        mfManaDelay{_rhs.mfManaDelay},
        mfHealthDelay{_rhs.mfHealthDelay},
        mfManaRegen{_rhs.mfManaRegen},
        mfHealthRegen{_rhs.mfHealthRegen}
    {
        for (int i = FStateTypes::INVALID + 1; i < FStateTypes::OUT_OF_RANGE; i++)
        {
            //<LoadState>
            mStates[0] = new Idling{ this };
            mStates[1] = new Running{ this };
            mStates[2] = new Jumping{ this };
            mStates[3] = new Attacking{ this };
            mStates[4] = new Casting{ this };
            mStates[5] = new Rolling{ this };
            mStates[6] = new Flinching{ this };
            mStates[7] = new Falling{ this };
            mStates[8] = new Landing{ this };
            mStates[9] = new Death{ this };
            mStates[10] = new Duck{this};
            //</LoadState>
        }
    }
    CharacterController::~CharacterController()
    {
        for (int i = FStateTypes::INVALID + 1; i < FStateTypes::OUT_OF_RANGE; i ++)
            delete mStates[i]; 
    }
    void CharacterController::Awake()
    {
        mpInput = nullptr;
        mpBody = nullptr;
        mpAnim = nullptr;
		mpCollider = nullptr;
		mpMyHitBox = nullptr;
        mpCastPivot = nullptr;
		mbChainedInto = false;
		mnManualSkill = 0;
		mChainSkillName[0] = "";
		mChainSkillName[1] = "";
		mManualSkillName[0][0] = "";
		mManualSkillName[0][1] = "";
		mManualSkillName[1][0] = "";
		mManualSkillName[1][1] = "";
        orgOffset = GetOwner()->GetComponent<Collider>()->GetOffSet();
        orgScale = GetOwner()->GetComponent<Collider>()->GetScale();
        mpMyShadow = EngineCore::Get<SceneSystem>()->FindGameObject("Wilo_Shadow");
        if (mpMyShadow)
            myShadowOrg = mpMyShadow->GetComponent<Transform>()->GetGlobalScale();
        if (mpInput)
         ReMapButtons();
    }
    void CharacterController::Init()
    {
        mpInput = EngineCore::GetInstance()->GetSystem<InputManager>();
        mpBody = GetOwner()->GetComponent<RigidBody>(); 
        mpAnim = GetOwner()->GetComponent<SpriteRenderer>();
        mpCollider = GetOwner()->GetComponent<Convex>();
        mpAudioSrc = GetOwner()->GetComponent<AudioSource>();
		mpMyHitBox = EngineCore::Get<SceneSystem>()->FindGameObject("CharacterHitBox");
        mpCamShaker = EngineCore::Get<SceneSystem>()->FindGameObject("Camera Shake");
        mpHitAudioSrc = mpMyHitBox ? mpMyHitBox->GetComponent<AudioSource>() : nullptr;
        auto& allchild = GetOwner()->GetComponent<Transform>()->GetAllChild();
        for (auto& c : allchild)
        {
            if (c->GetOwner()->GetName() == HashString{"CastPivot"})
                mpCastPivot = c->GetOwner();
        } 

		mChainSkillName[0] = "Force_Flame_Relay.dobj";
		mChainSkillName[1] = "Form_Spike_Relay.dobj";
		mManualSkillName[0][0] = "Force_Blast_Relay.dobj";
		mManualSkillName[0][1] = "";
		mManualSkillName[1][0] = "Form_Slam_Relay.dobj";
		mManualSkillName[1][1] = "";
		
        if (mpInput)
            MapButtonsDefault();
 
        mbFaceRight = mbIsInForce = true;
        mnPriorityDir = mnCurrentAttackChain = mnManualSkill = 0;
        mfCurrentSpeed = mfMaxSpeed;
        mbSpammedAttack = mbSpammedCast = mbChainedInto = false;
		mfAnalogSens = Math::Clamp(mfAnalogSens, 0.2f, 1.f);
        if (mpInput)
            ReMapButtons();
    }

    void CharacterController::FixedUpdate(const float)
    {
        
    }
    
    void CharacterController::Update(const float _fDeltaTime)
    {
        float fixedDT = _fDeltaTime;
        // TODO : Use timesystem fixed delta time
        magicMult = _fDeltaTime/0.02f;

        if (mfInvulTimer > 0.0f)
        {
            mfInvulTimer -= _fDeltaTime;
            GetOwner()->GetComponent<SpriteRenderer>()->SetColor({0.5f,0.3f,0.3f});
            GetOwner()->GetComponent<Collider>()->RemoveColLayer(LAYER_3);
            GetOwner()->GetComponent<Collider>()->RemoveColLayer(LAYER_2);
        }
        else
        { 
            mfInvulTimer = 0.0f;
            mfHealthDelay -= _fDeltaTime;

            if (mCurrentState != eRolling)
            {
                GetOwner()->GetComponent<Collider>()->SetColLayer(LAYER_2);
                GetOwner()->GetComponent<Collider>()->SetColLayer(LAYER_3);
                GetOwner()->GetComponent<SpriteRenderer>()->SetColor({1.0f,1.0f,1.0f});
            }
        }

        mfManaDelay -= _fDeltaTime;

        // MANA REGEN
        if (mfCurrMana < mfMaxMana && mfManaDelay <= 0.0f)
        {
            mfCurrMana += mfManaRegen * _fDeltaTime;
        }

        // HEALTH REGEN
        if (mfCurrHealth < mfMaxHealth && mfHealthDelay <= 0.0f)
        {
            mfCurrHealth += mfHealthRegen * _fDeltaTime;
        }

		mnStateDebug = static_cast<int>(mCurrentState);
		if (!mpInput || !mpBody)
		{
			mpInput = EngineCore::GetInstance()->GetSystem<InputManager>();
			mpBody = GetOwner()->GetComponent<RigidBody>();
			return; 
		}
		
		if (mCurrentState > FStateTypes::INVALID &&  mCurrentState < FStateTypes::OUT_OF_RANGE)
		{
			if (mCurrentState != mNextState && mStatus == Updating)
				mStatus = Exiting;

			switch (mStatus)
			{
			case Updating:
				mStates[mCurrentState]->Update(fixedDT);
				break;
			case Exiting:
				if (mStates[mCurrentState]->OnExitState(fixedDT))
					mStatus = Entering;
				break;
			case Entering:
				if (mStates[mNextState]->OnEnterState(fixedDT))
				{
					mStatus = Updating;
					mCurrentState = mNextState;
				}
				break;
			}
		}
		
		//float y = mpBody->GetLinearVelocity().y;
		//if (mpBody && y < 0.f)
		//{
		//	mbLanded = false;
		//	mNextState = eFalling;
		//	DEBUG_PRINT(eLog::MESSAGE, "Player velocity y %f", y);
		//}
		CollisionEvent col;
		GameObject* ptr[1] = {GetOwner()};
		auto pos = GetOwner()->GetComponent<Transform>()->GetGlobalPosition();
		// if(EngineCore::Get<CollisionSystem>()->RaycastFirstHit(Math::Vec3D{0,-1,0}, pos,&col,ptr,1,5.f))
		// 	{ 
		// 		DEBUG_PRINT(eLog::ERROR, "CLOG %f %f %f \n", static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z));
		// 	}
		
		if (mpInput->GetButtonDown("ChangeSkill"))
        {
            if (mbIsInForce)
			    mbIsInForce = false;
            else
            {
                mbIsInForce = true;
            }
        }
    }

    void CharacterController::GameObjectDestroy(void)
    {
        Behaviour::GameObjectDestroy();  
    }
    void Dystopia::CharacterController::OnCollisionEnter(const CollisionEvent& _colEvent)
    {
        GetOwner()->GetComponent<Collider>()->RemoveColLayer(LAYER_5);

        if (_colEvent.mCollidedWith->GetComponent<Collider>()->GetColLayer() & LAYER_5 && _colEvent.mCollidedWith->GetComponent<Collider>()->GetColLayer() != LAYER_GLOBAL)
        {
            const auto platform = _colEvent.mCollidedWith;
            if (GetOwner()->GetComponent<Transform>()->GetGlobalPosition().y - platform->GetComponent<Transform>()->GetGlobalPosition().y > 0.f)
                GetOwner()->GetComponent<Collider>()->SetColLayer(LAYER_5);
        }
        
        /*
        *   Hit an object with Layer "ENEMY"
        */
        if (_colEvent.mCollidedWith->GetComponent<Collider>()->GetColLayer() & LAYER_3 && _colEvent.mCollidedWith->GetComponent<Collider>()->GetColLayer() != LAYER_GLOBAL)
        {
            Math::Vector4 reflectVec;
            const auto EnemyPos = _colEvent.mCollidedWith->GetComponent<Transform>()->GetGlobalPosition().x;
            const auto dist = EnemyPos - GetOwner()->GetComponent<Transform>()->GetGlobalPosition().x;
            const auto oScale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();

            if (dist > 1.f) // enemy is on my right
            {
                reflectVec = Math::RotateZ(315_deg) * Math::Vector4{-1,0,0};
                mbFaceRight = true;
            }
            else if (dist < -1.f) // enemy is on my left
            {
                reflectVec = Math::RotateZ(45_deg) * Math::Vector4{1,0,0};
                mbFaceRight = false;
            }
            mpBody->SetLinearVel({mpBody->GetLinearVelocity().x,0,0});
            mpBody->AddLinearImpulse(reflectVec * mfInternalForceMultiplier * mpBody->GetMass());
             
            if ((oScale.x < 0.f && mbFaceRight) || (oScale.x > 0.f && !mbFaceRight))
                GetOwner()->GetComponent<Transform>()->SetScale(oScale.x * -1.f, oScale.y, oScale.z);
            
            mbLanded = false;
        }
    }
    void Dystopia::CharacterController::OnCollisionStay(const CollisionEvent& _colEvent)
    {
        const auto colBTrigger = _colEvent.mCollidedWith->GetComponent<Collider>()->IsTrigger();
        const float dotNormal = _colEvent.mEdgeNormal.Dot({ 0.0F,-1.0F,0.0F });
        if (dotNormal > 0.45F && dotNormal < 1.1F && !colBTrigger)
            mbLanded = true;

        if (_colEvent.mCollidedWith->GetComponent<Collider>()->GetColLayer() & LAYER_5 && _colEvent.mCollidedWith->GetComponent<Collider>()->GetColLayer() != LAYER_GLOBAL)
        {
            if (mpInput->GetButton("Vertical", 1))
            {
                if (mpInput->GetButton("Jump"))
                    GetOwner()->GetComponent<Collider>()->RemoveColLayer(LAYER_5); 
            } 
        }
       
    }
    void Dystopia::CharacterController::OnCollisionExit(const CollisionEvent& _colEvent)
    {
    }
    void Dystopia::CharacterController::OnTriggerEnter(GameObject * const _obj)
    {
    }
    void Dystopia::CharacterController::OnTriggerStay(GameObject * const _obj)
    {
    }
    void Dystopia::CharacterController::OnTriggerExit(GameObject * const _obj)
    {
    }
    CharacterController * CharacterController::Duplicate() const
    {
        return new CharacterController{*this};
    }
    TypeErasure::TypeEraseMetaData CharacterController::GetMetaData()
    { 
        static MetaData<Dystopia::CharacterController> mMetaData;
        static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
        return mReturn;
    }
    TypeErasure::TypeEraseMetaData const CharacterController::GetMetaData() const
    {
        static MetaData<Dystopia::CharacterController> mMetaData;
        static auto mReturn = TypeErasure::TypeEraseMetaData{mMetaData};
        return mReturn; 
    }
    void CharacterController::MapButtonsDefault(void)
    {
        // mpInput->MapButton("Run Left", eButton::KEYBOARD_LEFT);
        // mpInput->MapButton("Run Right", eButton::KEYBOARD_RIGHT);
        // mpInput->MapButton("Jump", eButton::KEYBOARD_SPACEBAR);
        // mpInput->MapButton("Skill B", eButton::KEYBOARD_V);
        // mpInput->MapButton("Skill Y", eButton::KEYBOARD_C);
        // mpInput->MapButton("Attack", eButton::KEYBOARD_X);
        // mpInput->MapButton("SetForm", eButton::KEYBOARD_D);
        // mpInput->MapButton("SetForce", eButton::KEYBOARD_F);
        // mpInput->MapButton("Roll", eButton::KEYBOARD_LCTRL);
    }
    void CharacterController::ReMapButtons(void)
    {
        // bool prevConnected = mbIsController;
        mbIsController = mpInput->IsController();
        // if (!prevConnected && mbIsController)
        // {
        //     mpInput->MapButton("Run Left", eButton::XBUTTON_DPAD_LEFT);
        //     mpInput->MapButton("Run Right", eButton::XBUTTON_DPAD_RIGHT);
        //     mpInput->MapButton("Jump", eButton::XBUTTON_A);
        //     mpInput->MapButton("Skill B", eButton::XBUTTON_B);
        //     mpInput->MapButton("Skill Y", eButton::XBUTTON_Y);
        //     mpInput->MapButton("Attack", eButton::XBUTTON_X);
        //     mpInput->MapButton("SetForm", eButton::XBUTTON_LEFT_SHOULDER);
        //     mpInput->MapButton("SetForce", eButton::XBUTTON_RIGHT_SHOULDER);
        // }
        // else if (prevConnected && !mbIsController)
        //     MapButtonsDefault();
    }    
    void CharacterController::PlayAnimation(int _animID, float _speed)
    {
        if (!mpAnim) return;
        mpAnim->Stop();
        mpAnim->SetAnimation(_animID);
        mpAnim->SetSpeed(_speed);
        mpAnim->Play();
    }    

    void CharacterController::UseMana(float _amount, float _delay)
    {
        mfCurrMana -= _amount;
        mfManaDelay = _delay;
    }

    bool CharacterController::HandleRunning(void)
    {
        if (mpInput->GetAxis("Horizontal") < -0.001f)
        {
            // priority to change dir
            mbFaceRight = false; 
            mnPriorityDir = -1; 
        }
        else if (mpInput->GetAxis("Horizontal") > 0.001f)
        {
            // priority to change dir
            mbFaceRight = true; 
            mnPriorityDir = 1;
        }
        else if (mbIsController && (mpInput->GetAxis("L Stick Horizontal") > mfAnalogSens || mpInput->GetAxis("L Stick Horizontal") < - 1.f * mfAnalogSens))
        {
            mbFaceRight = mpInput->GetAxis("L Stick Horizontal") > mfAnalogSens; 
            mnPriorityDir = mbFaceRight ? 1 : -1;
        }
        else if (mnPriorityDir == 1)
        {
            if (mpInput->GetAxis("Horizontal") > 0.001f)
                mbFaceRight = true;
            else if (mpInput->GetAxis("Horizontal") < -0.001f)
                mbFaceRight = false;
            else
                return false;
        }
        else if (mnPriorityDir == -1)
        {
            if (mpInput->GetAxis("Horizontal") < -0.001f)
                mbFaceRight = false; 
            else if (mpInput->GetAxis("Horizontal") > 0.001f)
                mbFaceRight = true;
            else
                return false;
        }
        else
            return false;
        return true;
    }
    void CharacterController::HandleTranslate(bool _inair, float _dt)
    {
		//prevFace = mbFaceRight;
        HandleRunning();
        float thumbstick = mpInput->GetAxis("L Stick Horizontal");
        bool rightPressed = mpInput->GetAxis("Horizontal") > 0.001f;
        bool leftPressed  = mpInput->GetAxis("Horizontal") < -0.001f;
        float controllerVal = mbIsController ? thumbstick : 0.0f;
        auto oScale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();

        if ((controllerVal && ((controllerVal < -1.f * mfAnalogSens) || (controllerVal > mfAnalogSens))) 
			|| rightPressed || leftPressed )
        {
			if (mnPriorityDir == 1) 
			{
				if (controllerVal > mfAnalogSens)
					mbFaceRight = true;
				else if (controllerVal < -1.f * mfAnalogSens)
					mbFaceRight = false;
				else
				{
					if (rightPressed)
						mbFaceRight = true;
					else if (leftPressed)
						mbFaceRight = false;
					else 
						mbFaceRight = controllerVal < 0.f;
				}
			}
			else if (mnPriorityDir == -1) 
			{
				if (controllerVal < -1.f * mfAnalogSens)
					mbFaceRight = false;
				else if (controllerVal > mfAnalogSens)
					mbFaceRight = true;
				else
				{
					if (leftPressed)
						mbFaceRight = false;
					else if (rightPressed)
						mbFaceRight = true;
					else 
						mbFaceRight = controllerVal >= 0.f;
				} 
			}
            // (mbIsController ? thumbstick : 1.f) *
            auto val = 2 * mfCurrentSpeed * mpBody->GetMass();
            if ((mbFaceRight && val < 0.f) || (!mbFaceRight && val > 0.f))
                val *= -1.f;
			
            if ((mbFaceRight && oScale.x < 0.f) || (!mbFaceRight && oScale.x > 0.f))
                oScale.x = -1.f * oScale.x;
			
            float dirScale = _inair ? mfAirMovementScalerForward : 1.f;
            GetOwner()->GetComponent<Transform>()->SetScale(oScale.x, oScale.y, oScale.z);
            mpBody->AddLinearImpulse({ val * dirScale * magicMult, 0, 0 });
			
            //if (!_inair)
            //{
            //    if ((mbFaceRight && oScale.x < 0.f) || (!mbFaceRight && oScale.x > 0.f))
            //        oScale.x = -1.f * oScale.x;
            //    GetOwner()->GetComponent<Transform>()->SetScale(oScale.x, oScale.y, oScale.z);
            //    mpBody->AddLinearImpulse({ val * 1.f , 0, 0 });
            //}
            //else
            //{ 
            //    float dirScale = mfAirMovementScalerForward;
            //    mbFaceRight = prevFace;
            //    if ((mbFaceRight && val < 0.f) || (!mbFaceRight && val > 0.f))
            //        dirScale = mfAirMovementScalerBackward;
            //
            //    mpBody->AddLinearImpulse({ val * dirScale, 0, 0 });
            //}
        }
		else if (!_inair && ((oScale.x < 0.f && mbFaceRight) || (oScale.x > 0.f && !mbFaceRight)))
            GetOwner()->GetComponent<Transform>()->SetScale(oScale.x * -1.f, oScale.y, oScale.z);
	}
    
	void CharacterController::TakeForce(float _force, Math::Vec2 _dir)
	{	
		if (mpBody)
		{
			float val = _force * mfInternalForceMultiplier;
			mpBody->AddLinearImpulse({ _dir.x * val , _dir.y * val, 0 });
            mpInput->InvokeVibration(_force/90.f, 40.f, 10.f, 0.1f);
            if (mpCamShaker)
                CharacterController_MSG::SendExternalMessage(mpCamShaker, "InvokeShake", _force/100.f, 40.0f, 12.0f, 4.0f);
		}
	}

    void CharacterController::TakeDamage(float _dmg, float vibrateStrength)
    {
        if (mpBody)
        {
            mfInvulTimer = 1.3f;
            mNextState = eFlinching;
            mpInput->InvokeVibration(vibrateStrength, 30.f, 9.f, 0.8f);
            if (mpCamShaker)
                CharacterController_MSG::SendExternalMessage(mpCamShaker, "InvokeShake", vibrateStrength, 30.0f, 5.0f, 3.0f);
            
        }
        if (!mbGotHit)
        {
            mbGotHit = true;
            mfCurrHealth -= _dmg;
        }
    }

    bool CharacterController::CheckLanded()
    {
        return mbLanded;
    }
		
    //<AllStates>
    //Idling
    CharacterController::Idling::Idling(CharacterController* _o) : IFiniteState{_o}{}
    CharacterController::Idling::~Idling(){}
    void CharacterController::Idling::Update(float) 
    { 
        if (owner->mpBody->GetLinearVelocity().y < -100.f) 
        {
            owner->mNextState = eFalling;
            owner->mbLanded = false;
        }

		if (!owner->mbDisableControls)
		{
			//<FS_CONDITION>
			// Condition for eRunning
			if (owner->HandleRunning())
				owner->mNextState = eRunning;
			// Condition for eJumping
			if (owner->mpInput->GetButton("Jump") || owner->mpInput->GetButtonDown("Jump"))
            {
                if (!owner->mpInput->GetButton("Vertical",1) && owner->mpInput->GetAxis("L Stick Vertical") > -0.2f)
				    owner->mNextState = eJumping;

                if (owner->mpInput->GetButton("Vertical",1) || owner->mpInput->GetAxis("L Stick Vertical") < -0.90f)
                { 
                   owner->GetOwner()->GetComponent<Collider>()->RemoveColLayer(LAYER_5);
                }
            }
			// Condition for eAttacking
			if (owner->mpInput->GetButtonDown("Attack"))
				owner->mNextState = eAttacking; 
			// Condition for eCasting
			if (owner->mpInput->GetButtonDown("Skill Y") || owner->mpInput->GetButtonDown("Skill B"))
			{
				owner->mNextState = eCasting;
				owner->mnManualSkill = owner->mpInput->GetButton("Skill Y") ? 1 : 2;
			}
			// Condition for eRolling
			if (owner->mpInput->GetButtonDown("Roll") && owner->mfCurrMana >= owner->mfRollCost)
				owner->mNextState = eRolling;

            if (owner->mpInput->GetButton("Vertical",1) || owner->mpInput->GetAxis("L Stick Vertical") < -0.20f)
            {
                owner->mNextState = eDuck;
            }
		}
        // Condition for eFlinching
        if (owner->mbGotHit)
            owner->mNextState = eFlinching;
        //</FS_CONDITION>

        
    }
    bool CharacterController::Idling::OnEnterState(float)
    {
        owner->PlayAnimation(0, owner->mfAnimSpdIdle);
        return true;
    }
    bool CharacterController::Idling::OnExitState(float)
    { 

        return true;
    }
    //Running
    CharacterController::Running::Running(CharacterController* _o) : IFiniteState{_o}{}
    CharacterController::Running::~Running(){}
    void CharacterController::Running::Update(float _dt)
    {
        owner->mfTimer -= _dt;

        if (owner->mpBody->GetLinearVelocity().y < -100.f)
        {
            owner->mNextState = eFalling;
            owner->mbLanded = false;
        }

        if (owner->mfTimer <= 0.0f)
        {
            owner->mnRunningCheck = owner->mnRunningCheck == 0? 1 : 0; 
            owner->mfTimer = 0.45f;

            if (owner->mnRunningCheck == 0)
            {
                if (owner->mpHitAudioSrc && owner->mpAudioSrc)
                {
                    owner->mpAudioSrc->Stop();

                    owner->mpAudioSrc->SetVolume(1.0f);
                    owner->mpAudioSrc->ChangeAudio(A_WILORUN_3);

                    owner->mpAudioSrc->Play();
                }
            }
            else
            {
                if (owner->mpHitAudioSrc && owner->mpAudioSrc)
                {
                    owner->mpAudioSrc->Stop();

                    owner->mpAudioSrc->SetVolume(1.0f);
                    owner->mpAudioSrc->ChangeAudio(A_WILORUN_5);

                    owner->mpAudioSrc->Play();
                }
            }
            
        }

        //<FS_CONDITION>
        // Condition for eIdling
         if ((owner->mbIsController && (owner->mpInput->GetAxis("L Stick Horizontal") < 0.2f && owner->mpInput->GetAxis("L Stick Horizontal") > -0.2f) || !owner->mbIsController) &&
            owner->mpInput->GetAxis("Horizontal") < 0.2f && owner->mpInput->GetAxis("Horizontal") > -0.2f)
            owner->mNextState = eIdling;

		if (!owner->mbDisableControls)
		{
			// Condition for eJumping
			if (owner->mpInput->GetButton("Jump") || owner->mpInput->GetButtonDown("Jump")) 
				owner->mNextState = eJumping;
			// Condition for eAttacking
			if (owner->mpInput->GetButtonDown("Attack"))
				owner->mNextState = eAttacking;
			// Condition for eCasting
			if (owner->mpInput->GetButtonDown("Skill Y") ||  owner->mpInput->GetButtonDown("Skill B"))
			{
				owner->mNextState = eCasting;
				owner->mnManualSkill = owner->mpInput->GetButton("Skill Y") ? 1 : 2;
			}
			// Condition for eRolling
			if (owner->mpInput->GetButtonDown("Roll") && owner->mfCurrMana >= owner->mfRollCost)
				owner->mNextState = eRolling;
			
			owner->HandleTranslate(false, _dt);
		}
        // Condition for eFlinching 
        if (owner->mbGotHit) 
            owner->mNextState = eFlinching;
        //</FS_CONDITION>

       
        
        
    }
    bool CharacterController::Running::OnEnterState(float)
    {
        owner->mfTimer = 0.45f;
        owner->mnRunningCheck = 0;
        if (owner->mpHitAudioSrc && owner->mpAudioSrc)
        {
            owner->mpAudioSrc->Stop();

            owner->mpAudioSrc->SetVolume(1.0f);
            owner->mpAudioSrc->ChangeAudio(A_WILORUN_3);

            owner->mpAudioSrc->Play();
        }
        owner->PlayAnimation(1, owner->mfAnimSpdRunning);
       // owner->mpMyShadow->GetComponent<Transform>()->SetGlobalScale(owner->myShadowOrg.x*2,owner->myShadowOrg.y,owner->myShadowOrg.z);
        return true;
    }
    bool CharacterController::Running::OnExitState(float)
    { 
        owner->mfTimer = 0;
       // owner->mpMyShadow->GetComponent<Transform>()->SetGlobalScale(owner->myShadowOrg.x,owner->myShadowOrg.y,owner->myShadowOrg.z);
        return true;
    }
    //Jumping
    CharacterController::Jumping::Jumping(CharacterController* _o) : IFiniteState{_o}{}
    CharacterController::Jumping::~Jumping(){}
    void CharacterController::Jumping::Update(float _dt)
    {
        //<FS_CONDITION>
        // Condition for eFalling
		if (owner->mpBody)
			if (owner->mpBody->GetLinearVelocity().y < 0.f)
				owner->mNextState = eFalling;

        // Condition for eLanding
        if (owner->mbLanded)
            owner->mNextState = eLanding;
        // Condition for eFlinching
        if (owner->mbGotHit)
            owner->mNextState = eFlinching;
        //</FS_CONDITION> 
		
		if (!owner->mbDisableControls)
		{
			owner->HandleTranslate(true, _dt);
		}
    }
    bool CharacterController::Jumping::OnEnterState(float)
    {
        auto jump = owner->mfJumpStrength * owner->mpBody->GetMass() + owner->mpBody->GetLinearVelocity().y;
        auto offset = owner->GetOwner()->GetComponent<Collider>()->GetOffSet();
        owner->mpBody->AddLinearImpulse(Math::Vector3D{ 0, jump, 0});
        owner->mbIsJumping = true;
        owner->mbLanded = false;
        owner->PlayAnimation(2, owner->mfAnimSpdJumping);
        owner->GetOwner()->GetComponent<Collider>()->SetOffSet(Math::Vec3D(offset.x, 0.060f, offset.z, offset.w));
        owner->GetOwner()->GetComponent<Collider>()->SetScale(Math::Vec3D(owner->orgScale.x, 0.4f, owner->orgScale.z));
        owner->GetOwner()->GetComponent<Collider>()->RemoveColLayer(LAYER_5);

        if (owner->mpHitAudioSrc && owner->mpAudioSrc)
        {
            owner->mpHitAudioSrc->Stop();
            owner->mpAudioSrc->Stop();

            owner->mpHitAudioSrc->SetVolume(3.0f);
            owner->mpHitAudioSrc->SetFrequency(0.3f);
            owner->mpHitAudioSrc->SetPitch(2.2f);

            owner->mpAudioSrc->SetVolume(0.5f);
            owner->mpHitAudioSrc->ChangeAudio(A_WILOJUMP);
            owner->mpAudioSrc->ChangeAudio(A_WILOSTRIKE_1);

            owner->mpHitAudioSrc->Play();
            owner->mpAudioSrc->Play();
        }
        
        return true;
    }
    bool CharacterController::Jumping::OnExitState(float)
    { 
        owner->mbIsJumping = false;
        owner->GetOwner()->GetComponent<Collider>()->SetOffSet(owner->orgOffset);
        owner->GetOwner()->GetComponent<Collider>()->SetScale(Math::Vec3D(owner->orgScale.x, owner->orgScale.y, owner->orgScale.z));
        return true;
    }  
    //Attacking
    CharacterController::Attacking::Attacking(CharacterController* _o) : IFiniteState{_o}{}
    CharacterController::Attacking::~Attacking(){}
    void CharacterController::Attacking::Update(float _dt)
    {
        float spamWindow = owner->mfAttackDelay * 0.7f;
		bool oldFacing = owner->mbFaceRight;
        owner->mfTimer -= _dt;
        //<FS_CONDITION>
        // Condition for eIdling
        if (owner->mfTimer < 0.f)
            owner->mNextState = eIdling;
		
		if (!owner->mbDisableControls)
		{
			// Condition for eRunning
			if (owner->mfTimer < (owner->mfAttackDelay * 0.15f) && owner->HandleRunning())
				owner->mNextState = eRunning;
			// Condition for eCasting
			if (owner->mfTimer < spamWindow && 
			   (owner->mpInput->GetButton("Skill Y") || 
				owner->mpInput->GetButton("Skill B")))
			{
				owner->mbSpammedCast = true;
				owner->mnManualSkill = owner->mpInput->GetButton("Skill Y") ? 1 : 2;
			}
			if (owner->mpInput->GetButton("Attack"))
			{
				if (!owner->mbSpammedAttack && owner->mfTimer < spamWindow)
					owner->mbSpammedAttack = true;
				if (owner->mnCurrentAttackChain == 3)
				{
					owner->mbSpammedAttack = false;
					owner->mbSpammedCast = false;
					owner->mbChainedInto = true;
				}
			}
		}
		
        // Condition for eFlinching
        if (owner->mbGotHit)
            owner->mNextState = eFlinching;
        //</FS_CONDITION>
        
        float window = (owner->mfAttackDelay * 0.15f);
        if (owner->mbSpammedAttack && owner->mfTimer < window)
        {
			owner->mbFaceRight = oldFacing;
            owner->mNextState = eAttacking; 
            OnEnterState(0.016f);
        }
        else if (owner->mbSpammedCast && owner->mfTimer < window)
		{
			owner->mbFaceRight = oldFacing;
            owner->mNextState = eCasting;
		}
		else if (owner->mbChainedInto && owner->mfTimer < window)
		{
			owner->mbFaceRight = oldFacing;
			owner->mNextState = eCasting;
		}
		
		// time the impact
		if (!owner->mbAppliedAtk && owner->mfTimer < (owner->mfAttackDelay * 0.75f))
		{
			if (owner->mpMyHitBox)
			{
				CharacterController_MSG::SendExternalMessage(owner->mpMyHitBox, "SendDamage", owner->mfAttackPower);
				
				CharacterController_MSG::SendExternalMessage(
				owner->mpMyHitBox, "SendForce", 
				owner->mfAttackKnockback, 
				Math::Vec2{owner->mbFaceRight ? 1.f : -1.f, 0});
			}
			owner->mbAppliedAtk = true;
		}
    }
    bool CharacterController::Attacking::OnEnterState(float)
    {
        owner->mfTimer = owner->mfAttackDelay;
        
        owner->mnCurrentAttackChain = !owner->mnCurrentAttackChain ? 1 : 
                                       owner->mnCurrentAttackChain == 3 ? 1 : 
                                       owner->mnCurrentAttackChain + 1;
                             
        unsigned id = 2 + owner->mnCurrentAttackChain;                             
        owner->PlayAnimation(id, owner->mpAnim ?     
                                 owner->mfTimer / owner->mpAnim->GetFrameSize(id) : 0.016f);      
                                 
        if (owner->mpHitAudioSrc && owner->mpAudioSrc)
        {
            //owner->mpHitAudioSrc->Stop();
            owner->mpAudioSrc->Stop();                     

            owner->mpHitAudioSrc->SetVolume(0.6f);
            owner->mpHitAudioSrc->SetFrequency(1.1f);
            owner->mpHitAudioSrc->SetPitch(1.6f);

            owner->mpAudioSrc->SetVolume(0.6f);
            owner->mpAudioSrc->SetFrequency(1.0f);
            owner->mpAudioSrc->SetPitch(1.0f);

            switch(owner->mnCurrentAttackChain)
            {
                case 1:
                owner->mpHitAudioSrc->ChangeAudio(A_WILOATTACK_1);
                owner->mpAudioSrc->ChangeAudio(A_WILOSTRIKE_1);
                break;
                case 2: 
                owner->mpHitAudioSrc->ChangeAudio(A_WILOATTACK_2);
                owner->mpAudioSrc->ChangeAudio(A_WILOSTRIKE_2);
                break;
                case 3: 
                owner->mpHitAudioSrc->ChangeAudio(A_WILOATTACK_3);
                owner->mpAudioSrc->ChangeAudio(A_WILOSTRIKE_3);
                break;
            }

            owner->mpHitAudioSrc->Play();
            owner->mpAudioSrc->Play();
        }
								 
        owner->mbSpammedAttack = false;
        owner->mbSpammedCast = false;
		owner->mbAppliedAtk = false;
		
		if (owner->mpBody)
		{
			float force = owner->mfAttackPushfoward * owner->mfInternalForceMultiplier;
			owner->mpBody->AddLinearImpulse({ owner->mbFaceRight ? force: -force, 0, 0 });
		}
		
        return true;
    }
    bool CharacterController::Attacking::OnExitState(float)
    { 
        owner->mfTimer = 0;
        owner->mnCurrentAttackChain = 0;
        owner->mbSpammedAttack = false;
        owner->mbSpammedCast = false;
		owner->mbAppliedAtk = false;
        owner->mpAudioSrc->SetPitch(1.0f);
        return true;
    }
    //Casting
    CharacterController::Casting::Casting(CharacterController* _o) : IFiniteState{_o}{}
    CharacterController::Casting::~Casting(){}
    void CharacterController::Casting::Update(float _dt)
    { 
        float spamWindow = owner->mfCastingDelay * 0.7f;
        float window = owner->mfCastingDelay * 0.1f;
		bool oldFacing = owner->mbFaceRight;
        
        owner->mfTimer -= _dt;
        //<FS_CONDITION>
        // Condition for eIdling
        if (owner->mfTimer < 0.f)
            owner->mNextState = eIdling;

        if (owner->mpBody->GetLinearVelocity().y < -50.f)
        {   
            owner->mbLanded = false;
            owner->mNextState = eFalling;
        }

		if (!owner->mbDisableControls)
		{
			// Condition for eRunning
			if (owner->mfTimer < (owner->mfCastingDelay * 0.1f) && owner->HandleRunning())
				owner->mNextState = eRunning;
			// Condition for eAttacking
			if (owner->mfTimer < spamWindow && owner->mpInput->GetButton("Attack"))
				owner->mbSpammedAttack = true;
			if (owner->mfTimer < spamWindow && (owner->mpInput->GetButton("Skill Y") || 
											owner->mpInput->GetButton("Skill B")))
			{
				owner->mbSpammedCast = true;
				owner->mnManualSkill = owner->mpInput->GetButton("Skill Y") ? 1 : 2;
				owner->mbChainedInto = false;
			}
		}
			
        // Condition for eFlinching
        if (owner->mbGotHit)
            owner->mNextState = eFlinching;  
        //</FS_CONDITION>
        
        if (owner->mbSpammedAttack && owner->mfTimer < window)
		{
			owner->mbFaceRight = oldFacing;
            owner->mNextState = eAttacking; 
		}
        else if (owner->mbSpammedCast && owner->mfTimer < window)
        {
			owner->mbFaceRight = oldFacing;
            owner->mNextState = eCasting;
            OnEnterState(0.016f);
        }
    }
    bool CharacterController::Casting::OnEnterState(float)
    {
        owner->mfTimer = owner->mfCastingDelay;
        owner->mbSpammedAttack = false;
        owner->mbSpammedCast = false;
		
		HashString skill{};
		Math::Vec2 knockDir{0.f, 0.f};
		if (owner->mbChainedInto)
		{
			skill = owner->mChainSkillName[owner->mbIsInForce ? 0 : 1].c_str();
			knockDir.y = owner->mbIsInForce ? 0.f : 1.f;
			knockDir.x = owner->mbIsInForce ? (owner->mbFaceRight ? 1.f : -1.f) : 
											  (owner->mbFaceRight ? 0.35f : -0.35f);
		}
		else if (owner->mnManualSkill)
		{
			skill = owner->mManualSkillName[owner->mbIsInForce ? 0 : 1][owner->mnManualSkill - 1].c_str();
			knockDir.x = owner->mbIsInForce ? (owner->mbFaceRight ? 1.f : -1.f) : 0;
		}
		
		if (skill.length())
		{	
			auto spawnPt = owner->mpCastPivot ? owner->mpCastPivot->GetComponent<Transform>()->GetGlobalPosition() 
											  : owner->GetOwner()->GetComponent<Transform>()->GetGlobalPosition(); 
 
			auto inst = EngineCore::Get<SceneSystem>()->Instantiate(skill, spawnPt);
			if (inst)
			{
				CharacterController_MSG::SendExternalMessage(inst, "SetKnockDirection", knockDir);
				CharacterController_MSG::SendExternalMessage(inst, "SetFacing", owner->mbFaceRight);
				CharacterController_MSG::SendExternalMessage(inst, "StartSkill");
				
				unsigned id = owner->mbIsInForce ? 6 : 7;
				owner->PlayAnimation(id, owner->mpAnim ? owner->mfTimer / owner->mpAnim->GetFrameSize(id) : 0.016f);
                
                if (owner->mpHitAudioSrc && owner->mpAudioSrc)
                {
                    if (skill == "Force_Flame_Relay.dobj")
                    {
                        owner->mpHitAudioSrc->Stop();

                        owner->mpHitAudioSrc->SetVolume(1.0f);
                        owner->mpHitAudioSrc->SetFrequency(2.3f);
                        owner->mpHitAudioSrc->SetPitch(0.9f);

                        owner->mpHitAudioSrc->ChangeAudio(A_WILLO_FLAME);

                        owner->mpHitAudioSrc->Play();
                    }

                    if (skill == "Force_Blast_Relay.dobj")
                    { 
                        owner->mpHitAudioSrc->Stop();

                        owner->mpHitAudioSrc->SetVolume(1.0f);
                        owner->mpHitAudioSrc->SetFrequency(0.8f);
                        owner->mpHitAudioSrc->SetPitch(1.0f);

                        owner->mpHitAudioSrc->ChangeAudio(A_WILO_BLAST);

                        owner->mpHitAudioSrc->Play();
                    }

                    if (skill == "Form_Spike_Relay.dobj")
                    {
                        owner->mpHitAudioSrc->Stop();

                        owner->mpHitAudioSrc->SetVolume(1.0f);
                        owner->mpHitAudioSrc->SetFrequency(1.0f);
                        owner->mpHitAudioSrc->SetPitch(1.0f);

                        owner->mpHitAudioSrc->ChangeAudio(A_WILO_SPIKES);

                        owner->mpHitAudioSrc->Play();
                    }
                    
                }
			}
		}
        return true;
    }
    bool CharacterController::Casting::OnExitState(float)
    { 
        owner->mfTimer = 0; 
		owner->mnManualSkill = 0;
        owner->mbSpammedAttack = false;
        owner->mbSpammedCast = false;
		owner->mbChainedInto = false;
        return true;
    }
    //Rolling
    CharacterController::Rolling::Rolling(CharacterController* _o) : IFiniteState{_o}{}
    CharacterController::Rolling::~Rolling(){}
    void CharacterController::Rolling::Update(float _dt)
    {
        owner->mfTimer -= _dt;
        //<FS_CONDITION>
        // Condition for eIdling
        if (owner->mfTimer < 0.f)
            owner->mNextState = eIdling;
		
		if (!owner->mbDisableControls)
		{
			// Condition for eRunning
			if (owner->mfTimer < (owner->mfRollDelay * 0.1f) && owner->HandleRunning())
				owner->mNextState = eRunning;
			// Condition for eJumping
			if (owner->mfTimer < (owner->mfRollDelay * 0.1) && 
			   (owner->mpInput->GetButton("Jump") || 
				owner->mpInput->GetButtonDown("Jump")))
				owner->mNextState = eJumping;
			// Condition for eAttacking
			if (owner->mfTimer < (owner->mfRollDelay * 0.7f) && owner->mpInput->GetButton("Attack"))
				owner->mbSpammedAttack = true;
			// Condition for eCasting
			else if (owner->mfTimer < (owner->mfRollDelay * 0.7f) && 
			   (owner->mpInput->GetButton("Skill Y") || owner->mpInput->GetButton("Skill B")))
			{
				owner->mbSpammedCast = true;
				owner->mnManualSkill = owner->mpInput->GetButton("Skill Y") ? 1 : 2;
		    }
		}
		
        //</FS_CONDITION>
        
        if (owner->mbSpammedAttack && owner->mfTimer < (owner->mfRollDelay * 0.15f))
            owner->mNextState = eAttacking;
        else if (owner->mbSpammedCast && owner->mfTimer < (owner->mfRollDelay * 0.15f))
            owner->mNextState = eCasting;
        
		if (owner->mpBody)
		{
			float force = owner->mbFaceRight ? 1.f : -1.f;
			force *= (owner->mfTimer / owner->mfRollDelay) * owner->mfRollForce * owner->mfInternalForceMultiplier ;
			owner->mpBody->AddLinearImpulse(Math::Vector3D{force* owner->magicMult,0.f,0.f});
		}
    }
    bool CharacterController::Rolling::OnEnterState(float)
    {
        owner->UseMana(owner->mfRollCost, 1.0f);
        owner->mfTimer = owner->mfRollDelay;      
        owner->PlayAnimation(8, owner->mpAnim ? 
                                 owner->mfTimer / owner->mpAnim->GetFrameSize(8) : 0.016f);
        owner->mbSpammedAttack = false;
        owner->mbSpammedCast = false;
		if (owner->mpBody && owner->mpCollider)
		{
			 owner->GetOwner()->GetComponent<Collider>()->SetOffSet(Math::Vec3D(owner->orgOffset.x, -0.158f, owner->orgOffset.z));
            owner->GetOwner()->GetComponent<Collider>()->SetScale(Math::Vec3D(owner->orgScale.x, 0.287f, owner->orgScale.z));
            owner->GetOwner()->GetComponent<Collider>()->RemoveColLayer(LAYER_3);
            owner->GetOwner()->GetComponent<Collider>()->RemoveColLayer(LAYER_2);
		}
        
        owner->GetOwner()->GetComponent<SpriteRenderer>()->SetColor({0.3f,0.3f,0.3f});

       // owner->mpMyShadow->GetComponent<Transform>()->SetScale(2,owner->myShadowOrg.y,owner->myShadowOrg.z);
		owner->HandleRunning();
		auto scale = owner->GetOwner()->GetComponent<Transform>()->GetGlobalScale();
		if (owner->mbFaceRight && scale.x < 0.f || !owner->mbFaceRight && scale.x > 0.f)
		{
			scale.x = scale.x * -1.f;
			owner->GetOwner()->GetComponent<Transform>()->SetGlobalScale(scale);
		}

        if (owner->mpHitAudioSrc && owner->mpAudioSrc)
        {
            owner->mpHitAudioSrc->Stop();
            owner->mpAudioSrc->Stop();

            owner->mpHitAudioSrc->SetVolume(1.0f);
            owner->mpHitAudioSrc->SetFrequency(1.0f);
            owner->mpHitAudioSrc->SetPitch(1.0f);

            owner->mpAudioSrc->SetVolume(1.0f);
            owner->mpAudioSrc->ChangeAudio(A_WILOROLL);

            owner->mpAudioSrc->Play();
        }

        owner->mpInput->InvokeVibration(0.4f, 30.f, 5.f, 0.9f);

        return true;
    }
    bool CharacterController::Rolling::OnExitState(float)
    { 
        owner->mfTimer = 0;
        owner->mbSpammedAttack = false;
        owner->mbSpammedCast = false;

        
        owner->GetOwner()->GetComponent<SpriteRenderer>()->SetColor({1.0f,1.0f,1.0f});


		if (owner->mpBody && owner->mpCollider)
		{
			owner->GetOwner()->GetComponent<Collider>()->SetOffSet(Math::Vec3D(owner->orgOffset.x, owner->orgOffset.y, owner->orgOffset.z));
            owner->GetOwner()->GetComponent<Collider>()->SetScale(Math::Vec3D(owner->orgScale.x, owner->orgScale.y, owner->orgScale.z));
            //owner->mpMyShadow->GetComponent<Transform>()->SetScale(owner->myShadowOrg.x,owner->myShadowOrg.y,owner->myShadowOrg.z);
            owner->GetOwner()->GetComponent<Collider>()->SetColLayer(LAYER_3);
            owner->GetOwner()->GetComponent<Collider>()->SetColLayer(LAYER_2);
		}
        return true; 
    }
    //Flinching
    CharacterController::Flinching::Flinching(CharacterController* _o) : IFiniteState{_o}{}
    CharacterController::Flinching::~Flinching(){}
    void CharacterController::Flinching::Update(float _dt)
    {
        owner->mfTimer -= _dt; 
		
        //<FS_CONDITION>
        // Condition for eIdling
        //if (owner->mfTimer < 0.f)
          //  owner->mNextState = eIdling;
		
        // Condition for eFalling
		if (owner->mpBody && owner->mfTimer < owner->mfFlinchDelay * 0.5f)
				owner->mNextState = eFalling;
			
        // Condition for eDeath
        if (owner->mbDying)
            owner->mNextState = eDeath;
        //</FS_CONDITION>
    }
    bool CharacterController::Flinching::OnEnterState(float)
    {
        owner->mfTimer = owner->mfFlinchDelay;
        owner->PlayAnimation(9, owner->mpAnim ? 
                                 owner->mfLandDelay / owner->mpAnim->GetFrameSize(9): 0.016f);
                                 
        if (owner->mpAudioSrc)
        {
            owner->mpAudioSrc->Stop();

            owner->mpAudioSrc->SetVolume(5.0f);
            owner->mpAudioSrc->SetFrequency(1.5f);
            owner->mpAudioSrc->ChangeAudio(A_WILOHIT);

            owner->mpAudioSrc->Play();
        }
        return true;
    }
    bool CharacterController::Flinching::OnExitState(float)
    { 
        owner->mbGotHit = false;
        owner->mfTimer = 0;
        owner->mpAudioSrc->SetFrequency(1.0f);
        return true;
    }
    //Falling
    CharacterController::Falling::Falling(CharacterController* _o) : IFiniteState{_o}{}
    CharacterController::Falling::~Falling(){}
    void CharacterController::Falling::Update(float _dt)
    {
        if (!owner->mpInput->GetButton("Vertical",1) && owner->mpInput->GetAxis("L Stick Vertical") > -0.5f)
            owner->GetOwner()->GetComponent<Collider>()->SetColLayer(LAYER_5);
        //<FS_CONDITION>
        // Condition for eLanding
        if (!owner->mbIsJumping && owner->mbLanded)
        { 
			// if (!owner->mbDisableControls && owner->HandleRunning() )
			// 	owner->mNextState = eRunning;
			// else
				owner->mNextState = eLanding;
        }
        // Condition for eFlinching
        if (owner->mbGotHit)
            owner->mNextState = eFlinching;
        //</FS_CONDITION> 
        
		if (!owner->mbDisableControls)
			owner->HandleTranslate(true, _dt);
    }
    bool CharacterController::Falling::OnEnterState(float)
    {
        owner->mbIsJumping = false;
        owner->PlayAnimation(10, owner->mfAnimSpdFalling);

        if (!owner->mpInput->GetButton("Vertical",1) && owner->mpInput->GetAxis("L Stick Vertical") > -0.5f)
            owner->GetOwner()->GetComponent<Collider>()->SetColLayer(LAYER_5);

        return true;
    }
    bool CharacterController::Falling::OnExitState(float)
    { 
        return true;
    }
    //Landing
    CharacterController::Landing::Landing(CharacterController* _o) : IFiniteState{_o}{}
    CharacterController::Landing::~Landing(){}
    void CharacterController::Landing::Update(float _dt)
    {
        owner->mfTimer -= _dt;
        //<FS_CONDITION>
        // Condition for eIdling
        if (owner->mfTimer < 0.f)
            owner->mNextState = eIdling;
		
		if (!owner->mbDisableControls)
		{
			// Condition for eRunning
			if (owner->mfTimer < (owner->mfLandDelay * 0.25f) && owner->HandleRunning())
				owner->mNextState = eRunning;
			// Condition for eJumping
			if (owner->mfTimer < (owner->mfLandDelay * 0.25f) && (owner->mpInput->GetButton("Jump") || owner->mpInput->GetButtonDown("Jump")))
            {
                if (!owner->mpInput->GetButton("Vertical",1) && owner->mpInput->GetAxis("L Stick Vertical") > -0.20f)
				    owner->mNextState = eJumping;

                if (owner->mpInput->GetButton("Vertical",1) || owner->mpInput->GetAxis("L Stick Vertical") < -0.9f)
                {
                   owner->GetOwner()->GetComponent<Collider>()->RemoveColLayer(LAYER_5);
                }
            }
		}
		
        // Condition for eFlinching
        if (owner->mbGotHit)
            owner->mNextState = eFlinching;
        //</FS_CONDITION>
    }
    bool CharacterController::Landing::OnEnterState(float)
    {
        owner->mfTimer = owner->mfLandDelay;
        float spd = owner->mfLandDelay;
        if (owner->mpAnim)
            spd /= owner->mpAnim->GetFrameSize(11);
        owner->PlayAnimation(11, spd);

        if (owner->mpHitAudioSrc && owner->mpAudioSrc)
        {
            owner->mpHitAudioSrc->Stop();
            //owner->mpAudioSrc->Stop();

            owner->mpHitAudioSrc->SetVolume(1.5f);
            owner->mpHitAudioSrc->SetFrequency(1.0f);
            owner->mpHitAudioSrc->SetPitch(1.0f);

            owner->mpAudioSrc->SetVolume(0.4f);
            owner->mpAudioSrc->ChangeAudio(A_WILOLANDING);
            owner->mpHitAudioSrc->ChangeAudio(A_WILOLANDING_2);

            owner->mpAudioSrc->Play();
            owner->mpHitAudioSrc->Play();
        }

        return true;
    }
    bool CharacterController::Landing::OnExitState(float)
    { 
        owner->mfTimer = 0;
        return true;
    }
    //Death
    CharacterController::Death::Death(CharacterController* _o) : IFiniteState{_o}{}
    CharacterController::Death::~Death(){}
    void CharacterController::Death::Update(float)
    {
        //<FS_CONDITION>
        //</FS_CONDITION>
    }
    bool CharacterController::Death::OnEnterState(float)
    {
        return true;
    }
    bool CharacterController::Death::OnExitState(float)
    { 
        return true;
    }
     //Duck
    CharacterController::Duck::Duck(CharacterController* _o) : IFiniteState{_o}{}
    CharacterController::Duck::~Duck(){}
    void CharacterController::Duck::Update(float _dt)
    {
        //<FS_CONDITION>
        owner->mfTimer -= _dt;

        if (!owner->mbDisableControls)
		{
            if (owner->mpInput->GetButton("Jump"))
            {
                owner->GetOwner()->GetComponent<Collider>()->RemoveColLayer(LAYER_5); 
            }
                    
            if (owner->mpInput->GetButtonUp("Vertical",1))
            {
                owner->mNextState = eIdling;
            }

            if (!owner->mpInput->GetButton("Vertical",1) && owner->mpInput->GetAxis("L Stick Vertical") > -0.2f)
                owner->mNextState = eIdling;

            if (owner->HandleRunning())
				owner->mNextState = eRunning;
        }

        if (owner->mpBody->GetLinearVelocity().y < -50.f) 
        {
            owner->mNextState = eFalling;
            owner->mbLanded = false;
        }
        //</FS_CONDITION>
    }
    bool CharacterController::Duck::OnEnterState(float)
    {
        owner->mfTimer = owner->mfCastingDelay;
        owner->PlayAnimation(7, owner->mpAnim ? owner->mfTimer / owner->mpAnim->GetFrameSize(7) : 0.016f);
        if (owner->mpHitAudioSrc && owner->mpAudioSrc)
        {
            owner->mpHitAudioSrc->Stop();
            owner->mpAudioSrc->Stop();

            owner->mpHitAudioSrc->SetVolume(1.0f);
            owner->mpHitAudioSrc->SetFrequency(1.0f);
            owner->mpHitAudioSrc->SetPitch(1.0f);

            owner->mpAudioSrc->SetVolume(0.2f);
            owner->mpAudioSrc->SetFrequency(3.0f);
            owner->mpAudioSrc->SetPitch(1.0f);
            owner->mpAudioSrc->ChangeAudio(A_WILOROLL);

            owner->mpAudioSrc->Play();
        }
        return true; 
    }
    bool CharacterController::Duck::OnExitState(float)
    { 
        owner->mpAudioSrc->SetFrequency(1.0f);
        owner->mpAudioSrc->SetPitch(1.0f);
        owner->mfTimer = 0;
        return true;
    }
    //</AllStates>
    
}