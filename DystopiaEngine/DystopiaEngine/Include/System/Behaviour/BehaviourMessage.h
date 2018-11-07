#pragma once
#ifndef _BEHAVIOUR_MESSAGE_H
#define _BEHAVIOUR_MESSAGE_H

#include "DataStructure/Tuple.h"
#include "Utility/Utility.h"

namespace Dystopia
{
	struct BehaviourMessage
	{
	protected:

		struct Concept
		{
			virtual Concept* Clone() const = 0;
			virtual ~Concept() {}
		};

		template<typename T>
		struct Wrapper
		{
			Wrapper* Clone() const override
			{
				return new Wrapper{ *this };
			}
		};

		template<typename ... T>
		struct Wrapper<Tuple<T...>>
		{
			template<typename ... Ts>
			Wrapper(Ts&& ... params)
				:mtParams{ Tuple<T...>{params...} }
			{

			}
			Wrapper* Clone() const override
			{
				return new Wrapper<Tuple<T...>>{ *this };
			}

			Tuple<T...> Get()
			{
				return mtParams;
			}

			Tuple<T...> mtParams;
		};

	public:

		template<typename ... Ts, typename SFINAE = Ut::EnableIf_t<!(Ut::IsSame<Ut::Decay_t<Ts>, BehaviourMessage>::value || ...)>>
		BehaviourMessage(Ts&& ... _FuncParams)
			:mpWrapper{new Wrapper<Tuple<Ut::Decay_t<Ts>...>>{ Ut::Decay_t<Ts>{ Ut::Forward<Ts>( _FuncParams)  ...}}}
		{

		}

		BehaviourMessage(const BehaviourMessage & _rhs)
			:mpWrapper{nullptr}
		{
			if (_rhs.mpWrapper)
				mpWrapper = _rhs.mpWrapper->Clone();
		}

		BehaviourMessage(BehaviourMessage && _rhs)
			:mpWrapper{_rhs.mpWrapper}
		{
			_rhs.mpWrapper = nullptr;
		}
		BehaviourMessage& operator=(const BehaviourMessage & _rhs)
		{
			delete mpWrapper;
			if (_rhs.mpWrapper)
				mpWrapper = _rhs.mpWrapper->Clone();
			else
				mpWrapper = nullptr;
			return *this;
		}
		BehaviourMessage& operator=(BehaviourMessage && _rhs)
		{
			Ut::Swap(mpWrapper, _rhs.mpWrapper);
			return *this;
		}
		BehaviourMessage()
		{
			delete mpWrapper;
			mpWrapper = nullptr;
		}

		template<typename Behaviour_t, typename RetType, typename ... Params>
		bool operator()(Behaviour_t & Behaviour, RetType(Behaviour_t::*_Memptr)(Params...))
		{
			if ((auto ptr = dynamic_cast<Wrapper<Tuple<Ut::Decay_t<Params>...>>>(Concept)) != nullptr)
			{
				Invoke(Behaviour, _Memptr, ptr->Get());
				return true;
			}

			return false;
		}

	private:

		template<typename Behaviour_t, typename MemPtr_t, typename ... Ts>
		void Invoke(Behaviour_t & _Behaviour, MemPtr_t _MemPtr, Tuple<Ts...> _ParamTypes)
		{
			InvokeAux(_Behaviour, _MemPtr, _ParamTypes, std::make_index_sequence<sizeof...(Ts)>{});
		}
		template<typename Behaviour_t, typename MemPtr_t, typename ... Ts, size_t ... Indices>
		void InvokeAux(Behaviour_t & _Behaviour, MemPtr_t _MemPtr, Tuple<Ts...> _ParamTypes, std::index_sequence<Indices...> _Indices)
		{
			(_Behaviour.*_MemPtr)(_ParamTypes.get<Indices>() ...);
		}


		Concept * mpWrapper = nullptr;
	};
}


#endif




