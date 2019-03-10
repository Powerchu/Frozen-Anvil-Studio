#pragma once
#ifndef _VIDEO_RENDERER_H
#define _VIDEO_RENDERER_H

/*Includes*/

#include "Component/ComponentList.h"           /*Component List*/
#include "Component/Component.h"
#include "DataStructure/HashString.h"


/*Webm predeclaration*/
struct VpxInterface;
struct WebmInputContext;
struct VpxInputContext;
struct vpx_codec_ctx;
struct vpx_image;

namespace Dystopia
{
	class VideoSystem;


	typedef enum class VideoErrorCode : unsigned
	{
		OK = 0,
		VIDEO_FILE_NOT_FOUND,
		VIDEO_FILE_FAIL_TO_OPEN,
		VIDEO_FILE_NOT_WEBM,

		DECODDER_FAIL_INIT,

		WEBM_EOF,
		UNKNOWN_ERROR,
		
	}vid_error_c_t;

	typedef enum class VideoState : unsigned
	{
		NEUTRAL,
		PLAYING,
		PAUSE,
		STOP,

		FRAME_END,
		

	}vid_status_t;
	class _DLL_EXPORT VideoRenderer : public Component
	{
		public:

			using SYSTEM = VideoSystem;

			unsigned GetComponentType(void) const
			{
				return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllComponents>::value;
			};
			unsigned GetRealComponentType(void) const
			{
				return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, UsableComponents>::value;
			};

#if EDITOR
			static const std::string GetCompileName(void) { return "VideoRenderer"; }
			const std::string GetEditorName(void) const   { return GetCompileName(); }
#endif

			VideoRenderer();
			VideoRenderer(VideoRenderer const & rhs);
			~VideoRenderer();

			void Awake(void);
			/*Load the Component*/
			virtual void Load(void);
			/*Initialise the Component*/
			virtual void Init(void);

			virtual void Update(float);
			/*OnDestroy*/
			virtual void OnDestroy(void);
			/*Unload the Component*/
			virtual void Unload(void);
			/*Duplicate the Component*/
			virtual VideoRenderer* Duplicate() const;

			virtual void Serialise(TextSerialiser&) const override;
			virtual void Unserialise(TextSerialiser&)     override;

			_DLL_EXPORT_ONLY vid_error_c_t LoadVideo(HashString const & VidName);

			void CloseCurrentVideo();
			void Play();
			void ResetVideo();

			vid_error_c_t ReadNextFrame();
			vpx_image *   GetFrameImage();

#if EDITOR
			void EditorUI(void) noexcept override;
#endif

		protected:


		private:

			HashString            mVid;              /*Name of video           */
			FILE *                mVidFileHandle;    /*Handle to video file    */
			const VpxInterface *  decoder;           /*Decoder for Video       */
			WebmInputContext   *  mWebmHdl;          /*Handle for webm         */
			VpxInputContext    *  mVidHdl;
			vpx_codec_ctx      *  mDecodec;
			uint8_t            *  buffer;
			size_t                mBufferSize;
			const void *          mCodecIterator;

			VideoState            mState;
			unsigned              mRecentFlags;
			/*Functions*/


			friend VideoSystem;
	};
}


#endif


