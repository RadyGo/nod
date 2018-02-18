project(nod VERSION ${thenod_VERSION})

set(SOURCES
    nod/abstractnodemodel.cpp
    nod/common.cpp
    nod/connectionitem.cpp
    nod/connectionshape.cpp
    nod/defaultconnectionshape.cpp
    nod/defaultnodeitem.cpp
    nod/nodefactory.cpp
    nod/nodegrid.cpp
    nod/nodeitem.cpp
    nod/nodeitemfactory.cpp
    nod/nodemodel.cpp
    nod/nodescene.cpp
    nod/nodeview.cpp
    nod/serializer.cpp
    nod/undo.cpp
)

set(HEADERS
    nod/abstractnodemodel.h
    nod/common.h
    nod/connectionitem.h
    nod/connectionshape.h
    nod/defaultconnectionshape.h
    nod/defaultnodeitem.h
    nod/nodefactory.h
    nod/nodegrid.h
    nod/nodeitem.h
    nod/nodeitemfactory.h
    nod/nodemodel.h
    nod/nodescene.h
    nod/nodeview.h
    nod/serializer.h
    nod/undo.h
)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
