#pragma once
#ifndef _EVENT_H
#define _EVENT_H

#include "DataStructure/Tuple.h"
#include "DataStructure/HashString.h"
#include <type_traits>
#include "Utility/Utility.h"
#include <tuple>


//struct EventParams
//{
//protected:
//
//	template<unsigned Index, typename T>
//	struct Params
//	{
//		using type = T;
//	};
//
//	template<typename ... T>
//	struct ParamsList;
//
//	template<typename ... Ts, size_t ... Indices>
//	struct ParamsList<std::index_sequence<Indices...>, Ts...> : Params<Indices, Ts> ...
//	{
//		
//	};
//
//	struct Concept
//	{
//		virtual Concept* Clone() const = 0;
//		virtual ~Concept() {}
//	};
//
//	template<typename T>
//	struct Wrapper : Concept
//	{
//		Wrapper* Clone() const override
//		{
//			return new Wrapper{ *this };
//		}
//	};
//
//	template<typename ... T>
//	struct Wrapper<Tuple<T...>> : Concept , ParamsList<std::make_index_sequence<sizeof...(T)>, T...>
//	{
//		template<typename ... Ts>
//		Wrapper(Ts&& ... params)
//			:mtParams{ Tuple<T...>{params...} }
//		{
//
//		}
//		Wrapper* Clone() const override
//		{
//			return new Wrapper<Tuple<T...>>{ *this };
//		}
//
//		Tuple<T...> Get()
//		{
//			return mtParams;
//		}
//
//		Tuple<T...> mtParams;
//	};
//
//	template<typename ... T>
//	struct isFirstSame;
//
//	template<typename Comp, typename T, typename ... Ts>
//	struct isFirstSame<Comp, T, Ts...>
//	{
//		static constexpr bool value = Ut::IsSame<Ut::Decay_t<T>, Comp>::value;
//	};
//
//public:
//
//	template<typename ... Ts, typename SFINAE = typename Ut::EnableIf<sizeof...(Ts) == 0 || !isFirstSame<EventParams, Ts...>::value>::type>
//	EventParams(Ts ... _FuncParams)
//		:mpWrapper{ new Wrapper<Tuple<Ut::Decay_t<Ts>...>>{ Ut::Forward<Ts>(_FuncParams)  ... } }
//	{
//
//	}
//
//	EventParams(const EventParams & _rhs)
//		:mpWrapper{ nullptr }
//	{
//		if (_rhs.mpWrapper)
//			mpWrapper = _rhs.mpWrapper->Clone();
//	}
//
//	EventParams(EventParams && _rhs)
//		:mpWrapper{ _rhs.mpWrapper }
//	{
//		_rhs.mpWrapper = nullptr;
//	}
//	EventParams& operator=(const EventParams & _rhs)
//	{
//		delete mpWrapper;
//		if (_rhs.mpWrapper)
//			mpWrapper = _rhs.mpWrapper->Clone();
//		else
//			mpWrapper = nullptr;
//		return *this;
//	}
//	EventParams& operator=(EventParams && _rhs)
//	{
//		Ut::Swap(mpWrapper, _rhs.mpWrapper);
//		return *this;
//	}
//	~EventParams()
//	{
//		delete mpWrapper;
//		mpWrapper = nullptr;
//	}
//
//	operator bool() const
//	{
//		return mpWrapper;
//	}
//
//	template<typename Behaviour_t, typename RetType, typename ... Params>
//	bool operator()(Behaviour_t & Behaviour_Obj, RetType(Behaviour_t::*_Memptr)(Params...))
//	{
//		auto * ptr = dynamic_cast<Wrapper<Tuple<Ut::Decay_t<Params>...>> *>(mpWrapper);
//		if (ptr != nullptr)
//		{
//			Invoke(Behaviour_Obj, _Memptr, ptr->Get());
//			return true;
//		}
//		return false;
//	}
//
//	template<typename RetType, typename ... Params>
//	bool operator()(RetType(*ptr)(Params...))
//	{
//		auto * ptr = dynamic_cast<Wrapper<Tuple<Ut::Decay_t<Params>...>> *>(mpWrapper);
//		if (ptr != nullptr)
//		{
//			Invoke(ptr, ptr->Get());
//			return true;
//		}
//
//		return false;
//	}
//
//	template<typename Behaviour_t, typename RetType>
//	bool operator()(Behaviour_t & Behaviour_Obj, RetType(Behaviour_t::*_Memptr)(void))
//	{
//		auto * ptr = dynamic_cast<Wrapper<Tuple<> *>>(mpWrapper);
//		if (ptr != nullptr)
//		{
//			Invoke(Behaviour_Obj, _Memptr, ptr->Get());
//			return true;
//		}
//		return false;
//	}
//private:
//
//	template<typename Behaviour_t, typename MemPtr_t, typename ... Ts>
//	void Invoke(Behaviour_t & _Behaviour, MemPtr_t _MemPtr, Tuple<Ts...> _ParamTypes)
//	{
//		InvokeAux(_Behaviour, _MemPtr, _ParamTypes, std::make_index_sequence<sizeof...(Ts)>{});
//	}
//	template<typename Behaviour_t, typename MemPtr_t, typename ... Ts, size_t ... Indices>
//	void InvokeAux(Behaviour_t & _Behaviour, MemPtr_t _MemPtr, Tuple<Ts...> _ParamTypes, std::index_sequence<Indices...> _Indices)
//	{
//		(_Behaviour.*_MemPtr)(_ParamTypes.Get<Indices>() ...);
//	}
//
//	template<typename MemPtr_t, typename ... Ts>
//	void Invoke(MemPtr_t _MemPtr, Tuple<Ts...> _ParamTypes)
//	{
//		InvokeAux(_Behaviour, _MemPtr, _ParamTypes, std::make_index_sequence<sizeof...(Ts)>{});
//	}
//	template<typename MemPtr_t, typename ... Ts, size_t ... Indices>
//	void InvokeAux(MemPtr_t _MemPtr, Tuple<Ts...> _ParamTypes, std::index_sequence<Indices...> _Indices)
//	{
//		(*_MemPtr)(_ParamTypes.Get<Indices>() ...);
//	}
//
//	Concept * mpWrapper = nullptr;
//};

