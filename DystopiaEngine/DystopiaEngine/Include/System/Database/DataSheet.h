/* HEADER *********************************************************************************/
/*!
\file	DataSheet.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _DATA_SHEET_H_
#define _DATA_SHEET_H_
#include "System/Database/DataSheetElement.h"

#include "DataStructure/HashString.h"
#include "DataStructure/AutoArray.h"

#include "Allocator/DefaultAlloc.h"

#include "Utility/Utility.h"

#include "Globals.h"

namespace Dystopia
{
	class TextSerialiser;
	class _DLL_EXPORT DataSheet
	{
	public:

		DataSheet(const HashString& _fullpath);
		~DataSheet(void);

		bool OpenSheet(void);
		bool CloseSheet(bool = true);
		bool IsOpened(void) const;
		HashString GetOpened(void) const;

		template<typename T, typename SFNAE = Ut::EnableIf_t<Ut::IsSame<T, int>::value ||
			Ut::IsSame<T, float>::value ||
			Ut::IsSame<T, bool>::value ||
			Ut::IsSame<T, char>::value ||
			Ut::IsSame<T, HashString>::value, T>>
			T GetElement(const HashString& _name);
		template<typename T, typename SFNAE = Ut::EnableIf_t<Ut::IsSame<T, int>::value ||
			Ut::IsSame<T, float>::value ||
			Ut::IsSame<T, bool>::value ||
			Ut::IsSame<T, char>::value ||
			Ut::IsSame<T, HashString>::value, T>>
			void SetElement(const HashString& _name, const T& _val);
		template<typename T, typename SFNAE = Ut::EnableIf_t<Ut::IsSame<T, int>::value ||
			Ut::IsSame<T, float>::value ||
			Ut::IsSame<T, bool>::value ||
			Ut::IsSame<T, char>::value ||
			Ut::IsSame<T, HashString>::value, T>>
			bool InsertElement(const HashString& _name, const T& _val);

	private:

		HashString mCurrentSheetPath;
		bool mbIsOpened;
		AutoArray<DataSheetElementBase*> mArrSheetElements;

		void SaveSheet(void);
		void LoadSheet(void);
		void DelegateSaveElement(unsigned, DataSheetElementBase*, TextSerialiser&);
		void DelegateLoadElement(unsigned, TextSerialiser&);

		template <unsigned N, class T = typename Ut::MetaExtract_t<N, AcceptableTypes>::type>
		void SaveElement(DataSheetElementBase*, TextSerialiser&);
		template <unsigned N, class T = typename Ut::MetaExtract_t<N, AcceptableTypes>::type>
		void LoadElement(TextSerialiser&);
	};
}


// ============================================ DEFINITIONS ============================================ // 

template<typename T, typename SFNAE>
inline T Dystopia::DataSheet::GetElement(const HashString & _name)
{
	for (const auto& elem : mArrSheetElements)
		if (elem->mName == _name)
			return (*static_cast<const DataSheetElement<T>*>(elem))();

	return T{};
}

template<typename T, typename SFNAE>
inline void Dystopia::DataSheet::SetElement(const HashString& _name, const T& _val)
{
	for (const auto& elem : mArrSheetElements)
		if (elem->mName == _name)
			(*static_cast<const DataSheetElement<T>*>(elem))(_val);
}

template<typename T, typename SFNAE>
inline bool Dystopia::DataSheet::InsertElement(const HashString & _name, const T & _val)
{
	if (!mbIsOpened)
		return false;

	for (const auto& elem : mArrSheetElements)
		if (elem->mName == _name)
			return false;
	
	auto ptr = DefaultAllocator<DataSheetElement<T>>::ConstructAlloc(_name, _val);
	mArrSheetElements.Insert(ptr);
	return true;
}

template<unsigned N, class T>
inline void Dystopia::DataSheet::SaveElement(DataSheetElementBase* _target, TextSerialiser & _out)
{
	_out << N;
	_out << _target->mName;
	_out << (*static_cast<DataSheetElement<T>*>(_target))();
}

template<unsigned N, class T>
inline void Dystopia::DataSheet::LoadElement(TextSerialiser & _in)
{
	HashString name;
	T val;

	_in >> name;
	_in >> val;

	InsertElement(name, val);
}


#endif 






