#pragma once

#ifndef BEHAVIOUR_SYSTEM_H
#define BEHAVIOUR_SYSTEM_H

#include "System/Base/Systems.h"
#include "DataStructure/MagicArray.h"
#include "DataStructure/SharedPtr.h"
#include "System/Base/ComponentDonor.h"
#include "Component/BehaviourList.h"
#include "Behaviour/Behaviour.h"
#include "System/File/FileSystem.h"
#include "System/Scene/SceneSystem.h"

#include <memory>


#if !EDITOR
/*Include all the behaviour header files*/

#endif



namespace Dystopia
{

#if EDITOR
	template <unsigned TOTAL_FILE_DIRECTORIES>
	struct Hotloader;

	struct BehaviourWrap
	{
		BehaviourWrap() {}
		BehaviourWrap(std::string const & _name, Behaviour *  _pointer)
			:mName{ _name }, mpBehaviour{ _pointer }
		{
		}
		//BehaviourWrap(const BehaviourWrap &)            = delete;
		BehaviourWrap& operator=(const BehaviourWrap&) = delete;

		std::string mName;					      /*Name of BehaviourScript*/
		Behaviour * mpBehaviour;
		//std::shared_ptr<Behaviour> mpBehaviour;   /*SharedPtr to Behaviour Component*/
	};




#endif

	class BehaviourSystem
		: public Systems
	{
	public:
#if !EDITOR
		using AllBehaviour =
			Ut::MetaSortT_t <Ut::MetaLessThan, Ut::Collection
			<

			>>;
#endif 
		BehaviourSystem();
		~BehaviourSystem();

		virtual void PreInit(void);
		virtual bool Init(void);
		virtual void PostInit(void);

		virtual void FixedUpdate(float);
		virtual void Update(float);
		virtual void PostUpdate(void);
		virtual void Shutdown(void);
		void PollChanges(void);

		virtual void LoadDefaults(void);
		virtual void LoadSettings(TextSerialiser&);

		void Serialise(TextSerialiser &) const;

		void Unserialise(TextSerialiser &);

		void DeleteBehaviour(Behaviour *);

#if EDITOR

		MagicArray<BehaviourWrap *> const & GetDllChanges() const;
		bool hasDllChanges() const;
		MagicArray<BehaviourWrap> & GetAllBehaviour();
		Behaviour * RequestBehaviour(uint64_t const & _ID, std::string const & _name);
		Behaviour * RequestDuplicate(Behaviour * _PtrToDup, uint64_t _NewID);

		void ReplaceID(uint64_t _old, uint64_t _new, GameObject * _newOwner);

		void ReAttach(void);

		template<typename ... Ts>
		void SendInternalMessage(Behaviour * const _Behaviour, const char * const _FuncName, Ts ... _FuncParams)
		{
			if (_Behaviour)
			{
				auto pGameObject = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_Behaviour->GetOwnerID());
				BehaviourMessage Message(_FuncParams...);
				if (pGameObject)
				{
					auto AllBehaviours = pGameObject->GetAllBehaviours();
					for (auto & BehaveElem : AllBehaviours)
					{
						/*Try to send Message to other components*/
						_EDITOR_START_TRY
							BehaveElem->ReceiveMessage(_FuncName, Message);
						/*If behaviour throws, remove it from game object*/
						_EDITOR_CATCH(std::exception& e)
						{
							//_EDITOR_CODE(DEBUG_PRINT((eLog::WARNING), "Behaviour Message Error: %s!", e.what()));
							_EDITOR_CODE(pGameObject->RemoveComponent(BehaveElem));
							_EDITOR_CODE(BehaveElem->DestroyComponent());
						}
					}

				}
			}
		}

		template<typename ... Ts>
		void SendExternalMessage(uint64_t _ObjectID, const char * const _FuncName, Ts&& ... _FuncParams)
		{
			//return;

			auto pGameObject = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_ObjectID);
			if (!pGameObject)
				return;
			BehaviourMessage Message(_FuncParams...);

