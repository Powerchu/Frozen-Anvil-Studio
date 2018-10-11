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

#include "System/Base/Systems.h"		 // System
#include "System/Base/ComponentDonor.h"
#include "System/Graphics/Framebuffer.h"
#include "Math/MathFwd.h"
#include "Math/Vector4.h"

#include <map>
#include <string>


namespace Dystopia
{
	class Mesh;
	class Shader;
	class Camera;
	class Window;
	class Texture;
	class Renderer;

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

		void Update(float) override;		// Draws the currently bounded window
		void PostUpdate(void) override;
		void Shutdown(void);

		void SetGamma(float) noexcept;
		float GetGamma(void) noexcept;

		bool GetDebugDraw(void) const;
		void ToggleDebugDraw(void);

		// Sets up Window for openGL rendering
		bool InitOpenGL(Window&);
		void BindOpenGL(Window&) noexcept;

		void LoadDefaults(void) override;
		void LoadSettings(DysSerialiser_t&) override;
		void SaveSettings(DysSerialiser_t&) override;

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

		Framebuffer& GetFrameBuffer();

		void EditorUI(void);

	private:
		Math::Vector4 mvDebugColour;
		float mfGamma;
		float mfDebugLineWidth;

		void* mOpenGL;
		int mPixelFormat, mAvailable;
		Framebuffer mGameView, mUIView;

		bool mbDebugDraw;

		static int DRAW_MODE;

		void StartFrame(void);
		void EndFrame(void);

		void DrawSplash(void);
		void DrawScene(Camera&, Math::Matrix4&);
		void DrawDebug(Camera&, Math::Matrix4&);

		bool SelectOpenGLVersion(Window&) noexcept;
	};
}



#endif		// INCLUDE GUARD

