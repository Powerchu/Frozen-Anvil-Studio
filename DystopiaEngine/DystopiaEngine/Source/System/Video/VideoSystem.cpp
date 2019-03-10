#include "System/Video/VideoSystem.h"
#include "Component/VideoRenderer.h"
#include "System/Graphics/TextureSystem.h"
#include "System/Graphics/Texture.h"


/*VPX Library*/
#include "vpx/vpx_decoder.h"                   /*WebmInputContext, VpxInputContext*/
#include "video_reader.h"
#include "vpx_config.h"
#include "tools_common.h"
#include "webmdec.h"
#include "vpx/vpx_image.h"

/*Testing*/
#include "IO/ImageParser.h"
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
		static VideoRenderer * testvid = nullptr;
		static bool test = true;
		static int  once = 0;
		static int count = 0;
		if (test)
		{
			testvid = ComponentDonor<VideoRenderer>::RequestComponent();
			test = false;
			testvid->mVidHdl->length = 0;
			
		}
		else if(testvid && !once)
		{
			if(testvid->LoadVideo("TEST_VIDEO.webm") == VideoErrorCode::OK)
				testvid->Play();
			testvid->ReadNextFrame();
			once = 1;
		}

		for (auto && pVid : ComponentDonor<VideoRenderer>::mComponents)
		{
			if (pVid.mState == VideoState::PLAYING)
			{
 				auto img = pVid.GetFrameImage();
				if (!img)
				{
					if (pVid.ReadNextFrame() == VideoErrorCode::OK)
					{
						auto img = pVid.GetFrameImage();
						if (img)
						{
							Convert_YUV_RGB(&mBuffer, img);

							/*Pass to graphics to render*/
							/*TO DO*/
							
							/*For debugging*/
							ImageParser::WriteBMP("Output/" + std::to_string(count++) + ".bmp", mBuffer.rgb_buff, mBuffer.width, mBuffer.height );
						}
						memset(mBuffer.rgb_buff, 0, mBuffer.width * mBuffer.height *  mBuffer.stride);
					}
				}
				else
				{
					/*Convert to RGB*/
					Convert_YUV_RGB(&mBuffer, img);

					/*Pass to graphics for rendering*/
					/*TO DO*/

					ImageParser::WriteBMP("Output/" + std::to_string(count++) + ".bmp", mBuffer.rgb_buff, mBuffer.width, mBuffer.height);
				}
				/*Reset Buffer*/
				mBuffer.ResetCount();
			}
			else if (pVid.mState == VideoState::STOP)
			{
				/*Test Reset*/
				count = 0;
				pVid.Play();
			}
		}
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
		if (!buff || buff->width * buff->height < yuv_image->d_w * yuv_image->d_h)
			buff->Resize(yuv_image->d_h, yuv_image->d_w);
		uint8_t * YPlane = yuv_image->planes[VPX_PLANE_Y];
		uint8_t * UPlane = yuv_image->planes[VPX_PLANE_U];
		uint8_t * VPlane = yuv_image->planes[VPX_PLANE_V];


		for (unsigned y = 0; y < yuv_image->d_h; ++y)
			for (unsigned x = 0; x < yuv_image->d_w; ++x)
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
		:height{ 0 }, width{ 0 }, stride{ sizeof(uint8_t) * 4 }, count{ 0 }, rgb_buff{ nullptr }
	{

	}
	VideoSystem::RGB_BUFFER::RGB_BUFFER(unsigned h, unsigned w)
		: height{ h }, width{ w }, stride{ sizeof(uint8_t) * 4 }, count{ 0 }, rgb_buff{ static_cast<uint8_t*>(::operator new(h*w*stride)) }
	{
		memset(rgb_buff, 255, h*w*stride);
	}
	VideoSystem::RGB_BUFFER::~RGB_BUFFER()
	{
		::operator delete (rgb_buff);
	}
	void VideoSystem::RGB_BUFFER::Resize(unsigned h, unsigned w)
	{
		::operator delete(rgb_buff);
		rgb_buff = nullptr;
		rgb_buff = static_cast<uint8_t*>(::operator new(h*w*stride));
		memset(rgb_buff, 255, h*w*stride);
		height = h;
		width  = w;
		count  = 0;
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
}




