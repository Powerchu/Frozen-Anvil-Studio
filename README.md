### How to use:

## [EDITOR]

### Input
#### Hot keys (only available while not in play mode)
    
    - New                 Control + N
    - Open                Control + O
    - Save                Control + S
    - Save as             Control + Shift + S
    - Quit                Control + Alt + Q
    - Copy                Control + C
    - Paste               Control + V
    - Duplicate           Control + D
    - Undo                Control + Z (most edits can undo)
    - Redo                Control + Y (most edits can redo)
    - Play                Control + P (while not in play mode)
    - Stop                Control + P (while in play mode)
    - Clear history       Control + 1 

    Right click will have drop down menu when appropriate. Such as right 
    clicking a component or a game object in Hierarchy view.

### Getting Started

    Launching the editor will bring up a Project launcher that will allow the 
    users to create and edit multiple different projects. Click on any recent 
    project displayed or [browse] for an existing project to open with (.dyst) 
    
    OR 
    
    Click on [Get Started] to create a project of name and at the desired 
    location. The launcher will open up into the editor for that project.

#### Creating your first Game object

    Open up the Hierarchy View from the View option in the main menu bar. 
    Click on [Create]. 

#### Adding your first Component

    Add a image file (.png, .dds) into the project asset folder 
    (../Project_Name/Resource/Asset). Open up the Inspector from the 
    View option in the main menu bar. Click on the newly Created Game object 
    in the Hierarchy View and it’s properties will appear in the Inspector. 
    Click on [Add Component] and select Sprite Renderer. Drag an Image file 
    (.png, .dds) from the Project Resource into the Texture field. Click on 
    [Auto Resize] button under the preview in the Sprite Renderer component. 

#### Parenting your first Game object

    Create a new Game object and drag it from the Hierarchy view onto 
    another object on the Hierarchy view.

#### Creating your first Animation

    Open up the Sprite Editor from the View option in the main menu bar. 
    Drag in a sprite sheet image into the Sprite Sheet field. Click on 
    [Add new section] will generate a new section of id (01) and enter the 
    desired fields (eg. Start pos (0,0) size(1,1) col & row (2,1)). Click on 
    the game object with sprite renderer component. Drag in the same sprite 
    sheet into the Texture field. Click on [Add Anim] and select the created 
    section (01) drag the Columns and Rows field to the maximum and click 
    [Simulate].

#### Creating your first Behaviour 

    Click on [Add Behaviour] in the Inspector and enter the required 
    fields appropriately. Go to your project folder in windows and open 
    up BehavioursScripts folder to locate your behaviour files 
    (do not change their locations). Open up in a text editor of your choice 
    (visual code, notepad ++). 

    Open the Header (.h) file and add a public member variable float count.

    Comment out PP_REFLECT_EMPTY(Dystopia::YourBehaviour)

    Uncomment //PP_REFLECT(Dystopia::YourBehaviour)

    Change it to PP_REFLECT(Dystopia::YourBehaviour, count)

    Go to the CPP file and at the top of the file where the #includes are at, 
    type in #include “Component/Transform.h”. 

    Go to the void ScriptName::Init() function and type in:
    count = 0;

    Go to the void ScriptName::Update(float _fDeltaTime) function and type in:
    GetOwner()->GetComponent<Transform>()->SetPosition(Math::Pt3D{count++, 0, 1});
    
    Go back to the editor and wait. The editor will freeze for a moment to 
    compile the behaviour script, this is normal. Once done, the console will 
    output the result. Fix any errors if you have. Go back to the game object 
    you made, and click on [Add Behaviour] and select YourBehaviour.
    
    Use the hokey Play (Ctrl + P) to watch the results.

### Editor Stuff

#### [Inspector]
    Users can add, remove and edit components in an existing game object. 
    Generic game object properties will always be there and component 
    properties will only appear when available. For generic object properties, 
    Layer [World] for objects appearing in the game world, and [UI] for the UI 
    camera to render as a canvas. Transform component Z axis changes the objects 
    rendering order. Behaviours are cpp files that are added and compiled 
    during editor runtime. Click on the add behaviour button and fill in the 
    fields appropriately and the editor will generate a .cpp and .h file. 
    Users can code in cpp as usual and attach them to game objects as a 
    scripting means. Experiment with other various component properties.

#### [Console Log]
    DLL Compilation outputs as well as game debug prints outputs here. 

    [Hierarchy View]
    Displays all existing game objects in the current scene. Create a new 
    game object by clicking the create button. Click on a single game object 
    to view its properties in the Inspector. Ctrl + Left mouse click will allow 
    selective multiple picking. Currently selected game objects will be 
    highlighted and the scene view will visually display gizmos on them. 

    Search bar not working yet.

#### [Project Resource]
    
    Displays all resources that the current Project uses. Drag and drop an 
    existing game object form the Hierarchy View into the project resource 
    (Prefab folder) will save it as a prefab.
    Files that can be dragged and dropped to their specialized fields :
    
    Audio (.mp3, .wav) into audio source component,
    Images (.png, .dds, .bmp) into the sprite editor/spriterenderer/sprite previewer
    Prefabs (.dobj) iand Scenes (.dscene) into the scene view.
    
    Search bar not working yet.

#### [ColorScheme]
    Change color of editor ( visuals )
    
#### [StyleScheme]
    Change editor styles ( visuals ) 
    
#### [Performance Log]
    Views the different performances of various systems of the engine
    
#### [SceneView]
    Allow editing the current game scene mainly through the use of mouse. 
    While editing, pressing keyboard [F] will focus onto the selected 
    game object, [W] will use the translate tool (arrow heads on the gizmo), 
    [E] will use the scale tool (box heads on the gizmo). Gizmo can be dragged 
    through the X, Y-axis or the blue box for both axis. If there are multiple 
    selected objects, gizmo will appear at the average position.

#### [Sprite Previewer]
    Serves as a means to preview an existing sprite.
    
#### [Sprite Editor] 
    Serves mainly for editing sprite sheet data to be used in animations. 
    Drag a image file into the sprite field to get started. Fill in the 
    starting position, size, cols and row to create a atlas section. A 
    red grid will visually appear to reflect your data fields.
    
#### [ProjectSettings] 
    Displays the engine system’s settings for the current project. Such as 
    Graphics system, Physics system etc. These systems will affect the game 
    throughout.
    
#### [BehaviourTree Editor]
    Not implemented yet.
    However, a behaviour tree viewer is avaliable once you add an AI Component in the Inspector. More on this below under their respective categories.
    
#### [DataSheet]
    This is a serialized data format which allows read/write. Under the hood, it utilises a map data structure to store key-value types. How the team used Datasheets are in Character's default value, skills default values and interactables default attributes. In order to use them, callbacks are avaliable. However, if you want to understand how these work, please approach and contact Shannon.
    
#### [GameView]
    View port as seen from the game view camera. The game camera relies on having one Main Camera in the scene. So do make sure there is one. Game Cameras can be either Orthographic or Perspective camerea. 
    
#### [SceneView]
    This is the editor view port and is strictly Orthographic. Details of the camera position and zoom is avaliable on the top left corner. Hold right click to pan the camera, and use mousewheel to zoom in and out.
