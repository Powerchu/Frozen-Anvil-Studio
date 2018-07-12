/* HEADER *********************************************************************************/
/*!
\file	EGUI.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
	This is the Encapsulation of IMGUI functions to be of ease for other programmers to use.
	Please read the brief and usage of each function. 
	If none exist, either I forgot, or I didnt intend for you to use those. 
	
	If you need anything specificly, lemme know and I will create a user-friendly one for you. 
	Else, just hack it - include imgui.h and use more functions available at your own risk.

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
#include "Editor\Payloads.h"
#include "Utility\DebugAssert.h"
#include <string>
namespace Dystopia
{
	class CommandHandler;
}

/* =======================================================================================================================
	Brief:
		If you call any Start... functions, please remember to call their respective End... functions.
		Most Start.. function returns a bool, the return generally(mostly) means if the display is opened or closed.
		Meaning if a Tab is opened or closes, a Header is opened or closed
		USAGE :
		if (Start...)
		{
		// do your other stuff here
		}
		End...
======================================================================================================================= */
namespace EGUI
{
	void SetContext(Dystopia::CommandHandler *_pContext);
	void RemoveContext();
	bool StartMainMenuBar();
	bool StartMenuHeader(const std::string&);
	bool StartMenuBody(const std::string&, const std::string& _shortcut="");
	void EndMainMenuBar();
	void EndMenuHeader();

	/* =======================================================================================================================
	Brief:	
			You can ignore the last 2 parameters. Read the below usage
	Usage: 
			if (StartTab("name of the tab", defaulted ignore, defaulted ignore))
			{
				// other GUI stuff inside this Tab window
				// if (EGUI::StartChild(...))
				// {
				// }
				// EGUI::EndChild(...);
			}
			EndTab();
	======================================================================================================================= */
	// Call to Start a new Tab
	bool StartTab(const std::string&, bool* = nullptr, ImGuiWindowFlags = 0);
	// Call to End the Tab segment
	void EndTab();

	/* =======================================================================================================================
	Brief:
			Starts a child UI window inside the window you are currently in. Its like a sub window in a window. 
			(By default all Editor UI is the child of a EditorTab window.)
	Usage:
			if (EGUI::StartChild("name of the window", size of the window)
			{
				// whatever UI stuff that is needed in the child. 
				// EGUI::Display::Label("start");
				// EGUI::Display::Label("End);
			}
			EGUI::EndChild();
	======================================================================================================================= */
	// Call to Start Child window in the current window you are in 
	bool StartChild(const std::string&, const Math::Vec2& = Math::Vec2{ 0, 0 });
	// Call to Start Child window in the current window you are in 
	bool StartChild(const std::string&, const float& = 0, const float& = 0);
	// Call to end the child window and proceed back to the parent window
	void EndChild();

	/* =======================================================================================================================
	Brief:
			Makes the next UI item to be printed on the right side of the previous UI item. 
			(Defualt is - below the previous item)	
	Usage:
			EGUI::Display::Label("start");
			EGUI::SameLine();
			EGUI::Display::Label("end");
	======================================================================================================================= */
	void SameLine();

