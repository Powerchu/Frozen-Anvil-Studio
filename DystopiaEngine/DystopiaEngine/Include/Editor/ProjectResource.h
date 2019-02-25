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
#ifndef _PROJECT_RESOURCE_H_
#define _PROJECT_RESOURCE_H_
//#include "EditorTab.h"
#include "Editor/Payloads.h"
#include "Editor/EditorPanel.h"

typedef void *HANDLE;
typedef unsigned long DWORD;

namespace Editor
{
	/****************************************************************** PROJECT RESOURCE ********************************************************************/

	class ProjectResource : public EditorPanel //EditorTab
	{
	public:
		//static ProjectResource* GetInstance();
		ProjectResource(void);
		~ProjectResource(void);

		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		HashString GetLabel(void) const;

		void FocusOnFile(const HashString& _fileName);
		void RemoveFocusOnFile();

	private:
		const Editor::File*			mFocusedFile;
		AutoArray<Editor::File*>	mArrAllFiles;
		AutoArray<Editor::File*>	mArrFilesSearchedThisFrame;
		AutoArray<Editor::File*>	mArrFilesSearchedLastFrame;
		HashString					mLabel;
		HashString					mResetToFile;
		char						mSearchText[MAX_SEARCH];
		char						mSearchTextLastFrame[MAX_SEARCH];
		Editor::Folder				*mpRootFolder;
		Editor::Folder				*mpCurrentFolder;
		HANDLE						mChangeHandle[1];
		DWORD						mWaitStatus;
		DWORD						mWaitFlags;
		Math::Vec2					mPayloadRect;
		HashString					mResourcePath;
		HashString					mResourceName;

		bool				FindFirstOne(AutoArray<Editor::File*>& _outResult, const HashString& _item);
		void				FindFile(AutoArray<Editor::File*>& _outResult, HashString& _item, const AutoArray<Editor::File*>& _fromArr);
		void				GetAllFiles(AutoArray<Editor::File*>& _outResult, Editor::Folder*);
		void				SortAllFiles(AutoArray<Editor::File*>& _outResult);
		Editor::Folder*		FindFolder(const HashString& _name);
		void				FullCrawl(Editor::Folder*);
		void				FolderUI(Editor::Folder*);
		void				FileUI(Editor::File*);
		void				SearchWindow();
		void				FolderWindow();
		void				FileWindow(const Math::Vec2& _mySize);
		void				SearchResultWindow(const Math::Vec2& _mySize);
		void				MakeStringLower(HashString& _transformMe);
		void				UpdateSearch();
		void				RefreshResourceFolder();

		void				ShaderFolderUI(unsigned, const Math::Vec2&);
	};
}

#endif //_PROJECT_RESOURCE_H_
#endif //EDITOR




