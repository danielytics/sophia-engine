# Rudimentary Documentation

NOTE: Most of what is described in this doucment does not yet exist. Its currently more documentation of what is planned to be implemented and serves as a means to solidify ideas before they are committed to code. As such, this is a living document and is subject to change as the design changes or if the planned features are implemented differently than described. The main purpose, right now, is to focus my thoughts.

It is also envisioned that most of what is described here would not be edited directly in the YAML files, but instead be edited in a GUI editor (`dk/editor` branch).

# File Structure

```
 -- sophia
 -- config.yml
 -- data/
    -- game.yml
    -- sceneX.yml
    other data
```

 * `config.yml` - Basic human-readable configuration, for end-user-facing configuration, logging and bootstrapping.
 * `data/init.yml` - Game-specific bootstrapping. Name and location can be changed in `config.yml`.
 * `data/sceneX.yml` - Scene description. One file per scene in the game. Name and location can be changed in `data/init.yml`.

## config.yml

The config file consists of a number of sections:

 * `graphics` - The graphics section contains graphics/renderer configuration, such as resolution and vsync.
 * `telemetry` - This section sets the logging level and development/debug telemetry. Mostly unused in release builds.
 * `game` - This section is used to bootstrap the game by specifying where to look for game data and where to find the game-specific configuration.

### graphics

 * `resolution` - Set the window resolution. Can be either a 2-element list in thje form of `[width, height]` in pixels, or one of `720p` or `1080p`.
 * `fullscreen` - Set whether to run in fullscreen or windowed mode. Can be either `Yes` or `No`.
 * `vsync` - Whether to enable vertical sync or not. Can be either `Yes` or `No`.
 * `debug` - Whether to enable debug rendering. This option is ignored in release builds. Can be either `Yes` or `No`.

### telemetry

 * `dev_mode` - Set whether development mode is turned on. Development mode reports telemetry data to the editor. This option is ignored in release builds. Can be either `Yes` or `No`.
 * `logging` - Sets the logging level. The `trace` and `debug` levels are ignored in release builds. Valid options are `trace`, `debug`, `info`, `warn` and `error`.

### game

 * `sources` - A list of paths (relative to this file) to search for game data. Earlier paths are searched first. Paths can be either to directories or to archive files (any PhysicsFS format supported, eg ZIP or 7z).
 * `game_config` - Name of the game-specific bootstrap file (relative to one of the above sources).

## data/game.yml

```
game:
    name: <Name of the game>
    start: <name of start scene>
    input:
        <action name>: <input type and default value>
    scenes:
        <scene name>: <scene filename>
```

 * `name` is the name of the game, used to set the window title.
 * `start` is the name of the scene to start the game in (must be listed in `scenes`)
 * `input` lists the actions which are to be mapped to user input, input type specifies what type of input is supported (button, axis, analogue, touch etc). Default values are also specified, but may be overridden programmatically, eg by in-game input mappers. Overridden data is stored in a platform specific place.
 * `scenes` is a collection of key/value pairs where the key is the name of the scene and the value is a yaml file describing the scene (see `data/sceneX.yml`)

## data/sceneX.yml

This file describes a scene in the game. A scene is any screen (eg the title screen) or level/stage in the game and consists of a tree of entities and their components.
Scenes can dynamically change at runtime, this file describes the load-time state. The structure in this file is the same structure as seen in the editor, the runtime structure may be different as not all scene nodes (eg groups) exist at runtime but are for organisation only. The editor is essentially a GUI for these files.

```
scene:
 - <node-name>:
   type: entity
   components:
     <component-type>: <component attributes>
   children:
     <node-name>: ...
     ...
 - <node-name>:
   type: group
   children:
     <node-name>: ...
    ...
```

The root is always `scene` and contains a list of nodes. Each node has a name (the YAML key) a `type`, type specific attributes and `children`. Children is a list of child nodes (and have the same node structure).

Available node types are: `group`, `entity` and `template`.

### group

A group node is a collection of nodes. This does not exist at runtime and is used purely for easier scene organisation.

```
<node-name>:
  type: group
  comment: <optional description/comment, used in the editor>
  defaults:
    <component name>:
        <component>
        ...
  children:
    <child node>
    ...
```

### entity

