
#include "Math/MathLib.h"

#if EDITOR

#include "Editor/EditorMain.h"
#include "Editor/EditorFactory.h"
#endif
#include "System/Behaviour/BehaviourSystem.h"
#undef ERROR

#include "System/File/FileSystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Driver/Driver.h"
#include "Object/GameObject.h"
#include "System/Behaviour/BehaviourSystemHelper.h"
#include "System/Behaviour/BehaviourMessage.h"
#undef ERROR

#include "Globals.h"
#include "Utility/DebugAssert.h"
#include "System/Logger/LoggerSystem.h"
#include "System/Logger/LogPriority.h"
#include "System/Window/WindowManager.h"
#include "System/Window/Window.h"
#include "IO/TextSerialiser.h"
#include <utility>
#include "Editor/HotLoader.h"

#if !EDITOR
//#include "../Resource/Behaviours/BehavioursScripts/ChangeAudio.h"
//#include "../Resource/Behaviours/BehavioursScripts/CharacterController.h"
//#include "../Resource/Behaviours/BehavioursScripts/CombatBox.h"
//#include "../Resource/Behaviours/BehavioursScripts/DeleteSelf.h"
//#include "../Resource/Behaviours/BehavioursScripts/FollowTarget.h"
//#include "../Resource/Behaviours/BehavioursScripts/ForceTOne.h"
//#include "../Resource/Behaviours/BehavioursScripts/ForceTTwo.h"
//#include "../Resource/Behaviours/BehavioursScripts/FormTTwo.h"
//#include "../Resource/Behaviours/BehavioursScripts/FormTOne.h"
//#include "../Resource/Behaviours/BehavioursScripts/Goblin.h"
//#include "../Resource/Behaviours/BehavioursScripts/GoblinCombat.h"
//#include "../Resource/Behaviours/BehavioursScripts/Parallax.h"
//#include "../Resource/Behaviours/BehavioursScripts/SkillManager.h"
#endif

namespace Dystopia
{
	

	BehaviourSystem::BehaviourSystem()
#if EDITOR
		:mHotloader{ new Hotloader<1>() }
#else
		: mHotloader{ new Hotloader<1>() }
#endif
	{

	}

	BehaviourSystem::~BehaviourSystem()
	{
#if EDITOR
		delete mHotloader;
#endif
	}

	void Dystopia::BehaviourSystem::PreInit(void)
	{
	}

