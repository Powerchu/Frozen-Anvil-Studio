/* HEADER *********************************************************************************/
/*!
\file	BehaviourMemberFunc.h
\author Goh Kai Jie Keith (100%)
\par    email: keith.goh\@digipen.edu
\brief
	Behaviour Member Functions

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#pragma once
#ifndef _BEHAVIOUR_MEMBER_FUNC_H
#define _BEHVAIOUR_MEMBER_FUNC_H

#include "System/Behaviour/BehaviourMessage.h"
#include "Reflection/MetaData.h"
#include "PP/PPForEach.h"
#include "PP/PPStringify.h"

namespace Dystopia
{

	/*TypeErase Member function*/
	template<typename Behaviour_t>
	struct BehaviourMemberFunc
	{
		using type = Behaviour_t;
	protected:
		struct Concept
		{
			virtual void Invoke(Behaviour_t * const _ptr, Dystopia::BehaviourMessage _Message) = 0;
			virtual Concept* Clone() const = 0;
			virtual ~Concept() {}
		};

		template<typename T>
		struct Model : Concept
		{
			virtual Concept* Clone() const
			{
				return nullptr;
			}
			virtual void Invoke(Behaviour_t * const _ptr, Dystopia::BehaviourMessage _Message)
			{

			}
		};

		template <typename Ret_t, typename ... Param_ts>
		struct Model<Ret_t(Behaviour_t::*) (Param_ts...)> : Concept
		{
			using Model_t = Model<Ret_t(Behaviour_t::*) (Param_ts...)>;
			template <typename T>
			Model(T && _rhs)
				:mObj{ _rhs }
			{

			}

			virtual void Invoke(Behaviour_t * const _ptr, Dystopia::BehaviourMessage _Message)
			{
				_Message(*_ptr, mObj);
			}

			virtual Model_t * Clone() const
			{
				return new Model_t{ *this };
			}

			Ret_t(Behaviour_t::* mObj) (Param_ts...);
		};

	public:
		template<typename Ret_t, typename ... Param_ts>
		BehaviourMemberFunc(Ret_t(Behaviour_t::* _rhs)(Param_ts...))
			:mpWrapper{ new Model<Ret_t(Behaviour_t::*)(Param_ts...)>{_rhs} }
		{

		}

		BehaviourMemberFunc(const BehaviourMemberFunc & _rhs)
			:mpWrapper{ nullptr }
		{
			if (_rhs.mpWrapper)
				mpWrapper = _rhs.mpWrapper->Clone();
		}

		BehaviourMemberFunc(BehaviourMemberFunc && _rhs)
			:mpWrapper{ _rhs.mpWrapper }
		{
			_rhs.mpWrapper = nullptr;
		}
		BehaviourMemberFunc& operator=(const BehaviourMemberFunc & _rhs)
		{
			delete mpWrapper;
			if (_rhs.mpWrapper)
				mpWrapper = _rhs.mpWrapper->Clone();
			else
				mpWrapper = nullptr;
			return *this;
		}
		BehaviourMemberFunc& operator=(BehaviourMemberFunc && _rhs)
		{
			Ut::Swap(mpWrapper, _rhs.mpWrapper);
			return *this;
		}
		~BehaviourMemberFunc()
		{
			delete mpWrapper;
			mpWrapper = nullptr;
		}

		void Invoke(Behaviour_t * const _ptr, Dystopia::BehaviourMessage _Message)
		{
			mpWrapper->Invoke(_ptr, _Message);
		}

		operator bool() const
		{
			return mpWrapper;
		}
	private:
		Concept * mpWrapper = nullptr;
	};

}



#define STRINGIFY(_NAME_) #_NAME_
#define MEMBERFUNC_AUX(_TYPE_, _MEMBER_) std::make_pair(STRINGIFY(_MEMBER_), &_TYPE_::_MEMBER_)

#define PP_MEMBERFUNC(_STRUCT_, ...)                                                                                               \
std::map<const char *, BehaviourMemberFunc<_STRUCT_>> mMemberFunc                                                                  \
{																															       \
   PP_FOREACH(MEMBERFUNC_AUX, (_STRUCT_), __VA_ARGS__)                                                                             \
};                                                                                                                                 \
	void _STRUCT_::ReceiveMessage(const char * const _FuncName, BehaviourMessage _msg)                                             \
	{																															   \
		for (auto & elem : mMemberFunc)																							   \
		{																														   \
			if (!std::strcmp(elem.first, _FuncName))																			   \
			{																													   \
				elem.second.Invoke(this, _msg);																					   \
			}																													   \
		}																														   \
	}																															   \


#define PP_MEMBERFUNC_EMPTY(_STRUCT_)                                                                                              \
std::map<const char *, BehaviourMemberFunc<_STRUCT_>> mMemberFunc;                                                                 \
	void _STRUCT_::ReceiveMessage(const char * const _FuncName, BehaviourMessage _msg)                                             \
	{																															   \
		for (auto & elem : mMemberFunc)																							   \
		{																														   \
			if (!std::strcmp(elem.first, _FuncName))																			   \
			{																													   \
				elem.second.Invoke(this, _msg);																					   \
			}																													   \
		}																														   \
	}																															   \

#endif



