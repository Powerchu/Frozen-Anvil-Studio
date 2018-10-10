/* HEADER *********************************************************************************/
/*!
\file	Events.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EVENTS_H_
#define _EVENTS_H_
#include "DataStructure/AutoArray.h"
#include "Utility/MetaAlgorithms.h"

namespace Dystopia
{
	typedef unsigned int EventID;
	constexpr EventID FNV_PRIME = 16777619u;
	constexpr EventID OFFSET_BASIS = 2166136261u;
	EventID EventHash(const char* _eventName);

	template<unsigned int N>
	constexpr EventID EventHashC(const char(&_eventName)[N], unsigned int I = N)
	{
		return I == 1 ? (OFFSET_BASIS ^ _eventName[0]) * FNV_PRIME :
			(EventHashC(_eventName, I - 1) ^ (_eventName[I - 1])) * FNV_PRIME;
	}

	class EventCallback
	{
	public:
		EventCallback();
		EventCallback(void(&_rhs)(void));
		~EventCallback();

		void Fire() const;

		EventCallback& operator=(const EventCallback& _rhs);
		EventCallback& operator=(EventCallback&& _rhs);

		template<class Caller>
		EventCallback(void(Caller::*_rhs)(void), Caller* const _user)
			: mpModelEvent{ new Model<void(Caller::*)(void)>{ _rhs, _user } },
			mpGenericComparison{ _user }
		{}

		template<class Caller>
		bool IsBindedTo(Caller * const _toCheckAgainst)
		{
			void* const toGeneric = _toCheckAgainst;
			return (toGeneric == mpGenericComparison);
		}

	private:
		struct Concept
		{
			virtual ~Concept() {}

			virtual void		Fire()		const = 0;
			virtual Concept*	Duplicate() const = 0;
		};

		template<typename NonMemberFunction>
		struct Model : Concept
		{
			Model(NonMemberFunction& _fn)
				: mpNonMemFunc{ &_fn }
			{}

			void Fire() const override
			{
				mpNonMemFunc();
			}

			Model<NonMemberFunction>* Duplicate() const override
			{
				return new Model<NonMemberFunction>{ *mpNonMemFunc };
			}

			NonMemberFunction * const mpNonMemFunc;
		};

		template<typename ClassCaller>
		struct Model<void(ClassCaller::*)(void)> : Concept
		{
			Model(void(ClassCaller::*_fn)(void), ClassCaller * const _user)
				: mpMemberFunc{ _fn }, mpCaller{ _user }
			{}

			void Fire() const override
			{
				(mpCaller->*mpMemberFunc)();
			}

			Model<void(ClassCaller::*)(void)>* Duplicate() const override
			{
				return new Model<void(ClassCaller::*)(void)>{ mpMemberFunc, mpCaller };
			}

			void(ClassCaller::*mpMemberFunc)(void);
			ClassCaller * const mpCaller;
		};

		void*		mpGenericComparison;
		Concept*	mpModelEvent;
	};

	class Event
	{
	public:
		Event(EventID _byID);
		Event(const char *_ByName);
		~Event();

		EventID			GetID()	const;
		void			Fire()	const;
		void			Bind(void(&_fn)(void));
		void			UnbindAll();

		template<class Caller>
		void Bind(void(Caller::*_fn)(void), Caller * const _user)
		{
			EventCallback *p = new EventCallback{ _fn, _user };
			mArrEventCallbacks.push_back(p);
		}

		template<class Caller>
		void Unbind(Caller * const _user)
		{
			for (unsigned int i = 0; i < mArrEventCallbacks.size(); ++i)
			{
				EventCallback* e = mArrEventCallbacks[i];
				if (e->IsBindedTo(_user))
				{
					mArrEventCallbacks.Remove(i);
					delete e;
				}
			}
		}
	private:
		EventID						mID;
		AutoArray<EventCallback*>	mArrEventCallbacks;
	};

}

/*
namespace
{
	template<class Caller, typename ... Params>
	struct FunctionContainer
	{
	private:
		template<typename A>
		struct AuxExecution;

		template<size_t ... Ns>
		struct AuxExecution<std::index_sequence<Ns ...>>
		{
			AuxExecution(FunctionContainer<Caller, Params...>& p)
				: parent{ p }
			{}

			void Execute(Caller * const _toMod) const
			{
				(_toMod->*(parent.mfptr))(std::get<Ns>(parent.mTupleVariables) ...);
			}

			FunctionContainer<Caller, Params...> &parent;
		};

		void(Caller::*mfptr)(Params ...);
		std::tuple<std::remove_reference_t<Params>...> mTupleVariables;
		AuxExecution<std::make_index_sequence<sizeof...(Params)>> auxCaller;

	public:
		FunctionContainer(void(Caller::*_fnptr)(Params ...), std::remove_reference_t<Params> ... pack)
			: mfptr{ _fnptr },
			mTupleVariables{ pack... },
			auxCaller{ *this }
		{}

		FunctionContainer(const FunctionContainer& _rhs)
			: mfptr{ _rhs.mfptr },
			mTupleVariables{ _rhs.mTupleVariables },
			auxCaller{ *this }
		{}

		void Execute(Caller * const _toMod) const
		{
			auxCaller.Execute(_toMod);
		}

		FunctionContainer& operator=(const FunctionContainer& _rhs)
		{
			mfptr = _rhs.mfptr;
			mTupleVariables = _rhs.mTupleVariables;
		}
	};
}
* /

class EventWrapper
{
public:
	template<typename ... Params>
	EventWrapper(void(&_rhs)(Params ...))
	{}

	template<class Caller, typename ... Params>
	EventWrapper(void(Caller::*_rhs)(Params...), Caller* const _user)
		: mpModelEvent{ new Model<void(Caller::*)(Params...)>{ _rhs, _user } },
		mpGenericComparison{ _user }
	{}

	template<typename ... Params>
	void Fire(Params ... ps) const
	{
		mpModelEvent->Fire(ps ...);
	}

	template<class Caller>
	bool IsBindedTo(Caller * const _toCheckAgainst)
	{
		void* const toGeneric = _toCheckAgainst;
		return (toGeneric == mpGenericComparison);
	}

private:
	/*
	template <typename>
	struct EventCaller;

	template <typename T>
	struct EvemtCaller<Event<T>>
	{
		Event<T> sad;

		template <typename ... Ts>
		auto operator () (Ts&& ... args)
		{
			return static_cast<T&>(sad).Fire(Ut::Forward<Ts>(args)...);
		}
	};
	template <typename D>
	struct Concept
	{
		virtual ~Concept() {}

		template <typename ... Params>
		void Fire(Params ... ps) const
		{
			static_cast<D*>(this)->Fire(ps...);
		}
	};

	template<typename ... Params>
	struct Model : Concept<Model<Params ...>>
	{
		Model(void(*_fn)(Params...))
			: mpNonMemFunc{ _fn }
		{}

		template <typename ... Ps>
		void Fire(Ps ... p) const
		{
			mpNonMemFunc(p ...);
		}
		void(*mpNonMemFunc)(Params...);
	};


	template<typename ClassCaller, typename ... Params>
	struct Model<void(ClassCaller::*)(Params...)> : Concept<Model<void(ClassCaller::*)(Params...)>>
	{
		Model(void(ClassCaller::*_fn)(Params...), ClassCaller * const _user)
			: mpMemberFunc{ _fn }, mpCaller{ _user }
		{}

		template <typename ... Ps>
		void Fire(Ps ... p) const
		{
			(mpCaller->*mpMemberFunc)(p ...);
		}

		void(ClassCaller::*mpMemberFunc)(Params...);
		ClassCaller * const mpCaller;
	};

	void*		mpGenericComparison;
	Concept		*mpModelEvent;
};
	*/

