/* HEADER *********************************************************************************/
/*!
\file	Inspector.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_META_HELPER_H_
#define _EDITOR_META_HELPER_H_

#include "Component/ColliderList.h"
#include "Component/Component.h"

#include "System/Base/ComponentDonor.h"
#include "System/Driver/Driver.h"

#include "DataStructure/Array.h"
#include "Object/GameObject.h"

#include "Utility/Utility.h"
#include "Utility/MetaAlgorithms.h"
#include "Editor/EGUI.h"
#include <functional>
#include "DataStructure/HashString.h"

namespace Dystopia
{
	template<typename A, typename B>
	struct ListOfComponentsName;
	template <class List, size_t ... Ns>
	struct ListOfComponentsName<std::index_sequence<Ns ...>, List>
	{
		template<size_t N, typename T, typename ... Ts>
		static inline void AuxEx(Array<std::string, sizeof...(Ns)>& arr, T&& t, Ts&& ... ts)
		{
			arr[N] = t;
			if constexpr (N != sizeof...(Ns) - 1)
				AuxEx<N + 1>(arr, ts...);
		}
		template<size_t Num>
		static void Extract(Array<std::string, Num>& arr)
		{
			AuxEx<0>(arr, Ut::MetaExtract<Ns, List>::result::type::GetCompileName() ...);
		}
	};

	template <typename C, typename SFINAE = std::enable_if_t<!std::is_same_v<std::decay_t<C>, Component>, C>>
	struct AuxGenFunction
	{
		static Component* Extract(GameObject *_owner)
		{
			C* comp = static_cast<ComponentDonor<C>*>(EngineCore::GetInstance()->Get<typename C::SYSTEM>())->RequestComponent();
			comp->SetOwner(_owner);
			comp->SetActive(_owner->IsActive());
			comp->Init();
			return comp;
		}
	};

	struct ListOfComponents
	{
		static constexpr size_t size = Ut::SizeofList<UsableComponents>::value;

		template<typename A>
		struct GenerateCollection;

		template<size_t ... Ns>
		struct GenerateCollection<std::index_sequence<Ns ...>>
		{

			/*
			template <typename T>
			struct GetType
			{
				using type = T;
			};

			using tupleType = std::tuple
			<
				typename GetType<typename Ut::MetaExtract<Ns, UsableComponents>::result::type * (&)(void) >::type ...    //::result::type * (&)(void)
			>;

			struct ApplyFunction
			{
				template<typename T>
				T* operator()(T*(&_fn)(void))
				{
					return _fn();
				}
			};

			template<typename A, typename List>
			struct BreakTuple;
			template<typename List, size_t Head, size_t ... Rest>
			struct BreakTuple<std::index_sequence<Head, Rest ...>, List>
			{
				std::tuple< typename GetType<typename Ut::MetaExtract_t<Rest, List>::type* (&)(void)>::type ...> mData
					= { AuxGenFunction<typename Ut::MetaExtract_t<Rest, List>::type>::Extract ... };
			};
			template<typename List>
			struct BreakTuple<std::index_sequence<0>, List>
			{
				std::tuple<typename Ut::MetaExtract<size - 1, UsableComponents>::result::type* (&)(void)> mData
					= { AuxGenFunction<typename Ut::MetaExtract<size - 1, UsableComponents>::result::type>::Extract  };
			};


			template<typename A>
			struct Helper;
			template<size_t Head, size_t ... Rest>
			struct Helper<std::index_sequence<Head, Rest ...>>
			{
				template <typename List, typename... Ts>
				void* HelperFunction(unsigned int _i, std::tuple<Ts ...>& _data)
				{
					BreakTuple<Ut::MetaPopFront_t<List>> newData;
					if (!_i)
					{
						ApplyFunction a;
						return a(std::get<0>(_data));
					}
					return HelperFunction<Ut::MetaPopFront_t<List>>(_i - 1, newData.mData);
				}
			};
			*/

			Component* Get(unsigned int _i, GameObject *_owner)
			{
				static auto mData = Ctor::MakeArray<Component*(*)(GameObject *)>(AuxGenFunction<typename Ut::MetaExtract<Ns, UsableComponents>::result::type>::Extract
					... );
				if (_i < size || _i >= 0) 
					return mData[_i](_owner);
				return nullptr;
			}
		};

		Component* Get(unsigned int _i, GameObject *_owner)
		{
			return mCollection.Get(_i, _owner);
		}

		GenerateCollection<std::make_index_sequence<size>> mCollection;
	};

	struct SuperReflectFunctor
	{
		template<typename T, typename Settor>
		void operator()(const char *, T, Settor, void*) {  }

		/*
		 * Drag-field for Float Values (default)
		 */
		template<>
		void operator()( const char * _name, float value, std::function<void(float, void*)> _f, void* _addr)
		{
			float Temp = value;
			switch (EGUI::Display::DragFloat(_name, &Temp, 0.01f, -FLT_MAX, FLT_MAX))
			{
			case EGUI::eDragStatus::eDRAGGING:
				_f(Temp, _addr);
				break;
			case EGUI::eDragStatus::eSTART_DRAG:
				break;
			case EGUI::eDragStatus::eNO_CHANGE:
				break;
			case EGUI::eDragStatus::eEND_DRAG:
			case EGUI::eDragStatus::eENTER:
			case EGUI::eDragStatus::eDEACTIVATED:
			case EGUI::eDragStatus::eTABBED:
				EGUI::GetCommandHND()->EndRecording();
				break;
			default:
				break;
			}
		}

		/*
		 * Text-Field (std::string)
		 */
		template<>
		void operator()(const char * _name, std::string value, std::function<void(std::string, void*)> _f, void* _addr)
		{
			char buffer[1024];
			if (EGUI::Display::TextField(_name, buffer, 1024))
			{
				_f(buffer, _addr);
			}
		}

		/*
		 * Text-Field (HashString)
		 */
		template<>
		void operator()(const char * _name, HashString value, std::function<void(HashString, void*)> _f, void*_addr)
		{
			char buffer[1024];
			if (EGUI::Display::TextField(_name, buffer, 1024))
			{
				value = buffer;
				_f(value, _addr);
			}
		}

		/*
		 * Int Field 
		 */
		template<>
		void operator()(const char * _name, int value, std::function<void(int, void*)> _f, void*_addr)
		{
			int Temp = value;
			switch (EGUI::Display::DragInt(_name, &Temp, 0.1f, INT16_MIN, INT16_MAX))
			{
			case EGUI::eDragStatus::eDRAGGING:
				_f(Temp, _addr);
				break;
			case EGUI::eDragStatus::eSTART_DRAG:
				//Is it possible to make one that does not require the BehaviourClass to be known
				//EGUI::GetCommandHND()->StartRecording<RigidBody>(mnOwner, &mfAngularDrag);
				//EGUI::GetCommandHND()->StartRecording(&Temp);
			case EGUI::eDragStatus::eNO_CHANGE:
				break;
			case EGUI::eDragStatus::eEND_DRAG:
			case EGUI::eDragStatus::eENTER:
			case EGUI::eDragStatus::eDEACTIVATED:
			case EGUI::eDragStatus::eTABBED:
				EGUI::GetCommandHND()->EndRecording();
				break;
			default:
				break;
			}
		}
		template<>
		void operator()(const char *, double, std::function<void(double, void*)> , void*)
		{

		}

		/*
		 * Bool Checkbox
		 */
		template<>
		void operator()(const char * _name, bool _val, std::function<void(bool, void*)> _f, void* _addr)
		{
			bool toggleState = _val;
			if (EGUI::Display::CheckBox(_name, &toggleState))
			{
				_f(toggleState, _addr);
			}
		}
	};
	struct SuperGetFunctor
	{
		template<typename T>
		void operator()(T value) {  }

		template<>
		void operator()(float)
		{

		}
		template<>
		void operator()(int)
		{

		}	
	};
	struct SuperSetFunctor
	{
		template<typename T>
		void operator()(T _value, void *)
		{
			
		}

		template<>
		void operator()(std::function<void(float, void*)> _f, void * _addr)
		{

			float Temp;
			switch (EGUI::Display::DragFloat("Test", &Temp, 0.01f, 0.0f, 2.0f))
			{
			case EGUI::eDragStatus::eEND_DRAG:
				EGUI::GetCommandHND()->EndRecording();
				break;
			case EGUI::eDragStatus::eENTER:
				EGUI::GetCommandHND()->EndRecording();
				_f(Temp, _addr);
				break;
			case EGUI::eDragStatus::eDRAGGING:
				break;
			case EGUI::eDragStatus::eSTART_DRAG:
				break;
			case EGUI::eDragStatus::eDEACTIVATED:
				EGUI::GetCommandHND()->EndRecording();
				break;
			case EGUI::eDragStatus::eNO_CHANGE:
				_f(0, _addr);
				break;
			case EGUI::eDragStatus::eTABBED:
				EGUI::GetCommandHND()->EndRecording();
				break;
			default:
				break;
			}
		}

	};
}

#endif //_AUX_META_HELPER_H_
#endif //EDITOR

