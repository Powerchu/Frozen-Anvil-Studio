#include "System/Video/VideoSystem.h"
#include "Component/VideoRenderer.h"
namespace Dystopia
{
	VideoSystem::VideoSystem()
	{
	}
	VideoSystem::~VideoSystem()
	{
	}
	void VideoSystem::PreInit(void)
	{
	}
	bool VideoSystem::Init(void)
	{
		return true;
	}
	void VideoSystem::PostInit(void)
	{
	}
	void VideoSystem::FixedUpdate(float)
	{
	}
	void VideoSystem::Update(float)
	{
	}
	void VideoSystem::PostUpdate()
	{
	}
	void VideoSystem::Shutdown(void)
	{
	}
	void VideoSystem::LoadDefaults(void)
	{
	}
	void VideoSystem::LoadSettings(TextSerialiser &)
	{
	}
	void VideoSystem::SaveSettings(DysSerialiser_t &)
	{
	}

#if EDITOR
	void VideoSystem::EditorUI(void)
	{
	}
#endif
}




