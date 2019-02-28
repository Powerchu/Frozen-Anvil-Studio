/* HEADER *********************************************************************************/
/*!
\file	CollisionSystem.cpp
\author keith.goh (100%)
\par    email: keith.goh\@digipen.edu
\brief

	Collision System Implementation


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/ColliderList.h"
#include "Component/RigidBody.h"
#include "Component/Transform.h"
#include "System/Collision/CollisionSystem.h"
#include "System/Collision/CollisionEvent.h"
#include "System/Graphics/MeshSystem.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"
#include "Behaviour/Behaviour.h"
#include <utility>
#include <map>
#include "System/Graphics/Shader.h"
#include "PP/PPUtils.h"

#if EDITOR
#include "Editor/EGUI.h"
#endif
#define _COL_GENERATOR_(_NUM_) std::make_pair<eColLayer, eColLayer>(LAYER_##_NUM_ , static_cast<eColLayer>(0))

#define _COL_LAYER_(_SIZE_) PP_EVAL(_COLAUX_##_SIZE_)()
#define _COLAUX_32() _COL_GENERATOR_(1),  _COL_GENERATOR_(2),  _COL_GENERATOR_(3), _COL_GENERATOR_(4), _COL_GENERATOR_(5)        , \
				     _COL_GENERATOR_(6),  _COL_GENERATOR_(7),  _COL_GENERATOR_(8), _COL_GENERATOR_(9), _COL_GENERATOR_(10), _COL_GENERATOR_(11)     , \
					 _COL_GENERATOR_(12), _COL_GENERATOR_(13), _COL_GENERATOR_(14), _COL_GENERATOR_(15), _COL_GENERATOR_(16), _COL_GENERATOR_(17)  , \
                     _COL_GENERATOR_(18), _COL_GENERATOR_(19), _COL_GENERATOR_(20), _COL_GENERATOR_(21), _COL_GENERATOR_(22), _COL_GENERATOR_(23)  , \
                     _COL_GENERATOR_(24), _COL_GENERATOR_(25), _COL_GENERATOR_(26), _COL_GENERATOR_(27), _COL_GENERATOR_(28), _COL_GENERATOR_(29)  , \
                     _COL_GENERATOR_(30), _COL_GENERATOR_(31), _COL_GENERATOR_(32)

namespace Dystopia
{
	CollisionSystem::Map_t CollisionSystem::mIgnoreTable
	{
		_COL_LAYER_(32), std::make_pair(LAYER_NONE, static_cast < eColLayer>(0xFFFFFFFFu))
	};
	bool  CollisionSystem::mIgnoreBoolTable[32][32]
	{
		
	};
	std::string CollisionSystem::arrColLayer[33] =
	{
				"Default","Player", "Enemy", "Flying",
				"Passable","Foreground", "Midground", "Background",
				"Terrain","Particle", "Form", "Force",
				"Projectile","Layer 14", "Layer 15", "Layer 16",
				"Layer 17","Layer 18", "Layer 19", "Layer 20",
				"Layer 21","Layer 22", "Layer 23", "Layer 24",
				"Layer 25","Layer 26", "Layer 27", "Layer 28",
				"Layer 29","Layer 30", "Layer 31", "Layer 32","Layer_Global"
	};
	void CollisionSystem::PreInit(void)
	{
		// 

	}
	bool CollisionSystem::Init()
	{
		return true;
	}

	void CollisionSystem::PostInit(void)
	{
		//empty
	}

	void CollisionSystem::Update(float)
	{

	}

	void CollisionSystem::PostUpdate()
	{
		for (auto & col : ComponentDonor<Convex>::mComponents)
		{
			if (col.GetFlags() & FLAG_REMOVE)
				ComponentDonor<Convex>::mComponents.Remove(&col);
		}
		for (auto & col : ComponentDonor<AABB>::mComponents)
		{
			if (col.GetFlags() & FLAG_REMOVE)
				ComponentDonor<AABB>::mComponents.Remove(&col);
		}
		for (auto & col : ComponentDonor<Circle>::mComponents)
		{
			if (col.GetFlags() & FLAG_REMOVE)
				ComponentDonor<Circle>::mComponents.Remove(&col);
		}
		for (auto & col : ComponentDonor<PointCollider>::mComponents)
		{
			if (col.GetFlags() & FLAG_REMOVE)
				ComponentDonor<PointCollider>::mComponents.Remove(&col);
		}
	}

	void CollisionSystem::FixedUpdate(float _dt)
	{
		ScopedTimer<ProfilerAction> timeKeeper{ "Collision System", "Update" };

		BoundingColliderNode		mCollisionTree;
		PotentialContacts	        ArrayContacts[1024];
		unsigned				    ContactCount = 0;

		using CollisionTable = std::pair<eColliderType, eColliderType>;
		using fpCollisionResolution = bool(CollisionSystem::*)(Collider  * const &, Collider  * const &)const;
		using CollisionTableMap = std::map < CollisionTable, fpCollisionResolution>;

		static CollisionTableMap CollisionFuncTable = []()->CollisionTableMap
		{
			CollisionTableMap i
			{
			{ CollisionTable{ eColliderType::AABB    ,eColliderType::AABB }    ,&CollisionSystem::AABBvsAABB },
			{ CollisionTable{ eColliderType::AABB    ,eColliderType::CONVEX }  ,&CollisionSystem::ConvexVsConvex },
			{ CollisionTable{ eColliderType::CONVEX  ,eColliderType::AABB }    ,&CollisionSystem::ConvexVsConvex },
			{ CollisionTable{ eColliderType::CONVEX  ,eColliderType::CONVEX }  ,&CollisionSystem::ConvexVsConvex },
			{ CollisionTable{ eColliderType::CIRCLE  ,eColliderType::CIRCLE }  ,&CollisionSystem::CircleVsCircle },
			{ CollisionTable{ eColliderType::CIRCLE,  eColliderType::AABB }    ,&CollisionSystem::CircleVsAABB },
			{ CollisionTable{ eColliderType::AABB,    eColliderType::CIRCLE }  ,&CollisionSystem::AABBvsCircle },
			{ CollisionTable{ eColliderType::CIRCLE,  eColliderType::CONVEX }  ,&CollisionSystem::CircleVsConvex },
			{ CollisionTable{ eColliderType::CONVEX,  eColliderType::CIRCLE }  ,&CollisionSystem::ConvexVsCircle },
			{ CollisionTable{ eColliderType::POINT,   eColliderType::POINT }   ,&CollisionSystem::PointVsPoint },
			{ CollisionTable{ eColliderType::POINT,   eColliderType::CONVEX }  ,&CollisionSystem::PointVsConvex },
			{ CollisionTable{ eColliderType::POINT,   eColliderType::CIRCLE }  ,&CollisionSystem::PointVsCircle },
			{ CollisionTable{ eColliderType::CONVEX,  eColliderType::POINT }   ,&CollisionSystem::ConvexVsPoint },
			{ CollisionTable{ eColliderType::CIRCLE,  eColliderType::POINT }   ,&CollisionSystem::CircleVsPoint }
			};
			return i;
		}();

		AutoArray<Collider *> mColliders;

		for (auto & elem : ComponentDonor<Convex>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#else
			if (!(elem.GetFlags() & eObjFlag::FLAG_ACTIVE)) continue;
#endif 
			if (elem.GetOwner())
			{
				elem.Update(_dt);
				elem.ClearCurrentCollisionEvent(); //clear collision table
				Math::Matrix3D gobjMatrix = elem.GetOwner()->GetComponent<Transform>()->GetTransformMatrix();
				elem.SetOwnerTransform(gobjMatrix);
				elem.SetColliding(false);
				mColliders.push_back(&elem);
				mCollisionTree.Insert(&elem, elem.GetBroadPhaseCircle());
			}
		}

		for (auto & elem : ComponentDonor<AABB>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#else
			if (!(elem.GetFlags() & eObjFlag::FLAG_ACTIVE)) continue;
#endif 
			if (elem.GetOwner())
			{
				//elem.Recompute();
				elem.ClearCurrentCollisionEvent(); //clear collision table
				Math::Matrix3D gobjMatrix = elem.GetOwner()->GetComponent<Transform>()->GetTransformMatrix();
				elem.SetOwnerTransform(gobjMatrix);
				elem.SetColliding(false);
				mColliders.push_back(&elem);
				mCollisionTree.Insert(&elem, elem.GetBroadPhaseCircle());
			}

		}

		for (auto & elem : ComponentDonor<Circle>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#else
			if (!(elem.GetFlags() & eObjFlag::FLAG_ACTIVE)) continue;
#endif 
			if (elem.GetOwner())
			{
				elem.ClearCurrentCollisionEvent(); //clear collision table
				Math::Matrix3D gobjMatrix = elem.GetOwner()->GetComponent<Transform>()->GetTransformMatrix();
				elem.SetOwnerTransform(gobjMatrix);
				elem.SetColliding(false);
				mColliders.push_back(&elem);
				mCollisionTree.Insert(&elem, elem.GetBroadPhaseCircle());
			}
		}

		for (auto & elem : ComponentDonor<PointCollider>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#else
			if (!(elem.GetFlags() & eObjFlag::FLAG_ACTIVE)) continue;
#endif 
			if (elem.GetOwner())
			{
				elem.ClearCurrentCollisionEvent(); //clear collision table
				Math::Matrix3D gobjMatrix = elem.GetOwner()->GetComponent<Transform>()->GetTransformMatrix();
				elem.SetOwnerTransform(gobjMatrix);
				elem.SetColliding(false);
				mColliders.push_back(&elem);
				mCollisionTree.Insert(&elem, elem.GetBroadPhaseCircle());
			}
		}

		if (!mCollisionTree.isEmpty())
			ContactCount = mCollisionTree.GetNumPotentialContact(1024, ArrayContacts);

		for (unsigned i = 0; i < ContactCount; ++i)
		{
			if (nullptr == ArrayContacts[i].mContacts[0] || nullptr == ArrayContacts[i].mContacts[1]) continue;
			Collider * bodyA = ArrayContacts[i].mContacts[0];
			Collider * bodyB = ArrayContacts[i].mContacts[1];
			const auto ownerA = bodyA->GetOwner();
			const auto ownerB = bodyB->GetOwner();
			const auto rigidA = ownerA->GetComponent<RigidBody>();
			const auto rigidB = ownerB->GetComponent<RigidBody>();

			if (static_cast<Collider *>(bodyA) != static_cast<Collider *>(bodyB))
			{
				/*Check if there is a common collision layer*/
				if (this->ToIgnore(bodyA->GetColLayer(), bodyB->GetColLayer()))
					continue;

				// If the colliders belong to the same owner, continue
				if (ownerA == ownerB)
					continue;

				if (rigidA && rigidB)
				{
					// if both bodies are static, continue
					if (rigidA->Get_IsStaticState() && rigidB->Get_IsStaticState()) 
						continue;
				}

				const auto pair_key1 = std::make_pair(bodyA->GetColliderType(), (bodyB)->GetColliderType());
				const auto pair_key2 = std::make_pair(bodyB->GetColliderType(), (bodyA)->GetColliderType());
				bool hasCollision = true;
				for (auto & key : CollisionFuncTable)
				{
					if (key.first == pair_key1)
					{
						hasCollision &= (this->*key.second)(bodyA, bodyB);


						break;
					}
				}
				for (auto & key : CollisionFuncTable)
				{
					if (key.first == pair_key2)
					{
						hasCollision &= (this->*key.second)(bodyB, bodyA);
						break;
					}
				}
				bodyA->SetColliding(hasCollision);
				bodyB->SetColliding(hasCollision);
			}
		}




		//for (auto & bodyA : mColliders)
		//{
		//	const auto ownerA = bodyA->GetOwner();
		//	for (auto & bodyB : mColliders)
		//	{
		//		const auto ownerB = bodyB->GetOwner();
		//		const auto rigidA = ownerA->GetComponent<RigidBody>();
		//		const auto rigidB = ownerB->GetComponent<RigidBody>();

		//		if (rigidA == nullptr)
		//			bodyA->SetTrigger(true);
		//		if (rigidB == nullptr)
		//			bodyB->SetTrigger(true);

		//		if (static_cast<Collider *>(bodyA) != static_cast<Collider *>(bodyB))
		//		{
		//			if (rigidA && rigidB)
		//			{
		//				if (rigidA->Get_IsStaticState() && rigidB->Get_IsStaticState())
		//					continue;
		//				if (ownerA == ownerB)
		//					continue;
		//			}
		//			const auto pair_key = std::make_pair(bodyA->GetColliderType(), (bodyB)->GetColliderType());
		//			for (auto & key : CollisionFuncTable)
		//			{
		//				if (key.first == pair_key)
		//				{
		//					(this->*key.second)(bodyA, bodyB);
		//					bodyB->SetColliding(bodyB->Collider::HasCollision());
		//					bodyA->SetColliding(bodyA->Collider::HasCollision());
		//					break;
		//				}
		//			}
		//		}
		//	}
		//}

		for (auto & elem : mColliders)
		{
			const auto rigid = elem->GetOwner()->GetComponent<RigidBody>();
			if (rigid == nullptr)
				elem->SetTrigger(true);

			elem->InformOtherComponents();
		}
	}

	void CollisionSystem::Shutdown()
	{

	}

	void CollisionSystem::LoadDefaults(void)
	{
		memset(mIgnoreBoolTable, true, 32 * 32);
	}

	void CollisionSystem::LoadSettings(TextSerialiser & _in)
	{
		for (unsigned u = 0; u < 32; ++u)
			for (unsigned i = 0; i < 32; ++i)
			{
				_in >> mIgnoreBoolTable[u][i];
				if (!mIgnoreBoolTable[u][i])
					mIgnoreTable[static_cast<eColLayer>(0x00000001u << u)] = static_cast<eColLayer>(mIgnoreTable[static_cast<eColLayer>(0x00000001u << u)] | static_cast<eColLayer>(0x00000001u << i));
			}

		//memset(mIgnoreBoolTable, true, 32 * 32);
	}

	void CollisionSystem::SaveSettings(DysSerialiser_t & _out)
	{
		for(unsigned u = 0;u<32;++u)
			for (unsigned i = 0; i < 32; ++i)
			{
				_out << mIgnoreBoolTable[u][i];
			}
	}
