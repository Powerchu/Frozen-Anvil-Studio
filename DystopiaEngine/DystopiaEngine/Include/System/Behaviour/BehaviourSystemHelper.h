#pragma once
#ifndef _BEHAVIOUR_SYSTEM_HELPER_H
#define _BEHAVIOUR_SYSTEM_HELPER_H
#include <utility>
#include <functional>
#include "Math/MathLib.h"
#include "IO/TextSerialiser.h"
namespace Dystopia
{
	namespace BehaviourHelper
	{

		struct SuperSerialiseFunctor
		{
			template<typename T>
			void operator()(T, TextSerialiser&, void*)
			{
			}
			template<>
			void operator()(std::nullptr_t, TextSerialiser&, void*)
			{
				/*Failed to deduced to any known type*/
			}
			template<>
			void operator()(Math::Vec3D _v, TextSerialiser& _obj, void*)
			{
				_obj << static_cast<float>(_v.x);
				_obj << static_cast<float>(_v.y);
				_obj << static_cast<float>(_v.z);
				_obj << static_cast<float>(_v.w);
			}
			template<>
			void operator()(float _v, TextSerialiser& _obj, void*)
			{
				_obj << static_cast<float>(_v);
			}
			template<>
			void operator()(int _v, TextSerialiser& _obj, void*)
			{
				_obj << static_cast<int>(_v);
			}
			template<>
			void operator()(double _v, TextSerialiser& _obj, void*)
			{
				_obj << static_cast<double>(_v);
			}
			template<>
			void operator()(char _v, TextSerialiser& _obj, void*)
			{
				_obj << static_cast<char>(_v);
			}
			template<>
			void operator()(bool _v, TextSerialiser& _obj, void*)
			{
				_obj << static_cast<bool>(_v);
			}
			template<>
			void operator()(HashString /*_v*/, TextSerialiser& /*_obj*/, void*)
			{
				//_obj << static_cast<HashString>(_v);
			}
			template<>
			void operator()(std::string _v, TextSerialiser& _obj, void*)
			{
				_obj << static_cast<std::string>(_v);
			}
			template<>
			void operator()(const char * _v, TextSerialiser& _obj, void*)
			{
				_obj << std::string{ _v };
			}
		};
		struct SuperUnserialiseFunctor
		{
			template<typename T>
			void operator()(T, std::function<void(T, void*)>, void *, TextSerialiser &) {}

			template<>
			void operator()(float, std::function<void(float, void*)> _f, void * _addr, TextSerialiser & _obj)
			{
				float Temp = 0;
				_obj >> Temp;
				_f(Temp, _addr);
			}
			template<>
			void operator()(HashString, std::function<void(HashString, void*)> /*_f*/, void * /*_addr*/, TextSerialiser & /*_obj*/)
			{
				//HashString Temp;
				//_obj >> Temp;
				//_f(Temp, _addr);
			}
			template<>
			void operator()(int, std::function<void(int, void*)> _f, void * _addr, TextSerialiser & _obj)
			{
				int Temp = 0;
				_obj >> Temp;
				_f(Temp, _addr);
			}
			template<>
			void operator()(double, std::function<void(double, void*)> _f, void * _addr, TextSerialiser & _obj)
			{
				double Temp = 0;
				_obj >> Temp;
				_f(Temp, _addr);
			}
			template<>
			void operator()(char, std::function<void(char, void*)> _f, void * _addr, TextSerialiser & _obj)
			{
				char Temp = 0;
				_obj >> Temp;
				_f(Temp, _addr);
			}
			template<>
			void operator()(bool, std::function<void(bool, void*)> _f, void * _addr, TextSerialiser & _obj)
			{
				bool Temp = false;
				_obj >> Temp;
				_f(Temp, _addr);
			}
			template<>
			void operator()(short, std::function<void(short, void*)> _f, void * _addr, TextSerialiser & _obj)
			{
				short Temp = 0;
				_obj >> Temp;
				_f(Temp, _addr);
			}
			template<>
			void operator()(std::string, std::function<void(std::string, void*) > _f, void * _addr, TextSerialiser & _obj)
			{
				std::string Temp = 0;
				_obj >> Temp;
				_f(Temp, _addr);
			}
			template<>
			void operator()(Math::Vec3D, std::function<void(Math::Vec3D, void*)> _f, void * _addr, TextSerialiser & _obj)
			{
				float x, y, z, w;
				_obj >> x;
				_obj >> y;
				_obj >> z;
				_obj >> w;
				_f(Math::MakeVector3D(x, y, z), _addr);
			}

			void operator()(std::nullptr_t)
			{
				/*Cannot Successfully deduce type*/
			}
		};
	}
}



#endif