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
	class Texture2D;


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

	struct RGB_BUFFER
	{
		unsigned height;
		unsigned width;
		unsigned stride;
		unsigned count;



		RGB_BUFFER();

		RGB_BUFFER(unsigned h, unsigned w);

		~RGB_BUFFER();

		void Resize(unsigned h, unsigned w, VideoRenderer * _pRenderer);
		void ResetCount();
		void insert(int r, int g, int b);
		bool IsComplete()   const;
		unsigned GetPboID() const;

	private:
		uint8_t   * rgb_buff;
		vpx_image * mImg;
		unsigned    pboID;


		void DeleteBuffer();
	};

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
			/*No i do not want to allow this. Too many shit to handle*/
			VideoRenderer& operator=(VideoRenderer const &) = delete;
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

		    vid_error_c_t LoadVideo(HashString const & VidName);
		   
		    void CloseCurrentVideo();
		    void Play();
		    void ResetVideo();
		    void PlayOnStart(bool _b);
		    void Pause();

			Texture2D* GetTexture(void) const noexcept;

			vid_error_c_t ReadNextFrame();
			vpx_image *   GetFrameImage();
			bool          BufferIsComplete()  const;
			bool          BufferIsComplete();
			unsigned      RgbBufferBufferID() const;
			bool          IsDone()            const;
			bool          IsPlaying()         const;
			bool          IsPause()           const;
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
			void *                mAudioSegment;
			//void* glBuffer;

			Texture2D* mpTexture;
			vpx_image* mpCurrImg;
			RGB_BUFFER mBuffer;

			VideoState            mState;
			unsigned              mRecentFlags;
			float                 mElapsedTime;
			bool                  mPlayOnStart;
			bool                  mbLoop;
			bool                  mbPrevDone;

			/*Functions*/

			

			friend VideoSystem;
	};
}


#endif