	namespace Display
	{
		/* =======================================================================================================================
		Brief:
				Creates a text display (non-editable). Used like a printf function.
				The label name can contain C-style formats such as %d and put an integer in the elipse variable.
		Usage:
				EGUI::Display::Label("hello world %d %f", 10, 0.63f);
		======================================================================================================================= */
		void Label(const char *_label, ...);
		/* =======================================================================================================================
		Brief:
				NOT WORKING 10/7/18
				Creates a editable text field with a label to the left of the text field. If edited, the _pOutText will be changed
				accordingly. The size limits the number of characters accepted. 
		Usage:
				char buffer[size];
				EGUI::Display::TextField("This is an editable text field: ", &buffer, size);
		======================================================================================================================= */
		void TextField(const std::string& _label, char *_pOutText, size_t _size);
		/* =======================================================================================================================
		Brief:
				Starts a Collaspe-able header that can be clicked to open or close - Which is also the return-ed bool.
		Usage:
				if (EGUI::Display::CollapsingHeader("This function will return true when opened"))
				{
					// UI to do when this header is opened
					// EGUI::Display::Label("Things inside this header");
				}
		======================================================================================================================= */
		bool CollapsingHeader(const std::string& _label);
		/* =======================================================================================================================
		Brief:
				Creates an editable vector field that is pre-arranged for you. Returns true if any of the variable is changed.
				The format is coded to be:
					label of field [editable input] x [editable input] y [editable input] z
		Usage:
				Math::Vec4 position{0,0,0,0};
				if (EGUI::Display::VectorFields("Position", &position, dragging speed, min float, max float, display size of [editable input]))
				{
					// if the variable position is changed in any form. Do someting here:
				}
		======================================================================================================================= */
		bool VectorFields(const std::string& _label, Math::Vector4 *_outputVec, float _dragSpeed = 1.0f, float _min = 0.0f, float _max = 1.0f, float = 50.f);
		/* =======================================================================================================================
		Brief:
				Creats a check box for a boolean variable. Returns true when the check box is clicked, toggles the _pOutBool
		Usage:
				bool toggleMe = false;
				if (EGUI::Display::CheckBox("This is a checkBox", &toggleMe))
				{
					// The checkbox is clicked (toggleMe if its true or false does not matter). Do something here:
				}
		======================================================================================================================= */
		bool CheckBox(const std::string& _label, bool *_pOutBool);
		/* =======================================================================================================================
		Brief:
				Creates a draggable float editable field. Returns if the value is changed
		Usage:
				float MeFloat = 0.f;
				if (EGUI::Display::DragFloat("Drag this float", &MeFloat, drag speed, min float, max float))
				{
					// value of MeFloat has been changed. Do something here:
				}
		======================================================================================================================= */
		bool DragFloat(const std::string& _label, float *_pOutFloat, float _dragSpeed = 1.0f, float _min = 0.0f, float _max = 1.0f);
		/* =======================================================================================================================
		Brief:
				Creates a draggable int editable field. Returns if the value is changed
		Usage:
				int MeInt = 0;
				if (EGUI::Display::DragInt("Drag this integer", &MeInt, drag speed, min int, max int))
				{
					// value of MeInt has been changed. Do something here:
				}
		======================================================================================================================= */
		bool DragInt(const std::string& _label, int *_pOutInt, float _dragSpeed = 1.0f, int _min = 0, int _max = 0);
		/* =======================================================================================================================
		Brief:
				Creates a selectable text field. Returns if the text field is clicked is changed
		Usage:
				bool toggleSelection = false;
				if (EGUI::Display::SelectableTxt("I am a selectable text field", &toggleSelection))
				{
					// The textfield is clicked. Do something here:
				}
				if (EGUI::Display::SelectableTxt("I am a selectable text field"))
				{
					// The textfield is clicked. Do something here:
				}
		======================================================================================================================= */
		// Creates a selectable text field with an output bool to toggle
		bool SelectableTxt(const std::string& _label, bool *_pSelected);
		// Creates a selectable text field with a bool to determine if it is highlighted (selected) or not
		bool SelectableTxt(const std::string& _label, bool = false);
		/* =======================================================================================================================
		Brief:
				Creates a double click selectable text field. Returns true only when the text field is double clicked
		Usage:
				bool toggleSelection = false;
				if (EGUI::Display::SelectableTxtDouble("I am a double clickable text field", &toggleSelection))
				{
					// The textfield is double clicked. Do something here:
				}
				if (EGUI::Display::SelectableTxtDouble("I am a double clickable text field"))
				{
					// The textfield is double clicked. Do something here:
				}
		======================================================================================================================= */
		// Creates a selectable text field with an output bool to toggle
		bool SelectableTxtDouble(const std::string& _label, bool *_pSelected);
		// Creates a selectable text field with a bool to determine if it is highlighted (selected) or not
		bool SelectableTxtDouble(const std::string& _label, bool = false);
		/* =======================================================================================================================
		Brief:
				Start of a Tree Node (like the one you see in ResourceView I believe - 3 July 2018). Returns true if tree is opened.
				This is different from the other Start... functions. You must call the End.. function inside the if()
		Usage:
				if (EGUI::Display::StartTreeNode("I am a Tree Node"))
				{
					// Do something inside this tree
					// Tree inside a Tree
					if (EGUI::Display::StartTreeNode("I am inside the first Tree Node"))
					{
						// Tree inside a Tree inside a Tree
						if (EGUI::Display::StartTreeNode("I am inside the first Tree Node inside the first Tree Node"))
						{
							EGUI::Display::EndTreeNode();
						}
						EGUI::Display::EndTreeNode();
					}
					EGUI::Display::EndTreeNode();
				}
		======================================================================================================================= */
		// Start a tree node 
		bool StartTreeNode(const std::string& _label);
		// Set a specific tree node to be collapsed (closed) or not
		void OpenTreeNode(const std::string& _label, bool _collapseMe);
		//End a tree Node
		void EndTreeNode();

