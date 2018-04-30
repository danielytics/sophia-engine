# Rudimentary Documentation

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
    scenes:
        <scene name>: <scene filename>
```

 * `name` is the name of the game, used to set the window title.
 * `start` is the name of the scene to start the game in (must be listed in `scenes`)
 * `scenes` is a collection of key/value pairs where the key is the name of the scene and the value is a yaml file describing the scene (see `data/sceneX.yml`)

## data/sceneX.yml

This file describes a scene in the game. A scene is any screen (eg the title screen) or level/stage in the game and consists of a tree of entities and their components.
Scenes can dynamically change at runtime, this file describes the load-time state. The structure in this file is the same structure as seen in the editor, the runtime structure may be different as not all scene nodes (eg groups) exist at runtime but are for organisation only. The editor is essentially a GUI for these files.

```
scene:
 - <node-name>:
   type: entity
   components:
     - <component-type>: <component attributes>
   children:
     - <node-name>: ...
     ...
 - <node-name>:
   type: group
   children:
     - <node-name>: ...
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
  children:
    <list of child nodes>
```

### entity

An entity is a node that represents a "thing" in the scene. Whether or not its a physical thing depends on its components. Entities are made up of a collection of components. Entities and components are represented at runtime as entities and components in the EnTT Entity-Component-System.
All entities automatically get a `Parent` and `Children` component added in runtime, representing their hierarchical structure in the scene.

Many types of components can be added to entities, representing all of the Sophia built-in functionality as well as scripted functionality.
Transformation components represent the location of the entity in the scenes 2D or 3D space and, by default, are relative to the entities parents. This way, the parent/child relationship of entities can be used to create complex aggregate entities out of many smaller components.

```
<node-name>:
  type: entity
  components:
    <list of components>
  children:
    <list of child nodes>
```

### template

A template node is an instance of a entity node tree that has been precreated for convenience. The structure of template files is described below. Templates can expose attributes, which can be set in the node. Templates can have children as usual, which are added to the root nodes children in that template.

```
<node-name>:
  type: template
  source: <template yaml file>
  attributes:
    <attribute name>: <attribute value>
    ...
  children:
    <list of children>
```

### Template source files

A template source file is a yaml file describing the node tree for that template. When this template is instanced into a scene, at runtime the template node in the scene is replaced with a copy/instance of the node tree in this file, with all of the placeholder attributes replaced by the attributes listed in the template node and all of the templates child nodes appended to the children of the root node in the templated tree.

The root node must be of type `entity` or `template` (ie `group` is not allowed), however, child nodes can be of any type. If the type is `template`, then the instance is created recursively by instanciating that template first, setting its attributes, appending the children and then insterting that into the scene, setting its attributes and appending children. Attributes can be forwarded. Template nesting can be arbitrarily deep.

```
template:
  type: <'entity' or 'template'>
  comment: <optional description/comment, used in the editor>
  <type specific attributes>
  children:
    <list of children>
```

Attribute placeholders are defined by setting their values as `$(attribute name: default value)` or `$(attribute name)`.

Example template file: `mycharacter.yml`
```
template:
  type: entity
  comment: Template for characters.
  components:
    - position: x=$(position-x) y=$(position-y)
    - sprite:
       image: $(sprite-image: images/default-character.png)
    - rigid-body:
      shape:
        box2d: [0.5, 0.2]
  children:
    - weapon:
      type: entity
      components:
        - position: x=0.2 y=0.3 # Relative to parent!
        - sprite:
          image: $(weapon-image: images/default-weapon.png)
```

Example of scene using this template:
```
scene:
  - player:
    type: template
    source: mycharacter.yml
    attributes:
      position-x: 10
      position-y: 0
      sprite-image: images/overridden-character.png
      weapon-image: images/overridden-weapon.png
    children:
      - hat:
        type: entity
        components:
          - position: x=0 y=1
          - sprite:
            image: images/hat.png
```
