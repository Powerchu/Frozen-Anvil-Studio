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
#include "Editor\PLogger.h"
#include "Editor\PerformanceLog.h"
#include "Editor\ConsoleDebugger.h"
#include "Editor\EGUI.h"
#include "Math\MathUtility.h"
#include <algorithm>

namespace Dystopia
{
	static PerformanceLog* gpInstance = 0; // singleton
	namespace Performance
	{
		/* ===================================================== Performance Logger definitions ===================================================== */
		void LogDataS(const std::string& _category, const std::string& _graphLabel,
			float _val)
		{
			DEBUG_ASSERT(!gpInstance, "No instance of Performance Log found!");
			gpInstance->LogData(_category, _graphLabel, _val, false);
		}

		void LogDataG(const std::string& _catMainGraph, float _val)
		{
			DEBUG_ASSERT(!gpInstance, "No instance of Performance Log found!");
			gpInstance->LogData(_catMainGraph, _val, true);
		}

		void LogTaskMgr(const PLogTaskManager& _data)
		{
			DEBUG_ASSERT(!gpInstance, "No instance of Performance Log found!");
			gpInstance->LogTaskMgr(_data);
		}
	}

	/* ===================================================== The Performance Logger for handling items/datas ===================================================== */
	PerformanceLog* PerformanceLog::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new PerformanceLog{};
		return gpInstance;
	}

	PerformanceLog::PerformanceLog()
		: EditorTab{ false },
		mLabel{ "Performance" },
		mGraphSizeB{ Math::Vec2{0,0} },
		mGraphSizeS{ Math::Vec2{0,0} },
		mArrLoggedData{},
		mGraphBigY{ 50 },
		mGraphSmallY{ 20 },
		mTaskMgrDetails{}
	{}

	PerformanceLog::~PerformanceLog()
	{
		gpInstance = nullptr;
	}

	void PerformanceLog::Init()
	{
	}

	void PerformanceLog::Update(const float& _dt)
	{
		static constexpr float offset	= -80.f;
		mGraphSizeB.x					= Math::Clamp(Size().x + offset, 50.f, Size().x);
		mGraphSizeS.x					= mGraphSizeB.x;
		mGraphSizeS.x					= Math::Clamp(mGraphSizeS.x, 50.f, Size().x -120.f);
	}

	void PerformanceLog::EditorUI()
	{
		ShowTaskMgrBreakdown();
		for (const auto& item : mArrLoggedData)
		{
			if (item.mShowGeneric)
			{
				ShowLog(item.mGenericOverview, mGraphSizeB);
			}
			if (item.mData.size() && EGUI::Display::StartTreeNode(item.mLabel))
			{
				for (unsigned int i = 0; i < item.mData.size(); ++i)
				{
					ShowLog(item.mData[i], mGraphSizeS);
				}
				EGUI::Display::EndTreeNode();
			}
			EGUI::Display::HorizontalSeparator();
		}
	}

	void PerformanceLog::Shutdown()
	{
		mArrLoggedData.clear();
	}

	std::string PerformanceLog::GetLabel() const
	{
		return mLabel;
	}

	void PerformanceLog::LogData(const std::string& _cat, const std::string& _label, float _val, bool _bigGraph)
	{
		for (auto& item : mArrLoggedData)
		{
			if (item.mGenericOverview.mLabel == _cat)
			{
				item.UpdateLog(_label, _val, _bigGraph);
				return;		//early out to avoid adding new PLogData;
			}
		}
		// If never early out, means no log of that name is found
		PLogItem newItem{ _cat };
		newItem.UpdateLog(_label, _val, _bigGraph);
		mArrLoggedData.push_back(newItem);
		SortLogs();
	}

	void PerformanceLog::LogData(const std::string& _catMainGraph, float _val, bool _bigGraph)
	{
		for (auto& item : mArrLoggedData)
		{
			if (item.mGenericOverview.mLabel == _catMainGraph)
			{
				item.UpdateG(_val, _bigGraph);
				return;		//early out to avoid adding new PLogData;
			}
		}
		PLogItem newItem{ _catMainGraph };
		newItem.UpdateG(_val, _bigGraph);
		mArrLoggedData.push_back(newItem);
	}

	void PerformanceLog::LogTaskMgr(const PLogTaskManager& _data)
	{
		mTaskMgrDetails = _data;
	}

	void PerformanceLog::SortLogs()
	{
		mArrLoggedData.Sort([](const PLogItem& p1, const PLogItem& p2) { return p1.mLabel < p2.mLabel; });
	}

	void PerformanceLog::ShowLog(const PLogData& _log, Math::Vec2 _size)
	{
		_size.y = (_log.mIsBigGraph) ? mGraphBigY : mGraphSmallY;
		EGUI::Display::LineGraph(_log.mLabel.c_str(), _log.mArrValues, 0, _log.mMax, _size,
						std::to_string(static_cast<int>(_log.mArrValues[_log.mCurrentIndex])));
	}

	void PerformanceLog::ShowTaskMgrBreakdown()
	{
		static constexpr float sizeY = 170;
		Math::Vec2 size{Size().x - 7.f, sizeY };
		if (EGUI::StartChild("Task Manager Breakdown", size))
		{
			EGUI::Display::Label("CPU Idle         : %f", mTaskMgrDetails.mCPUIdle);
			EGUI::Display::Label("CPU Busy         : %f", mTaskMgrDetails.mCPUBusy);
			EGUI::Display::Label("CPU OS           : %f", mTaskMgrDetails.mCPUOS);
			EGUI::Display::Label("CPU Proc         : %f", mTaskMgrDetails.mCPUProc);
			EGUI::Display::Label("Page Faults      : %d", mTaskMgrDetails.mPageFaults);
			EGUI::Display::Label("Memory Used      : %d", mTaskMgrDetails.mMemUsed);
			EGUI::Display::Label("RAM Used         : %d", mTaskMgrDetails.mRamUsed);
			EGUI::Display::Label("Memory Available : %d", mTaskMgrDetails.mMemAvail);
			EGUI::Display::Label("Memory Load      : %f", mTaskMgrDetails.mMemLoad);
		}
		EGUI::EndChild();
	}
}


#endif //EDITOR