	bool Dystopia::BehaviourSystem::Init(void)
	{
		/*Init Hotloader*/
#if EDITOR

		FileSys = EngineCore::GetInstance()->GetSubSystem<FileSystem>();

		std::wstring IncludeFolderPath = L"/I" + FileSys->GetProjectFolders<std::wstring>(eFileDir::eHeader);

#if _DEBUG
		FileSys->CreateFiles("BehaviourDLL",         eFileDir::eAppData);
#else
		FileSys->CreateFiles("BehaviourDLL_Release", eFileDir::eAppData);
#endif

		FileSys->CreateFiles("Dystopia/Temp", eFileDir::eAppData);
		FileSys->CreateFiles("Behaviours/BehavioursScripts", eFileDir::eResource);
		std::string PipePath = FileSys->GetProjectFolders<std::string>(eFileDir::eCurrent) + "\\Resource\\Piping\\BehaviourPiping.exe";
		mHotloader->SetPipeExePath(std::wstring{PipePath.begin(), PipePath.end()});

#if _DEBUG
		mHotloader->AddFilesToCrawl(L"DystopiaEngine_D.lib", eCompile);
		mHotloader->SetDllFolderPath(FileSys->GetFullPath("BehaviourDLL", eFileDir::eAppData));

#else
		mHotloader->AddFilesToCrawl(L"DystopiaEngine_Editor.lib", eCompile);
		mHotloader->SetDllFolderPath(FileSys->GetFullPath("BehaviourDLL_Release", eFileDir::eAppData));
#endif
		//mHotloader->SetParentHWND(EngineCore::GetInstance()->GetSystem<WindowManager>()->GetMainWindow().GetWindowHandle());
		
		mHotloader->SetTempFolder(FileSys->GetFullPath("Temp", eFileDir::eAppData));
		mHotloader->SetFileDirectoryPath<0>(FileSys->GetFullPath("BehavioursScripts", eFileDir::eResource));

		mHotloader->SetCompilerFlags(L"cl /W4 /EHsc /nologo /LD /DLL /DEDITOR /D_ITERATOR_DEBUG_LEVEL /std:c++17 " + IncludeFolderPath);

#else

		FileSys = EngineCore::GetInstance()->GetSubSystem<FileSystem>();
		std::wstring IncludeFolderPath = L"/I" + FileSys->GetProjectFolders<std::wstring>(eFileDir::eHeader);
		mHotloader->AddFilesToCrawl(L"DystopiaEngine.lib", eCompile);
		mHotloader->SetDllFolderPath(FileSys->GetFullPath("BehaviourDll", eFileDir::eResource));
		mHotloader->SetTempFolder(FileSys->GetFullPath("Temp", eFileDir::eAppData));
		mHotloader->SetFileDirectoryPath<0>(FileSys->GetFullPath("BehavioursScripts", eFileDir::eResource));

		mHotloader->SetCompilerFlags(L"cl /W4 /EHsc /nologo /LD /DLL /DEDITOR /D_ITERATOR_DEBUG_LEVEL /std:c++17 " + IncludeFolderPath);

		mHotloader->Init();
		auto const & ArrayDlls = mHotloader->GetDlls();
		for (auto & elem : ArrayDlls)
		{
			using fpClone = Behaviour * (*) ();

			fpClone BehaviourClone = elem.GetDllFunc<Behaviour *>(FileSys->RemoveFileExtension<std::wstring>(elem.GetDllName()) + L"Clone");

			if (BehaviourClone)
			{
				BehaviourWrap wrap;
				std::wstring name   = FileSys->RemoveFileExtension<std::wstring>(elem.GetDllName());
				std::string name_s  = FileSys->RemoveFileExtension<std::string> (std::string{ name.begin(), name.end() });
				wrap.mName = std::string{ name_s };
				wrap.mpBehaviour = (BehaviourClone());
				mvBehaviourReferences.Emplace(Ut::Move(wrap));
				mvBehaviours.push_back(std::make_pair(name_s, AutoArray<BehaviourPair>{}));
			}
			//mvBehaviourReferences.Emplace(BehaviourClone());
		}

		//std::filesystem::path DllPath{ FileSys->GetProjectFolders<std::string>(eFileDir::eResource) + "/BehaviourDll/" };
		//std::error_code error;
		//std::filesystem::directory_iterator iter{ DllPath, std::filesystem::directory_options::skip_permission_denied, error };
		//for (auto & DllFile : iter)
		//{
		//	std::wstring DllPathW = DllFile.path().wstring();
		//	auto Handle = LoadLibrary(DllPathW.c_str());

		//	if (Handle)
		//	{

		//		std::wstring DllNamew = (FileSys->RemoveFileExtension<std::wstring>(DllFile.path().wstring()));
		//		auto DllPtr = new DLLWrapper{ DllFile.path().wstring(), DllNamew, DllFile.path().wstring(), std::move(Handle) };
		//		DLLWrapper ** ptr = mvDllInstance.Emplace(DllPtr);
		//		using fpClone = Behaviour * (*) ();
		//		fpClone BehaviourClone = (*ptr)->GetDllFunc<Behaviour *>(DllNamew + L"Clone");

		//		if (BehaviourClone)
		//		{

		//			std::string DllName = (FileSys->RemoveFileExtension<std::string>(DllFile.path().string()));
		//			mvBehaviourReferences.Emplace(BehaviourWrap{ DllName, BehaviourClone()});
		//			mvBehaviours.push_back(std::make_pair(DllName, AutoArray<BehaviourPair>{}));
		//		}

		//		//if (Handle)
		//		//	FreeLibrary(Handle);
		//	}
		//}
#endif
#if EDITOR

		mHotloader->Init();
		auto const & ArrayDlls = mHotloader->GetDlls();
		for (auto & elem : ArrayDlls)
		{
			using fpClone = Behaviour * (*) ();

			fpClone BehaviourClone = elem.GetDllFunc<Behaviour *>(FileSys->RemoveFileExtension<std::wstring>(elem.GetDllName()) + L"Clone");

			if (BehaviourClone)
			{
				BehaviourWrap wrap;
				std::wstring name = FileSys->RemoveFileExtension<std::wstring>(elem.GetDllName());
				wrap.mName = std::string{ name.begin(), name.end() };
				wrap.mpBehaviour = (BehaviourClone());
				mvBehaviourReferences.Emplace(Ut::Move(wrap));
				mvBehaviours.push_back(std::make_pair(name, AutoArray<BehaviourPair>{}));
			}
			//mvBehaviourReferences.Emplace(BehaviourClone());
		}

#endif

		return true;
	}

