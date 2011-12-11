include_directories(${CMAKE_CURRENT_BINARY_DIR}/../${WEBHDFS_DIST_NAME}/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../${WEBHDFS_DIST_NAME}/lib)

add_executable(example-basic example-basic.c)
target_link_libraries(example-basic webhdfs)

add_executable(example-basic2 example-basic2.c)
target_link_libraries(example-basic2 webhdfs)

add_executable(example-read example-read.c)
target_link_libraries(example-read webhdfs)

add_executable(example-write example-write.c)
target_link_libraries(example-write webhdfs)


