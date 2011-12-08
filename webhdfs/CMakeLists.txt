set(PUBLIC_HEADERS webhdfs.h)
set(PRIVATE_HEADERS webhdfs_p.h buffer.h)
set(SOURCES webhdfs.c buffer.c request.c config.c)

find_library(CURL curl)
find_library(YAJL yajl)

set(libDir ${CMAKE_CURRENT_BINARY_DIR}/../${WEBHDFS_DIST_NAME}/lib)
set(incDir ${CMAKE_CURRENT_BINARY_DIR}/../${WEBHDFS_DIST_NAME}/include/webhdfs)
file(MAKE_DIRECTORY ${libDir})
file(MAKE_DIRECTORY ${incDir})

set(LIBRARY_OUTPUT_PATH ${libDir})

add_library(webhdfs_s STATIC ${SOURCES} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})
add_library(webhdfs SHARED ${SOURCES} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})

target_link_libraries(webhdfs ${CURL} ${YAJL})
target_link_libraries(webhdfs_s ${CURL} ${YAJL})

# copy public headers to output directory
foreach(header ${PUBLIC_HEADERS})
  set (header ${CMAKE_CURRENT_SOURCE_DIR}/${header})

  exec_program(${CMAKE_COMMAND} ARGS -E copy_if_different ${header} ${incDir})

  add_custom_command(TARGET webhdfs_s POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${header} ${incDir})
endforeach(header ${PUBLIC_HEADERS})

if(NOT WIN32)
  install(TARGETS webhdfs LIBRARY DESTINATION lib${LIB_SUFFIX})
  install(TARGETS webhdfs_s ARCHIVE DESTINATION lib${LIB_SUFFIX})
  install(FILES ${PUBLIC_HEADERS} DESTINATION include/webhdfs)
endif()

