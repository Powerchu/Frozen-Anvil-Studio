
# ![logo](https://i.ibb.co/qBW9s47/Untitled-2.png)
    Developed Game - Wilo & The Fabled Fist, a 2D Hack-n-Slash Action Adventure Platformer.

Dystopia Engine is a 3D game engine, running OpenGL 4.6 with powerful features which includes:
> - Hot Reloading (Runtime Script Compilation for Behaviours)
> - Versatile Behaviour Tree
> - Event Messaging
> - Script-To-Script Messaging
> - Robust Particle System Editor with multiple emittors capability and batch-rendered billboarded sprites 
> - Video Player (running on WebM library)
> - Runtime Shader Generation and Edit/Compile
> - Collision Layers (Like Unity's)
> - Support for Convex Shapes Collision
> - Optimised Discrete Physics System with:
>   - Sleeping
>   - Linear Damping
>   - Static and Dynamic Friction
>   - Linear Resolution 
>   - Restitution
> - Amazing Editor User Interaction
> - Prefab/Archetype Support
> - Undo/Redo for many different thing including the creation of prefab instance, deletion of gameobject and much more
> - Multiple Projects support 

---
**DISCLAMER: As of April 2019, please use Dystopia Engine with the included "Wilo_Beta_project.dyst" as you need the required shaders to render onto Game Camera View.**

---

#  A Quick Guide To Using the Editor

## Table of Contents
- [Getting Started](##getting-started)
- [Input](##input)
- [Making A Simple "Game"](##making-a-simple-"game")
- [Editor Panel Details](##editor-panel-details)
- [Components](##component-details)
- [ProjectSettings Details](##project-settings-details)
- [Behaviour Tree Details](##using-the-behaviour-tree)

## Getting Started
### **Launching The Editor**
<img src="https://i.ibb.co/8KHwQZZ/openingfromold.png" width="600"/>
    
    Launching the editor will bring up a Project launcher that will allow the users to create and edit multiple different projects. 
    Click on any recent project displayed or [browse] for an existing project to open with (.dyst) 

**OR**

<img src="https://i.ibb.co/zS48LTz/Capture.png" width="600"/>

    Click on [Get Started] to create a project of name and at the desired location. The launcher will open up into the editor for that project.

<img src="https://i.ibb.co/T86m31y/saveas.png" width="600"/>

    A dialog prompting the user to indicate Project Save Location will be shown. Once a folder is selected, click on [Create].

### **The Editor View**

<img src="https://i.ibb.co/BzMHbDt/windowed-Bug.png" width="600"/>

    The above is the windowed view of the editor, which as you can tell, is bugged. So please use the rest of the editor in FullScreen mode by maximizing the window.

<img src="https://i.ibb.co/rfKBMYR/optimal-Layout.png" width="600"/>

    You may find that the panels are all over the place. This will happen if this is the first time you are launching Dystopia Engine. Subsequent Launches will use the last-used layout. To move around the panel, just click and drag the header of the panel (the red one). You should see some squares prompting you to dock in them.

    The above image is Aaron's optimal layout as it gives you access to important details.

<img src="https://i.ibb.co/1TKtFBC/Desktop-Screenshot-2019-04-21-13-17-46-66.png" width="600"/> 

> *Above: Dragging the panel. Below: Docking the panel.*

<img src="https://i.ibb.co/RvGhH0n/Desktop-Screenshot-2019-04-21-13-17-50-58.png" width="600"/>


### **Editor Panels**
<img src="https://i.ibb.co/4mLKFZ5/Desktop-Screenshot-2019-04-21-13-18-01-60.png" width="600"/>

There are many different panels that serves as different interfaces for Dystopia Engine's many functions.

#### Table Of Panels:

| Panel               | Brief Function         
| -------------         |----------------: 
Behaviour Tree                               | **DEPRECATED: Do not use as it crashes the editor**
[Color Scheme    ](####color-scheme)        | Personalize your editor with your own colors
[Console         ](####console)        | Compile Error/Warnings, Debug_Print messages and other misc. engine info is displayed here
[DataSheets      ](####data-sheet)        | A front-end serializer/deserializer for saving game data
FlowChart      | **DEPRECATED: It used to show State Machine's Flow Chart.**
[Game View       ](####game-view)        | Game viewport rendered from a Main Camera/Game Camera.
[Hierarchy       ](####hierarchy)        | The scene graph of gameobjects in the scene. 
[Particle System ](####particle-system)        | An editor for Particle Systems (Like Unity's)
[Performance     ](####performance-log)        | Diagnostic Checking & Performance Logging
[Project         ](####project-resource)        | The Resource folder, along with draggable assets are here.
[Project Settings](####project-settings)        | Settings for Engine/Systems
[Scene View      ](####scene-view)        | A viewport rendered from a hidden Scene Camera.
[Shader View     ](####shader-view)        | An editor/previewer for Shaders
[Sprite Editor   ](####sprite-editor)        | An editor for Sprite Animation (using SpriteSheets)
[Previewer       ](####sprite-previewer)        | A utility panel usually paired with [Project] Panel - previews images
[Style Scheme    ](####style-scheme)        | Personalize your editor with your own editor preferences.
[Inspector       ](####inspector)        | Inspect the GameObject's components/behaviours.
   

## Input

### **Hot keys (only available while not in play mode)**

| Function            | Hotkey <img width=100/>         | Remarks |
| -------------       |:-------------:    | ----------- |
| New                 | Ctrl + N          | 
| Open                | Ctrl + O          | You can also drag a .dscene from Project Resource onto Scene View Panel
| Save                | Ctrl + S          |
| Save As             | Ctrl + Shift + S  |
| Quit                | Ctrl + Alt + Q    | 
| Copy                | Ctrl + C          |
| Paste               | Ctrl + V          |
| Duplicate           | Ctrl + D          | 
| Undo                | Ctrl + Z          | (Most edits can undo). The following cannot: Collider's offset, scale. Behaviours' reflected variables.
| Redo                | Ctrl + Y          | (Most edits can redo). The following cannot: Collider's offset, scale. Behaviours' reflected variables.
| Play                | Ctrl + P          | 
| Stop                | Ctrl + P          | (while in play mode)
| Clear History       | Ctrl + 1          | (please remember to do this as if the undo history goes above 200, performance issues arises)   
| Focus Object        | F                 | (shift scene camera to focus on currently selected gameObject)
| Reset Scene Camera  | Home              | (reset scene camera's transform)
    
Tip
    Right click will have drop down menu when appropriate. Such as right 
    clicking a component or a game object in Hierarchy view.

### **Scene Manipulation**
- W - Translation
- E - Scaling
- R - Rotation (Not implemented - use inspector panel)
- Hold Right-Click to Pan scene camera.
- Mouse Wheel zooms in and out

## Making A Simple "Game"

#### Creating your first Game object

    Open up the Hierarchy View from the View option in the main menu bar. 
    Click on [Create]. 

#### Adding your first Component

    Add a image file (.png, .dds) into the project asset folder 
    (../Project_Name/Resource/Asset). Open up the Inspector from the 
    View option in the main menu bar. Click on the newly Created Game object 
    in the Hierarchy View and itâ€™s properties will appear in the Inspector. 
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
    type in #include â€œComponent/Transform.hâ€. 

    Go to the void ScriptName::Init() function and type in:
    count = 0;

    Go to the void ScriptName::Update(float _fDeltaTime) function and type in:
    GetOwner()->GetComponent<Transform>()->SetPosition(Math::Pt3D{count++, 0, 1});
    
    Go back to the editor and wait. The editor will freeze for a moment to 
    compile the behaviour script, this is normal. Once done, the console will 
    output the result. Fix any errors if you have. Go back to the game object 
    you made, and click on [Add Behaviour] and select YourBehaviour.
    
    Use the hokey Play (Ctrl + P) to watch the results.

## Editor Panel Details

#### Inspector

<img src="https://i.ibb.co/ZhS9wLq/Desktop-Screenshot-2019-04-21-13-20-49-60-2.png" width="300"/>


    Users can add, remove and edit components in an existing game object. 
    Generic game object properties will always be there and component 
    properties will only appear when available. For generic object properties, 
    Layer [World] for objects appearing in the game world, and [UI] for the UI 
    camera to render as a canvas. Transform component Z axis changes the objects 
    rendering order. Behaviours are cpp files that are added and compiled 
    during editor runtime. Click on the add behaviour button and fill in the 
    fields appropriately and the editor will generate a .cpp and .h file. 
    Users can code in cpp as usual and attach them to game objects as a 
    scripting means. 

    Each component can be set inactive or active by toggling the checkbox beside the Component Name.
    
    Remove a component by right clicking the Header.
    
    
Learn other various component properties by clicking [here](#component-details)

#### Console

<img src="https://i.ibb.co/qFSDN0X/Desktop-Screenshot-2019-04-21-13-20-49-60-4.png" width="600"/>

    DLL Compilation outputs as well as game debug prints outputs here. 
    Click Clear button to clear the console, and check the [Cleared On Play] to auto-clear upon entering play mode.
    
    

#### Project Resource
    
<img src="https://i.ibb.co/8Bh9ftN/Desktop-Screenshot-2019-04-21-13-20-49-60-3.png" width="600"/>


    Displays all resources that the current Project uses. Drag and drop an 
    existing game object form the Hierarchy View into the project resource 
    (Prefab folder) will save it as a prefab.
    Files that can be dragged and dropped to their specialized fields :
    
    Audio (.mp3, .wav) into audio source component,
    Images (.png, .dds, .bmp) into the sprite editor/spriterenderer/sprite previewer
    Prefabs (.dobj) iand Scenes (.dscene) into the scene view.
    
    Search bar not working yet.

#### Color Scheme
    Change color of editor ( visuals )
    
#### Style Scheme
    Change editor styles ( visuals ) 
    
#### Performance Log

<img src="https://i.ibb.co/HgTqmfY/Desktop-Screenshot-2019-04-21-13-20-49-60-5.png" width="600"/>


    Views the different performances of various systems of the engine
    
#### Sprite Previewer

<img src="https://i.ibb.co/H2y4M23/Desktop-Screenshot-2019-04-21-13-28-18-71-2.png" width="300"/>

    Serves as a means to preview an existing sprite.
    
#### Sprite Editor

<img src="https://i.ibb.co/ZVRPq3g/Desktop-Screenshot-2019-04-21-13-27-42-16.png" width="600"/>

<img src="https://i.ibb.co/4V91by2/Desktop-Screenshot-2019-04-21-13-27-51-01.png" width="600"/>

    Serves mainly for editing sprite sheet data to be used in animations. 
    Drag a image file into the sprite field to get started. Fill in the 
    starting position, size, cols and row to create a atlas section. A 
    red grid will visually appear to reflect your data fields.

    Usually, a square sprite sheet with uniform dimension is optimal, since no adjustments is necessary
    and the user just needs to adjust the Column & Rows.
    
#### Project Settings

<img src="https://i.ibb.co/0DpBWz1/Desktop-Screenshot-2019-04-21-13-22-59-57-2.png" width="600"/>


    Displays the engine systemâ€™s settings for the current project. Such as 
    Graphics system, Physics system etc. These systems will affect the game 
    throughout.
    
#### Behaviour Tree Editor
    Not implemented yet.
    However, a behaviour tree viewer is avaliable once you add an AI Component 
    in the Inspector. More on this below under their respective categories.
    
#### DataSheet

<img src="https://i.ibb.co/W0SJWkp/Desktop-Screenshot-2019-04-21-13-33-34-67-2.png" width="600"/>

    This is a serialized data format which allows read/write. Under the hood, 
    it utilises a map data structure to store key-value types. How the team used 
    Datasheets are in Character's default value, skills default values and 
    interactables default attributes. In order to use them, callbacks are avaliable. 
    However, if you want to understand how these work, please approach and 
    contact Shannon.
    
#### Game View
    View port as seen from the game view camera. The game camera relies on having 
    one Main Camera in the scene. So do make sure there is one. Game Cameras can 
    be either Orthographic or Perspective camerea. 
    
#### Scene View
    This is the editor view port and is strictly Orthographic. Details of the camera 
    position and zoom is avaliable on the top left corner. Hold right click to pan the 
    camera, and use mousewheel to zoom in and out. 

    Allow editing the current game scene mainly through the use of mouse. 
    While editing, pressing keyboard [F] will focus onto the selected 
    game object, [W] will use the translate tool (arrow heads on the gizmo), 
    [E] will use the scale tool (box heads on the gizmo). Gizmo can be dragged 
    through the X, Y-axis or the blue box for both axis. If there are multiple 
    selected objects, gizmo will appear at the average position.

#### Hierarchy

<img src="https://i.ibb.co/B64bWvR/Desktop-Screenshot-2019-04-21-13-20-49-60-6.png" width="500"/>

    *Note: GameObject count is 2 due to hidden Scene Camera.
    

    Displays all existing game objects in the current scene. Create a new 
    game object by clicking the create button. Click on a single game object 
    to view its properties in the Inspector. Ctrl + Left mouse click will allow 
    selective multiple picking. Currently selected game objects will be 
    highlighted and the scene view will visually display gizmos on them. 

    Search bar does not work.

    This panel contains a list (not scene graph) of gameobjects in the scene.
    In here, you can right click on a gameobject to duplicate or delete, but that can be done
    through Hotkeys anyway.

    Creating An GameObject
        - Just click on create > gameobject

    Parenting/Unparenting
        - To parent on object, click and drag the selected object and hover over the object
        you wish to parent to
        - Let go and it should parent
        - To unparent an object, click and drag down to the empty hover area.

    Select/Multi Selection
        - Unfortunately, there is no drag-click-hold function. You have to hold ctrl+left click to multi select objects.

#### Shader View

<img src="https://i.ibb.co/Y3VZ8X9/Desktop-Screenshot-2019-04-21-13-24-43-66.png" width="600"/>
    

    This is where users edit and create shaders in runtime. For more information, please approach
    Shannon or Jacky.


#### Particle System

<img src="https://i.ibb.co/RYD8T8W/Desktop-Screenshot-2019-04-21-13-25-20-57.png" width="600"/>

    This is where users edit and create particles along with extensive customization capabilities.
    For more details on how to use the particle system, please approach Shannon.


[^ Return To Table Of Panels](####table-of-panels)


## Component Details
### Transform
<img src="https://i.ibb.co/NTT4sC7/Desktop-Screenshot-2019-04-21-13-21-22-70-2.png" width="400"/>

> This is the only component that every gameobject is required to have. Like in Unity, the values displayed are in Local Coordinate System. 
> 
> Hold Left Shift + Mouse Drag to drag faster or Hold Left Alt + Mouse Drag to adjust slower.

### Camera
<img src="https://i.ibb.co/NNyDCY0/Desktop-Screenshot-2019-04-21-13-20-49-60-7.png" width="400"/>

> The camera component renders onto a viewpoint (in this engine, specifically the game view). There can be multiple cameras but as of right now, only 1 main World Camera and 1 main UI camera can co-exists. Although multiple view rendering from multiple camera is techinically avaliable, the editor does **not** support this.

**Properties:**

    Master Camera   : Serves no purpose as of now.
    Debug Draw      : Whether to draw debug stuff onto this camera's render
    Surface         : Which Surface buffer to render to
    Projection      : Either Orthographic or Perspective
    Size            : Orthographic Zoom
    FOV             : Perspective "Zoom" (use Z-axis to zoom as well for perspective cameras)
    Clipping Planes : Only objects with a Z-axis of within [Near]-[Far] will be rendered, the rest will be clipped.
    Viewport Rect   : The adjustment of the viewport dimension.

### RigidBody
<img src="https://i.ibb.co/gFm985k/Desktop-Screenshot-2019-04-21-13-30-53-97-2.png" width="400"/>

> Having a rigidbody means the physics system will stimulate the gameobject with a set of physically accurate rules. Physics System settings and their details can be found [here](####physics-settings).

**Properties:**

    Body Type       : Static - This object will not be stimulated but will let other objects react to this.
                      Dynamic - This object will be stimulated fully.
                      Kinematic - This object will be stimulated, but this object will not react to other objects.
    Mass            : How heavy (or how large the inertia) is the gameobject. A gameobject with a larger mass will provide
                      more force/impulse to an object of a smaller mass.
    Linear Drag     : Without friction, this is the only way rigidbodies slow to a stop. A higher value means faster velocity decay.
    Angular Drag    : Affects angular velocity decay.
    Gravity Scale   : How much the system's global gravity force is applied on the rigidbody. 1.0 is 100%.
    Friction        : Static - An arbitrary coefficient that dictates how much force (determined by inverse mass) is necessary to move this object.
                      Dynamic - An arbitray coefficient that dictate how much opposing force is applied
    Bounciness      : Formally known as "Restitution", affects how much a rigidbody bounce (elasticity/impulse retainment).
    Collision Mode  : As of right now, options are arbitrary. Collision Detection and Resolution is a hybrid of discrete and continous.
    Sleeping Mode   : Start Awake - Default value, starts awake and sleep once weighted average velocity is below threshold.
                      Start Asleep - Starts asleep, then awake once collided into.
                      Never Sleep - Do not sleep this object.
                      A sleeping objects is optimized because the physics system skips checks.

**Example of Code Use:**

```cpp 
/*Code excerpt from BatAi.cpp*/
...
    // Launch the bat to prebite (and change direction)
    if (const auto rigid = mpOwner->GetComponent<RigidBody>())
    {
        rigid->SetGravityScale(0.5F); // Sets gameobject gravity scale to 0.5f, affecting only half as much
        const auto prevVel = rigid->GetLinearVelocity();
        rigid->SetLinearVel({prevVel.x, 0, 0}); // force reset his linear velocity
        rigid->AddLinearImpulse(moveDir*rigid->GetMass()*2000.0f); // add an impulse which changes velocity directly.

        // Set Looking Direction
        if (rigid->GetLinearVelocity().x > 0)
        {
            mpBlackboard->SetBool("LookAt", D_RIGHT);
        }
        else
        {
            mpBlackboard->SetBool("LookAt", D_LEFT);
        }
        
    }
...
```



### AABB Collider
> **DO NOT USE THIS. AABB is not specifically implemented, although Convex Collider is using AABB as well.**
>
> Please use Convex Collider. 

### Convex Collider
<img src="https://i.ibb.co/Mfw8mSv/Desktop-Screenshot-2019-04-21-13-30-10-67-2.png" width="400"/>

> A Collider is essentially for any game development. This registers the gameobject to the Collision System and allows it to be collided by other colliders. Dystopia Engine currently implements a GJK-algorithm to detect convex collision and as a side-effect, we have inherent Concave detection as well (although not stress-tested). A collider can either be Trigger or Non-Trigger, and uses their respective [OnTriggerEnter] or [OnCollisionEnter] callback functions.
>  
> This variant is the Convex Collider.

**Properties:**

    Attached Body       : If this gameobject has a rigidbody, shows the name of the current rigidbody's gameobject.
    Is Trigger          : Should this resolve collision with other physically-stimulated gameobjects?
    Offset              : How much offset from the gameobject's local transform position.
    Scale               : Scale multiplier from gameobject's transform scale. 
    Points              : Since this is a convex collider, you can adjust the number of points (do make sure to turn debug draw on). 
                          Adjust the vertex's position accordingly.
    Contacts            : By showing this tab, you can see which objects are currently colliding with this gameobject.
    Collision Layer     : Lets you set its collision layer. You can check which layer collides with which layer under [Physics Settings].

### Circle Collider
<img src="https://i.ibb.co/brQrNhT/Desktop-Screenshot-2019-04-21-13-34-36-98-2.png" width="400"/>

> A Collider is essentially for any game development. This registers the gameobject to the Collision System and allows it to be collided by other colliders. Dystopia Engine currently implements a GJK-algorithm to detect convex collision and as a side-effect, we have inherent Concave detection as well (although not stress-tested). A collider can either be Trigger or Non-Trigger, and uses their respective [OnTriggerEnter] or [OnCollisionEnter] callback functions.
>  
> This variant is the Circle Collider.

**Properties:**

    Attached Body       : If this gameobject has a rigidbody, shows the name of the current rigidbody's gameobject.
    Is Trigger          : Should this resolve collision with other physically-stimulated gameobjects?
    Offset              : How much offset from the gameobject's local transform position.
    Scale               : Scale multiplier from gameobject's transform scale. 
    Contacts            : By showing this tab, you can see which objects are currently colliding with this gameobject.
    Collision Layer     : Lets you set its collision layer. You can check which layer collides with which layer under [Physics Settings].

### Point Collider
<img src="https://i.ibb.co/b6rvKTW/Desktop-Screenshot-2019-04-21-13-35-07-38-2.png" width="400"/>

> A Collider is essentially for any game development. This registers the gameobject to the Collision System and allows it to be collided by other colliders. Dystopia Engine currently implements a GJK-algorithm to detect convex collision and as a side-effect, we have inherent Concave detection as well (although not stress-tested). A collider can either be Trigger or Non-Trigger, and uses their respective [OnTriggerEnter] or [OnCollisionEnter] callback functions.
>  
> This variant is the Point Collider. 
> 
> **Although this is implemented, do not use this component as it is not tested.**


### RayCollider
<img src="https://i.ibb.co/b6rvKTW/Desktop-Screenshot-2019-04-21-13-35-07-38-2.png" width="400"/>

> A Collider is essentially for any game development. This registers the gameobject to the Collision System and allows it to be collided by other colliders. Dystopia Engine currently implements a GJK-algorithm to detect convex collision and as a side-effect, we have inherent Concave detection as well (although not stress-tested). A collider can either be Trigger or Non-Trigger, and uses their respective [OnTriggerEnter] or [OnCollisionEnter] callback functions.
>  
> This variant is the Ray Collider. 
> 


**Although this is implemented, do not use this component as it is not tested. Instead, use the following callbacks:**

```cpp
// This raycast skips check once it has hit one gameobject. Take note, if the last bool is not false, it will ignore any gameobject with global collider layer
bool RaycastFirstHit(Math::Vec3D const & _Dir, Math::Point3D const & _mPos,CollisionEvent * _Output, float _MaxLength = 0.f, eColLayer layer = eColLayer::LAYER_GLOBAL, bool ignore_global = false) const;

// Raycast a line and returns an array of collision events - the gameobjects that are hit
bool RaycastAllHits (Math::Vec3D const & _Dir, Math::Point3D const & _mPos,AutoArray<CollisionEvent> & _Output, float _MaxLength = 0.f, eColLayer layer = eColLayer::LAYER_GLOBAL) const;
```

**Usage:**
```cpp
CollisionEvent colEvent; // optional

if (EngineCore::Get<CollisionSystem>()->RaycastFirstHit(dirNorm, pos, &colEvent,0.0f, eColLayer::LAYER_14, true))
{ 
    if (colEvent.mCollidedWith->HasTag("Player"))
    {
        //do something
    }
}
```

**OR**

```cpp
if (EngineCore::Get<CollisionSystem>()->RaycastFirstHit(dirNorm, pos, nullptr ,0.0f, eColLayer::LAYER_14, true))
{ 
    if (colEvent.mCollidedWith->HasTag("Player"))
    {
        //do something
    }
}
```

### SpriteRenderer
<img src="https://i.ibb.co/vL72gHD/Desktop-Screenshot-2019-04-21-13-29-03-70-2.png" width="400"/>

> A Sprite Renderer takes a supplied Texture (.dds, .png) and renders it onto the viewport. This component is also equipped with an animation saving/loading/playing. To create animations, users need to ensure that they supply a Sprite Sheet, and have already created an Atlas using [Sprite Editor](####sprite-editor).

**Properties:**

    Has Transparency      : Check this to ensure that particles are not culled by the alpha parts of the texture. 
    TexSampler            : Interactable Area to preview the image and to drag textures onto it to load.
    Tint                  : The Tint to apply to the sprite.
    Auto Resize           : Resize the image to its original aspect ratio and dimensions (1-1)
    Simulate              : Animate the Sprite.
    On Awake              : Should it be simulated on awake?
    Frame Time            : Interval in miliseconds over which one frame should be played.
    Using Anim            : Which Anim Set from this component's list of animations should it use? (For Preview)
    Add Anim              : Adds an empty animation.
    Shader                : Which Shader should this renderer use?
    Overrides             : Are there any overrides to the above Shader? (Attributes)
    
***Adding Animations***
    Name        : The name of this animation to map to.
    Section     : Which section of the generated Atlas should this renderer use for this animation.
    Loop        : Should this play the first frame once the end frame is reached?
    Columns     : Number of Columns to set
    Rows        : number of rows to set
    Start       : Start Frame Number (0-index)
    End         : End Frame Number (0-index)

**Setting and Playing an animation from Code**

```cpp
// Set Animation To Idle
auto mpRend = mpOwner->GetComponent<SpriteRenderer>();

if (mpRend)
{
    mpRend->Stop();
    mpRend->SetAnimation("Fly"); // The "Fly" here refers to the Name property of the Animation.
    mpRend->Play();
}
```


### TextRenderer
<img src="https://i.ibb.co/7JL12Dz/Desktop-Screenshot-2019-04-21-13-34-58-13-2.png" width="400"/>

> A Text Renderer takes a supplied font atlas and displays the text onto the viewport.

**Properties:**

    Anchor X        : Horizontal Alignment of the textbox
    Anchor Y        : Vertical Alignment of the textbox
    Text            : The text to render
    Font Size       : The size of the font (1:1 to local scale)
    Font            : The font.tff to be supplied
    Current State   : The current state of the text. This is not technically implemented and have to be implemented on the user end. Just set the state to      
                      the wanted state.
                      - Default
                      - Hover
                      - Clicked
                      - Disabled


### VideoRenderer
<img src="https://i.ibb.co/10x8s9K/Desktop-Screenshot-2019-04-21-13-35-18-23-2.png" width="400"/>

> A Video Renderer takes a video file directory and renders the video as animation.

**Properties:**

    Play on Startup        : Play video on Awake?
    Loop                   : Is this video loopable?

### AiController
<img src="https://i.ibb.co/2g1v7nx/Desktop-Screenshot-2019-04-21-13-38-24-31-2.png" width="400"/>

> The AI Controller is a simple component that contains a reference to a Behaviour Tree and updates the tree accordingly to the behaviour tree's rule. Unfortunately, although the concept is pretty easy to explain, how this component's programmer implemented this is slightly less-intuitive due to some template meta-programming. 
> 
> The AI Controller and AI System makes use of something called a Blackboard for the sharing of data. This is justa std::map of various pre-defined user and built-in types.
> 
> To create an AI Tree, you have to create a new behaviour and include "AISystem.h" in that behaviour. This guide will take more about Ai System below.

**Properties:**

    Current Node            : Which Node is the Controller currently updating 
    Current Status          : What is the status that the Tree is returning at the current frame 
    Blackboard Map          : A dictionary of user- and built-in- types as a data container.
    AI Tree                 : The view of the behaviour tree, in an hierarichal tree order list.

### ParticleEmitter
<img src="https://i.ibb.co/4NDXwtC/Desktop-Screenshot-2019-04-21-13-24-58-23-2.png" width="400"/>

> This is the Particle Component (similar to Unity). Users can add more than one emitter or trail emitter. For more imformation on how to edit the Particle Emission, refer to [Particle Editor](###particle-system).

**Properties:**

    Maximum            : Max number of particles this particle emitter is allowed to have at any given moment.

### AudioSource
<img src="https://i.ibb.co/Gkjz2bx/Desktop-Screenshot-2019-04-21-13-31-08-94-2.png" width="400"/>

> The AudioSource gives the user the capability of playing audio. As of right now, it is bare-bones. Users can play BGM or SFX. There is also an AudioListener component and the AudioSystem is inherently 3D/Spatial-Enabled. However, the developers ran into a bug and have to cut the feature.

**All AudioSource are therefore only 2D sounds.**

**Properties:**

    Audio            : Draw a audio clip here (.wav) or through code
    Category         : Either BGM or SFX
    On Awake         : Should this play immediately on awake/creation of gameobject?
    Loop             : Should this loop?
    Volume           : The volume of the supplied audio.
    Frequency        : Frequency of supplied audio, does the same exact thing as Pitch.
    Pitch            : Pitch of supplied audio, does the same exact thing as Frequency.
    Range            : Minimal Range for 3D-Spatial Audio (cut)
    Max Range        : Maximum Range for 3D-Spatial Audio (cut)

### AudioListener
> The AudioListener polls for all AudioSource components and plays the audios. This component is inside but is bugged and should **not** be used.

## Project Settings Details
### Graphics
<img src="https://i.ibb.co/pfmS466/graphics.png" width="600"/>

>This is where you adjust graphics settings.

**Properties**

- Gamma: The relationship between the input signal and the luminance of a monitor is not linear but exponential with gamma being the exponent. 
  - A gamma value of 2.2 has become the de facto standard in Windows, the Internet and the digital photography. This means that only monitors with a gamma value of 2.2 will produce a correct output.
- V Sync:               Should the rendering be locked to the refresh rate of the user's monitor?
- Resolution:           For rendering aspect ratios.
- Debug Draw:           Turn on/off the debug draw toggle of all cameras in the scene.
- Collider Color:       The color of the debug collider box.
- Debug Line Threshold: Keep it at 0.958-0.98
- Background:           Supposed to be the background of the empty scene view, but it is not working anymore.


### Physics
<img src="https://i.ibb.co/j82c3GR/collision.png" width="600"/>

**Properties**

    Gravity                     : The global gravity force that gives a downward acceleration (Y-axis) too all rigidbodies in the stimulation.
    Max Velocity                : The max velocity to exponentially be clamped down to
    Sleep Tolerance             : At above what velocity (units/s) be tolerated until it is put to sleep
    Sleep Bias                  : Basically changes how long the rigidbody have to be at the Sleep Tolerance for to be considered sleeping
    Velocity Iterations         : How many iterations of velocity-based resolution should occur
    Position Iterations         : How many iterations of position-based (translation) resolution should occur

Like Unity, Dystopia uses a Collision Layer Table to ignore certain collisions between layers.

### Input
<img src="https://i.ibb.co/QHYZDKk/2.png" width="600"/>

>Like Unity, Dystopia's Input Mapping allows for dual-keys to a single Action. Unfornately, duplicate Actions are not allowed (restricted by std::map). At this point of development, it is impossible to delete single axis, and instead must delate the whole axes by right clicking on "Axes" > Delete.

**Properties**

    Name                     : Name of the action to map to
    + Description            : The description of the positive button
    - Description            : The description of the negative button.
    + Button                 : Which key to map as Positive
    - Button                 : Which key to map as Negative
    Alt + Btn                : Alternate key to map as Positive
    Alt - Btn                : Alternate key to map as Negative
    Gravity                  : Rate of Change of values to neutral (0.00f)
    Dead                     : A range of [-Dead] to [+Dead] that will be snapped to 0.00f
    Sensitivity              : Rate of Change of values to -1 or +1, as supplied by the input
    Snap                     : Snap to neutral (0.00f) before changing to the opposite sign
    Invert                   : Invert positive and negative input (usually, up axis on left controller analog should be inverted)
    Type                     : Is this a Joystick Axis or Key/Mouse button?
    Axis                     : Which axis is it? For keyboard its usually X/Y axis.

### Time
<img src="https://i.ibb.co/k92t722/Capture.png" width="600"/>

> You can change what is the fixed timestep globally, or whether to simulate the time (given that it is in V-Sync).
> Time Scale affects the deltaTime for all Update loop (slow-mo, fast-mo).

## Using The Behaviour Tree
Samuel Lum, I'll explain this in Redmond. For now, please look at the following files:

- "System/AI/Blackboard.h"
- "System/AI/NeuralTree.h"
- "System/AI/TreeBuilder.h"
- "System/AI/Composite.h"
- "System/AI/Decorator.h"
- "System/AI/Task.h"
- "Component/AiController.h"	

It should be fairly commented and since you have experience with your own behaviour tree, you should be able to figure out how the system work.
