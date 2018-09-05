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
#include "EditorTab.h"
#include "DataStructure\AutoArray.h"

typedef void *HANDLE;
typedef unsigned long DWORD;

namespace Dystopia
{
	/********************************************************************* FILE & FOLDER *********************************************************************/

	struct File;
	struct CrawlItem
	{
		CrawlItem(const std::string& _name, const std::string& _path);
		std::string mName;
		std::string mPath;
		std::string mLowerCaseName;
	};

	struct Folder : CrawlItem
	{
		Folder(const std::string& _name, const std::string& _path, Folder * const _parent);
		~Folder();

		void				Crawl();
		void				ClearFolder();

		Folder*				mpParentFolder;
		AutoArray<File*>	mArrPtrFiles;
		AutoArray<Folder*>	mArrPtrFolders;
		bool				mToggle;
	};

	struct File : CrawlItem
	{
		File(const std::string& _name, const std::string& _path, Folder * const _parent);
		~File();
		bool operator<(const File&);

		Folder*	mpParentFolder;
	};

	/****************************************************************** PROJECT RESOURCE ********************************************************************/

	class ProjectResource : public EditorTab
	{
	public:
		static ProjectResource* GetInstance();
		~ProjectResource();

		/* Init() is called immediately after the creation of the object */
		virtual void Init() override;

		/* Update() is called before Window(), so alter most variables (frame based) here to be printed in Window() later */
		virtual void Update(const float&) override;

		/* Window() is where you do the EGUI/IMGUI functions. GUI variable changes will be recorded here */
		virtual void EditorUI() override;

		/* Shutdown() is called right before deleting this object */
		virtual void Shutdown() override;

		/* GetLabel() returns the string to identify this class. EditorTab requires this to create a tab for you using the label */
		virtual std::string GetLabel() const override;

		void FocusOnFile(const std::string& _fileName);
		void RemoveFocusOnFile();

	private:
		ProjectResource(void);

		const File*			mFocusedFile;
		AutoArray<File*>	mArrAllFiles;
		AutoArray<File*>	mArrFilesSearchedThisFrame;
		AutoArray<File*>	mArrFilesSearchedLastFrame;
		std::string			mLabel;
		char				mSearchText[MAX_SEARCH];
		char				mSearchTextLastFrame[MAX_SEARCH];
		Folder				*mpRootFolder;
		Folder				*mpCurrentFolder;
		HANDLE				mChangeHandle[1];
		DWORD				mWaitStatus;
		DWORD				mWaitFlags;
		Math::Vec2			mPayloadRect;

		bool				FindFirstOne(AutoArray<File*>& _outResult, const std::string& _item);
		void				FindFile(AutoArray<File*>& _outResult, std::string& _item, const AutoArray<File*>& _fromArr);
		void				GetAllFiles(AutoArray<File*>& _outResult, Folder*);
		void				SortAllFiles(AutoArray<File*>& _outResult);
		Folder*				FindFolder(const std::string& _name);
		void				FullCrawl(Folder*);
		void				FolderUI(Folder*);
		void				FileUI(File*);
		void				SearchWindow();
		void				FolderWindow();
		void				FileWindow(const Math::Vec2& _mySize);
		void				SearchResultWindow(const Math::Vec2& _mySize);
		void				MakeStringLower(std::string& _transformMe);
		void				UpdateSearch();
		void				RefreshResourceFolder();
	};
}

#endif //_PROJECT_RESOURCE_H_
#endif //EDITOR