	void Dystopia::BehaviourSystem::PostInit(void)
	{
	}

	void Dystopia::BehaviourSystem::FixedUpdate(float _dt)
	{
		for (auto & i : mvBehaviours)
		{
			for (auto & iter : i.second)
			{
				if (iter.second)
				{
					if (auto p = iter.second->GetOwner())
					{
						if (p->GetFlag() & eObjFlag::FLAG_EDITOR_OBJ)
						{
							continue;
						}
						if (p->GetFlag() & eObjFlag::FLAG_ACTIVE)
						{
							iter.second->FixedUpdate(_dt);
						}
					}
					else
					{
						iter.second->DestroyComponent();
					}
				}

			}
		}
	}
#if EDITOR
	void Dystopia::BehaviourSystem::PollChanges(void)
	{


		bool hasChange   = false;
		bool hasSaveFile = false;
		/*Update Hotloader*/
		std::vector<std::wstring> vTempFileName;
		mHotloader->Update();
		mHotloader->ChangesInTempFolder(vTempFileName);
		if (vTempFileName.size() > 0)
		{
			std::string SceneName = EngineCore::GetInstance()->GetSystem<SceneSystem>()->GetCurrentScene().GetSceneName();
			if (FileSys->GetFullPath(SceneName + ".dscene", eFileDir::eResource) != "")
			{
				EngineCore::GetInstance()->GetSystem<SceneSystem>()->SaveScene(FileSys->GetFullPath(SceneName + ".dscene", eFileDir::eResource), SceneName);
				hasSaveFile = true;
			}

		}
		
		for (auto const & elem : vTempFileName)
		{
			for (auto & i : mvBehaviourReferences)
			{
				std::wstring name{ i.mName.begin() , i.mName.end() };
				if (FileSys->RemoveFileExtension(elem) == name)
				{
					delete i.mpBehaviour;
					i.mpBehaviour = nullptr;
				}
			}
			for (auto & i : mvBehaviours)
			{
				if (i.first == FileSys->RemoveFileExtension(elem))
				{
					for (auto & iter : i.second)
					{
						if (iter.second)
						{
							//iter.first = iter.second->GetOwner()->GetID();
							if (auto x = iter.second->GetOwner())
								x->RemoveComponent(iter.second);
							//iter.second->GetOwner()->RemoveComponent(iter.second);
							delete iter.second;
							iter.second = nullptr;
						}
					}
				}
			}
			hasChange = true;
		}

		if (hasChange)
			mHotloader->InitialiseTransfer();

		DLLWrapper * arr[100]{ nullptr };

		/*Check Hotloader for changes in the Dll file*/
		if (mHotloader->ChangesInDllFolder(100, arr))
		{
			DLLWrapper** start = arr;
			/*Loop through all the changes*/
			while (*start != nullptr)
			{
				std::wstring wName{ (*start)->GetDllName() };
				/**/
				std::string DllName = FileSys->RemoveFileExtension<std::string>(std::string(wName.begin(), wName.end()));

				bool found = false;

				for (auto & elem : mvBehaviourReferences)
				{
					/*If the name matches*/
					if (DllName == elem.mName)
					{
						delete elem.mpBehaviour;
						elem.mpBehaviour = nullptr;
						/*Reload the Dll*/
						mHotloader->ReloadModifiedDll(*start);
						/*Get pointer to the clone function*/
						using fpClone = Behaviour * (*) ();
						fpClone BehaviourClone = (*start)->GetDllFunc<Behaviour *>(DllName + "Clone");

						/*If Clone is valid, Means Dll Reload did not fail*/
						if (BehaviourClone)
						{
							elem.mpBehaviour = BehaviourClone();
							found = true;

							mvRecentChanges.Insert(&elem);
						}
					}
				}

				if (!found)
				{
					BehaviourWrap wrap;
					using fpClone = Behaviour * (*) ();
					const fpClone BehaviourClone = (*start)->GetDllFunc<Behaviour *>(DllName + "Clone");
					wrap.mName = DllName;
					wrap.mpBehaviour = (BehaviourClone());
					mvRecentChanges.Insert(mvBehaviourReferences.Emplace(Ut::Move(wrap)));
					AutoArray< BehaviourPair> temp;
					mvBehaviours.push_back(std::make_pair(std::wstring{ DllName.begin(), DllName.end() }, temp));
				}
				++start;
			}
		}
		for (auto & elem : mvRecentChanges)
		{
			std::wstring strName{ elem->mName.begin(), elem->mName.end() };
			for (auto & i : mvBehaviours)
			{
				if (i.first == strName)
				{
					for (auto & iter : i.second)
					{
						if (iter.second != nullptr)
							continue;

						auto SceneSys = EngineCore::GetInstance()->GetSystem<SceneSystem>();
						auto ptr = elem->mpBehaviour->Duplicate();
						if (auto x = SceneSys->FindGameObject(iter.first))
							x->AddComponent(ptr, BehaviourTag{});
						iter.second = ptr;
					}
				}
			}
		}

		

		vTempFileName.clear();
		mvRecentChanges.clear();

		if (hasSaveFile)
			EngineCore::GetInstance()->GetSystem<SceneSystem>()->LoadScene(FileSys->GetFullPath(EngineCore::GetInstance()->GetSystem<SceneSystem>()->GetCurrentScene().GetSceneName() + ".dscene", eFileDir::eResource));
	}
#endif
	void Dystopia::BehaviourSystem::Update(float _dt)
	{
#if EDITOR
		for (auto & i : mvBehaviours)
		{
			for (auto & iter : i.second)
			{
				if(iter.second)
				{
					iter.first = iter.second->GetOwnerID();
					if (auto p = iter.second->GetOwner())
					{
						if (p->GetFlag() & eObjFlag::FLAG_EDITOR_OBJ)
						{
							continue;
						}
						if (p->GetFlag() & eObjFlag::FLAG_ACTIVE)
						{
							_EDITOR_START_TRY
								iter.second->Update(_dt);
							_EDITOR_CATCH(std::exception& e)
							{
								_EDITOR_CODE(DEBUG_PRINT((eLog::WARNING), "Behaviour Error: %s!", e.what()));
								_EDITOR_CODE(p->RemoveComponent(iter.second));
								_EDITOR_CODE(iter.second->DestroyComponent());
							}
						}
					}
					else
					{
						iter.second->DestroyComponent();
					}
				}

			}
		}
#else
		for (auto & i : mvBehaviours)
		{
			for (auto & iter : i.second)
			{
				if (iter.second)
				{
					iter.first = iter.second->GetOwnerID();
					if (auto p = iter.second->GetOwner())
					{
						if (p->GetFlag() & eObjFlag::FLAG_EDITOR_OBJ)
						{
							continue;
						}
						if (p->GetFlag() & eObjFlag::FLAG_ACTIVE)
						{
							iter.second->Update(_dt);
						}
					}
					else
					{
						iter.second->DestroyComponent();
					}
				}

			}
		}
#endif
	}