		/* =======================================================================================================================
		Brief:
			Sets the previos UI widget/item to be a payload type. Preferably call according to the usage please. 
			Usage shows that button will have additional of being a payload type
		Usage:
			ImGui::Button("Hello", ImVec2{ 200, 20 } );
			if (EGUI::Display::StartPayload(EGUI::ePAY_LOAD_1, &_file, sizeof(CrawlFile), "Hello"))
			{
				EGUI::Display::EndPayload();
			}
		======================================================================================================================= */
		bool StartPayload(ePayloadTags _tagLoad, void* _pData, size_t _dataSize, const std::string& _toolTip);
		// Call EndPayLoad at the end of StartPayLoad return true. See StartPayLoad usage
		void EndPayload();
		/* =======================================================================================================================
		Brief:
			Sets the previos UI widget/item to be receiving a payload type. Preferably call according to the usage please.
			Usage shows that button will have additional of being a payload reciever type
		Usage:
			ImGui::Button(("$$acceptDemo").c_str(), ImVec2{ 200, 20 });
			using PayloadType = Dystopia::ResourceView::CrawlFile;
			if (PayloadType *t = EGUI::Display::StartPayloadReceiver<PayloadType>(EGUI::ePAY_LOAD_1))
			{
				mDemoName = (*t).mFileName;
				EGUI::Display::EndPayloadReceiver();
			}
		======================================================================================================================= */
		template <typename Specified>
		Specified* StartPayloadReceiver(ePayloadTags _tagLoad)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(EGUI::ToString(_tagLoad)))
				{
					DEBUG_ASSERT(payload->DataSize == sizeof(Specified), "Error at EGUI");
					return static_cast<Specified*>(payload->Data);
				}
			}
			return nullptr;
		}
		// Call EndPayloadReceiver at the end of StartPayloadReceiver return true. See StartPayloadReceiver usage
		void EndPayloadReceiver();
		/* =======================================================================================================================
		Brief:
			
		Usage:
			
		======================================================================================================================= */
		bool DropDownSelection(const std::string& _label, unsigned int _currentIndex, const std::string *_arrayOfStrings, size_t _size);


	}
}

/*========================================================================================================================================================*/
/*====================================================================== GUI SYSTEM ======================================================================*/
/*========================================================================================================================================================*/

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

		bool			Init(WindowManager*, GraphicsSystem*, InputManager*, const char* = "MainDockable");
		void			StartFrame(const float&);
		void			EndFrame();
		void			Shutdown();

		void			UpdateCharInputs(unsigned int c);
		void			UpdateMouseInputs();
		void			UpdateKeyInputs();
		void			UpdateScrollInputs();
		const char*		GetMainDockspaceName() const;

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
		void			StartFullDockableSpace();
		void			EndFullDockableSpace();
		void			DefaultColorSettings();
	};
}

#endif // !_EDITOR_GUI_H_
#endif // EDITOR ONLY













