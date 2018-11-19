/* HEADER *********************************************************************************/
/*!
\file	RuntimeMeta.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _RUNTIME_META_H_
#define _RUNTIME_META_H_

#include <functional>
#include <string>

#include "Editor/EGUI.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"

#include "DataStructure/Array.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Utility/Utility.h"
#include "Utility/MetaAlgorithms.h"
#include "Utility/ComponentGUID.h"

#include "Component/Component.h"
#include "Component/ComponentList.h"
#include "Component/BehaviourList.h"

#include "Component/AABB.h"
#include "Component/AudioSource.h"
#include "Component/Camera.h"
#include "Component/Circle.h"
#include "Component/Convex.h"
#include "Component/Collider.h"
#include "Component/CharacterController.h"
#include "Component/Renderer.h"
#include "Component/RigidBody.h"
#include "Component/SpriteRenderer.h"
#include "Component/Transform.h"
#include "Component/TextRenderer.h"

#include "IO/TextSerialiser.h"

#include "System/SystemTypes.h"
#include "System/Base/ComponentDonor.h"
#include "System/Driver/Driver.h"

#include "System/Sound/SoundSystem.h"
#include "System/Input/InputSystem.h"
#include "System/Camera/CameraSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Collision/CollisionSystem.h"

namespace Dystopia
{
	template<typename A, typename B>
	struct MakeArrayOfNames;
	template<class MetaList, size_t ... Ns>
	struct MakeArrayOfNames<std::index_sequence<Ns ...>, MetaList>
	{
		template<size_t N, typename T, typename ... Ts>
		static inline void GetElement(Array<std::string, sizeof...(Ns)>& _outArray, T&& t, Ts&& ... ts)
		{
			_outArray[N] = t;
			if constexpr (N != sizeof...(Ns) - 1)
				GetElement<N + 1>(_outArray, ts ...);
		}
		template<size_t N>
		static void Make(Array<std::string, N>& _outArray)
		{
			GetElement<0>(_outArray, Ut::MetaExtract<Ns, MetaList>::result::type::GetCompileName() ...);
		}

	};

	template <typename C, typename SFINAE = std::enable_if_t<!std::is_same_v<std::decay_t<C>, Component>, C>>
	struct RequestComponent
	{
		static Component* Get(GameObject* _pRequester)
		{
			C* pComponent = static_cast<ComponentDonor<C>*>(EngineCore::GetInstance()->Get<typename C::SYSTEM>())->RequestComponent(); 
			::Editor::EditorMain::GetInstance()->GetSystem<::Editor::EditorCommands>()->AddComponent<C>(_pRequester->GetID(), pComponent);
			//pComponent->SetOwner(_pRequester);
			//pComponent->SetActive(_pRequester->IsActive());
			//pComponent->Awake();
			return pComponent;
		}
		static Component* GetA(GameObject* _pRequester)
		{
			C* pComponent = static_cast<ComponentDonor<C>*>(EngineCore::GetInstance()->Get<typename C::SYSTEM>())->RequestComponent();
			//::Editor::EditorMain::GetInstance()->GetSystem<::Editor::EditorCommands>()->AddComponent<C>(_pRequester->GetID(), pComponent);
			pComponent->SetOwner(_pRequester);
			pComponent->SetActive(_pRequester->IsActive());
			pComponent->Awake();
			return pComponent;
		}
	};

	struct ComponentList
	{
		static constexpr size_t size = Ut::SizeofList<UsableComponents>::value;

		template<typename A>
		struct Collection;
		template<size_t ... Ns>
		struct Collection<std::index_sequence<Ns ...>>
		{
			Component* Get(unsigned int _i, GameObject *_owner)
			{
				static auto mData = Ctor::MakeArray<Component*(*)(GameObject *)>(RequestComponent<typename Ut::MetaExtract<Ns, UsableComponents>::result::type>::Get...);
				if (_i < size || _i >= 0)
					return mData[_i](_owner);
				return nullptr;
			}
			Component* GetA(unsigned int _i, GameObject *_owner)
			{
				static auto mData = Ctor::MakeArray<Component*(*)(GameObject *)>(RequestComponent<typename Ut::MetaExtract<Ns, UsableComponents>::result::type>::GetA...);
				if (_i < size || _i >= 0)
					return mData[_i](_owner);
				return nullptr;
			}
		};

		Component* GetComponent(unsigned int _i, GameObject *_owner)
		{
			return mCollection.Get(_i, _owner);
		}
		Component* GetComponentA(unsigned int _i, GameObject *_owner)
		{
			return mCollection.GetA(_i, _owner);
		}

		Collection<std::make_index_sequence<size>> mCollection;
	};

	template<typename T>
	struct SystemList;

	template<size_t ... Ns>
	struct SystemList<std::index_sequence<Ns...>>
	{
		template<class C>
		struct SystemFunction
		{
			static void Serialise(TextSerialiser& _out)
			{
				static_cast<ComponentDonor<C>*>(EngineCore::GetInstance()->Get<typename C::SYSTEM>())->Serialise(_out);
			}

			static void Unserialise(TextSerialiser& _in)
			{
				static_cast<ComponentDonor<C>*>(EngineCore::GetInstance()->Get<typename C::SYSTEM>())->Unserialise(_in);
			}
		};

		static void Serialise(TextSerialiser& _out)
		{
			static auto SystemArray = Ctor::MakeArray<void(*)(TextSerialiser&)>(
				static_cast<void(*)(TextSerialiser&)>(&SystemFunction<typename Ut::MetaExtract<Ns, UsableComponents>::result::type>::Serialise) ...);

			for (size_t i = 0; i < sizeof...(Ns); ++i)
				SystemArray[i](_out);
		}

		static void Unserialise(TextSerialiser& _in)
		{
			static auto SystemArray = Ctor::MakeArray<void(*)(TextSerialiser&)>(
				static_cast<void(*)(TextSerialiser&)>(&SystemFunction<typename Ut::MetaExtract<Ns, UsableComponents>::result::type>::Unserialise) ...);

			for (size_t i = 0; i < sizeof...(Ns); ++i)
				SystemArray[i](_in);
		}
	};

	struct SuperReflectFunctor
	{
		template<typename T, typename Settor>
		void operator()(const char *, T, Settor, void*) {  }

		template<>
		void operator()(const char * _name, float value, std::function<void(float, void*)> _f, void* _addr)
		{
			float Temp = value;
			switch (EGUI::Display::DragFloat(_name, &Temp, 0.01f, 0.0f, 2.0f))
			{
			case EGUI::eDragStatus::eEND_DRAG:
				//EGUI::GetCommandHND()->EndRecording();
				break;
			case EGUI::eDragStatus::eENTER:
				//EGUI::GetCommandHND()->EndRecording();
				break;
			case EGUI::eDragStatus::eDRAGGING:
				_f(Temp, _addr);
				break;
			case EGUI::eDragStatus::eSTART_DRAG:
				break;
			case EGUI::eDragStatus::eDEACTIVATED:
				//EGUI::GetCommandHND()->EndRecording();
				break;
			case EGUI::eDragStatus::eNO_CHANGE:
				break;
			case EGUI::eDragStatus::eTABBED:
				//EGUI::GetCommandHND()->EndRecording();
				break;
			default:
				break;
			}
		}
		template<>
		void operator()(const char * _name, std::string value, std::function<void(std::string, void*)> _f, void* _addr)
		{
			std::string Temp = value;
			char buffer[1024];
			if (EGUI::Display::TextField(_name, buffer, 1024))
			{
				_f(buffer, _addr);
			}
		}
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
		template<>
		void operator()(const char * _name, int value, std::function<void(int, void*)> _f, void*_addr)
		{
			int Temp = value;
			switch (EGUI::Display::DragInt(_name, &Temp, 0.1f, 0, 2))
			{
			case EGUI::eDragStatus::eEND_DRAG:
				//EGUI::GetCommandHND()->EndRecording();
				break;
			case EGUI::eDragStatus::eENTER:
				//EGUI::GetCommandHND()->EndRecording();

				break;
			case EGUI::eDragStatus::eDRAGGING:
				_f(Temp, _addr);
				break;
			case EGUI::eDragStatus::eSTART_DRAG:
				//Is it possible to make one that does not require the BehaviourClass to be known
				//EGUI::GetCommandHND()->StartRecording<RigidBody>(mnOwner, &mfAngularDrag);
				//EGUI::GetCommandHND()->StartRecording(&Temp);
				break;
			case EGUI::eDragStatus::eDEACTIVATED:
				//EGUI::GetCommandHND()->EndRecording();
				break;
			case EGUI::eDragStatus::eNO_CHANGE:
				break;
			case EGUI::eDragStatus::eTABBED:
				//EGUI::GetCommandHND()->EndRecording();
				break;
			default:
				break;
			}
		}
		template<>
		void operator()(const char *, double, std::function<void(double, void*)>, void*)
		{

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
				//EGUI::GetCommandHND()->EndRecording();
				break;
			case EGUI::eDragStatus::eENTER:
				//EGUI::GetCommandHND()->EndRecording();
				_f(Temp, _addr);
				break;
			case EGUI::eDragStatus::eDRAGGING:
				break;
			case EGUI::eDragStatus::eSTART_DRAG:

				break;
			case EGUI::eDragStatus::eDEACTIVATED:
				//EGUI::GetCommandHND()->EndRecording();
				break;
			case EGUI::eDragStatus::eNO_CHANGE:
				_f(0, _addr);
				break;
			case EGUI::eDragStatus::eTABBED:
				//EGUI::GetCommandHND()->EndRecording();
				break;
			default:
				break;
			}
		}

	};
}


#endif
#endif







