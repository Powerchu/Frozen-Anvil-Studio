/* HEADER *********************************************************************************/
/*!
\file	fdfdfd.h
\author fdf (100%)
\par    email: df\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _fdfdfd_H_
#define _fdfdfd_H_

#define str(s) #s

#include "Behaviour\Behaviour.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class fdfdfd : Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(fdfdfd);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		unsigned GetBehaviourType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllBehaviours>::value;
		};

#endif
		static uint64_t constexpr mfdfdfdID = 40894464;

		fdfdfd();
		~fdfdfd();
		
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

		virtual fdfdfd * Duplicate() const;

	private:



	};



	extern "C"
	{
		DllExport fdfdfd * fdfdfdClone()
		{
			return new fdfdfd;
		}
	}
}

#endif //_fdfdfd_H_


