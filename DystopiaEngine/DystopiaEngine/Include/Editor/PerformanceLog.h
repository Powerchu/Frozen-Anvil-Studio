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
#include "Editor/EditorTab.h"
#include "Editor/PLogInfo.h"

namespace Dystopia
{
	class PerformanceLog : public EditorTab
	{
	public:
		static PerformanceLog* GetInstance();
		~PerformanceLog();

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

		void LogData(const std::string& _category, const std::string& _graphLabel, float _val, float _min, float _max, bool _bigGraph = false);
		void LogData(const std::string& _catMainGraph, float _val, float _min, float _max, bool _bigGraph = false);

	private:
		PerformanceLog();

		void					SortLogs();
		void					ShowLog(const PLogData&, Math::Vec2);

		AutoArray<PLogItem>		mArrLoggedData;
		std::string				mLabel;
		Math::Vec2				mGraphSizeB;
		Math::Vec2				mGraphSizeS;
		float					mGraphBigY;
		float					mGraphSmallY;
	};
}

#endif //_PERFORMANCE_LOG_H_
#endif //EDITOR