	void Dystopia::BehaviourSystem::PostUpdate(void)
	{
#if EDITOR

		/*Clear the recently change*/
		mvRecentChanges.clear();

		for (auto & i : mvBehaviours)
		{
			for (auto & iter : i.second)
			{
				if(iter.second != nullptr)
					if (eObjFlag::FLAG_REMOVE & iter.second->GetFlags())
					{
						delete iter.second;
						iter.second = nullptr;
						i.second.FastRemove(&iter);
					}
			}
		}
#else

		for (auto & i : mvBehaviours)
		{
			for (auto & iter : i.second)
			{
				if (iter.second != nullptr)
					if (eObjFlag::FLAG_REMOVE & iter.second->GetFlags())
					{
						delete iter.second;
						iter.second = nullptr;
						i.second.FastRemove(&iter);
					}
			}
		}
#endif
	}

	void Dystopia::BehaviourSystem::Shutdown(void)
	{
#if EDITOR
		for (auto & i : mvBehaviours)
		{
			for (auto & iter : i.second)
			{
				delete iter.second;
			}
		}

		for (auto const & elem : mvBehaviourReferences)
			delete elem.mpBehaviour;
#else
		for (auto & i : mvBehaviours)
		{
			for (auto & iter : i.second)
			{
				delete iter.second;
			}
		}

		for (auto const & elem : mvBehaviourReferences)
			delete elem.mpBehaviour;

		for (auto & elem : mvDllInstance)
		{
			delete elem;
		}
#endif

	}

