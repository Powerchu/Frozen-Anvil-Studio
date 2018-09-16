
#include "System/File/FileSystem.h"
#include "System/Behaviour/BehaviourSystem.h"
#include "System/Driver/Driver.h"

void Dystopia::BehaviourSystem::PreInit(void)
{
#if EDITOR

	FileSystem * FileSys = EngineCore::GetInstance()->GetSubSystem<FileSystem>();

	std::wstring IncludeFolderPath = L"/I" + FileSys->GetProjectFolders<std::wstring>(eFileDir::eHeader);

	FileSys->CreateFiles("Dystopia/BehaviourDLL", eFileDir::eAppData);

#if _DEBUG
	mHotloader.AddFilesToCrawl(L"DystopiaEngine_D.lib",eCompile);
#else
	mHotloader.AddFilesToCrawl(L"DystopiaEngine.lib"  ,eCompile);
#endif
	
	mHotloader.SetDllFolderPath(FileSys->GetFullPath("BehaviourDLL", eFileDir::eAppData));

	mHotloader.SetFileDirectoryPath<0>(FileSys->GetFullPath("BehaviourScripts",eFileDir::eResource));

	mHotloader.SetCompilerFlags(L"cl /W4 /EHsc /nologo /LD /DLL /std:c++latest " + IncludeFolderPath);

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
			wrap.mpBehaviour = CreateShared<Behaviour>(BehaviourClone());
			mvBehaviourReferences.Emplace(wrap);
		}
			//mvBehaviourReferences.Emplace(BehaviourClone());
	}
#endif
	
	return false;
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
	DLLWrapper* arr[100]{ nullptr };
	/*Check Hotloader for changes in the Dll file*/
	if (mHotloader.ChangesInDllFolder(100, arr))
	{
		FileSystem * FileSys = EngineCore::GetInstance()->GetSubSystem<FileSystem>();
		DLLWrapper** start = arr;
		while (start != nullptr)
		{
			std::string DllName = std::string{ (*start)->GetDllName().begin(), (*start)->GetDllName().end() };
			for (auto & elem : mvBehaviourReferences)
			{
				if (DllName == elem.mName)
				{
					using fpClone = Behaviour * (*) ();
					fpClone BehaviourClone = (*start)->GetDllFunc<Behaviour *>(FileSys->RemoveFileExtension<std::wstring>((*start)->GetDllName()) + L"Clone");
					if (BehaviourClone)
						elem.mpBehaviour = CreateShared<Behaviour>(BehaviourClone());
				}
					
			}
			++start;
		}
	}

#endif
}

void Dystopia::BehaviourSystem::PostUpdate(void)
{
}

void Dystopia::BehaviourSystem::Shutdown(void)
{
	//for (auto const & elem : mvBehaviourReferences)
		//delete elem;
}

void Dystopia::BehaviourSystem::LoadDefaults(void)
{
}

void Dystopia::BehaviourSystem::LoadSettings(TextSerialiser &)
{
}
