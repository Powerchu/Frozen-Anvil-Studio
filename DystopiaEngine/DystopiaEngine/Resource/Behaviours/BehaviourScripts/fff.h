/* HEADER *********************************************************************************/
/*!
\file	fff.h
\author f (100%)
\par    email: f\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _fff_H_
#define _fff_H_

#define str(s) #s

#include "Behaviour\Behaviour.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class fff : Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(fff);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		unsigned GetBehaviourType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllBehaviours>::value;
		};

#endif
		//static unsigned constexpr mfffID = ;

		fff();
		~fff();
		
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

		virtual fff * Duplicate() const;

	private:



	};



	extern "C"
	{
		DllExport fff * fffClone()
		{
			return new fff;
		}
	}
}

#endif //_fff_H_


