/* HEADER *********************************************************************************/
/*!
\file	Driver.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Internal part of Game Engine that runs everything

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Driver\Driver.h"

#include "Globals.h"
#include "DataStructure\SharedPtr.h"
#include "Utility\MetaAlgorithms.h"
#include "Utility\MetaDataStructures.h"

#include "System\Time\TimeSystem.h"
#include "System\Input\InputSystem.h"
#include "System\Sound\SoundSystem.h"
#include "System\Graphics\GraphicsSystem.h"
#include "System\Window\WindowManager.h"


namespace
{
	template <typename Ty>
	void RecursiveNewInsertAutoArray(AutoArray<Ty>&)
	{

	}

	template <typename Ty, typename T, typename ... Ts>
	void RecursiveNewInsertAutoArray(AutoArray<Ty>& _arr)
	{
		_arr.EmplaceBack(new T{});
		RecursiveNewInsertAutoArray<Ty, Ts...>(_arr);
	}

	template <typename Ty, typename ... T>
	AutoArray<Ty> MakeAutoArray(Utility::TypeList<T...>)
	{
		AutoArray<Ty> ret{sizeof...(T)};

		RecursiveNewInsertAutoArray<Ty, T...>(ret);

		return ret;
	}

	template <typename T>
	struct ErrorOnDupe;

	template <
		template <typename ...> class Set,
		template <unsigned, typename> class Holder,
		typename ... Tys, unsigned ... Ns,
		typename Ty1, typename Ty2, unsigned N1, unsigned N2>
	struct ErrorOnDupe <Set<Holder<N1, Ty1>, Holder<N2, Ty2>, Holder<Ns, Tys>...>>
	{
		static_assert(!(N1 == N2), "System Error: Systems cannot have the same index! (Same enum?).");

		using eval = typename ErrorOnDupe<Set<Holder<N2, Ty2>, Holder<Ns, Tys>...>>::eval;
	};

	template <
		template <typename ...> class Set,
		template <unsigned, typename> class Holder,
		typename Ty1, typename Ty2, unsigned N1, unsigned N2>
	struct ErrorOnDupe <Set<Holder<N1, Ty1>, Holder<N2, Ty2>>>
	{
		static_assert(!(N1 == N2), "System Error: Systems cannot have the same index! (Same enum?).");

		using eval = void;
	};
}



SharedPtr<Dystopia::EngineCore> const & Dystopia::EngineCore::GetInstance(void) noexcept
{
	static SharedPtr<EngineCore> pInstance = CreateShared(new EngineCore{});
	return pInstance;
}

Dystopia::EngineCore::EngineCore(void) :
	mTime{}, SystemList{ Utility::SizeofList<AllSys>::value },
	SystemTable{ MakeAutoArray<Systems*>(Utility::MakeTypeList_t<Utility::TypeList, AllSys>{}) }
{
	using SanityCheck = typename ErrorOnDupe<AllSys>::eval;
}

void Dystopia::EngineCore::LoadSettings(void)
{
	for (auto& e : SystemTable)
		e->LoadDefaults();
}

void Dystopia::EngineCore::Init(void)
{
	mTime.Lap();

	for (auto& e : SystemTable)
		e->PreInit();

	for (auto& e : SystemTable)
	{
		if (e->Init())
		{
			SystemList.EmplaceBack(e);
		}
		else
		{
			delete e;
			e = nullptr;
		}
	}

	for (auto& e : SystemList)
		e->PostInit();

	mTime.Lap();
}

void Dystopia::EngineCore::FixedUpdate(void)
{
	static float dt = .0f;

	while (dt > _FIXED_UPDATE_DT)
	{
		for (auto& e : SystemList)
		{
			e->FixedUpdate(dt);
		}

		dt -= _FIXED_UPDATE_DT;
	}
}

void Dystopia::EngineCore::Update(void)
{
	float dt = mTime.Elapsed();
	mTime.Lap();

	for (auto& e : SystemList)
	{
		e->Update(dt);
	}
}

void Dystopia::EngineCore::Shutdown(void)
{
	for (auto& e : SystemList)
		e->Shutdown();

	for (auto& e : SystemList)
		delete e;

	SystemList.clear();
	SystemTable.clear();
}

#if !EDITOR

int WinMain(HINSTANCE, HINSTANCE, char *, int)
{
	auto CORE = Dystopia::EngineCore::GetInstance();

	/*
	CORE->ExecuteGame();
	*/

	return 0;
}

#endif

