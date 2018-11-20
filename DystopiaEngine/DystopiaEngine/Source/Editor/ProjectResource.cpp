/* HEADER *********************************************************************************/
/*!
\file	ProjectResource.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
//#include "Editor/EditorEvents.h"
//#include "Editor/DefaultFactory.h"
#include "Editor/Payloads.h"
#include "Editor/EInput.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorFactory.h"

#include "System/Scene/Scene.h"
#include "System/Scene/SceneSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/Texture2D.h"
#include "System/File/FileSystem.h"
#include "System/Input/InputSystem.h"

#include "Object/GameObject.h"

#include "Allocator/DefaultAlloc.h"

#include <filesystem>
#include <algorithm>
#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <tchar.h>
#include <string>

//static const HashString DEFAULT_PATH = "..\\DystopiaEngine\\Resource";
//static const HashString DEFAULT_NAME = "Resource";
//static float delay = 5;

namespace Editor 
{
	ProjectResource::ProjectResource(void)
		: 
		mLabel{ "Project" }, mSearchText{ "" }, mSearchTextLastFrame{ "" }, mpRootFolder{ nullptr },
		mpCurrentFolder{ nullptr }, mArrAllFiles{100}, mArrFilesSearchedThisFrame{}, mArrFilesSearchedLastFrame{},
		mChangeHandle{}, mWaitStatus{}, mWaitFlags{}, mFocusedFile{ nullptr }, mPayloadRect{ 70, 90 },
		mResetToFile{}, mResourcePath{}, mResourceName{}
	{
		EditorPanel::SetOpened(true);
	}

	ProjectResource::~ProjectResource(void)
	{
	}

	void ProjectResource::Load(void)
	{
		mWaitFlags = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_LAST_WRITE;

		auto fs = Dystopia::EngineCore::GetInstance()->GetSubSystem<Dystopia::FileSystem>();
		auto resFolder = fs->GetProjectFolders<std::string>(Dystopia::eFileDir::eResource);
		size_t pos = resFolder.rfind("\\");
		if (pos == std::string::npos)
			pos = resFolder.rfind("/");

		mResourcePath = HashString{ resFolder.c_str() };
		mResourceName = HashString{ resFolder.c_str() + pos + 1, resFolder.c_str() + resFolder.size() };

		auto buf = Dystopia::DefaultAllocator<Editor::Folder>::Alloc();
		mpRootFolder = new (buf) Editor::Folder{ mResourceName , mResourcePath, nullptr }; 

		mpCurrentFolder = mpRootFolder;
		FullCrawl(mpRootFolder);

		mArrAllFiles.clear();
		GetAllFiles(mArrAllFiles, mpRootFolder);
		SortAllFiles(mArrAllFiles);

		std::wstring wPath{ resFolder.cbegin(), resFolder.cend() };
		mChangeHandle[0] = FindFirstChangeNotification(wPath.c_str(), true, mWaitFlags);
		if (mChangeHandle[0] == INVALID_HANDLE_VALUE || !mChangeHandle[0])
		{
			ExitProcess(GetLastError());
		}

		//for (auto& f : mArrAllFiles)
		//{
		//	if (f->mTag == EGUI::ePayloadTags::PNG || f->mTag == EGUI::ePayloadTags::BMP || f->mTag == EGUI::ePayloadTags::DDS)
		//	{
		//		Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::GraphicsSystem>()->LoadTexture(f->mPath.c_str());
		//	}
		//}
	}

	bool ProjectResource::Init(void)
	{
		for (auto& f : mArrAllFiles)
			if (f->mTag == EGUI::ePayloadTags::PREFAB)
				EditorMain::GetInstance()->GetSystem<EditorFactory>()->LoadAsPrefab(f->mPath);
		return true;
	}

	void ProjectResource::Update(float)
	{
		auto input = EditorMain::GetInstance()->GetSystem<EInput>();
		if (input->GetInputManager()->IsKeyTriggered(eButton::MOUSE_LEFT) ||
			input->GetInputManager()->IsKeyTriggered(eButton::MOUSE_RIGHT))
		{
			RemoveFocusOnFile();
		}

		mWaitStatus = WaitForMultipleObjects(1, mChangeHandle, false, 0);
		switch (mWaitStatus)
		{
		case WAIT_OBJECT_0:
			RefreshResourceFolder();
			if (!FindNextChangeNotification(mChangeHandle[0]))
			{
				ExitProcess(GetLastError());
			}
			break;
		case WAIT_FAILED:
			ExitProcess(GetLastError());
			break;
		}
		UpdateSearch();
	}

	void ProjectResource::EditorUI(void)
	{
		if (mResetToFile.length())
		{
			FocusOnFile(mResetToFile);
			mResetToFile.clear();
		}

		Math::Vec2 fileWindowSize = Math::Vec2{ Size().x - 210, Size().y - 55 };
		SearchWindow();
		EGUI::Display::OpenTreeNode();
		FolderWindow();
		EGUI::SameLine(2);
		ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4{ 0,0,0,0 });
		EGUI::StartChild("FileWindow", fileWindowSize);
		if (mpCurrentFolder)
		{
			auto origin = ImGui::GetCursorPos();
			EGUI::Display::Dummy(fileWindowSize.x, fileWindowSize.y);
			if (uint64_t *id = EGUI::Display::StartPayloadReceiver<uint64_t>(EGUI::GAME_OBJ))
			{
				Dystopia::GameObject *t = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::SceneSystem>()->GetCurrentScene().FindGameObject(*id);
				HashString fullPath = mpCurrentFolder->mPath;
				fullPath += "\\";

				HashString fileName = t->GetName().c_str();
				fileName += ".";
				fileName += Gbl::PREFAB_EXT;

				fullPath += fileName;

				FILE *pFile;
				fopen_s(&pFile, fullPath.c_str(), "a");
				fclose(pFile);

				auto serial = Dystopia::TextSerialiser::OpenFile(fullPath.c_str(), Dystopia::TextSerialiser::MODE_WRITE);
				if (EditorMain::GetInstance()->GetSystem<EditorFactory>()->SaveAsPrefab(*id, serial))
				{
					mResetToFile = fileName;
				}
				EGUI::Display::EndPayloadReceiver();
			}
			ImGui::SetCursorPos(origin);
			if (!strlen(mSearchText))
				FileWindow(fileWindowSize);
			else
				SearchResultWindow(fileWindowSize);
		}
		EGUI::EndChild();
		ImGui::PopStyleColor();
	}

	void ProjectResource::Shutdown(void)
	{
		mArrFilesSearchedThisFrame.clear();
		mArrFilesSearchedLastFrame.clear();
		mArrAllFiles.clear();
		Dystopia::DefaultAllocator<Editor::Folder>::DestructFree(mpRootFolder);
		mFocusedFile = nullptr;
		mpRootFolder = nullptr;
		mpCurrentFolder = nullptr;
	}

	void ProjectResource::Message(eEMessage)
	{}

	void ProjectResource::SaveSettings(Dystopia::TextSerialiser&) const
	{}

	void ProjectResource::LoadSettings(Dystopia::TextSerialiser&)
	{}

	HashString ProjectResource::GetLabel(void) const
	{
		return mLabel;
	}

	void ProjectResource::UpdateSearch(void)
	{
		HashString currentSearch = mSearchText;
		HashString previousSearch = mSearchTextLastFrame;
		if (currentSearch.length() && currentSearch != previousSearch)
		{
			mArrFilesSearchedThisFrame.clear();
			MakeStringLower(currentSearch);
			MakeStringLower(previousSearch);
			if ((currentSearch.find(previousSearch) != HashString::nPos) && mArrFilesSearchedLastFrame.size())
			{
				FindFile(mArrFilesSearchedThisFrame, currentSearch, mArrFilesSearchedLastFrame);
			}
			else
			{
				FindFile(mArrFilesSearchedThisFrame, currentSearch, mArrAllFiles);
			}
			mArrFilesSearchedLastFrame.clear();
			mArrFilesSearchedLastFrame = mArrFilesSearchedThisFrame;
			strcpy_s(mSearchTextLastFrame, mSearchText);
		}
		else if (!currentSearch.length() && previousSearch.length())
		{
			mArrFilesSearchedLastFrame.clear();
		}
	}

	void ProjectResource::SearchWindow(void)
	{
		float width = Size().x - 20;
		width = (width < 20) ? 20 : width;
		EGUI::Indent(4);
		EGUI::ChangeLabelSpacing(10);
		EGUI::Display::TextField("Search", mSearchText, MAX_SEARCH, false, width);
		EGUI::ChangeLabelSpacing();
		EGUI::UnIndent(4);
		EGUI::Display::HorizontalSeparator();
	}

	void ProjectResource::FolderWindow(void)
	{
		EGUI::StartChild("FolderWindow", Math::Vec2{ 200, Size().y - 55 });
		FolderUI(mpRootFolder);
		EGUI::EndChild();
	}

	void ProjectResource::FileWindow(const Math::Vec2& _mySize)
	{
		const Math::Vec2 buffedSize{ mPayloadRect.x * 1.25f, mPayloadRect.y * 1.5f };
		unsigned int columns = static_cast<unsigned int>(_mySize.x / buffedSize.x);
		columns = columns ? columns : 1 ;

		EGUI::Display::Label(mpCurrentFolder->mPath.c_str());
		EGUI::Display::HorizontalSeparator();
		for (unsigned int i = 0; i < mpCurrentFolder->mArrPtrFiles.size(); ++i)
		{
			Editor::File* pFile = mpCurrentFolder->mArrPtrFiles[i];
			HashString id = "ProjectResourceFileWindow" + pFile->mName;
			id  += i;
			if (i % columns) EGUI::SameLine();
			if (EGUI::StartChild(id.c_str(), buffedSize, false, Math::Vec4{ 0,0,0,0 }))
			{
				EGUI::Indent(10);
				EGUI::PushID(i);
				FileUI(pFile);
				EGUI::PopID();
				EGUI::UnIndent(10);
			}
			EGUI::EndChild();
		}
	}
	
	void ProjectResource::SearchResultWindow(const Math::Vec2& _mySize)
	{
		const Math::Vec2 buffedSize{ mPayloadRect.x * 1.25f, mPayloadRect.y * 1.5f };
		unsigned int columns = static_cast<unsigned int>(_mySize.x / buffedSize.x);
		columns = columns ? columns : 1;

		EGUI::Display::Label("Searching: %s", mSearchText);
		EGUI::Display::HorizontalSeparator();
		size_t size = mArrFilesSearchedThisFrame.size();
		if (size)
		{
			for (unsigned int i = 0; i < size; ++i)
			{
				Editor::File* pFile = mArrFilesSearchedThisFrame[i];
				HashString id = "ProjectResourceSearchResultWindow" + pFile->mName;
				id += i;
				if (i % columns) EGUI::SameLine();
				if (EGUI::StartChild(id.c_str(), buffedSize, false, Math::Vec4{ 0,0,0,0 }))
				{
					EGUI::Indent(10);
					EGUI::PushID(i);
					FileUI(pFile);
					EGUI::PopID();
					EGUI::UnIndent(10);
				}
				EGUI::EndChild();
			}
		}
		else
		{
			EGUI::Display::Label("%s cannot be found", mSearchText);
		}
	}

	void ProjectResource::RefreshResourceFolder(void)
	{
		if (strlen(mSearchText))
		{
			strcpy_s(mSearchText, "");
			strcpy_s(mSearchTextLastFrame, "");
		}
		HashString currentSelectionName = mpCurrentFolder ? mpCurrentFolder->mName : "";

		mArrAllFiles.clear();
		mArrFilesSearchedThisFrame.clear();
		mArrFilesSearchedLastFrame.clear();

		Dystopia::DefaultAllocator<Editor::Folder>::DestructFree(mpRootFolder); 
		mpRootFolder = Dystopia::DefaultAllocator<Editor::Folder>::ConstructAlloc(mResourceName, mResourcePath, nullptr);  

		FullCrawl(mpRootFolder);
		GetAllFiles(mArrAllFiles, mpRootFolder);
		SortAllFiles(mArrAllFiles);
		mpCurrentFolder = FindFolder(currentSelectionName);
	}

	void ProjectResource::FocusOnFile(const HashString& _fileName)
	{
		strcpy_s(mSearchText, ""); 
		strcpy_s(mSearchTextLastFrame, "");
		mArrFilesSearchedLastFrame.clear();
		mArrFilesSearchedThisFrame.clear(); 
		if (FindFirstOne(mArrFilesSearchedThisFrame, _fileName))
		{
			mpCurrentFolder = mArrFilesSearchedThisFrame[0]->mpParentFolder;
			mFocusedFile = mArrFilesSearchedThisFrame[0];
		}
		else
		{
			mpCurrentFolder = mpRootFolder;
			mFocusedFile = nullptr;
		}
	}

	void ProjectResource::RemoveFocusOnFile(void)
	{
		mFocusedFile = nullptr;
	}

	Editor::Folder* ProjectResource::FindFolder(const HashString& _name)
	{
		if (_name.length())
		{
			Editor::Folder* pFound = mpRootFolder->FindFolder(_name);
			if (pFound) 
				return pFound;
		}	
		return mpRootFolder;
	}

	void ProjectResource::FindFile(AutoArray<Editor::File*>& _outResult, HashString& _item, const AutoArray<Editor::File*>& _fromArr)
	{
		MakeStringLower(_item);
		for (auto& e : _fromArr)
		{
			if (!e->mLowerCaseName.find(_item))
				_outResult.push_back(e);
		}
	}

	bool ProjectResource::FindFirstOne(AutoArray<Editor::File*>& _outResult, const HashString& _item)
	{
		for (auto& e : mArrAllFiles)
		{
			if (!e->mName.find(_item))
			{
				_outResult.push_back(e);
				return true;
			}
		}
		return false;
	}

	void ProjectResource::GetAllFiles(AutoArray<Editor::File*>& _outResult, Editor::Folder* _folder)
	{
		for (auto& e : _folder->mArrPtrFiles)
			_outResult.push_back(e);
		for (auto& e : _folder->mArrPtrFolders)
			GetAllFiles(_outResult, e);
	}

	void ProjectResource::SortAllFiles(AutoArray<Editor::File*>& _outResult)
	{
		_outResult.Sort([](Editor::File* lhs, Editor::File* rhs)->bool { return *lhs < *rhs; });
	}

	void ProjectResource::FolderUI(Editor::Folder* _folder)
	{
		bool clickedThisFrame = false;
		bool clickedFolderIcon = false;
		bool flagIt = (mpCurrentFolder == _folder) ? true : false;
		bool hideArrow = _folder->mArrPtrFolders.size() ? false : true;
		if (EGUI::Display::IconFolder(_folder->mLowerCaseName.c_str(), 15, 9, flagIt))
		{
			clickedFolderIcon = true;
			EGUI::Display::OpenTreeNode();
		}
		EGUI::SameLine();
		if (EGUI::Display::StartTreeNode(_folder->mName.c_str(), &clickedThisFrame, flagIt, hideArrow))
		{
			for (auto& e : _folder->mArrPtrFolders)
				FolderUI(e);
			EGUI::Display::EndTreeNode();
		}
		if (clickedThisFrame || clickedFolderIcon)
		{
			mFocusedFile = nullptr;
			mpCurrentFolder = _folder;
		}
	}

	void ProjectResource::FileUI(Editor::File* _file)
	{
		if (_file == mFocusedFile) EGUI::Display::Outline(mPayloadRect.x, mPayloadRect.y);

		if (EGUI::Display::CustomPayload(("###ProjectView" + _file->mName).c_str(), _file->mName.c_str(), 
			_file->mName.c_str(), mPayloadRect, _file->mTag, &(*_file), sizeof(Editor::File)))
		{
			mFocusedFile = _file;
		}
	}

	void ProjectResource::FullCrawl(Editor::Folder* _folder)
	{
		_folder->Crawl();
		for (auto& e : _folder->mArrPtrFolders)
			FullCrawl(e);
	}

	void ProjectResource::MakeStringLower(HashString& _transformMe)
	{
		std::string temp{ _transformMe.cbegin(), _transformMe.cend() };
		std::string temp2 = temp;
		std::transform(temp.begin(), temp.end(), temp2.begin(), my_tolower);
		_transformMe = temp2.c_str();
	}

}

#endif //EDITOR