An entity is a node that represents a "thing" in the scene. Whether or not its a physical thing depends on its components. Entities are made up of a collection of components. Entities and components are represented at runtime as entities and components in the EnTT Entity-Component-System.
All entities automatically get a `Parent` and `Children` component added in runtime, representing their hierarchical structure in the scene.

Many types of components can be added to entities, representing all of the Sophia built-in functionality as well as scripted functionality.
Transformation components represent the location of the entity in the scenes 2D or 3D space and, by default, are relative to the entities parents. This way, the parent/child relationship of entities can be used to create complex aggregate entities out of many smaller components.

```
<node-name>:
  type: entity
  comment: <optional description/comment, used in the editor>
  components:
    <component>
    ...
  children:
    <child node>
    ...
```

Each component is has the name of the component as the key and the value is a map of attributes. For example, a `position` component might have an `x` and `y` attribute and would be defined as follows:
```
position:
  x: <value for x>
  y: <value for y>
```
Since it is YAML, it could also be defined as:
```
position: x=100 y=100
```

An example of a simple entity is:
```
player:
  type: entity
  comment: The player character
  components:
    position: x=10 y=0
    sprite:
      image: images/default-character.png
    rigid-body:
      shape:
        box2d: [0.5, 0.2]
  children:
    weapon:
      type: entity
      components:
        position: x=0.2 y=0.3 # Relative to parent!
        sprite:
          image: images/default-weapon.png
```

### template

A template node is a node which defines an entity using a template source file. A tenplate source file is similar to a scene file and defines a tree of entities, which is loaded in place of the template node. Any children of the template node are appended to the children of the entity in the template source file. The structure of the template source file is described below.
Template nodes are not loaded into the scene by default when the scene is loaded, but can define an optional list of instances. A copy of the instantiated entity tree is inserted for each instance listed in the template nodes instance list and the attributes in the template source file are replacd with those specified for each instance. This way, zero or more copies of the template node can be loaded into the scene.
Template nodes can also be dynamically instantiated at runtime throuwh spawner components.

```
<node-name>:
  type: template
  comment: <optional description/comment, used in the editor>
  source: <template yaml file>
  instances:
    - <attribute name>: <attribute value>
      ...
  children:
    <child>
    ...
```

**Limitation:** currently, the node name of a template node must be unique (for template nodes) within the file in which it is found (scene file or template source file). Defining multiple template nodes with the same node name will cause scene loading to fail. The same node name used by a template node *may* be used for entity or group nodes.

### Template source files

A template source file is a yaml file describing the node tree for that template. When this template is instantiated into a scene, at runtime the template node in the scene is replaced with a copy/instance of the node tree in this file, with all of the placeholder attributes replaced by the attributes listed in the template nodes instance list and all of the templates child nodes appended to the children of the root node in the templated tree.

The root node must be of type `entity` or `template` (ie `group` is not allowed), however, child nodes can be of any type. If the type is `template`, then the instance is created recursively by instantiating that template first, setting its attributes, appending the children and then insterting that into the scene, setting its attributes and appending children. Attributes can be forwarded. Template nesting can be arbitrarily deep.

```
template:
  type: <'entity' or 'template'>
  comment: <optional description/comment, used in the editor>
  <type specific attributes>
  children:
    <child>
    ...
```

Attribute placeholders are defined by setting their values as `$(attribute name: default value)` or `$(attribute name)`.

Example template file: `mycharacter.yml`
```
template:
  type: entity
  comment: Template for characters.
  components:
    position: x=$(position-x) y=$(position-y)
    sprite:
       image: $(sprite-image: images/default-character.png)
    rigid-body:
      shape:
        box2d: [0.5, 0.2]
  children:
    weapon:
      type: entity
      components:
        - position: x=0.2 y=0.3 # Relative to parent!
        - sprite:
          image: $(weapon-image: images/default-weapon.png)
```

Example of scene using this template:
```
scene:
  player:
    type: template
    source: mycharacter.yml
    instances:
      - position-x: 10
        position-y: 0
        sprite-image: images/overridden-character.png
        weapon-image: images/overridden-weapon.png
    children:
      hat:
        type: entity
        components:
          - position: x=0 y=1
          - sprite:
            image: images/hat.png
```

# Components

## List of Components

Below is a list of built-in components.

 * **transform**
