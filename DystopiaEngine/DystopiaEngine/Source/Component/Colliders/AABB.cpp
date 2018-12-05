#include "Component/AABB.h"
#include "Component/RigidBody.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Scene/SceneSystem.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"
#include "IO/TextSerialiser.h"
#include "Editor/EGUI.h"
#include "System/Graphics/VertexDefs.h"

#if EDITOR
#endif 

namespace Dystopia
{
	/*Default Constructor*/
	AABB::AABB()
		: mfWidth{ 50 }
		, mfHeight{ 50 }
	{
		mVertices.clear();

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
		mVertices.clear();

		Math::Point3D ArrPoints[]
		{
			Math::Point3D{ mfWidth/2, mfHeight/2, 1, 0 } + _v3Offset,
			Math::Point3D{ mfWidth/2,-mfHeight/2, 1, 0 } + _v3Offset,
			Math::Point3D{-mfWidth/2,-mfHeight/2, 1, 0 } + _v3Offset,
			Math::Point3D{-mfWidth/2, mfHeight/2, 1, 0 } + _v3Offset
		};
		Convex::operator=(AABB::Convex{ ArrPoints });
		mMin = &this->mVertices[2];
		mMax = &this->mVertices[0];
	}

	void AABB::Awake()
	{
		mVertices.clear();

		mVertices = {
			Vertice{ Math::MakePoint3D(.5f,.5f,0) },
			Vertice{ Math::MakePoint3D(-.5f,.5f,0) },
			Vertice{ Math::MakePoint3D(-.5f,-.5f,0) },
			Vertice{ Math::MakePoint3D(.5f,-.5f,0) }
		};

		mNumPoints = 4;

		mDebugVertices.clear();

		const auto OffSetA = GetOffSet();
		const auto ColAWorldMatrix = GetOwnerTransform() * Math::Translate(OffSetA.x, OffSetA.y, OffSetA.z) * GetTransformationMatrix();

		for (auto & elem : mVertices)
		{
			elem.mPosition = ColAWorldMatrix * elem.mPosition;

			Collider::mDebugVertices.push_back(Dystopia::Gfx::Vertex{ elem.mPosition.x, elem.mPosition.y, elem.mPosition.z });
		}

		mMin = &this->mVertices[0];
		mMax = &this->mVertices[0];

		for (unsigned i = 1; i < 4; ++i)
		{
			mMin = Math::Min(mMin, &this->mVertices[i]);
			mMax = Math::Max(mMax, &this->mVertices[i]);
		}
		Collider::Awake();
	}

	/*Initialise the Component*/
	void AABB::Init(void)
	{
		mVertices.clear();

		// Basic Bounding Box
		if (nullptr != GetOwner()) // owner exist
		{
			const auto scale = GetOwner()->GetComponent<Transform>()->GetGlobalScale();

			Math::Point3D ArrPoints[]
			{
				Math::Point3D{  scale.x / 2,  scale.y / 2, 1, 0 },
				Math::Point3D{  scale.x / 2, -scale.y / 2, 1, 0 },
				Math::Point3D{ -scale.x / 2, -scale.y / 2, 1, 0 },
				Math::Point3D{ -scale.x / 2,  scale.y / 2, 1, 0 }
			};

			Convex::operator=(AABB::Convex{ ArrPoints });

			const auto OffSetA = GetOffSet();
			const auto ColAWorldMatrix = GetOwnerTransform() * Math::Translate(OffSetA.x, OffSetA.y, OffSetA.z) * GetTransformationMatrix();

			for (auto& _v : mVertices)
			{
				_v.mPosition = ColAWorldMatrix * _v.mPosition;
			}

			mMin = &this->mVertices[0];
			mMax = &this->mVertices[0];

			for (unsigned i = 1; i < 4; ++i)
			{
				mMin = Math::Min(mMin, &this->mVertices[i]);
				mMax = Math::Max(mMax, &this->mVertices[i]);
			}
			
		}		
	}