#if EDITOR
	void CollisionSystem::EditorUI(void)
	{
		static char buffer[256];
		//EGUI::StartChild("make unique", ImVec2{ 800.f, 760.f }, false, true);
		ImGui::BeginChild("makeUnique", ImVec2{ 800.f, 720.f }, false, ImGuiWindowFlags_NoScrollWithMouse);
		//ImGui::BeginGroup();
		EGUI::PushLeftAlign(150.f);

		EGUI::PushLeftAlign(300.f);
		for (unsigned i = 32; i >= 1; --i)
		{
			//EGUI::Display::LabelWrapped(std::to_string(i).c_str());
			if (i == 0)
			{
				EGUI::Display::LabelWrapped("  ");
				continue;
			}
			EGUI::Display::Label("%02d", i);
			ImGui::SameLine(0,9.0f);
			ImGui::NextColumn();
		}
		EGUI::PopLeftAlign();
		ImGui::Separator();

		int unique = 0;
		for (unsigned i = 1; i <= 32; ++i)
		{
			//EGUI::PushID(i);
			for (unsigned u = 32; u >= i; --u)
			{
				EGUI::PushID(unique++);
				ImGui::PushItemWidth(10.f);
				if (EGUI::Display::CheckBox(std::to_string(u * i).c_str(), &mIgnoreBoolTable[i-1][u-1], false, nullptr, 0.5f))
				{
					eColLayer curr  = mIgnoreTable[static_cast<eColLayer>(0x01u << (i - 1))];
					eColLayer curr2 = mIgnoreTable[static_cast<eColLayer>(0x01u << (u - 1))];
					mIgnoreBoolTable[u - 1][i - 1] = mIgnoreBoolTable[i - 1][u - 1];
					bool isClick   = mIgnoreBoolTable[i - 1][u - 1];
					mIgnoreTable[static_cast<eColLayer>(0x01u << (i - 1))] = static_cast<eColLayer>(!isClick ? curr | ((0x00000001u) << (u-1))  : curr & (~(0x00000001u << (u - 1))));
					mIgnoreTable[static_cast<eColLayer>(0x01u << (u - 1))] = static_cast<eColLayer>(!isClick ? curr2 | ((0x00000001u) << (i-1)) : curr2 & (~(0x00000001u << (i - 1))));
				}
				ImGui::PopItemWidth();
				EGUI::PopID();
				ImGui::SameLine(0, 5.0f);
			}
			
			EGUI::PushLeftAlign(1500.f);
			ImGui::PushItemWidth(30.f);
			EGUI::Display::LabelWrapped(arrColLayer[i-1].c_str());
			ImGui::PopItemWidth();
			EGUI::PopLeftAlign();
			ImGui::NextColumn();
		}
		EGUI::PopLeftAlign();
		ImGui::EndChild();
		//EGUI::EndChild();


	}

	void CollisionSystem::RenderVerticalColName()
	{
		static size_t Max = 0;
		static bool     hasChange = false;
		for(auto & elem : arrColLayer)
		{
			if (elem.size() > Max)
			{
				Max = elem.size();
				hasChange = true;
			}
		}

	}
