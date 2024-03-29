/* HEADER *********************************************************************************/
/*!
\file	PhysicsSystem.cpp
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief


All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Component/Collider.h"
#include "Component/RigidBody.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"
#include "System/Collision/CollisionSystem.h"

#if EDITOR
#include "Editor/EGUI.h"
#endif 

namespace Dystopia
{
	PhysicsSystem::PhysicsSystem()
		: mbIsDebugActive(false)
		  , mInterpolation_mode(none)
		  , mGravity(400.0F)
		  , mMaxVelocityConstant(2560.0F)
		  , mMaxVelSquared(mMaxVelocityConstant * mMaxVelocityConstant)
		  , mPenetrationEpsilon(0.01F)
		  , mfSleepVelEpsilon(1.000F)
		  , mfSleepBias(0.97F)
		  , mVelocityIterations(4)
		  , mPositionalIterations(4)
	{
	}

	void PhysicsSystem::PreInit(void)
	{
	}

	bool PhysicsSystem::Init(void)
	{
		return true;
	}

	void PhysicsSystem::PostInit(void)
	{

	}

	void PhysicsSystem::CheckSleepingBodies(float _dt)
	{
		for (auto& body : mComponents)
		{
#if EDITOR
			if (nullptr == body.GetOwner())	continue;
			if (body.GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
			if (body.GetOwner())
			{
				if (!body.GetOwner()->GetFlags() & eObjFlag::FLAG_ACTIVE || !body.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;


				if (!body.Get_IsStaticState() && !body.Get_IsKinematic())
				{
					body.CheckSleeping(_dt);

					if (body.GetOwner()->GetFlags() & eObjFlag::FLAG_DRAGGING)
					{
						body.SetSleeping(false);
					}

					for (auto col : body.mparrCol)
					{
						if (nullptr != col)
						{
							if (!col->IsTrigger())
								col->SetSleeping(!body.GetIsAwake());
						}
					}
				}
			}
		}
	}

	void PhysicsSystem::IntegrateRigidBodies(float _dt)
	{
		for (auto& body : mComponents)
		{
			if (nullptr == body.GetOwner())	continue;
#if EDITOR
			if (body.GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
			if ( !body.GetOwner()->GetFlags() & eObjFlag::FLAG_ACTIVE || body.GetOwner()->GetFlags() & eObjFlag::FLAG_DRAGGING
				|| !body.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;

			if (!body.Get_IsStaticState() && body.GetIsAwake())
			{
				body.Integrate(_dt);
			}
		}
	}

	void PhysicsSystem::ResolveCollision(const float _dt)
	{
		for (int i = 0; i < mVelocityIterations; ++i)
		{
			for (auto& body : mComponents)
			{
				if (body.GetOwner() == nullptr) continue;
#if EDITOR
				if (body.GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
				if (!body.GetOwner()->GetFlags() & eObjFlag::FLAG_ACTIVE || body.GetOwner()->GetFlags() & eObjFlag::FLAG_DRAGGING
					|| !body.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;

				for (auto col : body.mparrCol)
				{
					if (!col->IsTrigger())
					{
						for (auto& manifold : col->GetCollisionEvents())
						{
							manifold.ApplyImpulse();
						}
					}
				}
			}
		}

		for (auto& body : mComponents)
		{
			if (body.GetOwner() == nullptr) continue;
#if EDITOR
			if (body.GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
			if (body.Get_IsStaticState() || !body.GetIsAwake() || body.Get_IsKinematic()) continue;

			if (!body.GetOwner()->GetFlags() & eObjFlag::FLAG_ACTIVE || body.GetOwner()->GetFlags() & eObjFlag::FLAG_DRAGGING
				|| !body.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;

			body.PreUpdatePosition(_dt);

		}

		for (int i = 0; i < mPositionalIterations; ++i)
		{
			for (auto& body : mComponents)
			{
				if (body.GetOwner() == nullptr) continue;
#if EDITOR
				if (body.GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
				if (!body.GetOwner()->GetFlags() & eObjFlag::FLAG_ACTIVE || body.GetOwner()->GetFlags() & eObjFlag::FLAG_DRAGGING
					|| !body.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;

				for (auto col : body.mparrCol)
				{
					if (!col->IsTrigger())
					{
						auto worstPene = mPenetrationEpsilon;

						for (auto& manifold : col->GetCollisionEvents())
						{
							if (manifold.mCollidedWith->GetComponent<Collider>()->IsTrigger())
								continue;

							if (manifold.mfPeneDepth > worstPene)
							{
								const auto worstContact = &manifold;
								worstPene = manifold.mfPeneDepth;

								if (nullptr != worstContact)
								{
									//manifold.ApplyPenetrationCorrection(mPositionalIterations);
									worstContact->ApplyPenetrationCorrection(mPositionalIterations);
								}
							}
						}
					}
				}
			}
		}


	}

	void PhysicsSystem::UpdateResults(float _dt)
	{
		for (auto& body : mComponents)
		{
			if (body.GetOwner() == nullptr) continue;
#if EDITOR
			if (body.GetOwner()->GetFlags() & eObjFlag::FLAG_EDITOR_OBJ || body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
			if (body.Get_IsStaticState() || body.Get_IsKinematic()) continue;

			if (!body.GetOwner()->GetFlags() & eObjFlag::FLAG_ACTIVE 
				|| body.GetOwner()->GetFlags() & eObjFlag::FLAG_DRAGGING 
				|| !body.GetFlags() & eObjFlag::FLAG_ACTIVE) continue;

			body.UpdateResult(_dt);
		}

		// If Event System is running: this is where to Broadcast Collision Messages
	}

	void PhysicsSystem::DebugPrint()
	{
		for (auto& body : mComponents)
		{
			if (body.GetOwner())
			{
				body.DebugPrint();
			}
		}
	}

	void PhysicsSystem::Step(float _dt)
	{
		/* Broad Phase Collision Detection*/

		/* Narrow Phase Collision Detection*/

		/* Collision Resolution (Response) Logic */
		ResolveCollision(_dt);

		/*Update positions and rotation as result*/
		UpdateResults(_dt);

		// Set all objects at rest to sleeping
		CheckSleepingBodies(_dt);


	}

	void PhysicsSystem::PreFixedUpdate(float _dt)
	{
		ScopedTimer<ProfilerAction> timeKeeper{ "Physics System", "PreFixed Update" };
		// Integrate RigidBodies
		IntegrateRigidBodies(_dt);
	}


	void PhysicsSystem::FixedUpdate(float _dt)
	{
		ScopedTimer<ProfilerAction> timeKeeper{ "Physics System", "Fixed Update" };

		Step(_dt);

		if (mbIsDebugActive)
		{
			DebugPrint();
		}
	}

	void PhysicsSystem::Update(float)
	{

	}

	void PhysicsSystem::PostUpdate()
	{
		for (auto& body : mComponents)
		{
			if (nullptr == body.GetOwner()) continue;

			if (body.GetFlags() & FLAG_REMOVE)
			{
				mComponents.Remove(&body);
			}
		}
	}

	void PhysicsSystem::Shutdown(void)
	{

	}

	void PhysicsSystem::LoadDefaults(void)
	{

	}

	void PhysicsSystem::LoadSettings(DysSerialiser_t& _in)
	{
		_in >> mGravity;
		_in >> mMaxVelocityConstant;
		_in >> mPenetrationEpsilon;
		_in >> mVelocityIterations;
		_in >> mPositionalIterations;
		_in >> mbIsDebugActive;
		_in >> mfSleepVelEpsilon;
		_in >> mfSleepBias;
	}

	void PhysicsSystem::SaveSettings(DysSerialiser_t& _out)
	{
		_out << mGravity;
		_out << mMaxVelocityConstant;
		_out << mPenetrationEpsilon;
		_out << mVelocityIterations;
		_out << mPositionalIterations;
		_out << mbIsDebugActive;
		_out << mfSleepVelEpsilon;
		_out << mfSleepBias;
	}

	void PhysicsSystem::EditorUI(void)
	{
#if EDITOR			
		EGUI::PushLeftAlign(160);
		IsDebugUI();
		GravityUI();
		MaxVelocityUI();
		SleepEpsilonUI();
		SleepBiasUI();
		VelocityIterationUI();
		PositionalIterationUI();
		EGUI::PopLeftAlign();
#endif 
	}

	AutoArray<RigidBody*> PhysicsSystem::GetAllBodies() const
	{
		AutoArray<RigidBody*> ToRet;

		for (auto & elem : mComponents)
		{
			ToRet.push_back(&elem);
		}

		return Ut::Move(ToRet);
	}