	/*OnDestroy*/
	void AABB::OnDestroy(void)
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
		Component::Serialise(_out);
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
		Component::Unserialise(_in);
		_in >> mv3Offset[0];
		_in >> mv3Offset[1];
		_in >> mv3Offset[2];
		_in >> mfHeight;
		_in >> mfWidth;
		_in.ConsumeEndBlock();
	}

	//TODO: not yet
	/*float AABB::SweepingCheck(const AABB & other_col) const
	{
		if (GetOwner()->GetComponent<RigidBody>() == nullptr)
		{
			DEBUG_PRINT()
		}
	}*/

	void AABB::Recompute()
	{
		if (nullptr != GetOwner())
		{
			const auto ownerRot = GetOwner()->GetComponent<Transform>()->GetGlobalRotation();
			if (mRotation == ownerRot) return;
			mRotation = ownerRot;
		}

		const auto OffSetA = GetOffSet();
		const auto ColAWorldMatrix = GetOwnerTransform() * Math::Translate(OffSetA.x, OffSetA.y, OffSetA.z) * GetTransformationMatrix();

		for (auto& _v : mVertices)
		{
			_v.mPosition = ColAWorldMatrix * _v.mPosition;
		}

		mMin = &this->mVertices[0];
		mMax = &this->mVertices[0];

		for (unsigned i = 1; i < 4; ++i)
		{
			mMin = Math::Min(mMin, &this->mVertices[i]);
			mMax = Math::Max(mMax, &this->mVertices[i]);
		}
	}

	void AABB::Update(float)
	{
		mRotation = GetOwner()->GetComponent<Transform>()->GetGlobalRotation();
	}

	void AABB::EditorUI() noexcept
	{
		//
	}

	bool AABB::isColliding(AABB & _ColB)
	{
		const auto OffSetA    = GetOffSet();
		const auto OffSetB    = _ColB.GetOffSet();
		const auto ColAWorldMatrix = GetOwnerTransform() * Math::Translate(OffSetA.x, OffSetA.y, OffSetA.z) * GetTransformationMatrix();
		const auto ColBWorldMatrix = _ColB.GetOwnerTransform() * Math::Translate(OffSetB.x, OffSetB.y, OffSetB.z)* _ColB.GetTransformationMatrix();

		const auto MinPosA         = ColAWorldMatrix * this->mMin->mPosition;
		const auto MaxPosA         = ColAWorldMatrix * this->mMax->mPosition;
		const auto MinPosB         = ColBWorldMatrix * _ColB.mMin->mPosition;
		const auto MaxPosB         = ColBWorldMatrix * _ColB.mMax->mPosition;

		if (   MinPosA.x <= MaxPosB.x && MaxPosA.x >= MinPosB.x
			&& MaxPosA.y >= MinPosB.y && MinPosA.y <= MaxPosB.y)
		{
			mbColliding = true;
		}
		else
		{
			mbColliding = false;
		}


		return mbColliding;
	}

	bool AABB::isColliding(AABB * const & _ColB)
	{
		return this->isColliding(*_ColB);
	}

	bool AABB::isColliding(const Math::Pt3D& _point)
	{
		return isColliding(GetOwnerTransform(), GetTransformationMatrix(), GetOffSet(), _point);
	}

	bool AABB::isColliding(Math::Pt3D* const& _point)
	{
		return this->isColliding(*_point);
	}

	bool AABB::isColliding(const Math::Mat3D&, const Math::Mat3D&, 
						   const Math::Vec3D&,	  const Math::Pt3D _point)
	{
		const auto MinPosA = mMin->mPosition;
		const auto MaxPosA = mMax->mPosition;

		if (MinPosA.x <= _point.x && MaxPosA.x >= _point.x
			&& MaxPosA.y >= _point.y && MinPosA.y <= _point.y)
		{
			mbColliding = true;
		}
		else
		{

			mbColliding = false;
		}

		return mbColliding;
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

#if EDITOR
	void AABB::eIsTriggerCheckBox()
	{
		if (EGUI::Display::CheckBox("Is Trigger		  ", &mbIsTrigger))
		{
			//mbIsTrigger = tempBool;
		}
		
	}

	void AABB::ePositionOffsetVectorFields()
	{
		EGUI::Display::Label("Offset");
		auto arrResult = EGUI::Display::VectorFields("    ", &mv3Offset, 0.01f, -FLT_MAX, FLT_MAX);
		for (auto &e : arrResult)
		{
			switch (e)
			{
			case EGUI::eDragStatus::eNO_CHANGE:
			case EGUI::eDragStatus::eDRAGGING:
				break;
			case EGUI::eDragStatus::eSTART_DRAG:
				//EGUI::GetCommandHND()->StartRecording<Collider>(mnOwner, &Collider::mv3Offset);
				break;
			case EGUI::eDragStatus::eDEACTIVATED:
			case EGUI::eDragStatus::eEND_DRAG:
			case EGUI::eDragStatus::eENTER:
			case EGUI::eDragStatus::eTABBED:
				//EGUI::GetCommandHND()->EndRecording();
				break;
			default:
				break;
			}
		}
	}
#endif
}
