#include "Component\Collider.h"
#include "Object\GameObject.h"
namespace Dystopia
{
	
	/*Default Constructor*/
	AABB::AABB()
		:Convex{}, mfWidth{ 1 }, mfHeight{ 1 }
	{
		static Math::Point3D ArrPoints[]
		{
			Math::Point3D{0.5f, 0.5f, 1, 0},
			Math::Point3D{0.5f, -0.5f, 1, 0 },
			Math::Point3D{-0.5f, -0.5f, 1, 0 },
			Math::Point3D{-0.5f, 0.5f, 1, 0 }
		};
		/*Remember to write own copy assignment so that you can reduce the number of constuct here*/
		Convex::operator=(AABB::Convex{ ArrPoints });
		mMin = &this->mVertices[2];
		mMax = &this->mVertices[0];
	}
	

	AABB::AABB(float const & _width, float const & _height, Math::Vec3D const & _v3Offset)
		:Convex{ _v3Offset },mfWidth{ _width }, mfHeight{_height}
	{
		Math::Point3D ArrPoints[]
		{
			Math::Point3D{ mfWidth/2, mfHeight/2, 1, 0 } +_v3Offset,
			Math::Point3D{ mfWidth/2,-mfHeight/2, 1, 0 } +_v3Offset,
			Math::Point3D{-mfWidth/2,-mfHeight/2, 1, 0 } +_v3Offset,
			Math::Point3D{-mfWidth/2, mfHeight/2, 1, 0 } +_v3Offset
		};
		Convex::operator=(AABB::Convex{ ArrPoints });
		mMin = &this->mVertices[2];
		mMax = &this->mVertices[0];
	}

	/*Load the Component*/
	void AABB::Load(void)
	{

	}
	/*Initialise the Component*/
	void AABB::Init(void)
	{

	}
	/*OnDestroy*/
	void AABB::OnDestroy(void)
	{

	}
	/*Unload the Component*/
	void AABB::Unload(void)
	{

	}
	/*Duplicate the Component*/
	AABB* AABB::Duplicate() const
	{
		return new AABB{};
	}

	/*Serialise and Unserialise*/
	void  AABB::Serialise(TextSerialiser&) const
	{

	}
	void  AABB::Unserialise(TextSerialiser&)
	{

	}
	bool AABB::isColliding(const AABB & _ColB)
	{

		/*Static AABB Collision Check*/
		if (_ColB.mMin->mPosition.x > mMax->mPosition.x)
			return false;
		else if (_ColB.mMin->mPosition.y > mMax->mPosition.y)
			return false;
		else if (_ColB.mMax->mPosition.x < mMin->mPosition.x)
			return false;
		else if (_ColB.mMax->mPosition.y < mMin->mPosition.y)
			return false;

		return true;
	}

	bool AABB::isColliding(const AABB * const & _ColB)
	{
		return this->isColliding(*_ColB);
	}
}