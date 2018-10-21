#include "Component/Collider.h"
#include "Component/RigidBody.h"
#include "System/Physics/PhysicsSystem.h"
#include "System/Logger/LoggerSystem.h"
#include "System/Profiler/ProfilerAction.h"
#include "System/Time/ScopedTimer.h"
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"
#include "System/Collision/CollisionSystem.h"

#if EDITOR
#include "Editor/EGUI.h"
#include "Editor/CommandList.h"
#include "Editor/Commands.h"
#endif 

namespace Dystopia
{
	PhysicsSystem::PhysicsSystem()
		: mbIsDebugActive(false)
		, mInterpolation_mode(none)
		, mGravity(400.0F)
		, mMaxVelocityConstant(1024.0F)
		, mMaxVelSquared(mMaxVelocityConstant*mMaxVelocityConstant)
		, mPenetrationEpsilon(0.05F)
		, mVelocityIterations(8)
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
			if (body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
			if (body.GetOwner())
			{
				if(!body.Get_IsStaticState())
					body.CheckSleeping(_dt);

				const auto col = body.GetOwner()->GetComponent<Collider>();

				if (nullptr != col)
				{
					col->SetSleeping(!body.GetIsAwake());
				}
			}
		}
	}

	void PhysicsSystem::IntegrateRigidBodies(float _dt)
	{
		for (auto& body : mComponents)
		{
#if EDITOR
			if (body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
			if (!body.Get_IsStaticState() && body.GetIsAwake())
			{
				body.Integrate(_dt);
			}
		}
	}

	void PhysicsSystem::ResolveCollision(float _dt)
	{
		for (int i = 0; i < mVelocityIterations; ++i)
		{
			for (auto& body : mComponents)
			{
#if EDITOR
				if (body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
				if (!body.GetIsAwake()) continue;

				const auto col = body.GetOwner()->GetComponent<Collider>();

				if (nullptr != col)
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
#if EDITOR
			if (body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
			if (body.Get_IsStaticState() || !body.GetIsAwake()) continue;

			body.PreUpdatePosition(_dt);
		}

		for (int i = 0; i < mPositionalIterations; ++i)
		{
			for (auto& body : mComponents)
			{
#if EDITOR
				if (body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
				if (!body.GetIsAwake()) continue;

				const auto col = body.GetOwner()->GetComponent<Collider>();

				if (nullptr != col)
				{
					if (!col->IsTrigger())
					{
						auto worstPene = mPenetrationEpsilon;
						for (auto& manifold : col->GetCollisionEvents())
						{
							if (manifold.mfPeneDepth > worstPene)
							{
								const auto worstContact = &manifold;
								worstPene = manifold.mfPeneDepth;

								if (nullptr != worstContact)
								{
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
#if EDITOR
			if (body.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;
#endif 
			if (body.Get_IsStaticState() || !body.GetIsAwake()) continue;

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

	void PhysicsSystem::LoadSettings(TextSerialiser& serial)
	{
		UNUSED_PARAMETER(serial);
	}

	void PhysicsSystem::EditorUI(void)
	{
#if EDITOR			
		IsDebugUI();
		GravityUI();
		MaxVelocityUI();
		VelocityIterationUI();
		PositionalIterationUI();
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
		const auto result = EGUI::Display::DragFloat("Gravity     ", &mGravity, 0.01f, -FLT_MAX, FLT_MAX);
		switch (result)
		{
		case EGUI::eDragStatus::eEND_DRAG:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			EGUI::GetCommandHND()->StartRecording<PhysicsSystem>(&mGravity);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		default: break;
		}
	}

	void PhysicsSystem::IsDebugUI(void)
	{
		bool tempBool = mbIsDebugActive;
		if (EGUI::Display::CheckBox("Debug Draw  ", &tempBool))
		{
			mbIsDebugActive = tempBool;
			EGUI::GetCommandHND()->InvokeCommand<PhysicsSystem>(&mbIsDebugActive, tempBool);
		}
	}

	void PhysicsSystem::MaxVelocityUI()
	{
		const auto result = EGUI::Display::DragFloat("Max Velocity", &mMaxVelocityConstant, 0.01F, 256.0F, 2048.0F);
		switch (result)
		{
		case EGUI::eDragStatus::eEND_DRAG:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			EGUI::GetCommandHND()->StartRecording<PhysicsSystem>(&mMaxVelocityConstant);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			EGUI::GetCommandHND()->EndRecording();
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
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			EGUI::GetCommandHND()->StartRecording<PhysicsSystem>(&mVelocityIterations);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		default: break;
		}
	}

	void PhysicsSystem::PositionalIterationUI()
	{
		const auto result = EGUI::Display::DragInt("Positional Iterations", &mPositionalIterations, 1, 1, 20);
		switch (result)
		{
		case EGUI::eDragStatus::eEND_DRAG:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eENTER:
			EGUI::GetCommandHND()->EndRecording();
			break;
		case EGUI::eDragStatus::eSTART_DRAG:
			EGUI::GetCommandHND()->StartRecording<PhysicsSystem>(&mPositionalIterations);
			break;
		case EGUI::eDragStatus::eDEACTIVATED:
			EGUI::GetCommandHND()->EndRecording();
			break;
		default: break;
		}
	}

#endif 
}



