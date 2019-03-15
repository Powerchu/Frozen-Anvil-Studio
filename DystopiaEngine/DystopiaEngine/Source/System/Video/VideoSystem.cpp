#include "System/Video/VideoSystem.h"
#include "System/Graphics/GraphicsSystem.h"
#include "System/Graphics/TextureSystem.h"
#include "System/Graphics/Texture2D.h"
#include "System/Time/Timer.h"
#include "System/Driver/Driver.h"

/*Object / Component*/
#include "Component/TextRenderer.h"
#include "Component/SpriteRenderer.h"
#include "Component/VideoRenderer.h"

#include "Object/GameObject.h"

/*Utility*/

/*VPX Library*/
#include "vpx/vpx_decoder.h"                   /*WebmInputContext, VpxInputContext*/
#include "video_reader.h"
#include "vpx_config.h"
#include "tools_common.h"
#include "webmdec.h"
#include "vpx/vpx_image.h"

/* OPEN GL */
#include "GL/glew.h"

/*Testing*/
#include "IO/ImageParser.h"


namespace
{
	static GLsync mFence = 0;
}

namespace Dystopia
{
	VideoSystem::VideoSystem()
		//:mCurrImg{ nullptr }
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
		//if (!mBuffer.pboID)
		//	glGenBuffers(1, &mBuffer.pboID);

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
		VideoRenderer * mCurrentVid = nullptr;

		/*Future support for */
		for (auto && pVid : ComponentDonor<VideoRenderer>::mComponents)
		{
			if (pVid.GetFlags() & FLAG_EDITOR_OBJ || !(pVid.GetFlags() & FLAG_ACTIVE))
				continue;

			if(pVid.mState == VideoState::PLAYING)
				mCurrentVid = &pVid;
		}

		// OpenGL not ready for next frame
		if (mFence)
		{
			if (GL_TIMEOUT_EXPIRED == glClientWaitSync(mFence, 0, 0))
				return;
		}

		mFence = 0;

		//if (mCurrentVid)
		//{
		//	/*Start the countdown*/
		//	mTimer.Countdown(mCurrentVid->mVidHdl->framerate.numerator * 1.f / mCurrentVid->mVidHdl->framerate.denominator);

		//	/*Prev buffer is done decoding, get the next frame*/
		//	if (!mCurrentVid->mBuffer.count)
		//	{
		//		/*Get the next image*/
		//		auto mCurrImg = mCurrentVid->GetFrameImage();
		//		/*If there is no more decoded image to be gotten*/
		//		if (!mCurrImg)
		//		{
		//			/*Get the next decoded frame*/
		//			if (mCurrentVid->ReadNextFrame() == VideoErrorCode::OK)
		//			{
		//				/*Get a new decode image*/
		//				mCurrImg = mCurrentVid->GetFrameImage();
		//				if (mCurrImg && !mTimer.Complete())
		//				{
		//					if (!mCurrentVid->mBuffer.rgb_buff || mCurrentVid->mBuffer.width * mCurrentVid->mBuffer.height < mCurrImg->d_w * mCurrImg->d_h)
		//						mCurrentVid->mBuffer.Resize(mCurrImg->d_h, mCurrImg->d_w, mCurrentVid);
		//					/*Convert to RGB*/
		//					mCurrentVid->mBuffer.mImg = mCurrImg;
		//					Convert_YUV_RGB(&mBuffer, mCurrImg);
		//				}
		//			}
		//			else
		//			{
		//				return;
		//			}
		//		}
		//		else
		//		{
		//			if (!mCurrentVid->mBuffer.rgb_buff || mBuffer.width * mBuffer.height < mCurrImg->d_w * mCurrImg->d_h)
		//				mBuffer.Resize(mCurrImg->d_h, mCurrImg->d_w, mCurrentVid);
		//			/*Convert to RGB*/
		//			mBuffer.mImg = mCurrImg;
		//			Convert_YUV_RGB(&mBuffer, mCurrImg);
		//		}
		//	}
		//	/*Previous video frame is not completely decoded and translated*/
		//	else
		//	{
		//		/*Conver to RGB*/
		//		Convert_YUV_RGB(&mBuffer, mBuffer.mImg);
		//	}

		//	/*Buffer is complete*/
		//	if (mBuffer.IsComplete())
		//	{
		//		/*Pass to graphic to draw the complete image*/
		//		/*TO DO*/
		//		mBuffer.mImg = nullptr;

		//		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mBuffer.pboID);
		//		if (auto err = glGetError())
		//			__debugbreak();
		//		glFlushMappedBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, mBuffer.width * mBuffer.height * mBuffer.stride);
		//		if (auto err = glGetError())
		//			__debugbreak();
		//		mCurrentVid->GetTexture()->Bind();
		//		if (auto err = glGetError())
		//			__debugbreak();
		//		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mBuffer.pboID);
		//		if (auto err = glGetError())
		//			__debugbreak();
		//		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mBuffer.width, mBuffer.height, GL_RGB, GL_UNSIGNED_BYTE, 0);
		//		if (auto err = glGetError())
		//			__debugbreak();
		//		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		//		if (auto err = glGetError())
		//			__debugbreak();
		//		mCurrentVid->GetTexture()->Unbind();
		//		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		//		if (auto err = glGetError())
		//			__debugbreak();
		//		mFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		//		if (auto err = glGetError())
		//			__debugbreak();

		//		//ImageParser::WriteBMP("Output/" + std::to_string(count++) + ".bmp", mBuffer.rgb_buff, mBuffer.width, mBuffer.height);
		//		/*Reset the buffer count*/
		//		mBuffer.ResetCount();
		//		mCurrImg = nullptr;
		//	}
		//}

#if EDITOR
		if (auto err = glGetError())
			__debugbreak();
#endif
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

	void VideoSystem::Convert_YUV_RGB(RGB_BUFFER * buff, vpx_image const * yuv_image)
	{

		uint8_t * YPlane = yuv_image->planes[VPX_PLANE_Y];
		uint8_t * UPlane = yuv_image->planes[VPX_PLANE_U];
		uint8_t * VPlane = yuv_image->planes[VPX_PLANE_V];

		for (unsigned y = buff->count / yuv_image->d_w; y < yuv_image->d_h; ++y)
		{
			for (unsigned x = buff->count % yuv_image->d_w; x < yuv_image->d_w; ++x)
			{


				int Y = YPlane[y      * yuv_image->stride[VPX_PLANE_Y] + x];
				int U = UPlane[(y / 2)* yuv_image->stride[VPX_PLANE_U] + (x / 2)];
				int V = VPlane[(y / 2)* yuv_image->stride[VPX_PLANE_V] + (x / 2)];

				int c = Y - 16;
				int d = U - 128;
				int e = V - 128;

				int r = Clamp((298 * c + 409 * e + 128) >> 8);
				int g = Clamp((298 * c - 100 * d - 208 * e + 128) >> 8);
				int b = Clamp((298 * c + 516 * d + 128) >> 8);

				buff->insert(r, g, b);
			}

			if (mTimer.Complete())
				return;
		}
	}

	int VideoSystem::Clamp(int v) const
	{
		return  v > 0 ? (v < 255 ? v : 255) : 0;
	}

#if EDITOR
	void VideoSystem::EditorUI(void)
	{
	}

#endif

}




