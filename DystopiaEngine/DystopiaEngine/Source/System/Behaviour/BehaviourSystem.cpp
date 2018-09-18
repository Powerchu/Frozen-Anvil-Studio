
#if EDITOR
#include "Editor/HotLoader.h"
#endif

#include "System/File/FileSystem.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Driver/Driver.h"

namespace Dystopia
{
	void Dystopia::BehaviourSystem::PreInit(void)
	{
#if EDITOR

		FileSystem * FileSys = EngineCore::GetInstance()->GetSubSystem<FileSystem>();

		std::wstring IncludeFolderPath = L"/I" + FileSys->GetProjectFolders<std::wstring>(eFileDir::eHeader);

		FileSys->CreateFiles("Dystopia/BehaviourDLL", eFileDir::eAppData);

#if _DEBUG
		mHotloader.AddFilesToCrawl(L"DystopiaEngine_D.lib", eCompile);
#else
		mHotloader.AddFilesToCrawl(L"DystopiaEngine.lib", eCompile);
#endif

		mHotloader.SetDllFolderPath(FileSys->GetFullPath("BehaviourDLL", eFileDir::eAppData));

		mHotloader.SetFileDirectoryPath<0>(FileSys->GetFullPath("BehaviourScripts", eFileDir::eResource));

		mHotloader.SetCompilerFlags(L"cl /W4 /EHsc /nologo /LD /DLL /DEDITOR /std:c++latest " + IncludeFolderPath);

#else

#endif
	}

	bool Dystopia::BehaviourSystem::Init(void)
	{
		/*Init Hotloader*/
#if EDITOR

		FileSystem * FileSys = EngineCore::GetInstance()->GetSubSystem<FileSystem>();
		mHotloader.Init();
		auto const & ArrayDlls = mHotloader.GetDlls();
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
		mHotloader.Update();
		static DLLWrapper * arr[100]{ nullptr };

		/*Check Hotloader for changes in the Dll file*/
		if (mHotloader.ChangesInDllFolder(100, arr))
		{
			FileSystem * FileSys = EngineCore::GetInstance()->GetSubSystem<FileSystem>();
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
						/*Get pointer to the clone function*/
						using fpClone = Behaviour * (*) ();
						fpClone BehaviourClone = (*start)->GetDllFunc<Behaviour *>(FileSys->RemoveFileExtension<std::wstring>((*start)->GetDllName()) + L"Clone");
						if (BehaviourClone)
							elem.mpBehaviour = (BehaviourClone());
						found = true;

						mvRecentChanges.Insert(&elem);
					}
					/*Insert New BehaviourScript*/
				}

				if (!found)
				{
					BehaviourWrap wrap;
					using fpClone = Behaviour * (*) ();
					fpClone BehaviourClone = (*start)->GetDllFunc<Behaviour *>(FileSys->RemoveFileExtension<std::wstring>((*start)->GetDllName()) + L"Clone");
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
}

