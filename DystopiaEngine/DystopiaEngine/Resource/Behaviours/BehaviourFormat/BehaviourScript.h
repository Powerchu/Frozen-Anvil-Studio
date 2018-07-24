/* HEADER *********************************************************************************/
/*!
\file	BehaviourScipt.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/

/* Start Script Format */
#ifndef _DefineScriptName_
#define _DefineScriptName_
#include "Component\Component.h"

namespace Dystopia
{
	class _ClassName_ : Component
	{
	public:
		_ClassName_();
		~_ClassName_();
		virtual void Load(void) override;
		virtual void Init(void) override;

		/*
		virtual void Update(const float _fDeltaTime);
		virtual void FixedUpdate(const float _fDeltaTime);
		virtual void PostUpdate(void);
		*/

		virtual void OnDestroy(void)  override;
		virtual void Unload(void)  override;
		virtual Component* Duplicate() const  override;

		virtual void Serialise(TextSerialiser&) override;

		virtual void Unserialise(TextSerialiser&) override;

	private:


	};
}

#endif 


/* End Script Format */


