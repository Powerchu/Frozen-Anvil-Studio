/* HEADER *********************************************************************************/
/*!
\file	ResourceView.h
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
#ifndef _RESOURCE_VIEW_H_
#define _RESOURCE_VIEW_H_
#include "DataStructure\AutoArray.h"
#include "EditorTab.h"
#include "Math\Vector4.h"
#include <string>
#include <vector>
 
static const std::string GLOBAL_DEFAULT_PROJECT_PATH = "..";

namespace Dystopia
{
	class ResourceView : public EditorTab
	{
	public:
		ResourceView();
		~ResourceView();

		virtual void Init() override;
		virtual void Update(const float&) override;
		virtual void Window() override;
		virtual void Shutdown() override;
		virtual std::string GetLabel() const override;

		void StartCrawl();
		struct CrawlFile
		{
			CrawlFile(const std::string&);
			std::string mFileName;
		};
		struct CrawlFolder
		{
			CrawlFolder(const char*);
			CrawlFolder(const std::string&);
			~CrawlFolder();

			CrawlFolder* GetParent();
			void SetParent(CrawlFolder*);
			void AddFolder(CrawlFolder*);
			void AddFile(const std::string&);
			void Crawl();
			void PrintAll();

			std::string mFolderName;
			CrawlFolder *mpParentFolder;
			AutoArray<CrawlFolder*> mArrChildFolders;
			AutoArray<CrawlFile> mArrFiles;
			bool mRefreshMe;
		};

	private:
		void FolderInterface(CrawlFolder*);
		void FileInterface(CrawlFile&);

		CrawlFolder *mpCrawlData;
		void		*mpFocusData;
		int			mLastSelected;
		bool		mRefreshCrawl;
		std::string mLabel;
	};
}

#endif //_RESOURCE_VIEW_H_
#endif //EDITOR





