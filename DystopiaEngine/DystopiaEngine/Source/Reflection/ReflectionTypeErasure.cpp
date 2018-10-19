#include "Reflection/ReflectionTypeErasure.h"


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
