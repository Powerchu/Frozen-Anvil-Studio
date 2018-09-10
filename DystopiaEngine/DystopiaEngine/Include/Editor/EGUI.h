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
#include "DataStructure\AutoArray.h"
#include "DataStructure\Array.h"
#include "Utility\DebugAssert.h"
#include "Editor\Dock.h"
#include "Editor\Payloads.h"
#include "../../Dependancies/ImGui/imgui.h"
#include "../../Dependancies/ImGui/imgui_internal.h"
#include <string>

/* forward declare just becuz */
struct ImDrawData;
struct ImGuiContext;

namespace Dystopia
{
	class CommandHandler;
	class WindowManager;
	class GraphicsSystem;
	class EditorInput;
	class GLState;
	typedef unsigned int GLuint;
}
/* ========================== */

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
	void ChangeLabelSpacing(float _amount);
	void ChangeLabelSpacing();
	void ChangeAlignmentYOffset(float _amount);
	void ChangeAlignmentYOffset();
	float GetAlignmentYOffset();
	float GetLabelSpacing();

	bool StartMainMenuBar();
	bool StartMenuHeader(const std::string&);
	bool StartMenuBody(const std::string&, const std::string& _shortcut="");
	void EndMainMenuBar();
	void EndMenuHeader();
	void Indent(float _spacing = 20.f);
	void UnIndent(float _spacing = 20.f);
	void PushID(int);
	void PopID();

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
	bool StartChild(const std::string&, const Math::Vec2& = { 0, 0 }, bool _showBorder = true,
					const Math::Vec4& _colour = { 0.1f, 0.1f, 0.1f, 0.1f });
	// Call to Start Child window in the current window you are in 
	bool StartChild(const std::string&, const float& = 0, const float& = 0, bool _showBorder = true, 
					const Math::Vec4& _colour = { 0.1f, 0.1f, 0.1f, 0.1f });
	// Call to end the child window and proceed back to the parent window
	void EndChild();

	/* =======================================================================================================================
	Brief:
			Makes the next UI item to be printed on the right side of the previous UI item. If custom offset > 0, starts 
			counting from left alignment of the imgui window its being called in
			(Defualt is - below the previous item)	
	Usage:
			EGUI::Display::Label("start");
			EGUI::SameLine();
			EGUI::Display::Label("end");
	======================================================================================================================= */
	void SameLine(float _customOffset = 5.f);

	namespace Display
	{
		/* =======================================================================================================================
		Brief:
				Draws a horizontal line in the current imgui window
		======================================================================================================================= */
		void HorizontalSeparator();
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
				Creates a editable text field with a label to the left of the text field. If edited, the _pOutText will be changed
				accordingly. The size limits the number of characters accepted. 
		Usage:
				char buffer[size];
				EGUI::Display::TextField("This is an editable text field: ", &buffer, size);
		======================================================================================================================= */
		bool TextField(const std::string& _label, char *_pOutText, size_t _size, bool _showLabel = true, float _width = 250);
		/* =======================================================================================================================
		Brief:
				Creates an empty box. Great for using alongside payloads if you unsure. returns if the box is clicked. 
				If the last bool param is false, hovering wont show and clicking wont return true
		Usage:
				EGUI::Display::EmptyBox("Accepting field", 100);
		======================================================================================================================= */
		bool EmptyBox(const std::string& _label, float _width, const std::string& _anythingToShowInside="", 
					  bool _iteractive = false, bool _showLabel = true);
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
		bool VectorFields(const std::string& _label, Math::Vector4 *_outputVec, float _dragSpeed = 1.0f, float _min = 0.0f, 
						  float _max = 1.0f, float = 50.f);
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
		bool CheckBox(const std::string& _label, bool *_pOutBool, bool _showLabel = true);
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
		bool SelectableTxt(const std::string& _label, bool _highlight = false);
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
		bool StartTreeNode(const std::string& _label, bool* _outClicked = nullptr, bool _highlighted = false, bool _noArrow = false);
		// Set a specific tree node to be collapsed (closed) or not
		void OpenTreeNode(const std::string& _label, bool _open);
		// opens the next tree node
		void OpenTreeNode(bool _open = true);
		//End a tree Node
		void EndTreeNode();
		/* =======================================================================================================================
		Brief:
				All in one payload creator. Created this function for intended use of resource viewer the custom
				drag and drop icons with its label below an icon. Might extend this some other day 23/7/18
		Usage:
				Very specific as of creation date.
		======================================================================================================================= */
		bool CustomPayload(const std::string& _uniqueId, const std::string& _label, const std::string& _tooltip,
						   const Math::Vec2& _displaytSize, ePayloadTags _tagLoad, void* _pData, size_t _dataSize);
		/* =======================================================================================================================
		Brief:
				Sets the previos UI widget/item to be a payload type. Preferably call according to the usage please. 
				Usage shows that button will have additional of being a payload type.
		Usage:
				EGUI::Button("Hello", Math::Vec2{ 200, 20 } );
				if (EGUI::Display::StartPayload(EGUI::FILE, &(*_file), sizeof(File), "Hello"))
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
				EGUI::Display::EmptyBox("Box to accept payload", 150, mDemoName);
				if (File *t = EGUI::Display::StartPayloadReceiver<File>(EGUI::FILE))
				{
					mDemoName = (*t).mName;
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
					DEBUG_ASSERT(payload->DataSize != sizeof(Specified), "Error at EGUI");
					return static_cast<Specified*>(payload->Data);
				}
			}
			return nullptr;
		}
		// Call EndPayloadReceiver at the end of StartPayloadReceiver return true. See StartPayloadReceiver usage
		void EndPayloadReceiver();
		/* =======================================================================================================================
		Brief:
				Opens a little popup window that is passed into the param. BASICALLY - the param is the same as the _uniqueID 
				of the StartPopup function you want to call.
				Note : For the second boolean param of OpenPopup is set to false, the popup will be defaulted to the
				bottom left of the last ui item called. In the usage case, it would be the bottom left of the Button("Add Co..")
				Leave it to true if can, or use the DropDownSelection function instead
		Usage:
				if (EGUI::Display::Button("Add Component", btnSize))
				{
					EGUI::Display::OpenPopup("Inspector Component List", false);
				}
				if (EGUI::Display::StartPopup("Inspector Component List"))
				{
					EGUI::Display::Dummy(235, 5);
					EGUI::Display::EndPopup();
				}
		======================================================================================================================= */
		void OpenPopup(const std::string& _thePopupID, bool _toOpenAtMousePos = true);
		// Creates a popup window that is tied to the _uniqueID. Call OpenPopup with its ID to open it
		bool StartPopup(const std::string& _uniqueID);
		// call this at the end of the StartPopup function when it is true. Determines the end of the popup
		void EndPopup();
		/* =======================================================================================================================
		Brief:
				Creates a drop down box with selectable strings. The currently selected index will be stored into the second
				parameter variable.
		Usage:
				static int i = 0;
				AutoArray<std::string> arr;
				arr.push_back("");
				arr.push_back("item1");
				arr.push_back("item2");
				EGUI::Display::DropDownSelection("TestDropDown", i, arr);
		======================================================================================================================= */
		bool DropDownSelection(const std::string& _label, int& _currentIndex, AutoArray<std::string>& _arrOfItems, float _width = 100);
		/* =======================================================================================================================
		Brief:
				Creates a button
		Usage:
				EGUI::Display::Button("Demo", ImVec2{ 200, 20 });
		======================================================================================================================= */
		bool Button(const std::string& _label, const Math::Vec2& _size = Math::Vec2{ 50,18 });
		/* =======================================================================================================================
		Brief:
				Creates a dummy area. Used only to offset ImGui stuff. Does absolutely nothing except for manipulating UI
				arrangements
		Usage:
				EGUI::Display::Dummy(10,10);
		======================================================================================================================= */
		void Dummy(float width = 0.f, float height = 0.f);
		/* =======================================================================================================================
		Brief:
				Creates a small tick icon
				For best display, use the same ratio of w and h
		Usage:
				EGUI::Display::IconTick(10,10);
		======================================================================================================================= */
		void IconTick(float width = 5.f, float height = 5.f, float _thickness = 1.f, 
					  const Math::Vec4& _colour = Math::Vec4{ 1.f, 1.f, 0.4f, 1.f });
		/* =======================================================================================================================
		Brief:
				Creates a small folder icon. DO NOT have a color with less than 0.3f for any of the RGBA.
				Returns true when clicked
				For best display, use a 10 : 6 ratio for width and height respectively.
		Usage:
				EGUI::Display::IconFolder("something");
		======================================================================================================================= */
		bool IconFolder(const std::string& _uniqueId, float width = 15.f, float height = 9.f, bool _open = false,
						const Math::Vec4& _colour = Math::Vec4{ 1.f, 1.f, 0.4f, 1.f });
		/* =======================================================================================================================
		Brief:
				Creates a small circle icon thingy. Like the one you see in unity field boxes.
				Returns true when clicked
				For best display, use small radiuses < 10
		Usage:
				EGUI::Display::IconCircle("something");
		======================================================================================================================= */
		bool IconCircle(const std::string& _uniqueID, float radius = 10.f, float offsetX = 0.f,
					    float offsetY = 0.f, const Math::Vec4& _colour = Math::Vec4{ 0.7f, 0.7f, 0.7f, 0.8f });
		/* =======================================================================================================================
		Brief:
				Creates a small game object icon. Returns true when clicked
		Usage:
				EGUI::Display::IconGameObj("something");
		======================================================================================================================= */
		bool IconGameObj(const std::string& _uniqueId, float _width = 10.f, float _height = 10.f);
		/* =======================================================================================================================
		Brief:
				Creates a small File icon. Returns true when clicked
		Usage:
				EGUI::Display::IconFile("something");
		======================================================================================================================= */
		bool IconFile(const std::string& _uniqueId, float _width = 10.f, float _height = 10.f,
					  const Math::Vec4& _colour = Math::Vec4{ 0.2f, 0.8f, 0.7f, 0.8f });
		/* =======================================================================================================================
		Brief:
				Creates a box outline given the width and height
		Usage:
				EGUI::Display::Outline(your ui width, height);
		======================================================================================================================= */
		void Outline(float _x, float _y);
		/* =======================================================================================================================
		Brief:
				Creates a labeled Graph
		Usage:
				static float arr[10] = {1,2,3,4,5,6,7,8,9,10};
				EGUI::Display::LineGrap("MyGraph", arr, 10, 0, 11, Math::Vec2{100,20}, "hovering");
		======================================================================================================================= */
		template<size_t N>
		void LineGraph(const std::string& _uniqueLabel, const float(&_array)[N], float _min = 0.f, 
					   float _max = 1.f, const Math::Vec2& _size = Math::Vec2{100, 20}, const std::string& _overlapText = "")
		{
			EGUI::Display::Label(_uniqueLabel.c_str());
			std::string intercerptName = "##" + _uniqueLabel;
			ImGui::PlotLines(intercerptName.c_str(), _array, N, 0, _overlapText.c_str(), _min, _max, ImVec2{ _size.x, _size.y });
		}
		template<size_t N>
		void LineGraph(const std::string& _uniqueLabel, const Array<float, N>& _array, float _min = 0.f,
			float _max = 1.f, const Math::Vec2& _size = Math::Vec2{ 100, 20 }, const std::string& _overlapText = "")
		{
			EGUI::Display::Label(_uniqueLabel.c_str());
			std::string intercerptName = "##" + _uniqueLabel;

			ImGui::PlotLines(intercerptName.c_str(), _array.begin(), _array.size(), 0, _overlapText.c_str(), _min, _max, ImVec2{ _size.x, _size.y });
		}
	}
}

/*========================================================================================================================================================*/
/*====================================================================== GUI SYSTEM ======================================================================*/
/*========================================================================================================================================================*/

namespace Dystopia
{
	class GuiSystem
	{
	public:
		GuiSystem();
		~GuiSystem();

		bool			Init(WindowManager*, GraphicsSystem*, EditorInput*, const char* = "MainDockable");
		void			StartFrame(const float&);
		void			EndFrame();
		void			Shutdown();

		void			UpdateChar(unsigned short);
		void			UpdateScroll(double _x, double _y);
		void			UpdateMouse(int, bool);
		void			UpdateKey(int _k, bool _down);

		const char*		GetMainDockspaceName() const;

	private:
		ImGuiContext	*mpCtx;
		WindowManager	*mpWin;
		GraphicsSystem	*mpGfx;
		EditorInput	*mpInput;
		ImDrawData		*mpDrawData;
		GLState			*mpGLState;
		GLuint			mFontTexture;
		std::string		mGlslVersion;
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













