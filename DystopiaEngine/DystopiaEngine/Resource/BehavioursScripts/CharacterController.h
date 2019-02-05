/* HEADER *********************************************************************************/
/*!
\file    CharacterController.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _CharacterController_H_
#define _CharacterController_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "Behaviour/BehaviourMemberFunc.h"

//custom include
#include "DataStructure/HashString.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
    class InputManager;
    class RigidBody;
	class SpriteRenderer;
	class Convex;
    class AudioSource;
    class CharacterController : public Behaviour
    {
    public:
        static constexpr const char * BehaviourName = str(CharacterController);
#if !EDITOR
        
        using SYSTEM = BehaviourSystem;
        // unsigned GetBehaviourType(void) const
        // {
        //     return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
        // };

#endif
        virtual const char * const GetBehaviourName() const {return CharacterController::BehaviourName;}
        virtual const std::string GetEditorName(void) const override {return "CharacterController";}
        static uint64_t constexpr mCharacterControllerID = 916459008;

        // Default Constructor for this Behaviour - init your variables here
        CharacterController();
		CharacterController(const CharacterController&);
        // Default Destructor for this Behaviour - don't touch if you dk, else ask the tech team
        ~CharacterController();

        // Load will never be called
        virtual void Load(void) override{}
        // Awake occurs when the editor is first started
        virtual void Awake(void) override; 
        // Init occurs when the scene is played
        virtual void Init(void) override; 

        // Update occurs every frame
        virtual void Update(const float _fDeltaTime) override; 
        // FixedUpdate occurs every 0.02ms
        virtual void FixedUpdate(const float _fDeltaTime) override;
        // PostUpdate occurs after the above two updates
        virtual void PostUpdate(void) override {}

        // GameObjectDestroy occurs when the gameobject is destroyed
        virtual void GameObjectDestroy(void) override;
        // Unload will never occur
        virtual void Unload(void) override {}

        // If Owner has a Collider, and enters another collider, this occurs
        virtual void OnCollisionEnter(const CollisionEvent&);
        // If Owner has a Collider, and stays in another collider, this occurs
        virtual void OnCollisionStay (const CollisionEvent&);
        // If Owner has a Collider, and exits another collider, this occurs
        virtual void OnCollisionExit (const CollisionEvent&);

        // If Owner has a Trigger, and enters another trigger, this occurs
        virtual void OnTriggerEnter(GameObject * const);
        // If Owner has a Trigger, and stays in another trigger, this occurs
        virtual void OnTriggerStay (GameObject * const);
        // If Owner has a Trigger, and exit another trigger, this occurs
        virtual void OnTriggerExit (GameObject * const);

        virtual void Serialise(TextSerialiser&) const override {}
        virtual void Unserialise(TextSerialiser&) override {}


        // Don't touch
        virtual CharacterController * Duplicate() const;

        // Don't touch
        virtual void EditorUI(void) noexcept override {}
        
        // Reflection Stuff - Don't Touch
        virtual TypeErasure::TypeEraseMetaData       GetMetaData();
        virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;
        
        enum FStateTypes
        {
            INVALID = -1,
            //<FStateTypes>
            eIdling,
            eRunning,
            eJumping,
            eAttacking,
            eCasting,
            eRolling,
            eFlinching,
            eFalling,
            eLanding,
            eDeath,
            eDuck,
            //</FStateTypes>
            OUT_OF_RANGE
        };
        
        struct IFiniteState
        {
            IFiniteState(CharacterController* _owner) : owner{_owner}{}
            ~IFiniteState(void) {}
            virtual void Update(float) =0;
            virtual bool OnEnterState(float) =0;
            virtual bool OnExitState(float) =0;
            CharacterController* owner;
        };
        
        //<AllStates>
        //Idling
        struct Idling : IFiniteState
        {
            Idling(CharacterController*);
            ~Idling(void);
            void Update(float);
            bool OnEnterState(float);
            bool OnExitState(float);
        };
        //Running
        struct Running : IFiniteState
        {
            Running(CharacterController*);
            ~Running(void);
            void Update(float);
            bool OnEnterState(float);
            bool OnExitState(float);
        };
        //Jumping
        struct Jumping : IFiniteState
        {
            Jumping(CharacterController*);
            ~Jumping(void);
            void Update(float);
            bool OnEnterState(float);
            bool OnExitState(float);
        };
        //Attacking
        struct Attacking : IFiniteState
        { 
            Attacking(CharacterController*);
            ~Attacking(void);
            void Update(float);
            bool OnEnterState(float);
            bool OnExitState(float);
        };
        //Casting
        struct Casting : IFiniteState
        {
            Casting(CharacterController*);
            ~Casting(void);
            void Update(float);
            bool OnEnterState(float);
            bool OnExitState(float);
        };
        //Rolling
        struct Rolling : IFiniteState
        {
            Rolling(CharacterController*);
            ~Rolling(void);
            void Update(float);
            bool OnEnterState(float);
            bool OnExitState(float);
        };
        //Flinching
        struct Flinching : IFiniteState
        {
            Flinching(CharacterController*);
            ~Flinching(void);
            void Update(float);
            bool OnEnterState(float);
            bool OnExitState(float);
        };
        //Falling
        struct Falling : IFiniteState
        {
            Falling(CharacterController*);
            ~Falling(void);
            void Update(float);
            bool OnEnterState(float);
            bool OnExitState(float);
        };
        //Landing
        struct Landing : IFiniteState
        {
            Landing(CharacterController*);
            ~Landing(void);
            void Update(float);
            bool OnEnterState(float);
            bool OnExitState(float);
        };
        //Death
        struct Death : IFiniteState
        {
            Death(CharacterController*);
            ~Death(void);
            void Update(float);
            bool OnEnterState(float);
            bool OnExitState(float);
        };
        //Duck
        struct Duck : IFiniteState
        {
            Duck(CharacterController*);
            ~Duck(void);
            void Update(float);
            bool OnEnterState(float);
            bool OnExitState(float);
        };
        //</AllStates>
        
        //<FS_VAR>
        FStateTypes mNextState;
        FStateTypes mCurrentState;
        IFiniteState* mStates[OUT_OF_RANGE];
        //</FS_VAR>
        
        enum StateStatus
        {
            Updating = 0,
            Exiting,
            Entering
        };
        InputManager*   mpInput;
        RigidBody*      mpBody;
        SpriteRenderer* mpAnim;
		Convex*			mpCollider;
        AudioSource*    mpAudioSrc;
        AudioSource*    mpHitAudioSrc;

		bool 			mbDisableControls;
		bool            mbIsController;
        bool            mbFaceRight;
        bool            mbGotHit;
		bool 			mbLanded;
		bool			mbIsJumping;
		
		float 			mfAttackDelay;
		float 			mfCastingDelay;
		float 			mfRollDelay;
		float			mfFlinchDelay;
		float			mfLandDelay;
		float 			mfTimer;
        float           mfInvulTimer;
		
		bool 			mbDying;
        StateStatus     mStatus;

		//float			mfAccumulatedTime;
		float			mfMaxSpeed;
		float			mfCurrentSpeed;
		float			mfJumpStrength;
		float			mfAirMovementScalerForward;
		float			mfAirMovementScalerBackward;
		float 			mfAnalogSens;
		
		float			mfAnimSpdIdle;
		float			mfAnimSpdRunning;
		float			mfAnimSpdJumping;
		float			mfAnimSpdFalling;  
		
		unsigned 		mnCurrentAttackChain;
		bool			mbSpammedAttack;
		bool			mbIsInForce;
		bool			mbSpammedCast;
		float 			mfAttackPower;
		float			mfAttackKnockback;
		float			mfAttackPushfoward;
		float			mfRollForce;


        /* Stats */
        float           mfMaxHealth = 100.0f;
        float           mfCurrHealth = 100.0f;
        float           mfMaxMana = 100.0f;
        float           mfCurrMana = 100.0f;

        /* Combat Costs */
        float           mfRollCost = 25.0f;
        float           mfManaDelay = 0.0f;
        float           mfHealthDelay = 0.0f;

        /* Upgrades */
        float           mfManaRegen = 25.0f;
        float           mfHealthRegen = 0.0f;
		
		int mnStateDebug;
        int mnPriorityDir;

		void MapButtonsDefault(void);
		void ReMapButtons(void);
		bool HandleRunning(void);
		void HandleTranslate(bool, float);
		void PlayAnimation(int _animID, float _speed);
        void UseMana(float _amount, float _delay);

		bool prevFace;
		bool mbAppliedAtk;
		float mfInternalForceMultiplier;
		bool mbChainedInto;
		int mnManualSkill;
        int mnRunningCheck = 0;

        Math::Vector3D orgOffset;
        Math::Vector3D orgScale; 
        
        GameObject * mpMyHitBox;
        GameObject * mpCastPivot;
        GameObject * mpMyShadow = nullptr;
        GameObject * mpCamShaker = nullptr;
        Math::Vec3D myShadowOrg;
		
		HashString mChainSkillName[2];
		HashString mManualSkillName[2][2];
		
		void TakeForce(float _force, Math::Vec2 _dir);
        void TakeDamage(float _dmg, float vibrateStrength = 0.0f);
        bool CheckLanded();
		
		PP_MEMBERFUNC(Dystopia::CharacterController, TakeForce, CheckLanded, TakeDamage)
        
    private:
        // Don't touch
        friend MetaData<CharacterController>;
        float magicMult = 0.02f;
        
    };

    extern "C"
    {
        inline DllExport CharacterController * CharacterControllerClone()
        {
            return new CharacterController;
        }
    }
}

/*Keep this if you do not want to show anything in Editor*/
//PP_REFLECT_EMPTY(Dystopia::CharacterController)
/*
  Uncomment the line PP_REFLECT and add in the names of the variable you want to show
  Comment out PP_REFLECT_EMPTY.
*/
PP_REFLECT_STUFF(Dystopia::CharacterController, mbDisableControls,  mnPriorityDir, mbFaceRight, mfTimer, mfMaxSpeed, mfJumpStrength, mfAirMovementScalerForward, mfAirMovementScalerBackward, mnStateDebug, mfAttackDelay, mfCastingDelay, mfRollDelay, mfFlinchDelay, mfLandDelay, mfAnimSpdIdle, mfAnimSpdRunning, mfAnimSpdJumping, mfAnimSpdFalling, mbSpammedAttack, mbIsInForce, mbSpammedCast, mfAttackPower, mfAttackKnockback, mfAttackPushfoward, mfAnalogSens, mfRollForce, mfInvulTimer, mfMaxHealth, mfCurrHealth, mfMaxMana, mfCurrMana, mfRollCost, mnRunningCheck, mfInternalForceMultiplier);

#endif //_CharacterController_H_


