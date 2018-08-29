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
#include "Behaviour\Behaviour.h"

namespace Dystopia
{
	class _SF_ClassName_ : Behaviour
	{
	public:
		_SF_ClassName_();
		~_SF_ClassName_();
		
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

	private:



	};
}

#endif //_SF_DefineScriptName_H_


