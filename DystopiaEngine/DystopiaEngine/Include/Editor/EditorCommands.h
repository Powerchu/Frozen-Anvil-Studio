/* HEADER *********************************************************************************/
/*!
\file	EditorCommands.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_COMMANDS_H_
#define _EDITOR_COMMANDS_H_
#include "Editor/EditorSystem.h"
#include "Editor/CommandTypes.h"

#include "DataStructure/AutoArray.h"

#include "Allocator/DefaultAlloc.h"

#include "Utility//Utility.h"

#include <deque>

namespace Editor
{
	class EditorCommands : public EditorSystem
	{
	public:
		EditorCommands(void);
		~EditorCommands(void);

		void Load(void);
		bool Init(void);
		void StartFrame(void);
		void Update(float);
		void EndFrame(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);

		/****************************************** Actual commands ******************************************/
		void ClearAllCommmands(void);

		void SavedChanges(void);
		bool HasChanges(void) const;

		void Undo(void);
		void Redo(void);

		void InsertNewGameObject(const Math::Pt3D& = Math::Pt3D{ 0, 0, 0 });
		void RemoveGameObject(const uint64_t&);
		void RemoveGameObject(AutoArray<uint64_t>&);
		void InstantiatePrefab(const HashString&, const Math::Pt3D& = Math::Pt3D{ 0, 0, 0, 1.f });
		void DuplicateGameObject(const uint64_t&);
		void DuplicateGameObject(AutoArray<uint64_t>&);


		template <class Component, typename ... Ts>
		void FunctionCommand(const uint64_t& _objID, const ComponentFunction<Component, Ts ...>& _oldFn, 
													 const ComponentFunction<Component, Ts ...>& _newFn);
		template<class C, typename ... Ts>
		auto MakeFnCommand(void(C::*_function)(Ts...), const Ut::RemoveRef_t<Ts>& ... _params);
	



		template<class C, typename T>
		void StartRec(T C::* _ptrMemVar, C* _obj);
		template<class C, typename T>
		void EndRec(T C::* _ptrMemVar, C* _obj, bool C::* = nullptr);




		template<class C, typename ... Ts>
		void StartRec(void(C::*_function)(Ts...), const Ut::RemoveRef_t<Ts>& ... _params);
		template<class C, typename ... Ts>
		void EndRec(const uint64_t _id, void(C::*_function)(Ts...), const Ut::RemoveRef_t<Ts>& ... _params);




		template<typename T>
		void ChangeValue(T*);
		template<class C, typename T>
		void ChangeValue(const uint64_t& _id, T C::* _ptrMemVar, const T& oldV, const T& newV, bool C::* = nullptr);
		template<class C, typename T>
		void ChangeValue(AutoArray<uint64_t>& _arrIDs, T C::* _ptrMemVar, const T& oldV, const T& newV, bool C::* = nullptr);
		template<class S, typename T>
		void ChangeValue(T S::* _ptrMemVar, const T& oldV, const T& newV, bool S::* = nullptr);



		template<class C, typename ... Ts>
		void StartMultiRec(void(C::*_function)(Ts...), const Ut::RemoveRef_t<Ts>& ... _params);
		template<class C, typename ... Ts>
		void EndMultiRec(const uint64_t& _id, void(C::*_function)(Ts...), const Ut::RemoveRef_t<Ts>& ... _params);
		void ExecuteMultiRec(void);




		template<typename C>
		void AddComponent(const uint64_t& _id, C * _pComponent);
		template<typename C>
		void RemoveComponent(const uint64_t& _id, C * _pComponent);

	private:
		std::deque<Command*> mDeqRedo;
		std::deque<Command*> mDeqUndo;
		bool mbChangesMade;
		bool mbDisableCommands;
		size_t mnUndo;
		size_t mnRedo;

		Recordings* mpRecordingVal;

		AutoArray<Recordings*> mArrMultiRec;
		AutoArray<Command*> mArrMultiCommands;

		void RemoveStray(std::deque<Command*>&);
		void ExecuteDo(Command*);
	};
}

/***************************************************************** Template Function Definitions *****************************************************************/

template <class C, typename ... Ts>
void Editor::EditorCommands::FunctionCommand(const uint64_t& _objID, const ComponentFunction<C, Ts ...>& _oldFn, 
																	 const ComponentFunction<C, Ts ...>& _newFn)
{
	using fnType = FunctionComd<ComponentFunction<C, Ts ...>, ComponentFunction<C, Ts ...>>;
	if (mbDisableCommands)
		return;

	ExecuteDo(Dystopia::DefaultAllocator<fnType>::ConstructAlloc(_objID, _newFn, _oldFn));
}

template<class C, typename ... Ts>
inline auto Editor::EditorCommands::MakeFnCommand(void (C::*_function)(Ts...), const Ut::RemoveRef_t<Ts>& ... _params)
{
	return ComponentFunction<C, Ts...>{ _function, _params ... };
}

template<class C, typename T>
void Editor::EditorCommands::StartRec(T C::* _ptrMemVar, C* _obj)
{
	if (mbDisableCommands)
		return;

	if (mpRecordingVal)
		__debugbreak();

	mpRecordingVal = Dystopia::DefaultAllocator<RecordValue<T>>::ConstructAlloc(_obj->*_ptrMemVar);
}

