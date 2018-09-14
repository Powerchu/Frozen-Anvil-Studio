#pragma once

#ifndef BEHAVIOUR_SYSTEM_H
#define BEHAVIOUR_SYSTEM_H

#include "System/Base/Systems.h"
#include "DataStructure/MagicArray.h"

#if EDITOR
#include "Editor/HotLoader.h"
#endif


namespace Dystopia
{
	struct BehaviourSystem : Systems
	{
		virtual void PreInit(void);
		virtual bool Init(void);
		virtual void PostInit(void);

		virtual void FixedUpdate(float);
		virtual void Update(float);
		virtual void PostUpdate(void);
		virtual void Shutdown(void);

		virtual void LoadDefaults(void);
		virtual void LoadSettings(TextSerialiser&);

	private:

#if EDITOR
		Hotloader<2> mHotloader;
#endif
	};

}



#endif