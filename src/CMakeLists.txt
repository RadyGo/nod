project(nod VERSION ${thenod_VERSION})

set(SOURCES
    nod/abstractnodemodel.cpp
    nod/aligndialog.cpp
    nod/common.cpp
    nod/connectionitem.cpp
    nod/connectionshape.cpp
    nod/createnodedialog.cpp
    nod/defaultconnectionshape.cpp
    nod/defaultnodeitemfactory.cpp
    nod/defaultnodeitem.cpp
    nod/nodefactory.cpp
    nod/nodegrid.cpp
    nod/nodeitem.cpp
    nod/nodeitemfactory.cpp
    nod/nodemodel.cpp
    nod/nodescene.cpp
    nod/nodeview.cpp
    nod/serialized.cpp
    nod/undo.cpp
)

set(HEADERS
    nod/abstractnodemodel.h
    nod/aligndialog.h
    nod/common.h
    nod/connectionitem.h
    nod/connectionshape.h
    nod/createnodedialog.h
    nod/defaultconnectionshape.h
    nod/defaultnodeitemfactory.cpp
    nod/defaultnodeitem.h
    nod/nodefactory.h
    nod/nodegrid.h
    nod/nodeitem.h
    nod/nodeitemfactory.h
    nod/nodemodel.h
    nod/nodescene.h
    nod/nodeview.h
    nod/serialized.h
    nod/undo.h
)

set(UI
    nod/defaultcreatenodedialog.ui
)

qt5_wrap_ui(UIC ${UI})

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${UI} ${UIC})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
