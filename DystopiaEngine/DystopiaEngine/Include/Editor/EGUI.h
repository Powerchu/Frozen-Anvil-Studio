/* HEADER *********************************************************************************/
/*!
\file	EGUI.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_GUI_H_
#define _EDITOR_GUI_H_
#include "Math\Vector4.h"
#include "Editor\Dock.h"
#include <string>

namespace EGUI
{
	bool StartTab(const char*, bool* = nullptr, ImGuiWindowFlags = 0);
	void EndTab();
	
	bool StartMainMenuBar();
	bool StartMenuHeader(const char*);
	bool StartMenuBody(const char*);
	void EndMainMenuBar();
	void EndMenuHeader();

	bool StartChild(const char*, const Math::Vec4& = Math::Vec4{ 0, 0, 0, 0 });
	bool StartChild(const char*, const float& = 0, const float& = 0);
	void EndChild();

	void SameLine();

	namespace Display
	{
		void Label(const char*, ...);
		void TextField(const char*, char*, size_t);
		bool CollapseHeader(const char*);
		bool VectorFields(const char* _label, Math::Vector4 *_outputVec, float _dragSpeed = 1.0f, float _min = 0.0f, float _max = 0.0f);
		bool CheckBox(const char*, bool*);
		bool DragFloat(const char*, float*, float _dragSpeed = 1.0f, float _min = 0.0f, float _max = 0.0f);
		bool DragInt(const char*, int*, float _dragSpeed = 1.0f, int _min = 0, int _max = 0);
		bool SelectableTxt(const char*, bool*);
		bool SelectableTxt(const char*, bool = false);
		bool SelectableTxtDouble(const char*, bool*);
		bool SelectableTxtDouble(const char*, bool = false);
	}
}

struct ImDrawData;
struct ImGuiContext;
namespace Dystopia
{
	class WindowManager;
	class GraphicsSystem;
	class InputManager;
	class GLState;
	typedef unsigned int GLuint;

	class GuiSystem
	{
	public:
		GuiSystem();
		~GuiSystem();

		bool Init(WindowManager*, GraphicsSystem*, InputManager*, const char* = "MainDockable");
		void StartFrame(const float&);
		void EndFrame();
		void Shutdown();

		const char* GetMainDockspaceName() const;

	private:
		ImGuiContext	*mpCtx;
		WindowManager	*mpWin;
		GraphicsSystem	*mpGfx;
		InputManager	*mpInput;
		ImDrawData		*mpDrawData;
		GLState			*mpGLState;
		std::string		mGlslVersion;
		GLuint			mFontTexture;
		const char		*mpMainDockspace;
		int				mShaderHandle;
		int				mVertHandle;
		int				mFragHandle;
		int				mAttribLocationTex;
		int				mAttribLocationProjMtx;
		int				mAttribLocationPosition;
		int				mAttribLocationUV;
		int				mAttribLocationColor;
		unsigned int	mVboHandle;
		unsigned int	mElementsHandle;
		bool			mMouseJustPressed[3];	// 0 is left, 1 is right, 2 is middle

		void			CreateDefaultFont();
		void			UpdateCharInputs();
		void			UpdateMouseInputs();
		void			UpdateKeyInputs();
		void			UpdateScrollInputs();

		void			StartFullDockableSpace();
		void			EndFullDockableSpace();
	};
}

#endif // !_EDITOR_GUI_H_
#endif // EDITOR ONLY













