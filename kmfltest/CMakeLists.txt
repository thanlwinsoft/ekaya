project(kmfltest)

enable_language(C CXX)

include_directories(
	${PROJECT_BINARY_DIR}/../winkmfl/include 
	${PROJECT_SOURCE_DIR}/../winkmfl)

if (MSVC)
	add_definitions(-wd4710 -wd4548 -wd4571
		-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS -DUNICODE)
endif (MSVC)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
add_custom_target(copy_kmfltest_dlls ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${winkmfl_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}winkmfl${CMAKE_SHARED_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${win_iconv_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}iconv${CMAKE_SHARED_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}
	)
endif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	# find_package(PkgConfig)
	find_library(LIBKMFLCOMP kmflcomp)
	find_library(LIBKMFL kmfl)
endif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

add_executable(kmfltest kmfltest.cpp)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_dependencies(copy_kmfltest_dlls winkmfl iconv)
	add_dependencies(kmfltest winkmfl copy_kmfltest_dlls)
	target_link_libraries(kmfltest winkmfl)
else (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	target_link_libraries(kmfltest kmfl kmflcomp)
endif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")