struct EventParams
{
protected:

	template<unsigned Index, typename T>
	struct Params
	{
		using type = T;

		template<typename T>
		operator T()
		{
			
		}
	};

	template<typename ... T>
	struct ParamsList;

	template<typename ... Ts, size_t ... Indices>
	struct ParamsList<std::index_sequence<Indices...>, Ts...> : Params<Indices, Ts> ...
	{

	};

	struct Concept
	{
		virtual Concept* Clone() const = 0;
		virtual ~Concept() {}
	};

	template<typename T>
	struct Wrapper : Concept
	{
		Wrapper* Clone() const override
		{
			return new Wrapper{ *this };
		}
	};

	template<typename ... T>
	struct Wrapper<std::tuple<T...>> : Concept, ParamsList<std::make_index_sequence<sizeof...(T)>, T...>
	{
		template<typename ... Ts>
		Wrapper(Ts&& ... params)
			:mtParams{ std::tuple<T...>{params...} }
		{

		}
		Wrapper* Clone() const override
		{
			return new Wrapper<std::tuple<T...>>{ *this };
		}

		std::tuple<T...> Get()
		{
			return mtParams;
		}

		std::tuple<T...> mtParams;
	};


	template<typename ... T>
	struct isFirstSame;

	template<typename Comp, typename T, typename ... Ts>
	struct isFirstSame<Comp, T, Ts...>
	{
		static constexpr bool value = Ut::IsSame<Ut::Decay_t<T>, Comp>::value;
	};

public:

	template<typename ... Ts, typename SFINAE = typename Ut::EnableIf<!isFirstSame<EventParams, Ts...>::value>::type>
	EventParams(Ts ... _FuncParams)
		:mpWrapper{ new Wrapper<std::tuple<Ut::Decay_t<Ts>...>>{ Ut::Forward<Ts>(_FuncParams)  ... } }
	{

	}

	EventParams()
		: mpWrapper{ new Wrapper<std::tuple<>>{} }
	{

	}

	EventParams(const EventParams & _rhs)
		:mpWrapper{ nullptr }
	{
		if (_rhs.mpWrapper)
			mpWrapper = _rhs.mpWrapper->Clone();
	}

	EventParams(EventParams && _rhs)
		:mpWrapper{ _rhs.mpWrapper }
	{
		_rhs.mpWrapper = nullptr;
	}
	EventParams& operator=(const EventParams & _rhs)
	{
		delete mpWrapper;
		if (_rhs.mpWrapper)
			mpWrapper = _rhs.mpWrapper->Clone();
		else
			mpWrapper = nullptr;
		return *this;
	}
	EventParams& operator=(EventParams && _rhs)
	{
		Ut::Swap(mpWrapper, _rhs.mpWrapper);
		return *this;
	}
	~EventParams()
	{
		delete mpWrapper;
		mpWrapper = nullptr;
	}

	operator bool() const
	{
		return mpWrapper;
	}

