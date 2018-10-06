/* HEADER *********************************************************************************/
/*!
\file	Clipboard.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _CLIP_BOARD_H_
#define _CLIP_BOARD_H_
#include "DataStructure/AutoArray.h"
#include <string>

namespace Dystopia
{
	enum eClipDataTypes
	{
		eCLIP_GAME_OBJECT = 0,
		eCLIP_VALUES,
		eCLIP_COMPONENTS
	};

	class ClipData
	{
	public:
		ClipData(eClipDataTypes _dataType, void* _data, const size_t _dataSize);
		~ClipData(void);

		void* GetData();
		bool IsSameType(eClipDataTypes _dataType) const;
		bool IsSameSize(const size_t& _dataSize) const;

		bool operator==(const ClipData& _rhs) const;

	private:
		void			*mpData;
		size_t			mDataSize;
		eClipDataTypes	mDataType;
	};

	class Clipboard
	{
	public: 
		Clipboard(void);
		~Clipboard(void);

		void Close(void);
		void ClearSelection(void);
		void ClearData(void);

		void InsertSelection(const uint64_t & _uniqueID);
		void InsertData(eClipDataTypes, void* _data, const size_t _dataSize);

		AutoArray<void*> RetrieveDatas(eClipDataTypes, const size_t& _dataSize) const;
		const AutoArray<uint64_t>& RetrieveSelections(void) const;

	private:

		AutoArray<uint64_t> mArrSelectionID;
		AutoArray<ClipData> mArrClipData;
	};
}


#endif //_CLIPBOARD_H_
#endif //EDITOR


