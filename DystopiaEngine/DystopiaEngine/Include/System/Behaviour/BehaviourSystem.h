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

	template <unsigned TOTAL_FILE_DIRECTORIES>
	struct Hotloader;
	struct DLLWrapper;
	struct BehaviourWrap
	{
		BehaviourWrap() {}
		BehaviourWrap(std::string const & _name, Behaviour *  _pointer)
			:mName{ _name }, mpBehaviour{ _pointer }
		{
		}
		std::string mName;					      /*Name of BehaviourScript*/
		Behaviour * mpBehaviour = nullptr;
		//std::shared_ptr<Behaviour> mpBehaviour;   /*SharedPtr to Behaviour Component*/
	};


	class BehaviourSystem
		: public Systems
	{
	public:

		BehaviourSystem();
		~BehaviourSystem();

		virtual void PreInit(void);
		virtual bool Init(void);
		virtual void PostInit(void);

		virtual void FixedUpdate(float);
		virtual void Update(float);
		virtual void PostUpdate(void);
		virtual void Shutdown(void);


		virtual void LoadDefaults(void);
		virtual void LoadSettings(TextSerialiser&);

		void Serialise(TextSerialiser &) const;

		void Unserialise(TextSerialiser &);

#if EDITOR

		void PollChanges(void);
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
#if EDITOR
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
#else
			if (_Behaviour)
			{
				auto pGameObject = EngineCore::GetInstance()->Get<SceneSystem>()->FindGameObject(_Behaviour->GetOwnerID());
				BehaviourMessage Message(_FuncParams...);
				if (pGameObject)
				{
					auto AllBehaviours = pGameObject->GetAllBehaviours();
					for (auto & BehaveElem : AllBehaviours)
					{
						BehaveElem->ReceiveMessage(_FuncName, Message);
					}

				}
			}
#endif
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
#if EDITOR
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
#else
				BehaveElem->ReceiveMessage(_FuncName, Message);
#endif
			}
		}

		template<typename ... Ts>
		void SendExternalMessage(GameObject * _GameObj, const char * const _FuncName, Ts&& ... _FuncParams)
		{
			if (!_GameObj)
				return;

			uint64_t _ID = _GameObj->GetID();
			BehaviourMessage Message(_FuncParams...);

			auto Array = _GameObj->GetAllBehaviours();
			for (auto & BehaveElem : Array)
			{
#if EDITOR
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
#else
				BehaveElem->ReceiveMessage(_FuncName, Message);
#endif
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
#if EDITOR
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
#else
				BehaveElem->ReceiveMessage(_FuncName, Message);

#endif

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
#if EDITOR
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
#else
					BehaveElem->ReceiveMessage(_FuncName, Message);

#endif
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
#if EDITOR
					_EDITOR_START_TRY
						BehaveElem->ReceiveMessage(_FuncName, Message);
					/*If behaviour throws, remove it from game object*/
					_EDITOR_CATCH(std::exception& e)
					{
						_EDITOR_CODE(DEBUG_PRINT((eLog::WARNING), "Behaviour Message Error: %s!", e.what()));
						_EDITOR_CODE(const_cast<GameObject *>(_GameObj)->RemoveComponent(iter.second));
						_EDITOR_CODE(BehaveElem->DestroyComponent());
					}
#else

					BehaveElem->ReceiveMessage(_FuncName, Message);

#endif

				}
			}
		}
#else
		MagicArray<BehaviourWrap> & GetAllBehaviour();
		Behaviour * RequestBehaviour(uint64_t const & _ID, std::string const & _name);
		Behaviour * RequestDuplicate(Behaviour * _PtrToDup, uint64_t _NewID);

#endif


	private:
		/*Array of Behaviours components*/
		//MagicArray< SharedPtr<Behaviour> > mBehaviours;
		FileSystem * FileSys;
#if EDITOR
		using BehaviourPair  = std::pair<uint64_t, Behaviour *>;
		using BehaviourTable = std::pair<std::wstring, AutoArray<BehaviourPair>>;
		Hotloader<1>* mHotloader;
		/*A reference copy of all the available Behaviour Component created from a List of Dlls*/
		MagicArray<BehaviourWrap>   mvBehaviourReferences;
		MagicArray<BehaviourWrap *> mvRecentChanges;
		AutoArray< BehaviourTable > mvBehaviours;


		/*DO NOT TOUCH MY PRIVATES*/
#else
		using BehaviourPair = std::pair<uint64_t, Behaviour *>;
		using BehaviourTable = std::pair<std::string, AutoArray<BehaviourPair>>;
		MagicArray<BehaviourWrap>    mvBehaviourReferences;
		AutoArray< BehaviourTable >  mvBehaviours;
		MagicArray<DLLWrapper *>     mvDllInstance;
#endif

		void ClearAllBehaviours();
	};

}




#endif