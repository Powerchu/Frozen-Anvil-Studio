
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

		if(BehaviourClone)
			mvBehaviourReferences.Emplace(BehaviourClone());
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
}

void Dystopia::BehaviourSystem::PostUpdate(void)
{
}

void Dystopia::BehaviourSystem::Shutdown(void)
{
	for (auto const & elem : mvBehaviourReferences)
		delete elem;
}

void Dystopia::BehaviourSystem::LoadDefaults(void)
{
}

void Dystopia::BehaviourSystem::LoadSettings(TextSerialiser &)
{
}
