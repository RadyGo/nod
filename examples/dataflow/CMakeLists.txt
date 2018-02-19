project(nod-dataflow VERSION ${thenod_VERSION})

set(SOURCES
    dataflow.cpp
)
set(HEADERS
)

set(RES
    resources.qrc
)

qt5_add_resources(QRC ${RES})

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${QRC} ${RES})
target_link_libraries(${PROJECT_NAME} nod)
