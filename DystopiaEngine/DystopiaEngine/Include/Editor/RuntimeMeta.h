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
#ifndef _RUNTIME_META_H_
#define _RUNTIME_META_H_

#include <functional>
#include <string>
#include <utility>

#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"
#include "EGUI.h"
#endif

#include "DataStructure/Array.h"

#include "Object/ObjectFlags.h"
#include "Object/GameObject.h"

#include "Utility/Utility.h"
#include "Utility/MetaAlgorithms.h"
#include "Utility/ComponentGUID.h"

#include "Component/Component.h"
#include "Component/ComponentList.h"

#include "Component/AiController.h"
#include "Component/AABB.h"
#include "Component/AudioSource.h"
#include "Component/Camera.h"
#include "Component/Circle.h"
#include "Component/Convex.h"
#include "Component/Collider.h"
#include "Component/PointCollider.h"
#include "Component/RayCollider.h"
#include "Component/Renderer.h"
#include "Component/RigidBody.h"
#include "Component/SpriteRenderer.h"
#include "Component/Transform.h"
#include "Component/TextRenderer.h"
#include "Component/Emitter.h"
#include "Component/AudioListener.h"

#include "IO/TextSerialiser.h"

#include "System/SystemTypes.h"
#include "System/Base/ComponentDonor.h"
#include "System/Driver/Driver.h"

#include "System/AI/AISystem.h"
#include "System/Sound/SoundSystem.h"
#include "System/Input/InputSystem.h"
#include "System/Camera/CameraSystem.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Particle/ParticleSystem.h"

