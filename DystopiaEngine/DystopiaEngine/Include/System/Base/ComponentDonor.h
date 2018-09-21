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

#include "Utility\Utility.h"
#include "DataStructure\MagicArray.h"
#include "IO/TextSerialiser.h"


namespace Dystopia
{
	template <typename Ty, typename Settings = Ctor::MagicArrayBuilder<Ty>::SetBlockSize<64>>
	class ComponentDonor
	{
	public:
		using Component_t = Ty;

		template <typename ... U>
		Ty* RequestComponent(U&& ...);

		void Serialise(TextSerialiser &) const;

		void Unserialise(TextSerialiser &);


	protected:

		typename Settings::type mComponents;
	};

}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename Ty, typename S> template <typename ... U>
inline Ty* Dystopia::ComponentDonor<Ty, S>::RequestComponent(U&& ... _Args)
{
	return mComponents.Emplace(Utility::Forward<U>(_Args)...);
}


template<typename Ty, typename Settings>
inline void Dystopia::ComponentDonor<Ty, Settings>::Serialise(TextSerialiser & _Serialiser) const
{
	_Serialiser.InsertStartBlock("ComponentDonor");

	_Serialiser << mComponents.size();

	for (auto & elem : mComponents)
	{
		
		_Serialiser.InsertStartBlock("Component");
		elem.Serialise(_Serialiser);
		_Serialiser.InsertEndBlock("Component End");
	}
	_Serialiser.InsertEndBlock("ComponentDonor End");

}

template<typename Ty, typename Settings>
inline void Dystopia::ComponentDonor<Ty, Settings>::Unserialise(TextSerialiser & _Serialiser)
{
	/*Clear Current Components*/
	unsigned Size = 0;

	mComponents.clear();

	_Serialiser.ConsumeStartBlock();

	_Serialiser.Read(Size);

	
	for (unsigned i = 0; i < Size; ++i)
	{
		_Serialiser.ConsumeStartBlock();

		Ty * pComp = RequestComponent();
		pComp->Unserialise(_Serialiser);

		_Serialiser.ConsumeEndBlock();
	}

	_Serialiser.ConsumeEndBlock();
}


#endif		// INCLUDE GUARD

