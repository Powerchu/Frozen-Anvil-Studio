
#if EDITOR
#include "Editor/HotLoader.h"
#endif

#include "System/File/FileSystem.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Scene/SceneSystem.h"
#include "System/Driver/Driver.h"
#include "Object/GameObject.h"

#include <utility>

namespace Dystopia
{
	BehaviourSystem::BehaviourSystem()
		:mHotloader{ CreateShared<Hotloader<1>>(new Hotloader<1>) }
	{

	}


	void Dystopia::BehaviourSystem::PreInit(void)
	{
#if EDITOR

		FileSys = EngineCore::GetInstance()->GetSubSystem<FileSystem>();

		std::wstring IncludeFolderPath = L"/I" + FileSys->GetProjectFolders<std::wstring>(eFileDir::eHeader);

		FileSys->CreateFiles("Dystopia/BehaviourDLL", eFileDir::eAppData);
		FileSys->CreateFiles("Dystopia/Temp", eFileDir::eAppData);

#if _DEBUG

		
		//std::string strDystopia_Lib{ FileSys->GetFullPath("DystopiaEngine_D.lib",eFileDir::eCurrent) };
		//InterestedFiles Dystopia_Lib{  ,eCompile };
		mHotloader->AddFilesToCrawl(L"DystopiaEngine_D.lib", eCompile);
		//mHotloader->AddAdditionalSourcePath(std::wstring{ strDystopia_Lib.begin(),strDystopia_Lib.end() });
#else
		mHotloader->AddFilesToCrawl(L"DystopiaEngine.lib", eCompile);
#endif

		mHotloader->SetDllFolderPath(FileSys->GetFullPath("BehaviourDLL", eFileDir::eAppData));
		mHotloader->SetTempFolder(FileSys->GetFullPath("Temp", eFileDir::eAppData));
		mHotloader->SetFileDirectoryPath<0>(FileSys->GetFullPath("BehaviourScripts", eFileDir::eResource));

		mHotloader->SetCompilerFlags(L"cl /W4 /EHsc /nologo /LD /DLL /DEDITOR /D_ITERATOR_DEBUG_LEVEL /std:c++17 " + IncludeFolderPath);

#else

#endif
	}

	bool Dystopia::BehaviourSystem::Init(void)
	{
		/*Init Hotloader*/
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
				mvBehaviourReferences.Emplace(Utility::Move(wrap));
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

	void Dystopia::BehaviourSystem::FixedUpdate(float)
	{
	}

	void Dystopia::BehaviourSystem::Update(float)
	{
#if EDITOR


		/*Update Hotloader*/
		static std::vector<std::wstring> vTempFileName;
		mHotloader->Update();
		mHotloader->ChangesInTempFolder(vTempFileName);
		bool hasChange = false;
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
							iter.first = iter.second->GetOwner()->GetID();
							iter.second->GetOwner()->RemoveComponent(iter.second);
							iter.second = nullptr;
						}
					}
				}
			}
			hasChange = true;
		}

		if(hasChange)
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
				std::string DllName = FileSys->RemoveFileExtension<std::string>(std::string(wName.begin(),wName.end()));

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
					fpClone BehaviourClone = (*start)->GetDllFunc<Behaviour *>(DllName + "Clone");
					wrap.mName = DllName;
					wrap.mpBehaviour = (BehaviourClone());
					mvRecentChanges.Insert(mvBehaviourReferences.Emplace(Utility::Move(wrap)));
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
						auto SceneSys = EngineCore::GetInstance()->GetSystem<SceneSystem>();
						auto ptr = elem->mpBehaviour->Duplicate();
						SceneSys->FindGameObject(iter.first)->AddComponent(ptr, BehaviourTag{});
						iter.second = ptr;
					}
				}
			}
		}

		for (auto & i : mvBehaviours)
		{
			for (auto & iter : i.second)
			{
				iter.second->Update(0.f);
			}
			
		}
		vTempFileName.clear();
#endif
	}

	void Dystopia::BehaviourSystem::PostUpdate(void)
	{
		/*Clear the recently change*/
		mvRecentChanges.clear();
	}

	void Dystopia::BehaviourSystem::Shutdown(void)
	{
		for (auto const & elem : mvBehaviourReferences)
		delete elem.mpBehaviour;
	}

	void Dystopia::BehaviourSystem::LoadDefaults(void)
	{
	}

	void Dystopia::BehaviourSystem::LoadSettings(TextSerialiser &)
	{
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
	Behaviour * BehaviourSystem::RequestBehaviour(uint64_t const & _ID, std::string const & _name)
	{
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
	}

#endif
}