template<class C, typename T>
void Editor::EditorCommands::EndRec(T C::* _ptrMemVar, C* _obj, bool C::* _ptrNotify)
{
	if (mbDisableCommands)
		return;

	if (!mpRecordingVal)
		__debugbreak();

	T oldValue;
	mpRecordingVal->GetValue(static_cast<void*>(&oldValue));
	
	ExecuteDo(Dystopia::DefaultAllocator<ModifyValue<C, T, void>>::ConstructAlloc(_obj->GetOwnerID(), _ptrMemVar, oldValue, _obj->*_ptrMemVar, _ptrNotify));

	Dystopia::DefaultAllocator<Recordings>::DestructFree(mpRecordingVal);
	mpRecordingVal = nullptr;
}

template<class C, typename ... Ts>
void Editor::EditorCommands::StartRec(void(C::*_function)(Ts...), const Ut::RemoveRef_t<Ts>& ... _params)
{
	using CFn = ComponentFunction<C, Ts...>;

	if (mbDisableCommands)
		return;

	if (mpRecordingVal)
		return;//__debugbreak();

	mpRecordingVal = Dystopia::DefaultAllocator<RecordValue<CFn>>::ConstructAlloc(CFn{ _function, _params ... });
}

template<class C, typename ... Ts>
void Editor::EditorCommands::EndRec(const uint64_t _id, void(C::*_function)(Ts...), const Ut::RemoveRef_t<Ts>& ... _params)
{
	using CFn = ComponentFunction<C, Ts...>;

	if (mbDisableCommands)
		return;

	if (!mpRecordingVal)
		return; //__debugbreak();

	auto oFn = MakeFnCommand(_function, _params ...);
	auto nFn = MakeFnCommand(_function, _params ...);
	mpRecordingVal->GetValue(static_cast<void*>(&oFn));
	FunctionCommand(_id, oFn, nFn);

	Dystopia::DefaultAllocator<Recordings>::DestructFree(mpRecordingVal);
	mpRecordingVal = nullptr;
}

template<typename T>
void Editor::EditorCommands::ChangeValue(T*)
{
	__debugbreak();
}

template<class C, typename T>
void Editor::EditorCommands::ChangeValue(const uint64_t& _id, T C::* _ptrMemVar, const T& _oldV, const T& _newV, bool C::*_ptrNotify)
{
	if (mbDisableCommands)
		return;

	ExecuteDo(Dystopia::DefaultAllocator<ModifyValue<C, T, void>>::ConstructAlloc(_id, _ptrMemVar, _oldV, _newV, _ptrNotify));
}

template<class C, typename T>
void Editor::EditorCommands::ChangeValue(AutoArray<uint64_t>& _arrIDs, T C::* _ptrMemVar, const T& _oldV, const T& _newV, bool C::*_ptrNotify)
{
	if (mbDisableCommands)
		return;

	AutoArray<::Editor::Command*> mArrComds;
	for (const auto& id : _arrIDs)
		mArrComds.push_back(Dystopia::DefaultAllocator<ModifyValue<C, T, void>>::ConstructAlloc(id, _ptrMemVar, _oldV, _newV, _ptrNotify));

	ExecuteDo(Dystopia::DefaultAllocator<BatchExecute>::ConstructAlloc(mArrComds));
}

template<class S, typename T>
void Editor::EditorCommands::ChangeValue(T S::* _ptrMemVar, const T& _oldV, const T& _newV, bool S::*_ptrNotify)
{
	if (mbDisableCommands)
		return;

	ExecuteDo(Dystopia::DefaultAllocator<ModifyValue<S, T, void>>::ConstructAlloc(_ptrMemVar, _oldV, _newV, _ptrNotify));
}

template<class C, typename ... Ts>
void Editor::EditorCommands::StartMultiRec(void(C::*_function)(Ts...), const Ut::RemoveRef_t<Ts>& ... _params)
{
	using CFn = ComponentFunction<C, Ts...>;

	if (mbDisableCommands)
		return;

	if (mpRecordingVal)
		__debugbreak();

	mArrMultiRec.push_back(Dystopia::DefaultAllocator<RecordValue<CFn>>::ConstructAlloc(CFn{ _function, _params ... }));
}

template<class C, typename ... Ts>
void Editor::EditorCommands::EndMultiRec(const uint64_t& _id, void(C::*_function)(Ts...), const Ut::RemoveRef_t<Ts>& ... _params)
{
	using CFn = ComponentFunction<C, Ts...>;

	if (mbDisableCommands)
		return;

	auto oFn = MakeFnCommand(_function, _params ...);
	auto nFn = MakeFnCommand(_function, _params ...);
	size_t first = 0;
	mArrMultiRec[first]->GetValue(static_cast<void*>(&oFn));
	mArrMultiRec.FastRemove(first);

	mArrMultiCommands.push_back(Dystopia::DefaultAllocator<FunctionComd<ComponentFunction<C, Ts ...>, ComponentFunction<C, Ts ...>>>::ConstructAlloc(_id, nFn, oFn));
}

template<typename C>
void Editor::EditorCommands::AddComponent(const uint64_t& _id, C * _pComponent)
{
	if (mbDisableCommands)
		return;
	
	if (auto o = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject(_id))
	{
		ExecuteDo(Dystopia::DefaultAllocator<::Editor::AddComponent<C>>::ConstructAlloc(_id, _pComponent));
	}
}

template<typename C>
void Editor::EditorCommands::RemoveComponent(const uint64_t& _id, C * _pComponent)
{
	if (mbDisableCommands)
		return;

	if (auto o = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject(_id))
	{
		ExecuteDo(Dystopia::DefaultAllocator<::Editor::RemoveComponent<C>>::ConstructAlloc(_id, _pComponent));
	}
}
#endif
#endif 










