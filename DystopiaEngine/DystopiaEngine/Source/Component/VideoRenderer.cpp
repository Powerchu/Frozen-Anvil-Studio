#include "Component/VideoRenderer.h"

/*System*/
#include "System/Logger/LoggerSystem.h"
#include "System/Video/VideoSystem.h"
/*I/O*/
#include "IO/TextSerialiser.h"					/*Serialiser*/

/*Object*/
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"

#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"
#endif 


namespace Dystopia
{
	VideoRenderer::VideoRenderer()
	{
	}
	Dystopia::VideoRenderer::VideoRenderer(VideoRenderer const & rhs)
	{
	}

	VideoRenderer::~VideoRenderer()
	{
	}

	void Dystopia::VideoRenderer::Awake(void)
	{
	}

	void Dystopia::VideoRenderer::Load(void)
	{
	}

	void Dystopia::VideoRenderer::Init(void)
	{
	}

	void VideoRenderer::Update(float)
	{
	}

	void Dystopia::VideoRenderer::OnDestroy(void)
	{
	}

	void Dystopia::VideoRenderer::Unload(void)
	{
	}

	VideoRenderer * Dystopia::VideoRenderer::Duplicate() const
	{
		return EngineCore::GetInstance()->GetSystem<VideoSystem>()->RequestComponent(*this);
	}
	void VideoRenderer::Serialise(TextSerialiser & in) const
	{
		in.InsertStartBlock("VideoRenderer");



		in.InsertEndBlock("VideoRenderer");
	}
	void VideoRenderer::Unserialise(TextSerialiser & out)
	{
		out.ConsumeStartBlock();



		out.ConsumeEndBlock();
	}
}