	template<typename Behaviour_t, typename RetType, typename ... Params>
	bool operator()(Behaviour_t & Behaviour_Obj, RetType(Behaviour_t::*_Memptr)(Params...))
	{
		auto * ptr = dynamic_cast<Wrapper<std::tuple<Ut::Decay_t<Params>...>> *>(mpWrapper);
		if (ptr != nullptr)
		{
			Invoke(Behaviour_Obj, _Memptr, ptr->Get());
			return true;
		}
		return false;
	}

	template<typename RetType, typename ... Params>
	bool operator()(RetType(*ptr)(Params...))
	{
		auto * ptr2 = dynamic_cast<Wrapper<std::tuple<Ut::Decay_t<Params>...>> *>(mpWrapper);

		if (ptr2 != nullptr)
		{
			Invoke(ptr, ptr2->Get());
			return true;
		}

		return false;
	}

	template<typename Behaviour_t, typename RetType>
	bool operator()(Behaviour_t & Behaviour_Obj, RetType(Behaviour_t::*_Memptr)(void))
	{
		auto * ptr = dynamic_cast<Wrapper<std::tuple<>> *>(mpWrapper);
		if (ptr != nullptr)
		{
			Invoke(Behaviour_Obj, _Memptr, ptr->Get());
			return true;
		}
		return false;
	}

	template<typename RetType>
	bool operator()(RetType(*ptr)(void))
	{
		auto * ptr2 = dynamic_cast<Wrapper<std::tuple<>> *>(mpWrapper);

		if (ptr2 != nullptr)
		{
			Invoke(ptr, ptr2->Get());
			return true;
		}

		return false;
	}

private:

	template<typename Behaviour_t, typename MemPtr_t, typename ... Ts>
	void Invoke(Behaviour_t & _Behaviour, MemPtr_t _MemPtr, std::tuple<Ts...> _ParamTypes)
	{
		InvokeAux(_Behaviour, _MemPtr, _ParamTypes, std::make_index_sequence<sizeof...(Ts)>{});
	}
	template<typename Behaviour_t, typename MemPtr_t, typename ... Ts, size_t ... Indices>
	void InvokeAux(Behaviour_t & _Behaviour, MemPtr_t _MemPtr, std::tuple<Ts...> _ParamTypes, std::index_sequence<Indices...>)
	{
		UNUSED_PARAMETER(_ParamTypes);
		(_Behaviour.*_MemPtr)(std::get<Indices>(_ParamTypes) ...);
	}

	template<typename MemPtr_t, typename ... Ts>
	void Invoke(MemPtr_t _MemPtr, std::tuple<Ts...> _ParamTypes)
	{
		InvokeAux(_MemPtr, _ParamTypes, std::make_index_sequence<sizeof...(Ts)>{});
	}
	template<typename MemPtr_t, typename ... Ts, size_t ... Indices>
	void InvokeAux(MemPtr_t _MemPtr, std::tuple<Ts...> _ParamTypes, std::index_sequence<Indices...>)
	{
		UNUSED_PARAMETER(_ParamTypes);
		(*_MemPtr)(std::get<Indices>(_ParamTypes) ...);
	}

	Concept * mpWrapper = nullptr;
};

struct CallBackEvent
{
protected:

	struct Concept
	{
		virtual Concept const * Get() const = 0;
		virtual Concept * Get() = 0;
		virtual Concept* Clone() const = 0;
		virtual size_t GetParamSize() const = 0;
		virtual ~Concept(){}
		virtual Concept* Convert(void *) const = 0;

		virtual bool Fire(EventParams _msg) = 0;
		template<typename T, typename ... Params>
		void Fire(T * ptr, Params ... _p)
		{
			ptr->Fire(_p...);
		}
	};

	template<typename T>
	struct Model;

	template<typename RetType, typename ... Params_t>
	struct Model<RetType(*)(Params_t...)> : Concept
	{

		template<typename ... Params>
		Model(RetType(*_MemFunc)(Params_t ...))
			:mFnc(_MemFunc)
		{

		}
		virtual Model<RetType(*)(Params_t...)>* Convert(void * ptr) const override
		{
			return reinterpret_cast<Model<RetType(*)(Params_t...)> *>(ptr);
		}
		virtual Model<RetType(*)(Params_t...)> const * Get() const
		{
			return this;
		}

		virtual Model<RetType(*)(Params_t...)> * Get()
		{
			return this;
		}
		virtual Model* Clone() const
		{
			return new Model<RetType(*)(Params_t...)>{*this};
		}
		virtual size_t GetParamSize() const
		{
			return sizeof...(Params_t);
		}
		template<typename ... Ts>
		RetType Fire(Ts ... _params) const
		{
			(*mFnc)(_params...);
		}


