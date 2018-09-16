#pragma once

#ifndef BEHAVIOUR_SYSTEM_H
#define BEHAVIOUR_SYSTEM_H

#include "System/Base/Systems.h"
#include "DataStructure/MagicArray.h"
#include "DataStructure/SharedPtr.h"
#include "System/Base/ComponentDonor.h"
#if EDITOR
#include "Editor/HotLoader.h"
#endif








#endif

namespace Dystopia
{
	class Behaviour;

#if EDITOR
	struct BehaviourWrap
	{
		BehaviourWrap(){}
		BehaviourWrap(std::string const & _name, SharedPtr<Behaviour> _pointer)
			:mName{_name}, mpBehaviour{ _pointer }
		{
		}
		std::string mName;
		SharedPtr<Behaviour> mpBehaviour;
	};

	struct BehaviourSystem : Systems, public ComponentDonor<Behaviour>
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

		MagicArray< SharedPtr<Behaviour> > mBehaviours;

#if EDITOR
		
		Hotloader<1> mHotloader;
		MagicArray<BehaviourWrap> mvBehaviourReferences;
#endif
	};

}



#endif