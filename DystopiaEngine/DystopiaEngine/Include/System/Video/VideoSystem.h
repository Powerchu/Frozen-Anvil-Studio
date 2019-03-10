#pragma once
#ifndef _VIDEO_SYSTEM_H
#define _VIDEO_SYSTEM_H

#include "System/Base/Systems.h"
#include "System/Base/ComponentDonor.h"
#include "System/Time/Timer.h"

struct vpx_image;
namespace Dystopia
{
	class VideoRenderer;
	class VideoSystem : public Systems, public ComponentDonor<VideoRenderer>
	{
		struct RGB_BUFFER
		{
			unsigned height;
			unsigned width;
			unsigned stride;
			unsigned count;
			unsigned pboID;
			uint8_t * rgb_buff;

			RGB_BUFFER();

			RGB_BUFFER(unsigned h, unsigned w);

			~RGB_BUFFER();

			void Resize(unsigned h, unsigned w);
			void ResetCount();
			void insert(int r, int g, int b);
			bool IsComplete() const;
		};
	public:

		VideoSystem();
		~VideoSystem();


		virtual void PreInit(void) override;
		virtual bool Init(void) override;
		virtual void PostInit(void) override;

		virtual void FixedUpdate(float) override;
		virtual void Update(float) override;

		virtual void PostUpdate() override;

		virtual void Shutdown(void) override;

		virtual void LoadDefaults(void) override;
		virtual void LoadSettings(TextSerialiser&) override;
		virtual void SaveSettings(DysSerialiser_t&);

#if EDITOR
		void EditorUI(void);
#endif

	protected:



	private:
		RGB_BUFFER  mBuffer;
		RGB_BUFFER  mBuffer2;

		static unsigned pboID;

		vpx_image * mCurrImg;
		Timer         mTimer;
		void Convert_YUV_RGB(RGB_BUFFER * buffer, vpx_image const * yuv_image);
		int  Clamp(int v) const;
	};

}




#endif



