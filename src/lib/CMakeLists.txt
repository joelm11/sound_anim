add_library(mesh STATIC mesh.cc)
target_link_libraries(mesh PUBLIC glm)
target_include_directories(mesh PUBLIC "${CMAKE_LIST_CURRENT_DIR}")