	void Dystopia::BehaviourSystem::LoadDefaults(void)
	{
	}	

	void Dystopia::BehaviourSystem::LoadSettings(TextSerialiser &)
	{
	}

	void Dystopia::BehaviourSystem::Serialise(TextSerialiser & _obj) const
	{
#if EDITOR
		_obj.InsertStartBlock("BEHAVIOUR_SYSTEM_BLOCK");
		/*Save number of Behaviours*/
		_obj << mvBehaviours.size();
		for (auto & i : mvBehaviours)
		{
			/*Save Behaviour Name*/
			auto * ptr = i.first.c_str();
			std::string str;
			while(*ptr != L'\0')
			{
				char c = static_cast<char>(*ptr);
				str += c;
				++ptr;
			}

			_obj.InsertStartBlock("str");
			_obj << str;
			/*Save the number of Pointers*/
			_obj << i.second.size();
			for (auto & iter : i.second)
			{
				_obj.InsertStartBlock("BEHAVIOUR_BLOCK");
				/*Owner ID*/
				_obj << iter.first;
				/*Save the Member Data*/
				if (iter.second)
				{
					auto && BehaviourMetaData = iter.second->GetMetaData();
					if (BehaviourMetaData)
					{
						_obj.InsertStartBlock("BEHAVIOUR_MEMBER_VARIABLE_BLOCK");
						auto Allnames = BehaviourMetaData.GetAllNames();
						for (auto names : Allnames)
						{
							if (BehaviourMetaData[names])
							{
								_obj << names;
								_obj.InsertStartBlock(names);
								BehaviourMetaData[names].Serialise(iter.second, _obj, BehaviourHelper::SuperSerialiseFunctor{});
								_obj.InsertStartBlock("MEMBER VAR");
							}
						}
						_obj << "END";
						_obj.InsertEndBlock("BEHAVIOUR_MEMBER_VARIABLE_BLOCK");
					}
				}
				_obj.InsertEndBlock("BEHAVIOUR_BLOCK");
			}
		}
		_obj.InsertEndBlock("BEHAVIOUR_SYSTEM_BLOCK");
#else
				_obj.InsertStartBlock("BEHAVIOUR_SYSTEM_BLOCK");
		/*Save number of Behaviours*/
		_obj << mvBehaviours.size();
		for (auto & i : mvBehaviours)
		{
			/*Save Behaviour Name*/
			auto * ptr = i.first.c_str();
			std::string str;
			while(*ptr != L'\0')
			{
				char c = static_cast<char>(*ptr);
				str += c;
				++ptr;
			}

			_obj.InsertStartBlock("str");
			_obj << str;
			/*Save the number of Pointers*/
			_obj << i.second.size();
			for (auto & iter : i.second)
			{
				_obj.InsertStartBlock("BEHAVIOUR_BLOCK");
				/*Owner ID*/
				_obj << iter.first;
				/*Save the Member Data*/
				if (iter.second)
				{
					auto && BehaviourMetaData = iter.second->GetMetaData();
					if (BehaviourMetaData)
					{
						_obj.InsertStartBlock("BEHAVIOUR_MEMBER_VARIABLE_BLOCK");
						auto Allnames = BehaviourMetaData.GetAllNames();
						for (auto names : Allnames)
						{
							if (BehaviourMetaData[names])
							{
								_obj << names;
								_obj.InsertStartBlock(names);
								BehaviourMetaData[names].Serialise(iter.second, _obj, BehaviourHelper::SuperSerialiseFunctor{});
								_obj.InsertStartBlock("MEMBER VAR");
							}
						}
						_obj << "END";
						_obj.InsertEndBlock("BEHAVIOUR_MEMBER_VARIABLE_BLOCK");
					}
				}
				_obj.InsertEndBlock("BEHAVIOUR_BLOCK");
			}
		}
		_obj.InsertEndBlock("BEHAVIOUR_SYSTEM_BLOCK");
#endif


	}