#endif
	bool CollisionSystem::AABBvsAABB(Collider * const & _ColA, Collider * const & _ColB) const
	{
		const auto col_a = dynamic_cast<AABB * const>(_ColA);
		const auto col_b = dynamic_cast<AABB * const>(_ColB);

		return col_a->isColliding(col_b);
	}

	bool CollisionSystem::ConvexVsConvex(Collider * const & _ColA, Collider * const & _ColB) const
	{
		const auto col_a = dynamic_cast<Convex * const>(_ColA);
		const auto col_b = dynamic_cast<Convex * const>(_ColB);

		return col_a->isColliding(col_b);
	}

	bool CollisionSystem::CircleVsCircle(Collider* const& _ColA, Collider* const& _ColB) const
	{
		const auto col_a = dynamic_cast<Circle * const>(_ColA);
		const auto col_b = dynamic_cast<Circle * const>(_ColB);

		return col_a->isColliding(col_b);
	}

	bool CollisionSystem::CircleVsAABB(Collider * const & _ColA, Collider * const & _ColB) const
	{
		UNUSED_PARAMETER(_ColA);
		UNUSED_PARAMETER(_ColB);
		return false;
	}

	bool CollisionSystem::AABBvsCircle(Collider * const & _ColA, Collider * const & _ColB) const
	{
		UNUSED_PARAMETER(_ColA);
		UNUSED_PARAMETER(_ColB);
		return false;
	}

	bool CollisionSystem::CircleVsConvex(Collider * const & _ColA, Collider * const & _ColB) const
	{
		Circle   * pCircle;
		Convex   * pConvex;

		if (_ColA->GetColliderType() == eColliderType::CIRCLE)
		{
			pCircle = dynamic_cast<Circle *>(_ColA);
			pConvex = dynamic_cast<Convex *>(_ColB);
		}
		else
		{
			pCircle = dynamic_cast<Circle *>(_ColB);
			pConvex = dynamic_cast<Convex *>(_ColA);
		}
		const bool isColliding = pCircle->isColliding((*pConvex));

		return isColliding;
	}

	bool CollisionSystem::ConvexVsCircle(Collider* const& _ColA, Collider* const& _ColB) const
	{
		Circle   * pCircle;
		Convex   * pConvex;

		if (_ColA->GetColliderType() == eColliderType::CIRCLE)
		{
			pCircle = dynamic_cast<Circle *>(_ColA);
			pConvex = dynamic_cast<Convex *>(_ColB);
		}
		else
		{
			pCircle = dynamic_cast<Circle *>(_ColB);
			pConvex = dynamic_cast<Convex *>(_ColA);

		}
		const bool isColliding = pConvex->isColliding((*pCircle));

		return isColliding;
	}

	bool CollisionSystem::PointVsPoint(Collider * const & _ColA, Collider * const & _ColB) const
	{
		PointCollider * a, * b;
		a = dynamic_cast<PointCollider *>(_ColA);
		b = dynamic_cast<PointCollider *>(_ColB);
		
		return (a && b) || a->isColliding(b);
	}

	bool CollisionSystem::PointVsConvex(Collider * const & _ColA, Collider * const & _ColB) const
	{
		PointCollider * a;
		Convex *b;
		a = dynamic_cast<PointCollider *>(_ColA);
		b = dynamic_cast<Convex *>(_ColB);

		return (a && b) || a->isColliding(b);
	}

	bool CollisionSystem::ConvexVsPoint(Collider * const & _ColA, Collider * const & _ColB) const
	{
		PointCollider * b;
		Convex *a;
		a = dynamic_cast<Convex * const>(_ColA);
		b = dynamic_cast<PointCollider * const>(_ColB);
		
		return (!a && !b) && a->isColliding(b);
	}

	bool CollisionSystem::PointVsCircle(Collider * const & _ColA, Collider * const & _ColB) const
	{
		PointCollider * a;
		Circle *b;
		a = dynamic_cast<PointCollider *>(_ColA);
		b = dynamic_cast<Circle *>(_ColB);

		return (a && b) || a->isColliding(b);
	}

	bool CollisionSystem::CircleVsPoint(Collider * const & _ColA, Collider * const & _ColB) const
	{
		PointCollider * b;
		Circle *a;
		a = dynamic_cast<Circle * const>(_ColA);
		b = dynamic_cast<PointCollider * const>(_ColB);

		return (!a && !b) && a->isColliding(b);
	}

	bool CollisionSystem::PointVsAABB(Collider * const & _ColA, Collider * const & _ColB) const
	{
		_ColA;
		_ColB;
		return false;
	}

	bool CollisionSystem::AABBVsPoint(Collider * const & _ColA, Collider * const & _ColB) const
	{
		_ColA;
		_ColB;
		return false;
	}

	AutoArray<Collider*> CollisionSystem::GetAllColliders() const
	{
		AutoArray<Collider*> ToRet;
		for (auto & elem : ComponentDonor<Convex>::mComponents)
		{
			ToRet.push_back(&elem);
		}
		for (auto & elem : ComponentDonor<AABB>::mComponents)
		{
			ToRet.push_back(&elem);
		}
		for (auto & elem : ComponentDonor<Circle>::mComponents)
		{
			ToRet.push_back(&elem);
		}
		return Ut::Move(ToRet);
	}

	bool CollisionSystem::RaycastFirstHit(Math::Vec3D const & _Dir, Math::Point3D const & _mPos,CollisionEvent * _Output, float _MaxLength, eColLayer layer) const
	{
		CollisionEvent ray;
		ray.mTimeIntersection = 999999.9f;
		bool isColliding = false;
		for (auto & elem : ComponentDonor<Convex>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
			
#endif 
			if (elem.GetOwner())
			{
				if (this->ToIgnore(elem.GetColLayer(), layer))
					continue;

				isColliding |= RayCollider::Raycast(_Dir, _mPos, &elem, &ray, _MaxLength);
			}
		}

		for (auto & elem : ComponentDonor<AABB>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			if (elem.GetOwner())
			{
				//isColliding = RayCollider::Raycast(_Dir, _mPos, &elem, &ray, _MaxLength);
			}

		}

		for (auto & elem : ComponentDonor<Circle>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			

			if (elem.GetOwner())
			{
				if (this->ToIgnore(elem.GetColLayer(), layer))
					continue;

				isColliding |= RayCollider::Raycast(_Dir, _mPos, &elem, &ray, _MaxLength);
			}
		}
		if (isColliding && _Output) *_Output = ray;
		return isColliding;
	}

	bool CollisionSystem::RaycastAllHits(Math::Vec3D const & _Dir, Math::Point3D const & _mPos, AutoArray<CollisionEvent>& _Output, float _MaxLength, eColLayer layer) const
	{
		bool isColliding = false;
		for (auto & elem : ComponentDonor<Convex>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			

			if (elem.GetOwner())
			{
				if (this->ToIgnore(elem.GetColLayer(), layer))
					continue;

				CollisionEvent ColEvent{ nullptr, elem.GetOwner() };
				const bool result = RayCollider::Raycast(_Dir, _mPos, &elem, &ColEvent, _MaxLength);
				if (result)
					_Output.push_back(Ut::Move(ColEvent));
				isColliding |= result;
			}
		}

		for (auto & elem : ComponentDonor<AABB>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			if (elem.GetOwner())
			{
				//isColliding = RayCollider::Raycast(_Dir, _mPos, &elem, _Output, _MaxLength);
			}

		}

		for (auto & elem : ComponentDonor<Circle>::mComponents)
		{
#if EDITOR
			if (elem.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || !elem.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;
#endif 
			

			if (elem.GetOwner())
			{
				if (this->ToIgnore(elem.GetColLayer(), layer))
					continue;

				CollisionEvent ColEvent{ nullptr, elem.GetOwner() };
				const bool result = RayCollider::Raycast(_Dir, _mPos, &elem, &ColEvent, _MaxLength);
				if (result)
					_Output.push_back(Ut::Move(ColEvent));
				isColliding |= result;
			}
		}

		return isColliding;
	}

	void CollisionSystem::MapIgnoreLayer(eColLayer _layer, eColLayer _toIgnore)
	{
		mIgnoreTable[_layer] = static_cast<eColLayer>(mIgnoreTable[_layer] | _toIgnore);
	}

	bool CollisionSystem::ToIgnore(eColLayer _Layer1, eColLayer _Layer2) const
	{
		if ((!_Layer1 || !_Layer2)) return true;

		if ((_Layer1 == 0xFFFFFFFFu || _Layer2 == 0xFFFFFFFFu)) return false;

		unsigned flags = static_cast<unsigned>(_Layer1);
		for (unsigned count = 0; count < 32; ++count)
		{
			if (auto isolate = flags & (0x00000001u << count))
			{
				auto res = mIgnoreTable[static_cast<eColLayer>(isolate)];
				if (res & _Layer2)
					return true;
			}
		}

		return false;
	}

	void CollisionSystem::SetIgnore(unsigned _Layer1, unsigned _Layer2, bool _toignore)
	{
		mIgnoreTable[static_cast<eColLayer>(_Layer1)] = _toignore ? static_cast<eColLayer>(mIgnoreTable[static_cast<eColLayer>(_Layer1)] | _Layer2) : static_cast<eColLayer>(mIgnoreTable[static_cast<eColLayer>(_Layer1)] & ~_Layer2);
		mIgnoreTable[static_cast<eColLayer>(_Layer2)] = _toignore ? static_cast<eColLayer>(mIgnoreTable[static_cast<eColLayer>(_Layer2)] | _Layer1) : static_cast<eColLayer>(mIgnoreTable[static_cast<eColLayer>(_Layer2)] & ~_Layer1);

		unsigned count = 0;
		unsigned count2 = 0;

		while (_Layer1)
		{
			_Layer1 >>= 1;
			count++;
		}

		while(_Layer2)
		{
			_Layer2 >>= 1;
			count2++;
		}

		if (count && count2)
		{
			mIgnoreBoolTable[(count - 1)][(count2 - 1)] = !_toignore;
			mIgnoreBoolTable[(count2 - 1)][(count - 1)] = !_toignore;
		}

		/*auto i = _Layer1;
		auto u = _Layer2;

		eColLayer curr  = mIgnoreTable[static_cast<eColLayer>(0x01u << (i - 1))];
		eColLayer curr2 = mIgnoreTable[static_cast<eColLayer>(0x01u << (u - 1))];

		mIgnoreBoolTable[i - 1][u - 1] = _toignore;
		mIgnoreBoolTable[u - 1][i - 1]  = _toignore;
		mIgnoreTable[static_cast<eColLayer>(0x01u << (i - 1))] = static_cast<eColLayer>(_toignore ? curr  | ((0x00000001u) << (u - 1)) : curr  & (~(0x00000001u << (u - 1))));
		mIgnoreTable[static_cast<eColLayer>(0x01u << (u - 1))] = static_cast<eColLayer>(_toignore ? curr2 | ((0x00000001u) << (i - 1)) : curr2 & (~(0x00000001u << (i - 1))));*/

	}

	std::string const* CollisionSystem::GetColLayerNames()
	{
		return arrColLayer;
	}

	unsigned CollisionSystem::GetColLayerSize()
	{
		return sizeof(arrColLayer) / (sizeof(arrColLayer[0]));
	}

	CollisionSystem::CollisionSystem()
	{

	}

	CollisionSystem::~CollisionSystem()
	{

	}


}
