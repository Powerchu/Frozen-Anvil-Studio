#include "Component/Collider.h"
#include "Object/GameObject.h"
#include "Component/RigidBody.h"

namespace Dystopia
{
	
	/*Default Constructor*/
	AABB::AABB()
		:Convex{}
		, mfWidth{ 50 }
		, mfHeight{ 50 }
	{
		Math::Point3D ArrPoints[]
		{
			Math::Point3D{ mfWidth / 2, mfHeight / 2, 1, 0 }  ,
			Math::Point3D{ mfWidth / 2,-mfHeight / 2, 1, 0 }  ,
			Math::Point3D{ -mfWidth / 2,-mfHeight / 2, 1, 0 } ,
			Math::Point3D{ -mfWidth / 2, mfHeight / 2, 1, 0 } 
		};

		//TODO
		/*Remember to write own copy assignment so that you can reduce the number of constuct here*/
		Convex::operator=(AABB::Convex{ ArrPoints });
		mMin = &this->mVertices[2];
		mMax = &this->mVertices[0];
	}
	

	AABB::AABB(float const & _width, float const & _height, Math::Vec3D const & _v3Offset)
		: Convex{ _v3Offset }
		, mfWidth{ _width }
		, mfHeight{_height}
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

	//TODO: not yet
	/*float AABB::SweepingCheck(const AABB & other_col) const
	{
		if (GetOwner()->GetComponent<RigidBody>() == nullptr)
		{
			DEBUG_PRINT()
		}
	}*/

	bool AABB::isColliding(AABB & _ColB)
	{
		/*Static vs. Dynamic AABB Collision Check*/
		auto TransformA = GetOwner()->GetComponent<Transform>();
		auto TransformB = _ColB.GetOwner()->GetComponent<Transform>();

		if (   TransformA->GetGlobalPosition().x + this->mMin->mPosition.x <= TransformB->GetGlobalPosition().x + _ColB.mMax->mPosition.x
			&& TransformA->GetGlobalPosition().x + this->mMax->mPosition.x >= TransformB->GetGlobalPosition().x + _ColB.mMin->mPosition.x
			&& TransformA->GetGlobalPosition().y + this->mMax->mPosition.y >= TransformB->GetGlobalPosition().y + _ColB.mMin->mPosition.y
			&& TransformA->GetGlobalPosition().y + this->mMin->mPosition.y <= TransformB->GetGlobalPosition().y + _ColB.mMax->mPosition.y)
		{
			Colliding = true;
			return Colliding;
		}

		Colliding |= false;
		return false;
	}

	bool AABB::isColliding(AABB * const & _ColB)
	{
		return this->isColliding(*_ColB);
	}

	float AABB::GetWidth() const
	{
		return mfWidth;
	}

	float AABB::GetHeight() const
	{
		return mfHeight;
	}

	float AABB::GetHalfWidth() const
	{
		return mfWidth / 2;
	}

	float AABB::GetHalfHeight() const
	{
		return mfHeight / 2;
	}
}
