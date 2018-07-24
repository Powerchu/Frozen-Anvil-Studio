/* HEADER *********************************************************************************/
/*!
\file	MyScriptFromInspector.h
\author Tan Shannon (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/

#ifndef _MyScriptFromInspector_H_
#define _MyScriptFromInspector_H_
#include "Component\Component.h"

namespace Dystopia
{
	class MyScriptFromInspector : Component
	{
	public:
		MyScriptFromInspector();
		~MyScriptFromInspector();
		
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

#endif //_MyScriptFromInspector_H_


