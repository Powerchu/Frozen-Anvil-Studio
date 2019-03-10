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
		:mVidFileHandle{ nullptr },
		 decoder{ nullptr }, 
		 mWebmHdl{ new WebmInputContext },
		 mVidHdl{ new VpxInputContext },
		 mDecodec{new vpx_codec_ctx_t},
		 mState{VideoState::NEUTRAL},
		 buffer{nullptr},
		 mCodecIterator{nullptr},
	     mRecentFlags{0}
	{
		mWebmHdl->buffer = nullptr;
	}

	Dystopia::VideoRenderer::VideoRenderer(VideoRenderer const & rhs)
	{

	}

	VideoRenderer::~VideoRenderer()
	{
		delete mWebmHdl;
		delete mVidHdl;
		delete mDecodec;
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

		/*Pass file handle to Video Handle*/
		mVidHdl->file   = mVidFileHandle;
		mVidHdl->length = 0;

		/*Prevent invalid pointer*/
		mWebmHdl->buffer        = nullptr;
		mWebmHdl->block         = nullptr;
		mWebmHdl->block_entry   = nullptr;
		mWebmHdl->cluster       = nullptr;

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
				/*Decoder success*/
				mState = VideoState::NEUTRAL;
			}
			else if (!decoder)
			{
#if EDITOR && _DEBUG 

				DEBUG_PRINT(eLog::MESSAGE, "Decoder failed to initialise : line 127 VideoRenderer.cpp");
				return VideoErrorCode::DECODDER_FAIL_INIT;
#endif
			}
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
		mVidFileHandle = nullptr;

		webm_free(mWebmHdl);
		mWebmHdl->buffer = nullptr;
	}

	void VideoRenderer::Play()
	{
		ResetVideo();
		mState = VideoState::PLAYING;
	}

	void VideoRenderer::ResetVideo()
	{
		/*Reset EOF flag*/
		std::clearerr(mVidFileHandle);
		/*Reset file back to top*/
		std::fseek(mVidFileHandle, 0, SEEK_SET);

		buffer      = nullptr;
		mBufferSize = 0;

		mState = VideoState::NEUTRAL;

		if (file_is_webm(mWebmHdl, mVidHdl))
		{
			vpx_codec_dec_cfg_t cfg;

#if EDITOR && _DEBUG
			/*Success file is webm*/
			//DEBUG_PRINT(eLog::MESSAGE, "%s is a webm file", VidName.c_str());

			/*Want to guess frame rate???*/
			webm_guess_framerate(mWebmHdl, mVidHdl);

			/*Get video instance decode*/
			decoder = get_vpx_decoder_by_fourcc(mVidHdl->fourcc);

			if (vpx_codec_dec_init(mDecodec, decoder->codec_interface(), &cfg, NULL))
			{
				/*Decoder success*/
				mState = VideoState::NEUTRAL;
			}
			else if (!decoder)
			{
#if EDITOR && _DEBUG 

				DEBUG_PRINT(eLog::MESSAGE, "Decoder failed to initialise : line 216 VideoRenderer.cpp");
				//return VideoErrorCode::DECODDER_FAIL_INIT;
#endif
			}
#endif
		}
	}

	vid_error_c_t VideoRenderer::ReadNextFrame()
	{
		int code = webm_read_frame(mWebmHdl, &buffer, &mBufferSize);
		if (code == 0)
		{
			if (vpx_codec_decode(mDecodec, buffer, mBufferSize, NULL, 0))
			{

#if EDITOR && _DEBUG
				DEBUG_PRINT(eLog::MESSAGE, "Failed to decode frame in ReadNextFrame");
#endif
				return VideoErrorCode::UNKNOWN_ERROR;
			}

			mCodecIterator = nullptr;
			return VideoErrorCode::OK;
		}
		else if (code == 1)
		{
			webm_free(mWebmHdl);
			mState = VideoState::STOP;
			buffer           = nullptr;
			mCodecIterator   = nullptr;
			mWebmHdl->buffer = NULL;
			mRecentFlags = 0;
			mBufferSize  = 0;

			return VideoErrorCode::WEBM_EOF;
		}
		return VideoErrorCode::UNKNOWN_ERROR;
	}

	vpx_image * VideoRenderer::GetFrameImage()
	{
		return vpx_codec_get_frame(mDecodec, &mCodecIterator);
		/*If current frame has not been completely read*/
		//if (mRecentFlags)
		//{
			//vpx_image_t * img = nullptr;
			//if (img = vpx_codec_get_frame(mDecodec, &mCodecIterator))
			//{
			//	return img;
			//}
			//else
			//{
			//	mRecentFlags = 0;
			//	//mCodecIterator = nullptr;
			//	return nullptr;
			//}
		//}
		/*Get next frame*/
//		else
//		{
//			vid_error_c_t error_code = ReadNextFrame();
//			if (error_code == VideoErrorCode::WEBM_EOF)
//			{
//				/*Free and reset the webm handle*/
//				webm_free(mWebmHdl);
//				mState           = VideoState::STOP;
//				buffer           = nullptr;
//				mCodecIterator   = nullptr;
//				mWebmHdl->buffer = NULL;
//				mRecentFlags     = 0;
//				return nullptr;
//			}
//			else if (error_code == VideoErrorCode::UNKNOWN_ERROR)
//			{
//#if EDITOR && _DEBUG
//				DEBUG_PRINT(eLog::MESSAGE, "Unknown Error in GetFrameImage");
//#endif
//				return nullptr;
//			}
//			else
//			{
//				mRecentFlags = 1;
//				return GetFrameImage();
//			}
//		}

	}


	void VideoRenderer::EditorUI(void) noexcept
	{
		EGUI::PushLeftAlign(140.f);
		//if (EGUI::Display::TextField("Video File", mVid))
		//{
		//	LoadVideo(mVid);
		//}
		EGUI::PopLeftAlign();
	}
}

