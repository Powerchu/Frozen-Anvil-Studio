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
#include "System/File/FileSystem.h"      // eFileDir
#include "Math/MathFwd.h"
#include "Math/Vectors.h"

#include "Utility/Utility.h"
#include "DataStructure/AutoArray.h"

#include <map>
#include <string>


namespace Dystopia
{
	struct Font;
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

		_DLL_EXPORT void SetGamma(float) noexcept;
		_DLL_EXPORT float GetGamma(void) noexcept;

		_DLL_EXPORT void ToggleVsync(bool) noexcept;
		_DLL_EXPORT bool GetVsync(void) noexcept;

		bool GetDebugDraw(void) const;
		void ToggleDebugDraw(bool) const;

		float GetAspectRatio() const;

		void SetAllCameraAspect(float _x, float _y) const;

		// Sets up Window for openGL rendering
		void MakeActiveWindow(Window&) noexcept;

		void LoadDefaults(void) override;
		void LoadSettings(DysSerialiser_t&) override;
		void SaveSettings(DysSerialiser_t&) override;

		void     LevelLoad(TextSerialiser&);
		void     LoadMesh(const std::string&);
		Texture* LoadTexture(HashString const&);
		_DLL_EXPORT Texture* LoadTexture(const char *);
		_DLL_EXPORT void LoadAllTexture();
		Font* LoadFont(const std::string&);

		Framebuffer& GetGameView(void) const noexcept;
		Framebuffer& GetUIView(void) const noexcept;
		Framebuffer& GetFrameBuffer(void) const noexcept;
		//Framebuffer& GetSceneView(void) const noexcept;
		Framebuffer& GetView(int) const;

		template <typename ...Ty>
		Framebuffer* CreateView(Ty&&...);

		static const int& GetDrawMode(void) noexcept;
		static void SetDrawMode(int) noexcept;

		void SetResolution(unsigned w, unsigned h) noexcept;
		void UpdateResolution(void) noexcept;

		void EditorAspectRatio(void);
		void EditorUI(void);

		void DrawSplash(void);
	private:

		Math::Vector4 mvDebugColour;
		Math::Vector4 mvClearCol;
		Math::Vector2 mvResolution;

		float mfGamma;
		float mfDebugLineThreshold;

		int mPixelFormat;
		int mAvailable, mSettings;

		Ctor::MagicArrayBuilder<Framebuffer>::SetBlockLimit<1>::type mViews;

		static int DRAW_MODE;

		void StartFrame(void);
		void EndFrame(void);

		void DrawScene(Camera& /*, Math::Mat4 const&, Math::Mat4 const&*/ );
		void DrawDebug(Camera& /*, Math::Mat4 const&, Math::Mat4 const&*/ );

		void LoadFramebuffers(void) noexcept;

		AutoArray<Renderer*>       mSet1;
		AutoArray<SpriteRenderer*> mSet2;
		AutoArray<TextRenderer*>   mSet3;
		AutoArray<Renderer*>       mTransparency;

#	if EDITOR
		bool mbVSync;
		bool mbDebugDrawCheckBox = false;
#   endif
	};
}

template <typename ...Ty>
Dystopia::Framebuffer* Dystopia::GraphicsSystem::CreateView(Ty&& ..._Args)
{
	return mViews.Emplace(Ut::Forward<Ty>(_Args)...);
}



#endif		// INCLUDE GUARD

