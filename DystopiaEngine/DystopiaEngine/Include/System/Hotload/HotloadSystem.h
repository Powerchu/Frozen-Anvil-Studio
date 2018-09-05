#pragma once
#ifndef HOTLOADSYSTEM_H
#define HOTLOADSYSTEM_H

#include "System/Base/Systems.h"
#include <windows.h>
#include <map>
#include <functional>
#include <utility>
namespace Dystopia
{
	struct ReferenceFunc
	{
	private:
		struct Concept
		{
			virtual ~Concept(){}
		};
		template< typename T>
		struct Model : Concept
		{
			template<typename U>
			Model(U && _rhs)
				:Obj{ std::forward<U>(_rhs) }
			{

			}

			T Obj;
		};

		Concept * ptr;
	public:

		template<typename RetType, typename ... ParamType>
		ReferenceFunc(std::function<RetType(ParamType ...)> const & _func)
			:ptr{operator::new(sizeof(_func))}

		{
			using ForwardType = std::function<RetType(ParamType ...)> const &;
			new (ptr) Model <std::forward<ForwardType &>{_func};
		}

		ReferenceFunc()
			:ptr{ nullptr }
		{

		}

		~ReferenceFunc()
		{
			operator::delete(ptr);
		}
	};

	struct HotloadSystem : public Systems
	{
		virtual void PreInit(void)	{ };
		virtual bool Init(void) { return true; };
		virtual void PostInit(void) { };

		virtual void FixedUpdate(float) { };
		virtual void Update(float);
		virtual void PostUpdate(void) { };
		virtual void Shutdown(void);

		virtual void LoadDefaults(void) { };
		virtual void LoadSettings(TextSerialiser&) { };

		virtual ~HotloadSystem(void) = default;

		ReferenceFunc GetDllFunc(LPCWSTR _dllFileName, LPCSTR _dllFuncName);
		FARPROC GetDllFuncTest(LPCWSTR _dllFileName, LPCSTR _dllFuncName);

	private : 
		

		static LPCSTR HEADER_DEFAULT_PATH;
		static LPCSTR SOURCE_DEFAULT_PATH;

		void Recompile(HANDLE const & _File_Handle);
	};
}


#endif