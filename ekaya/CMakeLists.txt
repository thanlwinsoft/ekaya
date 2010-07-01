CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0 FATAL_ERROR)

project(ekaya)
enable_language(C CXX RC)

include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/../winkmfl
	${PROJECT_BINARY_DIR}/../winkmfl/include)

find_program(NSIS_COMPILER makensis PATHS $ENV{ProgramFiles}/NSIS)

# 4548 expression before , has no effect
# 4820 padding bytes between data members
# 4668 not defined as preprocessor macro - found in windows headers
# 4710 function not inlined
# 4365 signed/unsigned mismatch
# 4917 GUID can only be associated with class, interface or namespace - in msctf.h
# 4264 virtual member function from base  function is hidden
# 4263 member function does not override any base class virtual member function
# 4061 enumerator in switch of enum is not explicitly handled by a case label
# 4571 semantics changed since Visual C++ 7.1 (SEH) are no longer caught
# 4711 selected for automatic inline expansion

file(GLOB EKAYA_HEADERS ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB EKAYA_CLASSES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# This works around a problem where the output path is bad for NMake
# also all the CXX defines get appended unnecessarily
#add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/ekaya.res
#	COMMAND ${CMAKE_RC_COMPILER} /fo${PROJECT_BINARY_DIR}/ekaya.res ${PROJECT_SOURCE_DIR}/src/ekaya.rc
#	DEPENDS ${PROJECT_SOURCE_DIR}/src/ekaya.rc ${PROJECT_SOURCE_DIR}/src/resource.h
#	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_library(ekaya MODULE
	src/DllMain.cpp
	src/Ekaya.cpp
	src/EkayaEditSession.cpp
	src/EkayaInputProcessor.cpp
	src/EkayaLangBarButton.cpp
	src/KmflKeyboard.cpp
	src/KmflKeyboardFactory.cpp
	src/MessageLogger.cpp
	src/Register.cpp
	src/Server.cpp
	src/UtfConversion.cpp
	${EKAYA_HEADERS}
	${PROJECT_SOURCE_DIR}/src/ekaya.def 
	${PROJECT_SOURCE_DIR}/src/ekaya.rc
	)

add_executable(ekaya_test test/ekayaTest.cpp src/Ekaya.cpp)

add_custom_target(copy_dlls
	COMMAND ${CMAKE_COMMAND} -E make_directory ${winkmfl_BINARY_DIR}/${CMAKE_CFG_INTDIR}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${winkmfl_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}winkmfl${CMAKE_SHARED_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${win_iconv_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}iconv${CMAKE_SHARED_LIBRARY_SUFFIX} ${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}
	)


set_source_files_properties(${PROJECT_SOURCE_DIR}/src/ekaya.def 
	PROPERTIES HEADER_FILE_ONLY true)

set_source_files_properties(${PROJECT_SOURCE_DIR}/src/ekaya.rc
	PROPERTIES COMPILE_FLAGS "-I${PROJECT_SOURCE_DIR}")
# add compile definitions only to .cpp files
set_source_files_properties(${EKAYA_CLASSES} PROPERTIES
    COMPILE_FLAGS
	"-wd4820 -wd4668 -wd4548 -wd4710 -wd4365 -wd4917 -wd4263 -wd4264 -wd4061 -wd4571 -wd4711 -Wall -wd4820 -wd4668"
	COMPILE_DEFINITIONS "_SCL_SECURE_NO_WARNINGS;_CRT_SECURE_NO_WARNINGS;UNICODE"
	)

set_target_properties(ekaya PROPERTIES LINK_FLAGS ${CMAKE_LINK_DEF_FILE_FLAG}${PROJECT_SOURCE_DIR}/src/ekaya.def)

add_dependencies(ekaya winkmfl iconv)
target_link_libraries(ekaya winkmfl gdiplus)

add_dependencies(copy_dlls winkmfl iconv)
add_dependencies(ekaya_test ekaya copy_dlls)
target_link_libraries(ekaya_test)

add_test(NAME ekaya_test_dll COMMAND $<TARGET_FILE:ekaya_test>)

# install(TARGETS ekaya LIBRARY DESTINATION lib RUNTIME DESTINATION lib)

if (${CMAKE_CFG_INTDIR} STREQUAL ".")
	file(TO_NATIVE_PATH "${PROJECT_BINARY_DIR}" EKAYA_BINARY_DIR)
	file(TO_NATIVE_PATH "${winkmfl_BINARY_DIR}" WINKMFL_BINARY_DIR)
	file(TO_NATIVE_PATH "${win_iconv_BINARY_DIR}" ICONV_BINARY_DIR)
	file(TO_NATIVE_PATH "${kmfltest_BINARY_DIR}" KMFLTEST_BINARY_DIR)
else (${CMAKE_CFG_INTDIR} STREQUAL ".")
	file(TO_NATIVE_PATH "${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}" EKAYA_BINARY_DIR)
	file(TO_NATIVE_PATH "${winkmfl_BINARY_DIR}/${CMAKE_CFG_INTDIR}" WINKMFL_BINARY_DIR)
	file(TO_NATIVE_PATH "${win_iconv_BINARY_DIR}/${CMAKE_CFG_INTDIR}" ICONV_BINARY_DIR)
	file(TO_NATIVE_PATH "${kmfltest_BINARY_DIR}/${CMAKE_CFG_INTDIR}" KMFLTEST_BINARY_DIR)
endif (${CMAKE_CFG_INTDIR} STREQUAL ".")

if (${CMAKE_CL_64})
	set(TARGET_ARCH "x86_64")
else (${CMAKE_CL_64}) 
	set(TARGET_ARCH "x86")
endif (${CMAKE_CL_64})

# disable installer target for debug to try to prevent accidental use with debug configuration
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} Installer target disabled")
else ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_custom_target(installer 
		COMMAND ${NSIS_COMPILER} "-DEKAYA_BINARY_DIR=${EKAYA_BINARY_DIR}"
			"-DWINKMFL_BINARY_DIR=${WINKMFL_BINARY_DIR}"
			"-DICONV_BINARY_DIR=${ICONV_BINARY_DIR}"
			"-DKMFLTEST_BINARY_DIR=${KMFLTEST_BINARY_DIR}"
			"-DARCH=${TARGET_ARCH}"
			${PROJECT_SOURCE_DIR}/ekaya.nsi
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMENT "NSIS installer")
endif ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