```
transform:
  position:
    x: <x position in the scene>
    y: <y position in the scene>
    z: <z position in the scene>
  rotation:
    x: <amount of rotation around the x axis, from no rotation (0.0) to full rotation (1.0)>
    y: <amount of rotation around the y axis, from no rotation (0.0) to full rotation (1.0)>
    z: <amount of rotation around the z axis, from no rotation (0.0) to full rotation (1.0)>
  scale:
    x: <amount of scaling on the x axis>
    y: <amount of scaling on the y axis>
    z: <amount of scaling on the z axis>
```

Position, orientation and scale factor of the entity in the scene.

 * **trigger-region**
```
trigger-region:
  shape: <shape data>
  event: <event data>
  triggers: <list of physics object types which trigger this region>
```

 * **rigid-body**
```
rigid-body:
  mass: <body mass>
  shape: <shape data>
```

 * **sprite**
```
sprite:
  color: <color data>
  image: <image filename>
```

 * **spawner**
```
spawner:
  event: <name of event>
  template: <template node name>
```
**Limitation:** currently spawner components can only instantiate templates defined in the same yaml file (scene or template source file) in which they are contained.

[IDEA: a potential workaround for the above limitation is to create event forwarder components that have a scene-wide addressing mechanism, which can forward events emitted by a node in a different template source file in the same scene to a spawner node in the same file as the forwarder node]

 * **behavior**
```
behavior:
  source: <lua file name>
  on:
    <event name>: <function name>
    ...
```
Behaviors call Lua funcitons in response to events. The Lua functions get passed two arguments: `(event, entity)`

`event` is an object containing the data of the event which triggered the function, as well as allowing the function to emit new events. The `entity` is an object which provides access to the entity to which the behavior component belongs and provides both read access to the entities other components. Component data may be modified by issuing events.

 * **persisted**
```
persisted:
  - <name of component>
  ...
```

This component causes all named components in this entity to be persisted when the game is saved and relaoded when the game is loaded.

 * **global**
```
global: Yes
```

This component causes this entity to remain in the scene even when scenes are changed and it does NOT get reloaded if the original scene is loaded again.
This does NOT cause any child entities to be global, if child entities should be global too, then they must be individually marked as global.

 * **model**
```
model:
  - mesh: <mesh data>
    material: <material data>
```

A model is a collection of meshes to be rendered. Each mesh has an optional material.

 * **light-source**
```
light-source:
  type: <point-light, spot-light>
  color: <color data>
  shadow-caster: <Yes or No>
```

## Component Data

Data types used by various components:

 * **Vector2D data**
```
<...>:
  x: <x component>
  y: <y component>
```

 * **Vector3D data**
```
<...>:
  x: <x component>
  y: <y component>
  z: <z component>
```

 * **Shape data**
```
box:
  w: <width of box>
  h: <height of box>

circle:
  r: <radius of circle>

triangle:
  - <[x, y] for first point>
  - <[x, y] for second point>
  - <[x, y] for third point>

polygon:
  vertices:
    - <[x, y] for point>
    ...

polygon:
  ref: <unique resource id for polygon>

```

 * **Color data**
```
rgb: [r, g, b]

rgba: [r, g, b, a]

hsl: [h, s, l]

hsla: [h, s, l, a]

hsv: [h, s, v]

hsva: [h, s, v, a]

gs: [grayscale brightness]

gsa: [grayscale brightness, a]
```

 * **Mesh data**
```
mesh:
  ref: <unique resource id for mesh>

mesh:
  source: <.mesh source file for mesh (if 'x.mesh', will load 'x.mesh' if found or 'x.mesh.yaml' if not found)>

mesh:
 primitive: <'point', 'line', 'line-strip', 'triangle', 'triangle-strip' or 'triangle-fan'>
 vertices:
   - <[[position.x, position.y, position.z], [normal.x, normal.y, normal.z], [u, v]] of vertex>
   ...
 indices: <[list] of vertex indices>
```

 * **Event data**
```
event:
  type: <event name>
  <event attribute name>: <attribute value>
  ...
```

Events also automatically get the following attributes added at runtime when sent. These are not allowed in the event data.
```
source-entity: <id of entity which triggered the event>
source-node: <name of node which triggered the event>
trigger-data: <event source specific data>
```
`trigger-data` depends on the emitter of the event and may be null. For example, `trigger-region` sets this property to contain data about which entity entered the region.

