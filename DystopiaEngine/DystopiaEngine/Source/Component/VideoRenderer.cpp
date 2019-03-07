#include "Component/VideoRenderer.h"

/*System*/
#include "System/Logger/LoggerSystem.h"
#include "System/Video/VideoSystem.h"
#include "System/File/FileSystem.h"
/*I/O*/
#include "IO/TextSerialiser.h"					/*Serialiser*/

/*Object*/
#include "Object/GameObject.h"
#include "Object/ObjectFlags.h"


/*VPX Library*/
#include "vpx/vpx_decoder.h"                   /*WebmInputContext, VpxInputContext*/
#include "video_reader.h"
#include "vpx_config.h"
#include "tools_common.h"
#include "webmdec.h"
#include "vpx/vpx_image.h"

#if EDITOR
#include "Editor/ProjectResource.h"
#include "Editor/EGUI.h"
#include "Editor/EditorMain.h"
#include "Editor/EditorCommands.h"
#endif 


namespace Dystopia
{
	VideoRenderer::VideoRenderer()
		:mVidFileHandle{ nullptr }, decoder{ nullptr }, mWebmHdl{ new WebmInputContext }, mVidHdl{ new VpxInputContext }, mDecodec{new vpx_codec_ctx_t}
	{

	}

	Dystopia::VideoRenderer::VideoRenderer(VideoRenderer const & rhs)
	{

	}

	VideoRenderer::~VideoRenderer()
	{
		delete mWebmHdl;
		delete mVidHdl;
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
	_DLL_EXPORT_ONLY vid_error_c_t VideoRenderer::LoadVideo(HashString const & VidName)
	{
		auto && path = EngineCore::Get<FileSystem>()->GetFullPath(VidName.c_str(), eFileDir::eResource);

		if (path.empty())
			return VideoErrorCode::VIDEO_FILE_NOT_FOUND;
		if (mVidFileHandle)
			CloseCurrentVideo();

		fopen_s(&mVidFileHandle,path.c_str(), "rb");

		if (!mVidFileHandle)
			return VideoErrorCode::VIDEO_FILE_FAIL_TO_OPEN;

		if (file_is_webm(mWebmHdl, mVidHdl))
		{
			vpx_codec_dec_cfg_t cfg;
#if EDITOR && _DEBUG
			/*Success file is webm*/
			DEBUG_PRINT(eLog::MESSAGE, "%s is a webm file", VidName.c_str());
			
			/*Want to guess frame rate???*/
			webm_guess_framerate(mWebmHdl, mVidHdl);
			/*Get video instance decode*/
			decoder = get_vpx_decoder_by_fourcc(mVidHdl->fourcc);
			if (vpx_codec_dec_init(mDecodec, decoder->codec_interface(), &cfg, NULL))
			{
				std::cout << "Decoder Init failed \n";
			}
			if (!decoder) die("Unknown input codec");
#endif
		}
		else
		{
			fclose(mVidFileHandle);
			mVidFileHandle = nullptr;
			return VideoErrorCode::VIDEO_FILE_NOT_WEBM;
		}

		return VideoErrorCode::OK;
	}

	void VideoRenderer::CloseCurrentVideo()
	{
		mVidFileHandle && fclose(mVidFileHandle);
	}
}

