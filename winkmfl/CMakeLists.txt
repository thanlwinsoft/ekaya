CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

project(winkmfl)

enable_language(CXX)
# 4267 conversion from size_t to int
# 4820 padding bytes between data members
# 4100 unreferenced formal parameter
add_definitions(-DWINKMFL_EXPORTS -DEKAYA -DYY_NO_UNISTD_H -wd4100 -wd4820 -wd4267)

include_directories(${PROJECT_SOURCE_DIR} 
	${PROJECT_SOURCE_DIR}/../kmfl/kmflcomp/include
	${PROJECT_SOURCE_DIR}/../kmfl/libkmfl/include
	${PROJECT_BINARY_DIR}/include
	${PROJECT_SOURCE_DIR}/../ekaya/src
	${win_iconv_SOURCE_DIR})

add_custom_target(kmfl_headers
	COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/include
	COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/include/kmfl
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/../kmfl/libkmfl/include/libkmfl.h ${PROJECT_BINARY_DIR}/include/kmfl
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/../kmfl/kmflcomp/include/kmfl.h ${PROJECT_BINARY_DIR}/include/kmfl
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/../kmfl/kmflcomp/include/kmflcomp.h ${PROJECT_BINARY_DIR}/include/kmfl
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/../kmfl/kmflcomp/include/kmflutfconv.h ${PROJECT_BINARY_DIR}/include/kmfl
	)

add_library(winkmfl SHARED
	../kmfl/libkmfl/src/kmfl_interpreter.c
	../kmfl/libkmfl/src/kmfl_load_keyboard.c
	../kmfl/libkmfl/src/kmfl_messages.c
	../kmfl/kmflcomp/src/kmflcomp.c
	../kmfl/kmflcomp/src/lex.c
	../kmfl/kmflcomp/src/memman.c
	../kmfl/kmflcomp/src/utfconv.c
	../kmfl/kmflcomp/src/yacc.c
	register_callbacks.c
	)

set_source_files_properties(../kmfl/kmflcomp/src/yacc.c PROPERTIES COMPILE_FLAGS "-wd4127 -wd4702")

add_dependencies(winkmfl iconv kmfl_headers)
target_link_libraries(winkmfl iconv)

# install(TARGETS winkmfl RUNTIME DESTINATION bin LIBRARY DESTINATION lib)
