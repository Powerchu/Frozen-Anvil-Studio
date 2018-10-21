#include "Reflection/ReflectionTypeErasure.h"


Dystopia::TypeErasure::TypeEraseMetaData::TypeEraseMetaData()
	:mpWrapper(nullptr)
{
}

Dystopia::TypeErasure::TypeEraseMetaData::TypeEraseMetaData(TypeEraseMetaData const & _TypeEraseRhs)
	:mpWrapper{ _TypeEraseRhs.mpWrapper->Duplicate() }
{

}

Dystopia::TypeErasure::TypeEraseMetaData::TypeEraseMetaData(TypeEraseMetaData && _TypeEraseRhs)
	:mpWrapper{ _TypeEraseRhs.mpWrapper }
{

}

Dystopia::TypeErasure::TypeEraseMetaData::~TypeEraseMetaData()
{
	
	delete mpWrapper;
	mpWrapper = nullptr;
	
}

Dystopia::TypeErasure::ReadWriteObject & Dystopia::TypeErasure::TypeEraseMetaData::operator[](std::string const & _name)
{
	return (*mpWrapper)[_name];
}

Dystopia::TypeErasure::ReadWriteObject const & Dystopia::TypeErasure::TypeEraseMetaData::operator[](std::string const & _name) const
{
	return (*mpWrapper)[_name];
}

std::map<char const*, Dystopia::TypeErasure::ReadWriteObject>& Dystopia::TypeErasure::TypeEraseMetaData::GetAllReflectedData()
{
	return mpWrapper->GetAllReflectedData();
}

std::map<char const*, Dystopia::TypeErasure::ReadWriteObject> const& Dystopia::TypeErasure::TypeEraseMetaData::GetAllReflectedData() const
{
	return mpWrapper->GetAllReflectedData();
}

Dystopia::TypeErasure::TypeEraseMetaData::operator bool() const
{
	return mpWrapper;
}

Dystopia::TypeErasure::TypeEraseMetaData::Concept* Dystopia::TypeErasure::TypeEraseMetaData::GetPointer()
{
	return mpWrapper;
}
