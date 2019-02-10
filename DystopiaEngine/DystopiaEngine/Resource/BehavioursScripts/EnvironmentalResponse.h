/* HEADER *********************************************************************************/
/*!
\file	EnvironmentalResponse.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _EnvironmentalResponse_H_
#define _EnvironmentalResponse_H_

#define str(s) #s

#include "Behaviour/Behaviour.h"
#include "Reflection/Reflection.h"
#include "Reflection/ReflectionTypeErasure.h"
#include "Behaviour/BehaviourMemberFunc.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class EnvironmentalResponse : public Behaviour
	{
	public:
		static constexpr const char * BehaviourName = str(EnvironmentalResponse);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "EnvironmentalResponse"; }
		static uint64_t constexpr mEnvironmentalResponseID = 18446744071651919872;

		// Default Constructor for this Behaviour - init your variables here
		EnvironmentalResponse();
		// Default Destructor for this Behaviour - don't touch if you dk, else ask the tech team
		~EnvironmentalResponse();

		// Load will never be called
		virtual void Load(void) override{}
		// Awake occurs when the editor is first started
		virtual void Awake(void) override{} 
		// Init occurs when the scene is played
		virtual void Init(void) override; 

		// Update occurs every frame
		virtual void Update(const float ) override; 
		// FixedUpdate occurs every 0.02ms
		virtual void FixedUpdate(const float ) override{} 
		// PostUpdate occurs after the above two updates
		virtual void PostUpdate(void) override{}

		// GameObjectDestroy occurs when the gameobject is destroyed
		virtual void GameObjectDestroy(void) override;
		// Unload will never occur
		virtual void Unload(void) override{}

		// If Owner has a Collider, and enters another collider, this occurs
		virtual void OnCollisionEnter(const CollisionEvent&){}
		// If Owner has a Collider, and stays in another collider, this occurs
		virtual void OnCollisionStay (const CollisionEvent&){}
		// If Owner has a Collider, and exits another collider, this occurs
		virtual void OnCollisionExit (const CollisionEvent&){}

		// If Owner has a Trigger, and enters another trigger, this occurs
		virtual void OnTriggerEnter(GameObject * const);
		// If Owner has a Trigger, and stays in another trigger, this occurs
		virtual void OnTriggerStay (GameObject * const){}
		// If Owner has a Trigger, and exit another trigger, this occurs
		virtual void OnTriggerExit (GameObject * const);

		virtual void Serialise(TextSerialiser&) const override{}
		virtual void Unserialise(TextSerialiser&) override{}

		virtual const char * const GetBehaviourName() const;

		// Don't touch
		virtual EnvironmentalResponse * Duplicate() const;

		// Don't touch
		virtual void EditorUI(void) noexcept override{}
		
		// Reflection Stuff - Don't Touch
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		bool AmbienceSmall;
		bool ForegroundReactions;
		
		float AmbienceIntensity;
		float AmbienceDuration;
		float AmbienceFreq;
		float ForegroundIntensity;
		float ForegroundDuration;
		float ForegroundFreq;
		
		bool Direction;
		
	private:
		// Don't touch
		friend MetaData<EnvironmentalResponse>;
		
		float mfAccuDt;
		float mfDurationCounter;
		float mfIntensityCounter;
		bool mbDoShake;
		
		void Ambience(float);
		void Foreground(float);
	};

	extern "C"
	{
		inline DllExport EnvironmentalResponse * EnvironmentalResponseClone()
		{
			return new EnvironmentalResponse;
		}
	}
}

/*Keep this if you do not want to show anything in Editor*/
//PP_REFLECT_EMPTY(Dystopia::EnvironmentalResponse)
/*
  Uncomment the line PP_REFLECT and add in the names of the variable you want to show
  Comment out PP_REFLECT_EMPTY.
*/
PP_REFLECT_STUFF(Dystopia::EnvironmentalResponse, Direction, AmbienceSmall, ForegroundReactions, AmbienceIntensity, AmbienceDuration, AmbienceFreq, ForegroundIntensity, ForegroundDuration, ForegroundFreq);

#endif //_EnvironmentalResponse_H_


