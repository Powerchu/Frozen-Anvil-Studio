/* HEADER *********************************************************************************/
/*!
\file	ComponentDonor.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	For systems that give out components to inherit

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _COMPONENTDONOR_H_
#define _COMPONENTDONOR_H_

#include "Utility/Utility.h"
#include "DataStructure/MagicArray.h"
#include "IO/TextSerialiser.h"
#include "Object/ObjectFlags.h"

namespace Dystopia
{
	template <typename Ty, typename Settings = Ctor::MagicArrayBuilder<Ty>::SetBlockSize<64>>
	class ComponentDonor
	{
	public:
		using Component_t = Ty;
		using Array_t     = typename Settings::type;

		template <typename ... U>
		Ty* RequestComponent(U&& ...);

		void Serialise(TextSerialiser &) const;

		void Unserialise(TextSerialiser &);

		void InitComponent(const uint64_t&);
		void InitComponents(void);

	protected:

		Array_t mComponents;
	};

}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename Ty, typename S> template <typename ... U>
inline Ty* Dystopia::ComponentDonor<Ty, S>::RequestComponent(U&& ... _Args)
{
	return mComponents.Emplace(Ut::Forward<U>(_Args)...);
}


template<typename Ty, typename Settings>
inline void Dystopia::ComponentDonor<Ty, Settings>::Serialise(TextSerialiser & _Serialiser) const
{
	_Serialiser.InsertStartBlock("ComponentDonor");
	int n = 0;
	for (auto& e : mComponents)
		n += !(e.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ);
	_Serialiser << n;
	_Serialiser.InsertEndBlock("ComponentDonor End");

	for (auto& elem : mComponents)
	{
		if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ)
			continue;
		_Serialiser.InsertStartBlock("Component");
		elem.Serialise(_Serialiser);
		_Serialiser.InsertEndBlock("Component End");
	}
}

template<typename Ty, typename Settings>
inline void Dystopia::ComponentDonor<Ty, Settings>::Unserialise(TextSerialiser & _Serialiser)
{
	/*Clear Current Components*/
	unsigned Size = 0;
	int n = 0;
	for (auto& e : mComponents)
		n += !(e.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ);
	for (auto i = 0; i < n; i++)
	{
		if (mComponents[i].GetFlags() & eObjFlag::FLAG_EDITOR_OBJ)
			continue;
		mComponents.Remove(&mComponents[i]);
	}
	//mComponents.clear();

	_Serialiser.ConsumeStartBlock();
	_Serialiser >> Size;
	_Serialiser.ConsumeEndBlock();

	for (unsigned i = 0; i < Size; ++i)
	{
		_Serialiser.ConsumeStartBlock();
		Ty * pComp = RequestComponent();
		pComp->Unserialise(_Serialiser);
		_Serialiser.ConsumeEndBlock();
	}

}

template<typename Ty, typename Settings>
inline void Dystopia::ComponentDonor<Ty, Settings>::InitComponent(const uint64_t& _objectID)
{
	for (auto& elem : mComponents)
		if (elem.GetOwnerID() == _objectID)
			elem.Init();
}

template<typename Ty, typename Settings>
inline void Dystopia::ComponentDonor<Ty, Settings>::InitComponents(void)
{
	for (auto& elem : mComponents)
	{
		if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ)
			continue;
		elem.Init();
	}
}

#endif		// INCLUDE GUARD

