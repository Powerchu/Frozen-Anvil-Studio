/* HEADER *********************************************************************************/
/*!
\file	GraphicsSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _GRAPHICS_SYS_H_
#define _GRAPHICS_SYS_H_

#include "System\Base\Systems.h"		 // System
#include "System\Base\ComponentDonor.h"
#include "System\Graphics\Framebuffer.h"

#include <string>
#include <map>
#include "DataStructure\AutoArray.h"


namespace Dystopia
{
	class Window;
	class Mesh;
	class Texture;
	class Shader;
	class Renderer;

	struct TextureInfo
	{
		TextureInfo(const uint64_t&  _id, float _x, float _y)
			: mID{ _id }, x{ _x }, y{ _y }
		{}
		uint64_t mID;
		float x;
		float y;
	};

	class GraphicsSystem : public Systems, public ComponentDonor<Renderer>
	{
	public :
		// ====================================== CONSTRUCTORS ======================================= // 

		GraphicsSystem(void) noexcept;

		~GraphicsSystem(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void PreInit(void);
		bool Init(void);
		void PostInit(void);

		void Update(float);		// Draws the currently bounded window
		void Shutdown(void);

		void SetGamma(float) noexcept;
		float GetGamma(void) noexcept;

		// Sets up Window for openGL rendering
		bool InitOpenGL(Window&);
		void BindOpenGL(Window&) noexcept;

		void LoadDefaults(void);
		void LoadSettings(DysSerialiser_t&);

		void     LevelLoad(TextSerialiser&);
		void     LoadMesh(const std::string&);
		Texture* LoadTexture(const std::string&);
		Shader*	 LoadShader(const std::string&);

		void SetMasterViewport(int _nX, int _nY, int _nWidth, int _nHeight) noexcept;

		static const int& GetDrawMode(void) noexcept;
		static void SetDrawMode(int) noexcept;

		// Temporary
		std::map<std::string, Shader*> shaderlist;
		std::map<std::string, Texture*> texturelist;

		AutoArray<TextureInfo> textureInfos;

	private:

		float mfGamma;

		void* mOpenGL;
		int mPixelFormat, mAvailable;
		Framebuffer mGameView, mUIView;


		static int DRAW_MODE;

		void StartFrame(void);
		void EndFrame(void);

		void DrawSplash(void);

		bool SelectOpenGLVersion(Window&) noexcept;
	};
}



#endif		// INCLUDE GUARD

