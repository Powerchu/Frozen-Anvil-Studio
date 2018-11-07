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
		BehaviourWrap(){}
		BehaviourWrap(std::string const & _name, Behaviour *  _pointer)
			:mName{_name}, mpBehaviour{ _pointer }
		{
		}
		std::string mName;					      /*Name of BehaviourScript*/
		Behaviour * mpBehaviour;
		//std::shared_ptr<Behaviour> mpBehaviour;   /*SharedPtr to Behaviour Component*/
	};




#endif

	class BehaviourSystem 
	: public Systems
	{
	public :
#if !EDITOR
		using AllBehaviour = 
			Ut::MetaSortT_t <Ut::MetaLessThan, Ut::Collection 
			<

			>>;
#endif 
		BehaviourSystem();

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

#if EDITOR

		MagicArray<BehaviourWrap *> const & GetDllChanges() const;
		bool hasDllChanges() const;
		MagicArray<BehaviourWrap> & GetAllBehaviour();
		Behaviour * RequestBehaviour(uint64_t const & _ID, std::string const & _name);
		Behaviour * RequestDuplicate(Behaviour * _PtrToDup, uint64_t _NewID);
		
		void ReplaceID(uint64_t _old, uint64_t _new, GameObject * _newOwner);

		void ReAttach(void);

		template<typename ... Ts>
		void SendInternalMessage(Behaviour * const _Behaviour, const char * const _FuncName, Ts&& ... _FuncParams);


		template<typename ... Ts>
		void SendMessage(uint64_t _ObjectID, const char * const _FuncName, Ts&& ... _FuncParams);

		template<typename ... Ts>
		void SendMessage(const char * const _FuncName, Ts&& ... _FuncParams);
#endif

	private:
		/*Array of Behaviours components*/
		//MagicArray< SharedPtr<Behaviour> > mBehaviours;
		FileSystem * FileSys;
#if EDITOR
		using BehaviourPair = std::pair<uint64_t, Behaviour *>;
		using BehaviourTable = std::pair<std::wstring, AutoArray<BehaviourPair>>;
		SharedPtr< Hotloader<1> > mHotloader;
		/*A reference copy of all the available Behaviour Component created from a List of Dlls*/
		MagicArray<BehaviourWrap>   mvBehaviourReferences;
		MagicArray<BehaviourWrap *> mvRecentChanges;
		AutoArray< BehaviourTable > mvBehaviours;


		/*DO NOT TOUCH MY PRIVATES*/
		void ClearAllBehaviours();
#endif
	};

}




#endif