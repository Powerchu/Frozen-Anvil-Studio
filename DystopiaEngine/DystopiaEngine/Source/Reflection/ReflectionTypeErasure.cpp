/* HEADER *********************************************************************************/
/*!
\file	ReflectionTypeErasure.cpp
\author Goh Kai Jie Keith (100%)
\par    email: keith.goh\@digipen.edu
\brief


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Reflection/ReflectionTypeErasure.h"


Dystopia::TypeErasure::TypeEraseMetaData::TypeEraseMetaData()
	:mpWrapper(nullptr)
{
}

Dystopia::TypeErasure::TypeEraseMetaData::TypeEraseMetaData(TypeEraseMetaData const & _TypeEraseRhs)
	:mpWrapper{ nullptr }
{
	if (_TypeEraseRhs.mpWrapper)
		mpWrapper = _TypeEraseRhs.mpWrapper->Duplicate();
}

Dystopia::TypeErasure::TypeEraseMetaData::TypeEraseMetaData(TypeEraseMetaData && _TypeEraseRhs)
	:mpWrapper{ _TypeEraseRhs.mpWrapper }
{
	_TypeEraseRhs.mpWrapper = nullptr;
}


Dystopia::TypeErasure::TypeEraseMetaData Dystopia::TypeErasure::TypeEraseMetaData::operator=(Dystopia::TypeErasure::TypeEraseMetaData const & _TypeEraseRhs)
{
	delete mpWrapper;
	if (_TypeEraseRhs.mpWrapper)
		mpWrapper = _TypeEraseRhs.mpWrapper->Duplicate();
	else
		mpWrapper = nullptr;
	return *this;
}

Dystopia::TypeErasure::TypeEraseMetaData Dystopia::TypeErasure::TypeEraseMetaData::operator=(Dystopia::TypeErasure::TypeEraseMetaData && _TypeEraseRhs)
{
	Ut::Swap(mpWrapper, _TypeEraseRhs.mpWrapper);
	return *this;
}

Dystopia::TypeErasure::TypeEraseMetaData::~TypeEraseMetaData()
{
	
	delete mpWrapper;
	mpWrapper = nullptr;
	
}

Dystopia::TypeErasure::ReadWriteObject Dystopia::TypeErasure::TypeEraseMetaData::operator[](const char * _name)
{
	if(mpWrapper)
		return (*mpWrapper)[_name];
	return Dystopia::TypeErasure::ReadWriteObject{};
}

Dystopia::TypeErasure::ReadWriteObject Dystopia::TypeErasure::TypeEraseMetaData::operator[](const char * _name) const
{
	if (mpWrapper)
		return (*mpWrapper)[_name];
	return Dystopia::TypeErasure::ReadWriteObject{};
}

//std::map<char const*, Dystopia::TypeErasure::ReadWriteObject, CustomMetaComp> Dystopia::TypeErasure::TypeEraseMetaData::GetAllReflectedData()
//{
//	return mpWrapper->GetAllReflectedData();
//}
//
//std::map<char const*, Dystopia::TypeErasure::ReadWriteObject, CustomMetaComp> Dystopia::TypeErasure::TypeEraseMetaData::GetAllReflectedData() const
//{
//	return mpWrapper->GetAllReflectedData();
//}

AutoArray<const char*> Dystopia::TypeErasure::TypeEraseMetaData::GetAllNames() const
{
	return mpWrapper->GetAllNames();
}

Dystopia::TypeErasure::TypeEraseMetaData::operator bool() const
{
	return mpWrapper;
}

Dystopia::TypeErasure::TypeEraseMetaData::Concept* Dystopia::TypeErasure::TypeEraseMetaData::GetPointer()
{
	return mpWrapper;
}
