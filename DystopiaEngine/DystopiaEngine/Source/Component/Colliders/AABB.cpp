#include "Component/AABB.h"
#include "Component/RigidBody.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Scene/SceneSystem.h"
#include "Object/GameObject.h"
#include "IO/TextSerialiser.h"

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
		if (0 == this->mVertices.size())
		{
			Math::Point3D ArrPoints[]
			{
				Math::Point3D{ mfWidth / 2, mfHeight / 2, 1, 0 }  +mv3Offset,
				Math::Point3D{ mfWidth / 2,-mfHeight / 2, 1, 0 }  +mv3Offset,
				Math::Point3D{ -mfWidth / 2,-mfHeight / 2, 1, 0 } +mv3Offset,
				Math::Point3D{ -mfWidth / 2, mfHeight / 2, 1, 0 } +mv3Offset
			};
			Convex::operator=(AABB::Convex{ ArrPoints });
			mMin = &this->mVertices[2];
			mMax = &this->mVertices[0];
		}
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
		return static_cast<ComponentDonor<AABB> *>(EngineCore::GetInstance()->GetSystem<AABB::SYSTEM>())->RequestComponent(*this);
	}

	/*Serialise and Unserialise*/
	void  AABB::Serialise(TextSerialiser& _out) const
	{
		_out.InsertStartBlock("Box_Collider2D");
		_out << GetOwner()->GetID();					// gObj ID
		_out << mv3Offset[0];
		_out << mv3Offset[1];
		_out << mv3Offset[2];
		_out << mfHeight;
		_out << mfWidth;
		_out.InsertEndBlock("Box_Collider2D");

	}
	void  AABB::Unserialise(TextSerialiser& _in)
	{
		_in.ConsumeStartBlock();
		_in >> mID;
		_in >> mv3Offset[0];
		_in >> mv3Offset[1];
		_in >> mv3Offset[2];
		_in >> mfHeight;
		_in >> mfWidth;
		_in.ConsumeEndBlock();

		if (GameObject* owner =
			EngineCore::GetInstance()->GetSystem<SceneSystem>()->GetCurrentScene().FindGameObject(mID))
		{
			owner->AddComponent(this, AABB::TAG{});
			Init();
		}
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
			mbColliding = true;
			return mbColliding;
		}

		mbColliding |= false;
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
