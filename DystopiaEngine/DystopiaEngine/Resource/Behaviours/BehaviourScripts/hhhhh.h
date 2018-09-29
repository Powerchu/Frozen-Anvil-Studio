/* HEADER *********************************************************************************/
/*!
\file	hhhhh.h
\author h (100%)
\par    email: ggh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _hhhhh_H_
#define _hhhhh_H_

#define str(s) #s

#include "Behaviour\Behaviour.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class hhhhh : Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(hhhhh);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		unsigned GetBehaviourType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllBehaviours>::value;
		};

#endif
		static uint64_t constexpr mhhhhhID = 1871708160;

		hhhhh();
		~hhhhh();
		
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

		virtual hhhhh * Duplicate() const;

	private:



	};



	extern "C"
	{
		DllExport hhhhh * hhhhhClone()
		{
			return new hhhhh;
		}
	}
}

#endif //_hhhhh_H_


