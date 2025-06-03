set(INIT_ROUTINES_SOURCES)
file(GLOB_RECURSE INIT_ROUTINES_SOURCES
     CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
     "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cc"
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp" 
)

add_library(init_routines STATIC ${INIT_ROUTINES_SOURCES})

target_include_directories(init_routines PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src")

target_link_libraries(init_routines 
     PRIVATE
          glfw
          glad)