			auto Array = pGameObject->GetAllBehaviours();
			for (auto & BehaveElem : Array)
			{
				/*Try to send Message to other components*/
				_EDITOR_START_TRY
					BehaveElem->ReceiveMessage(_FuncName, Message);
				/*If behaviour throws, remove it from game object*/
				_EDITOR_CATCH(std::exception& e)
				{
					_EDITOR_CODE(DEBUG_PRINT((eLog::WARNING), "Behaviour Message Error: %s!", e.what()));
					_EDITOR_CODE(pGameObject->RemoveComponent(BehaveElem));
					_EDITOR_CODE(BehaveElem->DestroyComponent());
				}

			}
		}

		template<typename ... Ts>
		void SendExternalMessage(GameObject * _GameObj, const char * const _FuncName, Ts&& ... _FuncParams)
		{
			if (!_GameObj)
				return;
			DEBUG_PRINT(eLog::MESSAGE, "Valid Game Object");
			uint64_t _ID = _GameObj->GetID();
			BehaviourMessage Message(_FuncParams...);

			auto Array = _GameObj->GetAllBehaviours();
			for (auto & BehaveElem : Array)
			{
				DEBUG_PRINT(eLog::MESSAGE, "Sending Message to other behaviours");
				/*Try to send Message to other components*/
				_EDITOR_START_TRY
					BehaveElem->ReceiveMessage(_FuncName, Message);
				/*If behaviour throws, remove it from game object*/
				_EDITOR_CATCH(std::exception& e)
				{
					_EDITOR_CODE(DEBUG_PRINT((eLog::WARNING), "Behaviour Message Error: %s!", e.what()));
					_EDITOR_CODE(_GameObj->RemoveComponent(BehaveElem));
					_EDITOR_CODE(BehaveElem->DestroyComponent());
				}

			}
		}

		template<typename ... Ts>
		void SendExternalMessage(GameObject const *  _GameObj, const char * const _FuncName, Ts&& ... _FuncParams)
		{
			if (!_GameObj)
				return;

			uint64_t _ID = _GameObj->GetID();
			BehaviourMessage Message(_FuncParams...);

			auto Array = _GameObj->GetAllBehaviours();
			for (auto & BehaveElem : Array)
			{
				/*Try to send Message to other components*/
				_EDITOR_START_TRY
					BehaveElem->ReceiveMessage(_FuncName, Message);
				/*If behaviour throws, remove it from game object*/
				_EDITOR_CATCH(std::exception& e)
				{
					_EDITOR_CODE(DEBUG_PRINT((eLog::WARNING), "Behaviour Message Error: %s!", e.what()));
					_EDITOR_CODE(const_cast<GameObject *>(_GameObj)->RemoveComponent(iter.second));
					_EDITOR_CODE(BehaveElem->DestroyComponent());
				}

			}
		}

		template<typename ... Ts>
		void SendAllMessage(const char * const _FuncName, Ts&& ... _FuncParams)
		{

			BehaviourMessage Message(_FuncParams...);
			auto & Array = EngineCore::GetInstance()->Get<SceneSystem>()->GetActiveScene().GetAllGameObjects();
			for (auto & elem : Array)
			{
				auto BehaviourArray = elem.GetAllBehaviours();
				for (auto & BehaveElem : BehaviourArray)
				{
					/*Try to send Message to other components*/
					_EDITOR_START_TRY
						BehaveElem->ReceiveMessage(_FuncName, Message);
					/*If behaviour throws, remove it from game object*/
					_EDITOR_CATCH(std::exception& e)
					{
						_EDITOR_CODE(DEBUG_PRINT((eLog::WARNING), "Behaviour Message Error: %s!", e.what()));
						_EDITOR_CODE(const_cast<GameObject *>(_GameObj)->RemoveComponent(iter.second));
						_EDITOR_CODE(BehaveElem->DestroyComponent());
					}

				}
			}
		}
		template<typename ... Ts>
		void SendMessageToTags(HashString const & _Tags, Ts && ... _FuncParams)
		{
			BehaviourMessage Message(_FuncParams...);
			auto & Array = EngineCore::GetInstance()->Get<SceneSystem>()->GetActiveScene().GetAllGameObjects();
			for (auto & elem : Array)
			{
				auto BehaviourArray = elem.GetAllBehaviours();
				for (auto & BehaveElem : BehaviourArray)
				{
					/*Try to send Message to other components*/
					if (!BehaveElem->GetOwner()->HasTag(_Tags))
						continue;

					_EDITOR_START_TRY
						BehaveElem->ReceiveMessage(_FuncName, Message);
					/*If behaviour throws, remove it from game object*/
					_EDITOR_CATCH(std::exception& e)
					{
						_EDITOR_CODE(DEBUG_PRINT((eLog::WARNING), "Behaviour Message Error: %s!", e.what()));
						_EDITOR_CODE(const_cast<GameObject *>(_GameObj)->RemoveComponent(iter.second));
						_EDITOR_CODE(BehaveElem->DestroyComponent());
					}

				}
			}
		}
#endif

	private:
		/*Array of Behaviours components*/
		//MagicArray< SharedPtr<Behaviour> > mBehaviours;
		FileSystem * FileSys;
#if EDITOR
		using BehaviourPair = std::pair<uint64_t, Behaviour *>;
		using BehaviourTable = std::pair<std::wstring, AutoArray<BehaviourPair>>;
		Hotloader<1>* mHotloader;
		/*A reference copy of all the available Behaviour Component created from a List of Dlls*/
		MagicArray<BehaviourWrap>   mvBehaviourReferences;
		MagicArray<BehaviourWrap *> mvRecentChanges;
		AutoArray< BehaviourTable > mvBehaviours;


		/*DO NOT TOUCH MY PRIVATES*/
		void ClearAllBehaviours();
#endif
	};

	//template<typename ...Ts>
	//inline void BehaviourSystem::SendInternalMessage(Behaviour * const _Behaviour, const char * const _FuncName, Ts  ..._FuncParams)


	//template <typename ... Ts>
	//inline void BehaviourSystem::SendExternalMessage(GameObject * const _GameObj, const char* const _FuncName, Ts&&... _FuncParams)


	//template <typename ... Ts>
	//inline void BehaviourSystem::SendExternalMessage(GameObject const * const _GameObj, const char* const _FuncName, Ts&&... _FuncParams);

	//template <typename ... Ts>
	//inline void BehaviourSystem::SendExternalMessage(uint64_t _ObjectID, const char* const _FuncName, Ts&&... _FuncParams);

	//template <typename ... Ts>
	//inline void BehaviourSystem::SendAllMessage(const char* const _FuncName, Ts&&... _FuncParams)

}




#endif