	void Dystopia::BehaviourSystem::Unserialise(TextSerialiser & _obj)
	{
#if EDITOR
		/*Clear all current Behaviour*/
		ClearAllBehaviours();
		/*Consume "BEHAVIOUR_SYSTEM BLOCK"*/
		_obj.ConsumeStartBlock();
		/*Get Number of Behaviour Scripts*/
		unsigned size = 0;
		_obj >> size;
		for (unsigned i = 0; i < size; ++i)
		{
			_obj.ConsumeStartBlock();
			std::string BehaviourScriptName;
			_obj >> BehaviourScriptName;
			for (auto const & elem : mvBehaviourReferences)
			{
				if (elem.mName == BehaviourScriptName)
				{
					unsigned NumOfBehaviour = 0;
					_obj >> NumOfBehaviour;
					for (unsigned u = 0; u < NumOfBehaviour; ++u)
					{
						/*Make new Behaviour*/
						_obj.ConsumeStartBlock(); /*Consume START BEHAVIOUR BLOCK*/
						uint64_t _ID = 0;
						_obj >> _ID;
						auto * ptr = RequestBehaviour(_ID, elem.mName);
						_obj.ConsumeStartBlock(); /*Consume Behaviour Member Variable Block*/
						auto BehaviourMetaData = ptr->GetMetaData();
						if (BehaviourMetaData)
						{

							std::string Var;
							_obj >> Var;
							while(Var != "END")
							{
								if (BehaviourMetaData[Var.c_str()])
								{
									_obj.ConsumeStartBlock();
									/*Call Unserialise*/
									BehaviourMetaData[Var.c_str()].Unserialise(ptr, _obj, BehaviourHelper::SuperUnserialiseFunctor{});
									_obj.ConsumeStartBlock();
								}
								else
								{
									_obj.ConsumeStartBlock();
									/*Call Unserialise*/
									_obj.ConsumeStartBlock();
								}
								_obj >> Var;
							}
						}
						_obj.ConsumeEndBlock();   /*Consume Behaviour Member Variable Block*/
						/*Time to Super Set Functor*/
						_obj.ConsumeEndBlock();  /*Consume END BEHAVIOUR BLOCK*/

						/*Insert to GameObject*/
						auto SceneSys = EngineCore::GetInstance()->GetSystem<SceneSystem>();
						if (auto x = SceneSys->GetActiveScene().FindGameObject(_ID))
						{
							ptr->SetOwner(x);
							x->AddComponent(ptr, BehaviourTag{});
						}
						else
						{
							/*GameObject with ID that was serialise could not be found*/
							/*Remove and delete the Behaviour from mvBehaviourReferences*/
							::Editor::EditorMain::GetInstance()->GetSystem<::Editor::EditorFactory>()->ReattachToPrefab(ptr, _ID, false);
						}
					}
					break;
				}
			}
		}
#else
		/*Clear all current Behaviour*/
		ClearAllBehaviours();
		/*Consume "BEHAVIOUR_SYSTEM BLOCK"*/
		_obj.ConsumeStartBlock();
		/*Get Number of Behaviour Scripts*/
		unsigned size = 0;
		_obj >> size;
		for (unsigned i = 0; i < size; ++i)
		{
			_obj.ConsumeStartBlock();
			std::string BehaviourScriptName;
			_obj >> BehaviourScriptName;
			for (auto const & elem : mvBehaviourReferences)
			{
				if (elem.mName == BehaviourScriptName)
				{
					unsigned NumOfBehaviour = 0;
					_obj >> NumOfBehaviour;
					for (unsigned u = 0; u < NumOfBehaviour; ++u)
					{
						/*Make new Behaviour*/
						_obj.ConsumeStartBlock(); /*Consume START BEHAVIOUR BLOCK*/
						uint64_t _ID = 0;
						_obj >> _ID;
						auto * ptr = RequestBehaviour(_ID, elem.mName);
						_obj.ConsumeStartBlock(); /*Consume Behaviour Member Variable Block*/
						auto BehaviourMetaData = ptr->GetMetaData();
						if (BehaviourMetaData)
						{

							std::string Var;
							_obj >> Var;
							while (Var != "END")
							{
								if (BehaviourMetaData[Var.c_str()])
								{
									_obj.ConsumeStartBlock();
									/*Call Unserialise*/
									BehaviourMetaData[Var.c_str()].Unserialise(ptr, _obj, BehaviourHelper::SuperUnserialiseFunctor{});
									_obj.ConsumeStartBlock();
								}
								else
								{
									_obj.ConsumeStartBlock();
									/*Call Unserialise*/
									_obj.ConsumeStartBlock();
								}
								_obj >> Var;
							}
						}
						_obj.ConsumeEndBlock();   /*Consume Behaviour Member Variable Block*/
												  /*Time to Super Set Functor*/
						_obj.ConsumeEndBlock();  /*Consume END BEHAVIOUR BLOCK*/

												 /*Insert to GameObject*/
						auto SceneSys = EngineCore::GetInstance()->GetSystem<SceneSystem>();
						if (auto x = SceneSys->GetActiveScene().FindGameObject(_ID))
						{
							ptr->SetOwner(x);
							x->AddComponent(ptr, BehaviourTag{});
						}
						else
						{
							/*GameObject with ID that was serialise could not be found*/
							/*Remove and delete the Behaviour from mvBehaviourReferences*/
							//::Editor::EditorMain::GetInstance()->GetSystem<::Editor::EditorFactory>()->ReattachToPrefab(ptr, _ID, false);
						}
					}
					break;
				}
			}
		}
#endif
	}

Behaviour * BehaviourSystem::RequestBehaviour(uint64_t const & _ID, std::string const & _name)
{
#if EDITOR
	for (auto & elem : mvBehaviourReferences)
	{
		if (elem.mName == _name)
		{
			for (auto & i : mvBehaviours)
			{
				if (i.first == std::wstring{ _name.begin(), _name.end() })
				{
					auto * ptr = elem.mpBehaviour->Duplicate();

					i.second.push_back(std::make_pair(_ID, ptr));
					return ptr;
				}
			}
		}
	}
	return nullptr;
#else
	for (auto & elem : mvBehaviourReferences)
	{
		if (elem.mName == _name)
		{
			for (auto & i : mvBehaviours)
			{
				if (i.first == _name)
				{
					auto * ptr = elem.mpBehaviour->Duplicate();

					i.second.push_back(std::make_pair(_ID, ptr));
					return ptr;
				}
			}
		}
	}
	return nullptr;

#endif
}

void Dystopia::BehaviourSystem::ClearAllBehaviours()
{
	for (auto & i : mvBehaviours)
	{
		for (auto & iter : i.second)
		{
			if (iter.second)
			{
				if (auto x = iter.second->GetOwner())
					x->RemoveComponent(iter.second);
				delete iter.second;
				iter.second = nullptr;
			}
		}
		i.second.clear();
	}
}

#if EDITOR

