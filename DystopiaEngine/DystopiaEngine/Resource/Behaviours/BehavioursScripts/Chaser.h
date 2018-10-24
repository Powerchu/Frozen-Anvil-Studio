/* HEADER *********************************************************************************/
/*!
\file	Chaser.h
\author Dan (100%)
\par    email: dan.kang\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _Chaser_H_
#define _Chaser_H_

#define str(s) #s

#include "Behaviour\Behaviour.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class Chaser : Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(Chaser);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		// unsigned GetBehaviourType(void) const
		// {
		// 	return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllBehaviours>::value;
		// };

#endif
		virtual const std::string GetEditorName(void) const override { return "Chaser"; }
		static uint64_t constexpr mChaserID = 18446744071765508608;

		Chaser();
		~Chaser();
		
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

		virtual Chaser * Duplicate() const;
		
		virtual void EditorUI(void) noexcept override;

	private:



	};



	extern "C"
	{
		DllExport Chaser * ChaserClone()
		{
			return new Chaser;
		}
	}
}

#endif //_Chaser_H_


