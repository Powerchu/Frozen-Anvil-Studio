/* HEADER *********************************************************************************/
/*!
\file	Scaling.h
\author dan (100%)
\par    email: dan\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _Scaling_H_
#define _Scaling_H_

#define str(s) #s

#include "Behaviour\Behaviour.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class Scaling : Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(Scaling);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		unsigned GetBehaviourType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllBehaviours>::value;
		};

#endif
		virtual const std::string GetEditorName(void) const override { return "Scaling"; }
		static uint64_t constexpr mScalingID = 2029529600;

		Scaling();
		~Scaling();
		
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

		virtual Scaling * Duplicate() const;
		
		virtual void EditorUI(void) noexcept override;

	private:



	};



	extern "C"
	{
		DllExport Scaling * ScalingClone()
		{
			return new Scaling;
		}
	}
}

#endif //_Scaling_H_


