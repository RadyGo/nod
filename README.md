This is a work in progress. Documentation will eventually appear here.

Goals

* provide a easy to use node editing library for Qt Widgets / QML
* implement common functionality:
** provide standard implementations with all / as much functionally as possible
** built-in cut / copy / paste
** built-in undo / redo
** built-in serialization (JSON)
* do not force a data representation on the user, use an adapter
** modeled after QAbstractItemModel
* still maintain customizability
** create objects using user-provided / customized factories 
** use virtual methods as much as possible so that library users can override them
*** provide implementation helpers for tedious boilerplate code
** use style objects in standard implementations which allow simple customizations such as colors and fonts
* provide several examples showing how to use the library
