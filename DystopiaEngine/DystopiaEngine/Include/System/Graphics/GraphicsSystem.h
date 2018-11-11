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

#include "Utility/Utility.h"

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
	class TextRenderer;
	class SpriteRenderer;

	class GraphicsSystem : public Systems, public ComponentDonor<Renderer>, public ComponentDonor<SpriteRenderer>, public ComponentDonor<TextRenderer>
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
		void Shutdown(void) override;

		void SetGamma(float) noexcept;
		float GetGamma(void) noexcept;

		void ToggleVsync(bool) noexcept;

		bool GetDebugDraw(void) const;
		void ToggleDebugDraw(bool);

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
		Texture* LoadFont(const std::string&);

		Framebuffer& GetGameView(void) const noexcept;
		Framebuffer& GetUIView(void) const noexcept;
		Framebuffer& GetFrameBuffer(void) const noexcept;
		Framebuffer& GetView(int) const;

		template <typename ...Ty>
		Framebuffer* CreateView(Ty&&...);

		static const int& GetDrawMode(void) noexcept;
		static void SetDrawMode(int) noexcept;

		// Temporary
		std::map<std::string, Shader*> shaderlist;

		void EditorUI(void);

	private:

		Math::Vector4 mvDebugColour;
		float mfGamma;
		float mfDebugLineWidth;

		void* mOpenGL; 
		int mPixelFormat;
		int mAvailable, mSettings;

		Ctor::MagicArrayBuilder<Framebuffer>::SetBlockLimit<1>::type mViews;

		static int DRAW_MODE;
		bool mbDebugDrawCheckBox = false;
		bool mbVsync = false;

		void StartFrame(void);
		void EndFrame(void);

		void DrawSplash(void);
		void DrawScene(Camera&, Math::Matrix4&);
		void DrawDebug(Camera&, Math::Matrix4&);

		bool SelectOpenGLVersion(Window&) noexcept;
	};
}

template <typename ...Ty>
Dystopia::Framebuffer* Dystopia::GraphicsSystem::CreateView(Ty&& ..._Args)
{
	return mViews.Emplace(Ut::Forward<Ty>(_Args)...);
}



#endif		// INCLUDE GUARD

