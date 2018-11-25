/* HEADER *********************************************************************************/
/*!
\file	PerformanceLog.h
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
#ifndef _PERFORMANCE_LOG_H_
#define _PERFORMANCE_LOG_H_
//#include "Editor/EditorTab.h"
#include "Editor/EditorPanel.h"
#include "Editor/PLogInfo.h"

namespace Editor//Dystopia
{
	struct PLogTaskManager;

	class PerformanceLog : public EditorPanel//EditorTab
	{
	public:
		//static PerformanceLog* GetInstance();
		PerformanceLog(void);
		~PerformanceLog(void);

		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		HashString GetLabel(void) const;


		void LogData(const HashString& _category, const HashString& _graphLabel, float _val, bool _bigGraph = false);
		void LogData(const HashString& _catMainGraph, float _val, bool _bigGraph = false);
		void LogTaskMgr(const PLogTaskManager&);

	private:

		void					SortLogs(void);
		void					ShowLog(const PLogData&, Math::Vec2);
		void					ShowTaskMgrBreakdown(void);

		PLogTaskManager			mTaskMgrDetails;
		AutoArray<PLogItem>		mArrLoggedData;
		HashString				mLabel;
		Math::Vec2				mGraphSizeB;
		Math::Vec2				mGraphSizeS;
		float					mGraphBigY;
		float					mGraphSmallY;
	};
}

#endif //_PERFORMANCE_LOG_H_
#endif //EDITOR

