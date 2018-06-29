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
#include "Math\Vector4.h"
#include <string>
#include <vector>
 
static const std::string GLOBAL_DEFAULT_PROJECT_PATH = "..";

namespace Dystopia
{
	class ResourceView
	{
	public:
		ResourceView();
		~ResourceView();

		void Init();
		void Update(const float&);
		void Shutdown();

		void StartCrawl();
		void Window();
		void SetWidth(const float&);
		void SetHeight(const float&);
		void SetSize(const Math::Vec4& );
		void SetSize(const float&, const float&);

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
		float		mWidth;
		float		mHeight;
	};
}

#endif //_RESOURCE_VIEW_H_
#endif //EDITOR





