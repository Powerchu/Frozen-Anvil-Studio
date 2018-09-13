/* HEADER *********************************************************************************/
/*!
\file	PLogInfo.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _PLOG_INFO_H_
#define _PLOG_INFO_H_
#include "DataStructure/AutoArray.h"
#include "DataStructure/Array.h"

namespace Dystopia
{
	struct PLogData
	{
		static constexpr int maxLogs = 60;
		PLogData() {}
		PLogData(const std::string&, bool);
		PLogData(const PLogData&);
		PLogData(PLogData&&);
		PLogData& operator=(const PLogData&);
		PLogData& operator=(PLogData&&);

		void					UpdateLog(float _val, float _min, float _max);

		Array<float, maxLogs>	mArrValues;
		std::string				mLabel;
		int						mCurrentIndex;
		bool					mIsBigGraph;
		float					mMin;
		float					mMax;
	};

	struct PLogItem
	{
		PLogItem() {}
		PLogItem(const std::string&);
		PLogItem(const PLogItem&);
		PLogItem(PLogItem&&);
		PLogItem& operator=(const PLogItem&);
		PLogItem& operator=(PLogItem&&);
		~PLogItem();

		void					UpdateG(float _val, float _min, float _max, bool);
		void					UpdateLog(const std::string&, float _val, float _min, float _max, bool);
		void					InsertLog(const PLogData&);
		void					SortLogs();

		bool					mShowGeneric;
		PLogData				mGenericOverview;
		AutoArray<PLogData>		mData;
		std::string				mLabel;
	};
}

#endif	//_PLOG_INFO_H_
#endif // EDITOR