		virtual bool Fire(EventParams _msg)
		{
			return _msg(mFnc);
		}

		constexpr operator void *()
		{
			return *this;
		}

		RetType(*mFnc)(Params_t...);
	};

	template<typename RetType, typename Class_t, typename ... Params_t>
	struct Model<RetType(Class_t::*)(Params_t...)> : Concept
	{

		Model() {};

		Model(Class_t* _ptr, RetType(Class_t::* _MemFunc)(Params_t ...))
			:mFnc(_MemFunc), mpClass_Obj{ _ptr } //, mParams{}
		{
			
		}

		virtual bool Fire(EventParams _msg)
		{
			return _msg(*mpClass_Obj, mFnc);
		}
		virtual Model<RetType(Class_t::*)(Params_t...)> const * Get() const
		{
			return this;
		}
		virtual Model<RetType(Class_t::*)(Params_t...)>  * Get() 
		{
			return this;
		}
		virtual Model<RetType(Class_t::*)(Params_t...)>* Convert(void * ptr) const override
		{
			return reinterpret_cast<Model<RetType(Class_t::*)(Params_t...)> *>(ptr);
		}

		virtual Model* Clone() const override
		{
			return new Model<RetType(Class_t::*)(Params_t...)>{ *this };
		}

		virtual size_t GetParamSize() const
		{
			return sizeof...(Params_t);
		}

		operator void *()
		{
			return *this;
		}

		RetType(Class_t::*mFnc)(Params_t...);
		Class_t*           mpClass_Obj;

	};

public:

	template<typename Class_t, typename RetType, typename ... Params_t>
	CallBackEvent(Class_t* _pClass, RetType(Class_t::* _MemFunc)(Params_t ... ))
		:mpWrapper{new Model<RetType(Class_t::*)(Params_t ...)>{_pClass, _MemFunc}}, mpGenericComparison{static_cast<void*>(_pClass)}
	{
		
	}

	template<typename RetType, typename ... Params_t>
	CallBackEvent(RetType(* _MemFunc)(Params_t ...))
		: mpWrapper{ new Model<RetType(*)(Params_t ...)>{_MemFunc} }
	{

	}

	template<typename Class_t, typename RetType, typename ... Params_t>
	CallBackEvent(HashString _mFuncName, Class_t* _pClass, RetType(Class_t::* _MemFunc)(Params_t ...))
		: mpWrapper{ new Model<RetType(Class_t::*)(Params_t ...)>{ _pClass, _MemFunc } }, mpGenericComparison{ static_cast<void*>(_pClass) }, mName{ _mFuncName }
	{

	}

	template<typename RetType, typename ... Params_t>
	CallBackEvent(HashString _mFuncName, RetType(*_MemFunc)(Params_t ...))
		: mpWrapper{ new Model<RetType(*)(Params_t ...)>{ _MemFunc } }, mName{ _mFuncName }
	{

	}

	template<typename ... Param_Ts>
	bool Fire(Param_Ts&& ... _param)
	{
		if(mpWrapper)
		{
			if(mpWrapper->GetParamSize() == sizeof...(Param_Ts))
			{
				return mpWrapper->Fire(EventParams{ _param ...});
			}
		}

		return false;
	}


	bool Fire(EventParams _param)
	{
		if (mpWrapper)
		{
			return mpWrapper->Fire(_param);

		}
		return false;
	}
	template<typename ... Param_Ts>
	bool operator()(Param_Ts &&... _param)
	{
		if (mpWrapper)
		{
			if (mpWrapper->GetParamSize() == sizeof...(Param_Ts))
			{
				return mpWrapper->Fire(EventParams{ _param ... });
			}
		}

		return false;
	}

	bool operator()(EventParams _param)
	{
		if (mpWrapper)
		{
			return mpWrapper->Fire(_param );
		}

		return false;
	}

	bool isAnonymous() const
	{
		return !*mName.c_str();
	}

	CallBackEvent(const CallBackEvent & _rhs)
		:mpWrapper{ nullptr }
	{
		if (_rhs.mpWrapper)
			mpWrapper = _rhs.mpWrapper->Clone();
	}

	bool operator==(HashString const & _name)
	{
		return _name == mName;
	}

	bool operator==(std::string const & _name)
	{
		return !std::strcmp(_name.c_str(), mName.c_str());
	}

	bool operator==(const char * const _name)
	{
		return !std::strcmp(_name, mName.c_str());
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

	HashString mName;
	Concept *  mpWrapper = nullptr;
	void*      mpGenericComparison = nullptr;

private:

};



#endif




