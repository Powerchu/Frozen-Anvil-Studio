/* HEADER *********************************************************************************/
/*!
\file	ParentToHero.h
\author aaron (100%)
\par    email: m.chu\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _ParentToHero_H_
#define _ParentToHero_H_

#define str(s) #s

#include "Behaviour\Behaviour.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class ParentToHero : Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(ParentToHero);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		unsigned GetBehaviourType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllBehaviours>::value;
		};

#endif
		virtual const std::string GetEditorName(void) const override { return "ParentToHero"; }
		static uint64_t constexpr mParentToHeroID = 16555008;

		ParentToHero();
		~ParentToHero();
		
		virtual void Load(void) override;
		virtual void Init(void) override;

		virtual void Update(const float _fDeltaTime) override;
		virtual void FixedUpdate(const float _fDeltaTime) override;
		virtual void PostUpdate(void) override;

		virtual void GameObjectDestroy(void) override;
		virtual void Unload(void) override;

		virtual void Serialise(TextSerialiser&) const override;

		virtual void Unserialise(TextSerialiser&) override;

		virtual const char * const GetBehaviourName() const;

		virtual ParentToHero * Duplicate() const;
		
		virtual void EditorUI(void) noexcept override;

	private:
		GameObject * mpTarget;
		//GameObject * mpOwner;

	};



	extern "C"
	{
		DllExport ParentToHero * ParentToHeroClone()
		{
			return new ParentToHero;
		}
	}
}

#endif //_ParentToHero_H_


