#pragma once
#ifndef _EVENT_H
#define _EVENT_H

#include "DataStructure/Tuple.h"
#include "System/Behaviour/BehaviourMessage.h"
#include <type_traits>
#include "Utility/Utility.h"
#include <tuple>
struct CallBackEvent
{
protected:

	struct Concept
	{
		virtual Concept const * Get() const = 0;
		virtual Concept * Get() = 0;
		virtual void Invoke() = 0;
		virtual Concept* Clone() const = 0;
		virtual size_t GetParamSize() const = 0;
		virtual ~Concept(){}
		virtual Concept* Convert(void *) const = 0;

		virtual void Fire(Dystopia::BehaviourMessage _msg);
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
		//using WrapType = RetType(*)(Params_t...);
		//using type     = Model<RetType(*)(Params_t...)>;

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
		virtual void Invoke() override
		{
			//Invoke(std::make_index_sequence<sizeof...(Params_t)>());
		}

		template<typename ... Ts>
		RetType Fire(Ts ... _params) const
		{
			(*mFnc)(_params...);
		}

		template<size_t ... Indices>
		void Invoke(std::index_sequence<Indices>)
		{
			//mFnc(mParams.Get<Indices>()...);
		}

		template<typename ... Ts>
		constexpr operator Tuple<Ts...>()
		{
			if ((std::is_convertible<Ut::Decay_t<Ts>, Params_t>::value  && ...))
			{
				return Tuple<Params_t...>{};
			}
			return Tuple<Ts...>{};
		}

		template<typename ... Ts>
		constexpr operator Tuple<Ts...> *()
		{
			if ((std::is_convertible<Ut::Decay_t<Ts>, Params_t>::value  && ...))
			{
				return reinterpret_cast<Tuple<Params_t ...> *>(reinterpret_cast<void*>(this));
			}
			return nullptr;
		}

		template<typename ... Ts>
		constexpr operator std::tuple<Ts...> *()
		{
			if ((std::is_convertible<Ut::Decay_t<Ts>, Params_t>::value  && ...))
			{
				return reinterpret_cast<std::tuple<Params_t ...> *>(reinterpret_cast<void*>(this));
			}
			return nullptr;
		}

		template<typename ... Ts>
		constexpr operator Model<RetType(*)(Ts ...)>()
		{
			if ((std::is_convertible<Ut::Decay_t<Ts>, Params_t>::value  && ...))
			{
				return Model<RetType(*)(Params_t ...)>{};
			}
			return Model<RetType(*)(Ts ...)>{};
		}

		virtual void Fire(Dystopia::BehaviourMessage _msg)
		{
			_msg.Invoke(mFnc);
		}

		constexpr operator void *()
		{
			return *this;
		}

		//Tuple<Params_t...> mParams;
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

		virtual void Fire(Dystopia::BehaviourMessage _msg)
		{
			_msg(*mpClass_Obj, mFnc);
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
		virtual void Invoke() override
		{
			//Invoke(std::make_index_sequence<sizeof...(Params_t)>());
		}

		template<typename ... Ts>
		RetType Fire(Ts ... _params) const
		{
			if (((std::is_same<Ut::Decay_t<Ts>, Params_t>::value ||std::is_convertible<Ut::Decay_t<Ts>, Params_t>::value ) && ...))
			{
				(mpClass_Obj->*mFnc)(_params...);
			}
			
		}
		template<size_t ... Indices>
		void Invoke(std::index_sequence<Indices>)
		{
			//(mpClass_Obj->*mFnc)(mParams.Get<Indices>()...);
		}


		template<typename ... Ts>
		operator Tuple<Ts...>() const
		{
			if((std::is_convertible<Ut::Decay_t<Ts>, Params_t>::value  && ...))
			{
				return Tuple<Params_t...>{};
			}
			return Tuple<Ts...>{};
		}

		template<typename ... Ts>
		operator Tuple<Ts...> *() const
		{
			if ((std::is_convertible<Ut::Decay_t<Ts>, Params_t>::value  && ...))
			{
				return reinterpret_cast<Tuple<Params_t ...>*>(reinterpret_cast<void*>(this));
			}
			return nullptr;
		}
		template<typename ... Ts>
		operator std::tuple<Ts...> *() const
		{
			if ((std::is_convertible<Ut::Decay_t<Ts>, Params_t>::value  && ...))
			{
				return reinterpret_cast<std::tuple<Params_t ...>*>(reinterpret_cast<void*>(this));
			}
			return nullptr;
		}

		operator void *()
		{
			return *this;
		}

		template<typename ... Ts>
		operator Model<RetType(Class_t::*)(Ts ...)>()
		{
			if ((std::is_convertible<Ut::Decay_t<Ts>, Params_t>::value  && ...))
			{
				return Model<RetType(Class_t::*)(Params_t ...)>{};
			}
			return Model<RetType(Class_t::*)(Ts ...)>{};
		}

		//Tuple<Ut::Decay_t<Params_t>...> mParams;
		RetType(Class_t::*mFnc)(Params_t...);
		Class_t*           mpClass_Obj;

	};

public:

	template<typename Class_t, typename RetType, typename ... Params_t>
	CallBackEvent(Class_t* _pClass, RetType(Class_t::* _MemFunc)(Params_t ... ))
		:mpWrapper{new Model<RetType(Class_t::*)(Params_t ...)>{_pClass, _MemFunc}}, mpGenericComparison{static_cast<void*>(_pClass)}
	{
		
	}

	//template<typename Class_t, typename RetType, typename ... Params_t>
	//CallBackEvent(Class_t* _pClass, RetType(Class_t::* _MemFunc)(Params_t ...))
	//	: mpWrapper{ new Model<Class_t>{} }, mpGenericComparison{ static_cast<void*>(_pClass) }
	//{

	//}

	template<typename RetType, typename ... Params_t>
	CallBackEvent(RetType(* _MemFunc)(Params_t ...))
		: mpWrapper{ new Model<RetType(*)(Params_t ...)>{_MemFunc} }
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
				auto ptr = mpWrapper->Get();
				auto ptr2 = mpWrapper->Convert(ptr);
				//ptr->Fire(ptr,_param...);
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




