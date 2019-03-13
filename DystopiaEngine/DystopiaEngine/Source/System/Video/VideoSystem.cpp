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
		:mCurrImg{ nullptr }
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
		if (!mBuffer.pboID)
			glGenBuffers(1, &mBuffer.pboID);

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
		if (mCurrentVid)
		{
			/*Start the countdown*/
			mTimer.Countdown(mCurrentVid->mVidHdl->framerate.numerator * 1.f / mCurrentVid->mVidHdl->framerate.denominator);

			/*Prev buffer is done decoding, get the next frame*/
			if (!mBuffer.count)
			{
				/*Get the next image*/
				auto mCurrImg = mCurrentVid->GetFrameImage();
				/*If there is no more decoded image to be gotten*/
				if (!mCurrImg)
				{
					/*Get the next decoded frame*/
					if (mCurrentVid->ReadNextFrame() == VideoErrorCode::OK)
					{
						/*Get a new decode image*/
						mCurrImg = mCurrentVid->GetFrameImage();
						if (mCurrImg && !mTimer.Complete())
						{
							if (!mBuffer.rgb_buff || mBuffer.width * mBuffer.height < mCurrImg->d_w * mCurrImg->d_h)
								mBuffer.Resize(mCurrImg->d_h, mCurrImg->d_w, mCurrentVid);
							/*Convert to RGB*/
							Convert_YUV_RGB(&mBuffer, mCurrImg);
						}
					}
				}
				else
				{
					if (!mBuffer.rgb_buff || mBuffer.width * mBuffer.height < mCurrImg->d_w * mCurrImg->d_h)
						mBuffer.Resize(mCurrImg->d_h, mCurrImg->d_w, mCurrentVid);
					/*Convert to RGB*/
					Convert_YUV_RGB(&mBuffer, mCurrImg);
				}
			}
			/*Previous video frame is not completely decoded and translated*/
			else
			{
				/*Conver to RGB*/
				Convert_YUV_RGB(&mBuffer, mCurrImg);
			}

			/*Buffer is complete*/
			if (mBuffer.IsComplete())
			{
				/*Pass to graphic to draw the complete image*/
				/*TO DO*/

				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mBuffer.pboID);
				if (auto err = glGetError())
					__debugbreak();
				glFlushMappedBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, mBuffer.width * mBuffer.height * mBuffer.stride);
				if (auto err = glGetError())
					__debugbreak();
				mCurrentVid->GetTexture()->Bind();
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, mBuffer.pboID);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mBuffer.width, mBuffer.height, GL_RGB, GL_UNSIGNED_BYTE, 0);
				if (auto err = glGetError())
					__debugbreak();
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
				if (auto err = glGetError())
					__debugbreak();
				mCurrentVid->GetTexture()->Unbind();
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
				if (auto err = glGetError())
					__debugbreak();
				mFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
				if (auto err = glGetError())
					__debugbreak();

				//ImageParser::WriteBMP("Output/" + std::to_string(count++) + ".bmp", mBuffer.rgb_buff, mBuffer.width, mBuffer.height);
				/*Reset the buffer count*/
				mBuffer.ResetCount();
				mCurrImg = nullptr;
			}
		}

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


		for (unsigned y = 0; y < yuv_image->d_h; ++y)
			for (unsigned x = 0; x < yuv_image->d_w; ++x)
			{
				if (mTimer.Complete())
					return;

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

	VideoSystem::RGB_BUFFER::RGB_BUFFER()
		:height{ 0 }, width{ 0 }, stride{ sizeof(uint8_t) * 3 }, count{ 0 }, rgb_buff{ nullptr }, pboID{ 0 }
	{
	}
	VideoSystem::RGB_BUFFER::RGB_BUFFER(unsigned h, unsigned w)
		: height{ h }, width{ w }, stride{ sizeof(uint8_t) * 3 }, count{ 0 }, rgb_buff{ nullptr /*static_cast<uint8_t*>(::operator new(h*w*stride))*/ }, pboID{ 0 }
	{
		//memset(rgb_buff, 255, h*w*stride);
	}
	VideoSystem::RGB_BUFFER::~RGB_BUFFER()
	{
		//::operator delete (rgb_buff);

		if (rgb_buff)
		{
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}
		rgb_buff = nullptr;
		glDeleteBuffers(1, &pboID);
	}
	void VideoSystem::RGB_BUFFER::Resize(unsigned h, unsigned w, VideoRenderer * _pRenderer)
	{
		constexpr GLbitfield mapFlags = GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT | GL_MAP_WRITE_BIT;

		//::operator delete(rgb_buff);
		//rgb_buff = nullptr;
		//rgb_buff = static_cast<uint8_t*>(::operator new(h*w*stride));
		//memset(rgb_buff, 255, h*w*stride);
		height = h;
		width  = w;
		count  = 0;

		_pRenderer->GetTexture()->ReplaceTexture(w, h, nullptr, false);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);
		if (rgb_buff) glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		glBufferStorage(GL_PIXEL_UNPACK_BUFFER, w*h*stride, nullptr, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
		//glBufferData(GL_PIXEL_UNPACK_BUFFER, w*h*stride, nullptr, GL_STREAM_DRAW);
		if (auto err = glGetError())
		{
			__debugbreak();
		}
		rgb_buff = static_cast<uint8_t*>(glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, w*h*stride, mapFlags));

		if (auto err = glGetError())
		{
			__debugbreak();
		}
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

		if (auto err = glGetError())
		{
			__debugbreak();
		}
	}
	void VideoSystem::RGB_BUFFER::ResetCount()
	{
		count = 0;
	}
	void VideoSystem::RGB_BUFFER::insert(int r, int g, int b)
	{
		if (count >= height * width)
			return;
		new (rgb_buff + count   * stride)       uint8_t{ static_cast<uint8_t>(r) };
		new (rgb_buff + count   * stride + 1)   uint8_t{ static_cast<uint8_t>(g) };
		new (rgb_buff + count++ * stride + 2)   uint8_t{ static_cast<uint8_t>(b) };
	}
	bool VideoSystem::RGB_BUFFER::IsComplete() const
	{
		return count == height * width;
	}
}




