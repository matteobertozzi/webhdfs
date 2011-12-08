include_directories(${CMAKE_CURRENT_BINARY_DIR}/../${WEBHDFS_DIST_NAME}/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../${WEBHDFS_DIST_NAME}/lib)

add_executable(example-basic example-basic.c)
target_link_libraries(example-basic webhdfs)

