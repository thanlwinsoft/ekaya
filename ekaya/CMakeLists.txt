CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0 FATAL_ERROR)

project(ekaya)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/../winkmfl
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
add_definitions(-wd4820 -wd4668 -wd4548 -wd4710 -wd4365 -wd4917 -wd4263 -wd4264 -wd4061 -wd4571)

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
	src/UtfConversion.cpp)

set_target_properties(ekaya PROPERTIES LINK_FLAGS ${CMAKE_LINK_DEF_FILE_FLAG}${PROJECT_SOURCE_DIR}/src/ekaya.def)

add_dependencies(ekaya winkmfl iconv)
target_link_libraries(ekaya winkmfl gdiplus)

install(TARGETS ekaya LIBRARY DESTINATION lib RUNTIME DESTINATION lib)

if (${CMAKE_CFG_INTDIR} STREQUAL ".")
	file(TO_NATIVE_PATH "${PROJECT_BINARY_DIR}" EKAYA_BINARY_DIR)
	file(TO_NATIVE_PATH "${winkmfl_BINARY_DIR}" WINKMFL_BINARY_DIR)
	file(TO_NATIVE_PATH "${win_iconv_BINARY_DIR}" ICONV_BINARY_DIR)
else (${CMAKE_CFG_INTDIR} STREQUAL ".")
	file(TO_NATIVE_PATH "${PROJECT_BINARY_DIR}/${CMAKE_CFG_INTDIR}" EKAYA_BINARY_DIR)
	file(TO_NATIVE_PATH "${winkmfl_BINARY_DIR}/${CMAKE_CFG_INTDIR}" WINKMFL_BINARY_DIR)
	file(TO_NATIVE_PATH "${win_iconv_BINARY_DIR}/${CMAKE_CFG_INTDIR}" ICONV_BINARY_DIR)
endif (${CMAKE_CFG_INTDIR} STREQUAL ".")

if (${CMAKE_CL_64})
	set(TARGET_ARCH "x86_64")
else (${CMAKE_CL_64}) 
	set(TARGET_ARCH "x86")
endif (${CMAKE_CL_64})

# disable installer target for debug to try to prevent accidental use with debug configuration
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} Installer target disabled")
else (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	add_custom_target(installer 
		COMMAND ${NSIS_COMPILER} "-DEKAYA_BINARY_DIR=${EKAYA_BINARY_DIR}"
			"-DWINKMFL_BINARY_DIR=${WINKMFL_BINARY_DIR}"
			"-DICONV_BINARY_DIR=${ICONV_BINARY_DIR}"
			"-DARCH=${TARGET_ARCH}"
			${PROJECT_SOURCE_DIR}/ekaya.nsi
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMENT "NSIS installer")
endif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
