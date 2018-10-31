# ![logo](https://raw.githubusercontent.com/Powerchu/Frozen-Anvil-Studio/StableRelease/images/dystopia_engine_ico%402x.png?token=AH2_fURrUrmN_vuIEjz6kmz0GpeE16tEks5b41kzwA%3D%3D) Dystopia Engine
> A Student Team's attempt at a Game Engine.

## 🚩 Table of Contents
- [Browser Support](#-browser-support)
- [Standard and Extensible](#-standard-and-extensible)
    - [CommonMark + GFM Specifications](#commonmark--gfm-specifications)
    - [Powerful Extensions](#powerful-extensions)
- [Features](#-features)
    - [Productive Markdown mode](#productive-markdown-mode)
    - [Easy WYSIWYG mode](#easy-wysiwyg-mode)
    - [And more](#and-more)
- [Install](#-install)
    - [using npm](#using-npm)
    - [using bower](#using-bower)
    - [Via Contents Delivery Network (CDN)](#via-contents-delivery-network-cdn)
    - [Download](#download)
- [Usage](#-usage)
    - [Editor](#editor)
        - [HTML](#html)
        - [javascript](#javascript)
        - [options](#options)
    - [Viewer](#viewer)
- [Docs](#-docs)
- [Examples](#-examples)
- [Contributing](#-contributing)
- [TOAST UI Family](#-toast-ui-family)
- [Used By](#-used-by)
- [License](#-license)

## 🌏 Browser Support


## <img src="https://user-images.githubusercontent.com/1215767/34336735-e7c9c4b0-e99c-11e7-853b-2449b51f0bab.png" height="18px" /> Standard and Extensible


### CommonMark + GFM Specifications


### Powerful Extensions
*CommonMark* and *GFM* are great, but we often need more abstraction. The **TOAST UI Editor** comes with powerful **Extensions** in compliance with the *Markdown* syntax. You also get the flexibility to develop your own extensions using simple APIs.

Here are some of the extensions you can start with:

* **Color picker**: [ColorPicker](https://github.com/nhnent/tui.color-picker) provides an easy way to color text with a GUI tool box
* **Chart code block**: A Code block marked as a 'chart' will render [charts](https://github.com/nhnent/tui.chart)
* **UML code block**: A Code block marked as an 'uml' will render [UML diagrams](http://plantuml.com/screenshot)
* **Table merge**: You can merge columns and rows in tables

 To learn more about **Extensions** check the [Using Extension](https://github.com/nhnent/tui.editor/blob/master/docs/using-extensions.md)

## 🎨 Features
**TOAST UI Editor** provides **Markdown mode** and **WYSIWYG mode**.

Depending on the type of use you want like production of *Markdown* or maybe to just edit the *Markdown*. The **TOAST UI Editor** can be helpful for both the usage. It offers **Markdown mode** and **WYSIWYG mode**, which can be switched any point in time.

### Productive Markdown mode
![markdown image](https://user-images.githubusercontent.com/1215767/34354737-b98a0736-ea73-11e7-8375-d4c83b8894d8.png)
* **Live Preview**: Edit Markdown while keeping an eye on the rendered HTML. Your edits will be applied immediately
* **Scrolling Sync**: Synchronous scrolling between Markdown and Preview. You don't need to scroll through each one separately
* **Auto indent**: The cursor will always be where you want it to be
* **Syntax highlight**: You can check broken Markdown syntax immediately

### Easy WYSIWYG mode
![wysiwyg image](https://user-images.githubusercontent.com/1215767/34354831-5f04c7e6-ea74-11e7-9664-97f71c4fee6e.png)
* **Copy and paste**: Paste anything from browser, screenshot, excel, powerpoint, etc.
* **Codeblock editor**: Highlight 170+ languages with full size code editor
* **Table**: Hate the Markdown table? You can do everything with a mouse

### And more
* **i18n**: English, Dutch, Korean, Japanese, Chinese, Spanish, German, Russian, French, Ukrainian, Turkish, Finnish + language you extend.
* **Viewer**: Renders Markdown content with extensions


## 💾 Install

### using npm


### using bower

### Via Contents Delivery Network (CDN)


### download


## 🔨 Usage
The code provided underneath is for *npm* + *bundler*. If you are using *bower* please see [Getting started with bower](https://github.com/nhnent/tui.editor/blob/master/docs/getting-started-with-bower.md).

### Editor

#### HTML
Place a `<div></div>` where you want TOAST UI Editor rendered.
```html
<body>
...
<div id="editSection"></div>
...
</body>
```

#### javascript
Add dependencies & initialize Editor class with given element to make an Editor.
```javascript
// deps for editor
require('codemirror/lib/codemirror.css'); // codemirror
require('tui-editor/dist/tui-editor.css'); // editor ui
require('tui-editor/dist/tui-editor-contents.css'); // editor content
require('highlight.js/styles/github.css'); // code block highlight

var Editor = require('tui-editor');
...
var editor = new Editor({
    el: document.querySelector('#editSection'),
    initialEditType: 'markdown',
    previewStyle: 'vertical',
    height: '300px'
});
```

or you can use jquery plugin.
```javascript
$('#editSection').tuiEditor({
    initialEditType: 'markdown',
    previewStyle: 'vertical',
    height: '300px'
});
```

#### options
* **height**: Height in string or auto ex) `300px` | `auto`
* **initialValue**: Initial value. Set Markdown string
* **initialEditType**: Initial type to show `markdown` | `wysiwyg`
* **previewType**: Preview style of Markdown mode `tab` | `vertical`
* **usageStatistics**: Let us know the *hostname*. We want to learn from you how you are using the editor. You are free to disable it. `true` | `false`

Find out more options [here](https://nhnent.github.io/tui.editor/api/latest/ToastUIEditor.html#ToastUIEditor)

### Viewer
**TOAST UI Editor** provides a **Viewer** in case you want to show *Markdown* content without loading the editor. The **Viewer** is much **lighter** than the editor.

```javascript
// deps for viewer.
require('tui-editor/dist/tui-editor-contents.css'); // editor content
require('highlight.js/styles/github.css'); // code block highlight

var Viewer = require('tui-editor/dist/tui-editor-Viewer');
...
var editor = new Viewer({
    el: document.querySelector('#viewerSection'),
    height: '500px',
    initialValue: '# content to be rendered'
});
...
```

Be careful not to load both the editor and the viewer at the same time because the editor already contains the viewer function, you can initialize editor [Editor.factory()](https://nhnent.github.io/tui.editor/api/latest/ToastUIEditor.html#.factory) and set the `viewer` option to value `true` in order to make the editor a viewer. You can also call [getHTML()](https://nhnent.github.io/tui.editor/api/latest/ToastUIEditor.html#getHtml) to render the HTML.


```javascript
var Editor = require('tui-editor');
...
var editor = Editor.factory({
    el: document.querySelector('#viewerSection'),
    viewer: true,
    height: '500px',
    initialValue: '# content to be rendered'
});
...
```

**TOAST UI Editor** respects *CommonMark* and *GFM*. So any *Markdown* renderer including [markdownit](https://github.com/markdown-it/markdown-it) can handle the content made using TOAST UI Editor. You can also use any of these renderer in place of TOAST UI Editor **Viewer**.


## 📙 Docs
* [Getting started](https://github.com/nhnent/tui.editor/blob/master/docs/getting-started.md)
* [Getting started with bower](https://github.com/nhnent/tui.editor/blob/master/docs/getting-started-with-bower.md)
* [Writing your own extension](https://github.com/nhnent/tui.editor/blob/master/docs/writing-your-own-extension.md)
* [APIs](https://nhnent.github.io/tui.editor/api/latest/)

## 🐾 Examples
* [editor basic](https://nhnent.github.io/tui.editor/api/latest/tutorial-example01-basic.html)
* [viewer basic](https://nhnent.github.io/tui.editor/api/latest/tutorial-example02-viewer-basic.html)
* [jQuery plugin](https://nhnent.github.io/tui.editor/api/latest/tutorial-example03-jquery.html)
* [uml extension](https://nhnent.github.io/tui.editor/api/latest/tutorial-example08-uml.html)
* [chart extension](https://nhnent.github.io/tui.editor/api/latest/tutorial-example11-chart.html)
* [writing extension](https://nhnent.github.io/tui.editor/api/latest/tutorial-example12-writing-extension.html)
* [all extensions](https://nhnent.github.io/tui.editor/api/latest/tutorial-example00-demo.html)

## 💬 Contributing
* [Code of Conduct](https://github.com/nhnent/tui.editor/blob/master/CODE_OF_CONDUCT.md)
* [Contributing guideline](https://github.com/nhnent/tui.editor/blob/master/CONTRIBUTING.md)
* [Commit convention](https://github.com/nhnent/tui.editor/blob/master/docs/COMMIT_MESSAGE_CONVENTION.md)

## 🍞 TOAST UI Family
* [TOAST UI Grid](https://github.com/nhnent/tui.grid)
* [TOAST UI Chart](https://github.com/nhnent/tui.chart)
* [TOAST UI Calendar](https://github.com/nhnent/tui.calendar)
* [TOAST UI Image Editor](https://github.com/nhnent/tui.image-editor)
* [TOAST UI Components](https://github.com/nhnent?q=tui)

## 🚀 Used By
* [TOAST Dooray! - Collaboration Service (Project, Messenger, Mail)](https://dooray.com)

## 📜 License
This software is licensed under the [MIT](https://github.com/nhnent/tui.editor/blob/master/LICENSE) © [NHN Ent.](https://github.com/nhnent)
