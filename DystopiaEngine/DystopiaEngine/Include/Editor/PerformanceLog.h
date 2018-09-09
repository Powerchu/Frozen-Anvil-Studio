/* HEADER *********************************************************************************/
/*!
\file	PerformanceLog.cpp
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
#include "DataStructure/AutoArray.h"

namespace Dystopia
{
	struct PLogData
	{
		static constexpr int maxValueLogged = 60;
		PLogData(const std::string&, bool);
		PLogData(const PLogData&);
		PLogData& operator=(const PLogData&);

		void			UpdateLog(float);

		int				mCurrentIndex;
		float			mArrValues[maxValueLogged];
		bool			mIsBigGraph;
		std::string		mLabel;
	};

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

		void LogData(const std::string& _label, const float& _val, bool _bigGraph = false);

	private:
		PerformanceLog();

		AutoArray<PLogData>		mArrLoggedData;
		std::string				mLabel;
		std::string				mDefaultLog;
		Math::Vec2				mGraphSize;
		float					mGraphBigY;
		float					mGraphSmallY;
	};
}

#endif //_PERFORMANCE_LOG_H_
#endif //EDITOR

