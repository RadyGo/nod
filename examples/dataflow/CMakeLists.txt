project(nod-dataflow VERSION ${thenod_VERSION})

set(SOURCES
    dataflow.cpp
)
set(HEADERS
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} nod)