#include "System/Particle/ParticleAffector.h"
#include "System/Particle/SpawnAffector.h"
#include "System/Particle/LocationAffector.h"
#include "System/Particle/LifetimeAffector.h"
#include "System/Particle/ColorAffector.h"
#include "System/Particle/VelocityAffector.h"
#include "System/Particle/SizeAffector.h"
#include "System/Particle/AccelerationAffector.h"
#include "System/Particle/AttractionAffector.h"

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
		static Component* Get(GameObject*_pRequester)
		{
			UNUSED_PARAMETER(_pRequester);
			C* pComponent = static_cast<ComponentDonor<C>*>(EngineCore::GetInstance()->Get<typename C::SYSTEM>())->RequestComponent(); 
#if EDITOR
			::Editor::EditorMain::GetInstance()->GetSystem<::Editor::EditorCommands>()->AddComponent<C>(_pRequester->GetID(), pComponent);
#endif 
			return pComponent;
		}

		static Component* GetA(GameObject*_pRequester)
		{
			C* pComponent = static_cast<ComponentDonor<C>*>(EngineCore::GetInstance()->Get<typename C::SYSTEM>())->RequestComponent();
			pComponent->SetActive(_pRequester->IsActive());
			pComponent->Awake();
			return pComponent;
		}


		static bool RemoveC(GameObject* _pOwner)
		{
			C* pComponent = _pOwner->GetComponent<C>();
			if (!pComponent)
				return false;

#if EDITOR
			::Editor::EditorMain::GetInstance()->GetSystem<::Editor::EditorCommands>()->RemoveComponent<C>(_pOwner->GetID(), pComponent);
#endif 
			return true;
		}

		static bool IsolateSerialise(GameObject* _pOwner, TextSerialiser& _out)
		{
			C* pComponent = _pOwner->GetComponent<C>();
			if (!pComponent)
				return false;
			pComponent->Serialise(_out);
			return true;
		}

		static bool IsolateCompSerialise(Component* _pComp, TextSerialiser& _out)
		{
			C* pComponent = dynamic_cast<C*>(_pComp);
			if (!pComponent)
				return false;
			pComponent->Serialise(_out);
			return true;
		}

		static bool IsolateUnserialise(Component* _pCom, TextSerialiser& _in)
		{
			static_cast<C*>(_pCom)->Unserialise(_in);
			static_cast<C*>(_pCom)->Awake();
			return true;
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
			bool RemoveCommand(unsigned int _i, GameObject *_owner)
			{
				static auto mData = Ctor::MakeArray<bool(*)(GameObject *)>(RequestComponent<typename Ut::MetaExtract<Ns, UsableComponents>::result::type>::RemoveC...);
				if (_i < size || _i >= 0)
					return mData[_i](_owner);
				return false;
			}

			bool IsolateSerialise(unsigned int _i, GameObject *_owner, TextSerialiser& _out)
			{
				static auto mData = Ctor::MakeArray<bool(*)(GameObject *, TextSerialiser&)>(RequestComponent<typename Ut::MetaExtract<Ns, UsableComponents>::result::type>::IsolateSerialise...);
				if (_i < size || _i >= 0)
					return mData[_i](_owner, _out);
				return false;
			}

			bool IsolateSerialise(unsigned int _i, Component * _comp, TextSerialiser& _out)
			{
				static auto mData = Ctor::MakeArray<bool(*)(Component *, TextSerialiser&)>(RequestComponent<typename Ut::MetaExtract<Ns, UsableComponents>::result::type>::IsolateCompSerialise...);
				if (_i < size || _i >= 0)
					return mData[_i](_comp, _out);
				return false;
			}
			bool IsolateUnserialise(unsigned int _i, Component* _pCom, TextSerialiser& _in)
			{
				static auto mData = Ctor::MakeArray<bool(*)(Component*, TextSerialiser&)>(RequestComponent<typename Ut::MetaExtract<Ns, UsableComponents>::result::type>::IsolateUnserialise...);
				if (_i < size || _i >= 0)
					return mData[_i](_pCom, _in);
				return false;
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

		bool RemoveComponentCommand(unsigned int _i, GameObject *_owner)
		{ 
			return mCollection.RemoveCommand(_i, _owner);
		}

		bool IsolateSerialise(unsigned int _i, GameObject *_owner, TextSerialiser& _out)
		{
			return mCollection.IsolateSerialise(_i, _owner, _out);
		}

		bool IsolateSerialise(unsigned int _i, Component *_owner, TextSerialiser& _out)
		{
			return mCollection.IsolateSerialise(_i, _owner, _out);
		}


		bool IsolateUnserialise(Component *_pCom, TextSerialiser& _in)
		{
			return mCollection.IsolateUnserialise(_pCom->GetRealComponentType(), _pCom, _in);
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

			static void InitDonor(void)
			{
				static_cast<ComponentDonor<C>*>(EngineCore::GetInstance()->Get<typename C::SYSTEM>())->InitComponents();
			}

			static void InitDonor(const uint64_t& _object)
			{
				static_cast<ComponentDonor<C>*>(EngineCore::GetInstance()->Get<typename C::SYSTEM>())->InitComponent(_object);
			}
		};

		static void InitDonors(void)
		{
			static auto SystemArray = Ctor::MakeArray<void(*)(void)>(
				static_cast<void(*)(void)>(&SystemFunction<typename Ut::MetaExtract<Ns, UsableComponents>::result::type>::InitDonor) ...);

			for (size_t i = 0; i < sizeof...(Ns); ++i)
				SystemArray[i]();
		}

		static void InitDonors(const uint64_t& _object)
		{
			static auto SystemArray = Ctor::MakeArray<void(*)(const uint64_t&)>(
				static_cast<void(*)(const uint64_t&)>(&SystemFunction<typename Ut::MetaExtract<Ns, UsableComponents>::result::type>::InitDonor) ...);

			for (size_t i = 0; i < sizeof...(Ns); ++i)
				SystemArray[i](_object);
		}

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

	template<typename C>
	struct RequestAffector
	{
		static C Get(void)
		{
			return C{};
		}
	};

	struct AffectorGet
	{
		static constexpr size_t size = Ut::SizeofList<AffectorList>::value;

		template<typename A>
		struct Collection;
		template<unsigned ... Ns, typename ... Affts>
		struct Collection<Ut::Collection<Ut::Indexer<Ns, Affts>...>>
		{
			static void Get(unsigned _n, Emitter& e)
			{
				static void(*mData[])(Emitter&) = {
					[](Emitter& _e) -> void { _e.AddAffector(Affts{}); }...
				};

				mData[_n](e);
			}
		};

		void Get(unsigned _i, Emitter& e)
		{
			return mCollection.Get(_i, e);
		}

		Collection<AffectorList> mCollection;
	};

	template <typename>
	struct AffectorUI;

	template <typename ... Ts, unsigned ... N>
	struct AffectorUI<Ut::Collection<Ut::Indexer<N, Ts>...>>
	{
		inline static auto GetPtrsToUIFunction(void) noexcept
		{
			return Ctor::MakeArray<void(ParticleAffector::*)(void)>(reinterpret_cast<void(ParticleAffector::*)(void)>(&Ts::EditorUI) ...);
		}
		inline static auto GetUIName(void) noexcept
		{
			return Ctor::MakeArray<const char*(ParticleAffector::*)(void)>(reinterpret_cast<const char*(ParticleAffector::*)(void)>(&Ts::EditorDisplayLabel) ...);
		}
	};


#if EDITOR
	struct SuperReflectFunctor
	{
		template<typename T, typename Settor>
		void operator()(const char *, T, Settor, void*) {  }

		template<>
		void operator()(const char * _name, float value, std::function<void(float, void*)> _f, void* _addr)
		{
			float Temp = value;
			switch (EGUI::Display::DragFloat(_name, &Temp, 0.01f, -FLT_MAX, FLT_MAX))
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
			std::string Temp = Ut::Move(value);
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
			switch (EGUI::Display::DragInt(_name, &Temp, 1.f, -INT32_MAX, INT32_MAX))
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
		void operator()(const char * _name, bool _value, std::function<void(bool, void*)> _f, void* _addr)
		{
			bool Temp = _value;
			if (EGUI::Display::CheckBox(_name, &Temp))
			{
				_f(Temp, _addr);
			}
		}
		/*template<>
		void operator()(const char *, double, std::function<void(double, void*)>, void*)
		{

		}*/
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
#endif
}


#endif






