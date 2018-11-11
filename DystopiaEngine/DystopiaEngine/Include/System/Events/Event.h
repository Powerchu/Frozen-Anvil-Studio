#pragma once
#ifndef _EVENT_H
#define _EVENT_H

#include "DataStructure/Tuple.h"
#include <type_traits>
#include "Utility/Utility.h"
struct CallBackEvent
{
protected:

	struct Concept
	{
		virtual void Invoke() = 0;
		virtual Concept* Clone() const = 0;
		virtual size_t GetParamSize() const = 0;
		virtual ~Concept(){}
	};

	template<typename T>
	struct Model : Concept
	{
		virtual void Invoke()
		{
			
		}
		virtual Model* Clone() const
		{
			return nullptr;
		}
	};

	template<typename RetType, typename ... Params_t>
	struct Model<RetType (*)(Params_t...)>  : Concept
	{
		using WrapType = RetType(*)(Params_t...);
		using type     = Model<RetType(*)(Params_t...)>;

		template<typename ... Params>
		Model(RetType(* _MemFunc)(Params_t ...), Params ... _params)
			:mParams{ _params... }, mFnc(_MemFunc)
		{

		}

		virtual Model* Clone() const
		{
			return new type{*this};
		}
		virtual size_t GetParamSize() const
		{
			return sizeof...(Params_t);
		}
		virtual void Invoke() override
		{
			Invoke(std::make_index_sequence<sizeof...(Params_t)>());
		}

		template<size_t ... Indices>
		void Invoke(std::index_sequence<Indices>)
		{
			mFnc(mParams.Get<Indices>()...);
		}

		Tuple<Params_t...> mParams;
		WrapType mFnc;
	};

	template<typename RetType, typename Class_t, typename ... Params_t>
	struct Model<RetType(Class_t::*)(Params_t...)> : Concept
	{
		using WrapType = RetType(*)(Params_t...);
		using type = Model<RetType(Class_t::*)(Params_t...)>;

		Model(Class_t* _ptr, RetType(Class_t::* _MemFunc)(Params_t ...), Params_t ... _params)
			:mParams{ _params... },mFnc(_MemFunc),mpClass_Obj{_ptr}
		{
			
		}
		virtual Model* Clone() const override
		{
			return new type{ *this };
		}

		virtual size_t GetParamSize() const
		{
			return sizeof...(Params_t);
		}
		virtual void Invoke() override
		{
			Invoke(std::make_index_sequence<sizeof...(Params_t)>());
		}

		template<size_t ... Indices>
		void Invoke(std::index_sequence<Indices>)
		{
			(mpClass_Obj->*mFnc)(mParams.Get<Indices>()...);
		}

		template<typename T, typename Convert, bool _b>
		struct ResolveConversionAux
		{
			using type = T;
		};
		template<typename T, typename Convert>
		struct ResolveConversionAux<T , Convert, true>
		{
			using type = Convert;
		};
		template<typename T, typename Convert>
		struct ResolveConversion
		{
			using type = typename ResolveConversionAux<T, Convert, std::is_convertible<T, Convert>::value>::type;
		};

		template<typename ... Ts>
		operator Tuple<Ts...>()
		{

		}

		Tuple<Ut::Decay_t<Params_t>...> mParams;
		WrapType           mFnc;
		Class_t*           mpClass_Obj;

	};

public:

	template<typename Class_t, typename RetType, typename ... Params_t>
	CallBackEvent(Class_t* _pClass, RetType(Class_t::* _MemFunc)(Params_t ... ), Params_t ... _params)
		:mpWrapper{new Model<RetType(Class_t::*)(Params_t ...)>{_pClass, _MemFunc,_params...}}, mpGenericComparison{static_cast<void*>(_pClass)}
	{
		
	}
	template<typename RetType, typename ... Params_t>
	CallBackEvent(RetType(* _MemFunc)(Params_t ...), Params_t ... _params)
		: mpWrapper{ new Model<RetType(*)(Params_t ...)>{_MemFunc,_params } }
	{

	}
	void Invoke()
	{
		if (mpWrapper)
			mpWrapper->Invoke();
	}

	template<typename ... Param_Ts>
	void Fire(Param_Ts&& ... _param)
	{
		if(mpWrapper)
		{
			if(mpWrapper->GetParamSize() == sizeof...(Param_Ts))
			{
				
			}
		}
	}
	CallBackEvent(const CallBackEvent & _rhs)
		:mpWrapper{ nullptr }
	{
		if (_rhs.mpWrapper)
			mpWrapper = _rhs.mpWrapper->Clone();
	}

	CallBackEvent(CallBackEvent && _rhs)
		:mpWrapper{ _rhs.mpWrapper }
	{
		_rhs.mpWrapper = nullptr;
	}
	CallBackEvent& operator=(const CallBackEvent & _rhs)
	{
		delete mpWrapper;
		if (_rhs.mpWrapper)
			mpWrapper = _rhs.mpWrapper->Clone();
		else
			mpWrapper = nullptr;
		return *this;
	}
	CallBackEvent& operator=(CallBackEvent && _rhs)
	{
		Ut::Swap(mpWrapper, _rhs.mpWrapper);
		return *this;
	}

	template<class Caller>
	bool IsBindedTo(Caller * const _toCheckAgainst)
	{
		void* const toGeneric = _toCheckAgainst;
		return (toGeneric == mpGenericComparison);
	}

	~CallBackEvent()
	{
		delete mpWrapper;
		mpWrapper = nullptr;
	}

	Concept * mpWrapper = nullptr;
	void*     mpGenericComparison = nullptr;
private:

};



#endif