/* Possible future expansion for return types and parameters not void */

template<typename A>
struct FunctionWrapper
{};

template<typename ReturnType, typename ... Args>
struct FunctionWrapper<ReturnType(&)(Args...)>
{
	FunctionWrapper(ReturnType(&rhs)(Args...))
		: fcallback{ rhs }
	{}

	ReturnType operator()(Args ... params)
	{
		return fcallback(params ...);
	}

	ReturnType(&fcallback)(Args...);
};

template<typename ReturnType, typename C, typename ... Args>
struct FunctionWrapper<ReturnType(C::*)(Args...)>
{
	FunctionWrapper(ReturnType(C::*mfp)(Args...), C*  user)
		: fcallback{ mfp }, caller{ user }
	{}

	ReturnType operator()(Args ... params)
	{
		return (caller->*fcallback)(params ...);
	}

	C* caller;
	ReturnType(C::*fcallback)(Args...);
};

template<typename ReturnType, typename ... Args>
auto Bind(ReturnType(&f)(Args...))
{
	FunctionWrapper<ReturnType(&)(Args...)> func = f;
	return func;
}

template<typename ReturnType, typename C, typename ... Args>
auto Bind(ReturnType(C::*f)(Args...), C* user)
{
	FunctionWrapper<ReturnType(C::*)(Args...)> func = { f, user };
	return func;
}



#endif 
#endif 

