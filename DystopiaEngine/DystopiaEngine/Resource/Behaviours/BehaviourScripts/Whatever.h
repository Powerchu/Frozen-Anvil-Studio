/* HEADER *********************************************************************************/
/*!
\file	Whatever.h
\author Tan Shannon (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _Whatever_H_
#define _Whatever_H_
#include "Behaviour\Behaviour.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class DllExport Whatever : Behaviour
	{
	public:

		static constexpr const char * BehaviourName = Whatever;
#if !EDITOR

		using SYSTEM = BehaviourSystem;
		unsigned GetBehaviourType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllBehaviours>::value;
		};

#endif
		static unsigned constexpr mWhateverID = ;
		Whatever();
		~Whatever();
		
		virtual void Load(void) override;
		virtual void Init(void) override;

		virtual void Update(const float _fDeltaTime) override;
		virtual void FixedUpdate(const float _fDeltaTime) override;
		virtual void PostUpdate(void) override;

		virtual void GameObjectDestroy(void) override;
		virtual void Unload(void) override;
		
		virtual Component* Duplicate() const override;

		virtual void Serialise(TextSerialiser&) const override;

		virtual void Unserialise(TextSerialiser&) override;

		virtual const char * const GetBehaviourName() const override;

		virtual Whatever * Duplicate() const;

	private:



	};



	extern "C"
	{
		DllExport Whatever * WhateverClone()
		{
			return new Whatever;
		}
	}
}

#endif //_Whatever_H_


