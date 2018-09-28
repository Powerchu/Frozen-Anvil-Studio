/* HEADER *********************************************************************************/
/*!
\file	_SF_ClassName_.h
\author _SF_CreatorName_ (100%)
\par    email: _SF_CreatorLogin_\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SF_DefineScriptName_H_
#define _SF_DefineScriptName_H_

#define str(s) #s

#include "Behaviour\Behaviour.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{
	class _SF_ClassName_ : Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(_SF_ClassName_);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		unsigned GetBehaviourType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllBehaviours>::value;
		};

#endif
		static uint64_t constexpr m_SF_ClassName_ID = _SF_UID_;

		_SF_ClassName_();
		~_SF_ClassName_();
		
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

		virtual _SF_ClassName_ * Duplicate() const;

	private:



	};



	extern "C"
	{
		DllExport _SF_ClassName_ * _SF_ClassName_Clone()
		{
			return new _SF_ClassName_;
		}
	}
}

#endif //_SF_DefineScriptName_H_


