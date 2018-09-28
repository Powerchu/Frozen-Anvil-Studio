
#if EDITOR
#include "Editor/HotLoader.h"
#endif

#include "System/File/FileSystem.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Driver/Driver.h"

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

#if _DEBUG

		
		//std::string strDystopia_Lib{ FileSys->GetFullPath("DystopiaEngine_D.lib",eFileDir::eCurrent) };
		//InterestedFiles Dystopia_Lib{  ,eCompile };
		mHotloader->AddFilesToCrawl(L"DystopiaEngine_D.lib", eCompile);
		//mHotloader->AddAdditionalSourcePath(std::wstring{ strDystopia_Lib.begin(),strDystopia_Lib.end() });
#else
		mHotloader->AddFilesToCrawl(L"DystopiaEngine.lib", eCompile);
#endif

		mHotloader->SetDllFolderPath(FileSys->GetFullPath("BehaviourDLL", eFileDir::eAppData));

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
		auto & vFileChanges = mHotloader->GetFileChanges();
		for (auto & elem : vFileChanges)
		{
			for (auto & i : mvBehaviourReferences)
			{
				std::wstring name{ i.mName.begin() , i.mName.end() };
				if (FileSys->RemoveFileExtension(elem.mDllFileName) == name)
				{
					delete i.mpBehaviour;
					i.mpBehaviour = nullptr;
					break;
				}
			}
			mHotloader->CompileFiles(elem.mDirectoryIndex, elem.mFileName);
		}

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
				}

				++start;
			}
		}
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
}

