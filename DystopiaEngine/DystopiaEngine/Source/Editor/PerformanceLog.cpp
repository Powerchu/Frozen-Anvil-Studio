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
#include "Editor\PerformanceLog.h"
#include "Editor\EGUI.h"
#include <algorithm>

namespace Dystopia
{
	PLogData::PLogData(const std::string& _name, bool _big)
		: mLabel{ _name }, mArrValues{ 0 }, mCurrentIndex{ 0 }, mIsBigGraph{ _big }
	{}

	PLogData::PLogData(const PLogData& _rhs)
		: mLabel{ _rhs.mLabel }, mArrValues{ 0 }, mCurrentIndex{ _rhs.mCurrentIndex }, mIsBigGraph{ _rhs.mIsBigGraph }
	{
		for (unsigned int i = 0; i < maxValueLogged; ++i)
			mArrValues[i] = _rhs.mArrValues[i];
	}

	PLogData& PLogData::operator=(const PLogData& _rhs)
	{
		mCurrentIndex = _rhs.mCurrentIndex;
		for (unsigned int i = 0; i < maxValueLogged; ++i)
			mArrValues[i] = _rhs.mArrValues[i];
		mLabel = _rhs.mLabel;
		mIsBigGraph = _rhs.mIsBigGraph;
		return *this;
	}

	void PLogData::UpdateLog(float _val)
	{
		mArrValues[mCurrentIndex] = _val;
		if (mCurrentIndex != (maxValueLogged - 1)) mCurrentIndex++;
		else std::rotate(&mArrValues[0], &mArrValues[1], &mArrValues[maxValueLogged]);
	}

	static PerformanceLog* gpInstance = 0;
	PerformanceLog* PerformanceLog::GetInstance()
	{
		if (gpInstance) return gpInstance;

		gpInstance = new PerformanceLog{};
		return gpInstance;
	}

	PerformanceLog::PerformanceLog()
		: EditorTab{ false },
		mLabel{ "Performance" },
		mGraphSize{ Math::Vec2{0,0} },
		mArrLoggedData{},
		mDefaultLog{ "Editor FR" },
		mGraphBigY{ 100 },
		mGraphSmallY{ 40 }
	{}

	PerformanceLog::~PerformanceLog()
	{
		gpInstance = nullptr;
	}

	void PerformanceLog::Init()
	{
		LogData(mDefaultLog, 0);
	}

	void PerformanceLog::Update(const float& _dt)
	{
		static constexpr float offset	= -15.f;
		mGraphSize.x					= Size().x + offset;
		LogData(mDefaultLog, _dt);
	}

	void PerformanceLog::EditorUI()
	{
		for (unsigned int i = 0; i < mArrLoggedData.size(); ++i)
		{
			const PLogData& e = mArrLoggedData[i];
			mGraphSize.y = (e.mIsBigGraph) ? mGraphBigY : mGraphSmallY;
			EGUI::Indent(5);
			EGUI::Display::LineGraph(e.mLabel.c_str(), e.mArrValues, 0.f, 0.1f, mGraphSize,
									 std::to_string(e.mArrValues[e.maxValueLogged - 1]));
			EGUI::UnIndent(5);
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

	void PerformanceLog::LogData(const std::string& _label, const float& _val, bool _bigGraph)
	{
		for (auto& e : mArrLoggedData)
		{
			if (e.mLabel == _label)
			{
				e.UpdateLog(_val);
				return;		//early out to avoid adding new PLogData;
			}
		}
		// If never early out, means no log of that name is found
		PLogData newLog{ _label, _bigGraph };
		newLog.UpdateLog(_val);
		mArrLoggedData.push_back(newLog);
	}
}


#endif //EDITOR