#if EDITOR
	void PhysicsSystem::GravityUI(void)
	{
		const auto result = EGUI::Display::DragFloat("Gravity", &mGravity, 0.01f, -FLT_MAX, FLT_MAX);
		switch (result)
		{
		case EGUI::eDragStatus::eEND_DRAG:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			//EGUI::GetCommandHND()->StartRecording<PhysicsSystem>(&PhysicsSystem::mGravity);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		default: break;
		}
	}

	void PhysicsSystem::IsDebugUI(void)
	{
		//bool tempBool = mbIsDebugActive;
		//if (EGUI::Display::CheckBox("Debug Draw", &mbIsDebugActive))
		//{
		//	//mbIsDebugActive = tempBool;
		//	//EGUI::GetCommandHND()->InvokeCommand<PhysicsSystem>(&PhysicsSystem::mbIsDebugActive, tempBool);
		//}
	}

	void PhysicsSystem::MaxVelocityUI()
	{
		const auto result = EGUI::Display::DragFloat("Max Velocity", &mMaxVelocityConstant, 0.01F, 256.0F, 2048.0F);
		switch (result)
		{
		case EGUI::eDragStatus::eEND_DRAG:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			//EGUI::GetCommandHND()->StartRecording<PhysicsSystem>(&PhysicsSystem::mMaxVelocityConstant);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		default: break;
		}
	}

	void PhysicsSystem::VelocityIterationUI(void)
	{
		const auto result = EGUI::Display::DragInt("Velocity Iterations", &mVelocityIterations, 1, 1, 20);
		switch (result)
		{
		case EGUI::eDragStatus::eEND_DRAG:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			//EGUI::GetCommandHND()->StartRecording<PhysicsSystem>(&PhysicsSystem::mVelocityIterations);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		default: break;
		}
	}

	void PhysicsSystem::PositionalIterationUI()
	{
		const auto result = EGUI::Display::DragInt("Position Iterations", &mPositionalIterations, 1, 1, 20);
		switch (result)
		{
		case EGUI::eDragStatus::eEND_DRAG:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			//EGUI::GetCommandHND()->StartRecording<PhysicsSystem>(&PhysicsSystem::mPositionalIterations);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		default: break;
		}
	}

	void PhysicsSystem::SleepEpsilonUI()
	{
		const auto result = EGUI::Display::DragFloat("Sleep Tolerance", &mfSleepVelEpsilon, 0.001F, 0.001F, 5.0F);
		switch (result)
		{
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eENTER:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			//EGUI::GetCommandHND()->StartRecording<PhysicsSystem>(&PhysicsSystem::mfSleepVelEpsilon);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		default: break;
		}
	}

	void PhysicsSystem::SleepBiasUI()
	{
		const auto result = EGUI::Display::DragFloat("Sleep Bias", &mfSleepBias, 0.01F, 0.1F, 1.0F);
		switch (result)
		{
		case EGUI::eDragStatus::eEND_DRAG:
		case EGUI::eDragStatus::eENTER:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			//EGUI::GetCommandHND()->StartRecording<PhysicsSystem>(&PhysicsSystem::mfSleepBias);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			//EGUI::GetCommandHND()->EndRecording();
			break;
		default: break;
		}
	}

#endif 
}



