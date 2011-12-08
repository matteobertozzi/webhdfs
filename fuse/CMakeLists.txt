set(SOURCES fuse-webhdfs.c)

find_library(FUSE fuse)

set(binDir ${CMAKE_CURRENT_BINARY_DIR}/../${WEBHDFS_DIST_NAME}/bin)
file(MAKE_DIRECTORY ${binDir})

include_directories(${CMAKE_CURRENT_BINARY_DIR}/../${WEBHDFS_DIST_NAME}/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../${WEBHDFS_DIST_NAME}/lib)

add_executable(fuse-webhdfs ${SOURCES})
target_link_libraries(fuse-webhdfs webhdfs ${FUSE})

get_target_property(binPath fuse-webhdfs LOCATION)
add_custom_command(TARGET fuse-webhdfs POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${binPath} ${binDir})

if (NOT WIN32)
  install(TARGETS fuse-webhdfs RUNTIME DESTINATION bin)
endif ()

