/* HEADER *********************************************************************************/
/*!
\file	DataSheetElement.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _DATA_SHEET_ELEMENT_H_
#define _DATA_SHEET_ELEMENT_H_
#include "DataStructure/HashString.h"
#include "DataStructure/AutoArray.h"
#include "Utility/Utility.h"
#include "Utility/MetaAlgorithms.h"

static constexpr const char* AcceptableTypeStrings[] = { "int", "float", "bool", "char", "HashString" };
using AcceptableTypes = Ut::Collection< Ut::Indexer<0, int>, 
										Ut::Indexer<1, float>, 
										Ut::Indexer<2, bool>, 
										Ut::Indexer<3, char>, 
										Ut::Indexer<4, HashString>>;

struct DataSheetElementBase
{
	DataSheetElementBase(const HashString& _name) : mName{ _name } {}
	virtual ~DataSheetElementBase(void) {}
	virtual unsigned GetTypeN(void) = 0;
	HashString mName;
};

template<typename T>
struct DataSheetElement;

template<>
struct DataSheetElement<int> : DataSheetElementBase
{
	static constexpr unsigned N = Ut::MetaFind<int, AcceptableTypes>::result::value;
	virtual unsigned GetTypeN(void) { return N; }

	DataSheetElement(const HashString&, int);
	void operator()(int);
	int operator()(void) const;
	int mVal;
};
template<>
struct DataSheetElement<float> : DataSheetElementBase
{
	static constexpr unsigned N = Ut::MetaFind<float, AcceptableTypes>::result::value;
	virtual unsigned GetTypeN(void) { return N; }

	DataSheetElement(const HashString&, float);
	void operator()(float);
	float operator()(void) const;
	float mVal;
};
template<>
struct DataSheetElement<bool> : DataSheetElementBase
{
	static constexpr unsigned N = Ut::MetaFind<bool, AcceptableTypes>::result::value;
	virtual unsigned GetTypeN(void) { return N; }

	DataSheetElement(const HashString&, bool);
	void operator()(bool);
	bool operator()(void) const;
	bool mVal;
};
template<>
struct DataSheetElement<char> : DataSheetElementBase
{
	static constexpr unsigned N = Ut::MetaFind<char, AcceptableTypes>::result::value;
	virtual unsigned GetTypeN(void) { return N; }

	DataSheetElement(const HashString&, char);
	void operator()(char);
	char operator()(void) const;
	char mVal;
};
template<>
struct DataSheetElement<HashString> : DataSheetElementBase
{
	static constexpr unsigned N = Ut::MetaFind<HashString, AcceptableTypes>::result::value;
	virtual unsigned GetTypeN(void) { return N; }

	DataSheetElement(const HashString&, HashString);
	void operator()(const HashString&);
	HashString operator()(void) const;
	HashString mVal;
};

// ============================================ DEFINITIONS ============================================ // 

inline DataSheetElement<int>::DataSheetElement(const HashString & _name, int _val)
	: DataSheetElementBase{ _name }, mVal{ _val }
{
}
inline void DataSheetElement<int>::operator()(int _val)
{
	mVal = _val;
}
inline int DataSheetElement<int>::operator()() const
{
	return mVal;
}

inline DataSheetElement<float>::DataSheetElement(const HashString & _name, float _val)
	: DataSheetElementBase{ _name }, mVal{ _val }
{
}
inline void DataSheetElement<float>::operator()(float _val)
{
	mVal = _val;
}
inline float DataSheetElement<float>::operator()() const
{
	return mVal;
}

inline DataSheetElement<bool>::DataSheetElement(const HashString & _name, bool _val)
	: DataSheetElementBase{ _name }, mVal{ _val }
{
}
inline void DataSheetElement<bool>::operator()(bool _val)
{
	mVal = _val;
}
inline bool DataSheetElement<bool>::operator()() const
{
	return mVal;
}

inline DataSheetElement<char>::DataSheetElement(const HashString & _name, char _val)
	: DataSheetElementBase{ _name }, mVal{ _val }
{
}
inline void DataSheetElement<char>::operator()(char _val)
{
	mVal = _val;
}
inline char DataSheetElement<char>::operator()() const
{
	return mVal;
}

inline DataSheetElement<HashString>::DataSheetElement(const HashString & _name, HashString _val)
	: DataSheetElementBase{ _name }, mVal{ _val }
{
}
inline void DataSheetElement<HashString>::operator()(const HashString& _val)
{
	mVal = _val;
}
inline HashString DataSheetElement<HashString>::operator()() const
{
	return mVal;
}


#endif 