	MagicArray<BehaviourWrap*> const & Dystopia::BehaviourSystem::GetDllChanges() const
	{
		return mvRecentChanges;
	}
	bool BehaviourSystem::hasDllChanges() const
	{
		return !mvRecentChanges.IsEmpty();
	}
	MagicArray<BehaviourWrap> & BehaviourSystem::GetAllBehaviour()
	{
		return mvBehaviourReferences;
	}


	Behaviour* BehaviourSystem::RequestDuplicate(Behaviour* _PtrToDup, uint64_t _NewID)
	{
		for (auto & i : mvBehaviours)
		{
			for (auto & iter : i.second)
			{
				if (iter.second == _PtrToDup || (iter.first == _PtrToDup->GetOwnerID() && !std::strcmp(iter.second->GetBehaviourName(), _PtrToDup->GetBehaviourName())))
				{
					auto ptr = iter.second->Duplicate();
					i.second.push_back(std::make_pair(_NewID, ptr));
					//iter.first = _NewID;
					return ptr;
				}
			}
		}
		return nullptr;
	}

	void BehaviourSystem::ReplaceID(uint64_t _old, uint64_t _new, GameObject * _newOwner)
	{
		for (auto & i : mvBehaviours)
		{
			for (auto & iter : i.second)
			{
				if (iter.first == _old)
				{
					iter.first = _new;
					iter.second->SetOwner(_newOwner);
				}
			}
		}
	}



#endif
}