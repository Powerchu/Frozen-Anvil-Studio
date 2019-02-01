/* HEADER *********************************************************************************/
/*!
\file	ParticleSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Particle Systems

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/ParticleSystem.h"
#include "System/Particle/ParticleAffector.h"
#include "System/Graphics/Shader.h"
#include "System/Graphics/Framebuffer.h"
#include "System/Camera/CameraSystem.h"

#include "System/Driver/Driver.h"
#include "System/Time/ScopedTimer.h"
#include "System/Profiler/ProfilerAction.h"

#include "Component/Emitter.h"
#include "Component/Camera.h"
#include "Object/GameObject.h"

#include "Math/Vectors.h"

#include <GL/glew.h>


Dystopia::ParticleSystem::ParticleSystem(void) noexcept
{
}

Dystopia::ParticleSystem::~ParticleSystem(void) noexcept
{
}

bool Dystopia::ParticleSystem::Init(void)
{
	return true;
}

void Dystopia::ParticleSystem::Update(float _dt)
{
	{
		ScopedTimer<ProfilerAction> timeKeeper{ "Particle System", "Update" };

		for (auto& e : mComponents)
		{
			if constexpr (EDITOR)
				if (e.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;

			if (e.GetFlags() & eObjFlag::FLAG_ACTIVE &&	e.GetOwner())
			{
				for (auto& worker : e.GetUpdateAffectors())
					worker.Update(e, _dt);

				unsigned int idx = static_cast<unsigned int>(e.GetLifetime().size());
				for (auto& life : Ut::Range(e.GetLifetime()).Reverse())
				{
					--idx;

					life -= _dt;

					if (e.GetInitialLifetime()[idx] > .0f && life <= .0f)
						e.KillParticle(idx);
				}

				e.Bind();
				e.UploadBuffers();
			}
		}
	}
	
	{
		ScopedTimer<ProfilerAction> timeKeeper{ "Particle System", "Draw" };

		for (auto& e : mComponents)
		{
			if constexpr (EDITOR)
				if (e.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;

			if ((e.GetFlags() & eObjFlag::FLAG_ACTIVE) && e.GetOwner())
			{
				auto& shader = e.GetShader();

				e.Bind();
				shader.Bind();

				for (auto& cam : CORE::Get<CameraSystem>()->GetAllCameras())
				{
					if constexpr (EDITOR)
						if (cam.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;

					// Check valid Camera
					auto ActiveFlags = cam.GetOwner()->GetFlags();
					if ( cam.GetOwner() && 
						(ActiveFlags & eObjFlag::FLAG_ACTIVE) && 
						(e.GetOwner()->GetFlags() & ActiveFlags & eObjFlag::FLAG_ALL_LAYERS))
					{
						cam.GetSurface()->Bind();
						const auto vp = cam.GetViewport();

						glViewport(static_cast<int>(vp.mnX), static_cast<int>(vp.mnY),
							static_cast<int>(vp.mnWidth), static_cast<int>(vp.mnHeight));

						auto const& M = cam.GetViewMatrix();
						auto const& P = cam.GetProjectionMatrix();

						shader.UploadUniform("ProjectMat", P);
						shader.UploadUniform("ModelViewMat", M);

						e.Render();
						cam.GetSurface()->Unbind();
					}

				}
			}

#   if defined(DEBUG) | defined(_DEBUG)
			if (auto err = glGetError())
				__debugbreak();
#    endif
		}
	}
}

void Dystopia::ParticleSystem::FixedUpdate(float _dt)
{
	ScopedTimer<ProfilerAction> timeKeeper{ "Particle System", "Fixed Update" };

	for (auto& e : mComponents)
	{
		if constexpr (EDITOR)
			if (e.GetFlags() & eObjFlag::FLAG_EDITOR_OBJ) continue;

		if (e.GetFlags() & eObjFlag::FLAG_ACTIVE &&	nullptr != e.GetOwner())
		{
			for (auto& worker : e.GetFixedUpdateAffectors())
				worker.Update(e, _dt);
			
			e.FixedUpdate(_dt);
		}
	}
}

void Dystopia::ParticleSystem::PostUpdate(void) 
{
	for (auto& e : mComponents)
		if (e.GetOwner() && (e.GetFlags() & FLAG_REMOVE))
			mComponents.Remove(&e);
}


void Dystopia::ParticleSystem::Shutdown(void)
{
	mComponents.clear();
}


