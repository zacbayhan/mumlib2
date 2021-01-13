#
# CMake/Global Settings
#
cmake_minimum_required(VERSION 3.13)
project(mumlib)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CMakePackageConfigHelpers)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED TRUE)

#
# CMake/Standalone Project Check
#
set(MUMLIB_STANDALONE OFF)
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(MUMLIB_STANDALONE ON)
endif()

#
# Option/Build Shared Libs
#
if(MUMLIB_STANDALONE AND NOT DEFINED BUILD_SHARED_LIBS)
	set(BUILD_SHARED_LIBS ON)
endif()
option(MUMLIB_BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" ${BUILD_SHARED_LIBS})
if(MUMLIB_BUILD_SHARED_LIBS)
	set(MUMLIB_LIBRARY_TYPE SHARED)
else()
	set(MUMLIB_LIBRARY_TYPE STATIC)
endif()

#
# Dependencies
#
find_package(Boost REQUIRED COMPONENTS system)
find_package(Protobuf REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Opus CONFIG REQUIRED)

#
# Library
#
add_library(mumlib ${MUMLIB_LIBRARY_TYPE})

# Library/Protobuf
protobuf_generate_cpp(MUMLIB_SOURCES_PROTO MUMLIB_HEADERS_PROTO "src/Mumble.proto")

# Library/Sources
set(MUMLIB_SOURCES
    src/mumlib.cpp

    src/Audio.cpp
    src/Callback.cpp
    src/CryptState.cpp
    src/Logger.cpp
    src/Transport.cpp
    src/VarInt.cpp
)

set(MUMLIB_HEADERS
    include/mumlib.hpp 
    include/mumlib/Callback.hpp
    include/mumlib/Constants.hpp
    include/mumlib/Enums.hpp
    include/mumlib/Logger.hpp
    
    include/mumlib_private/Audio.hpp
    include/mumlib_private/CryptState.hpp
    include/mumlib_private/Transport.hpp
    include/mumlib_private/VarInt.hpp
)

target_sources(mumlib PRIVATE 
    ${MUMLIB_SOURCES}
    ${MUMLIB_SOURCES_PROTO}
    ${MUMLIB_HEADERS}
    ${MUMLIB_HEADERS_PROTO}
)

# Library/flags
if(MSVC)
    target_compile_options(mumlib PUBLIC /wd4244 /wd4251 /wd6001 /wd6031 /wd6255 /wd6258 /wd6385 /wd6386 /wd6387 /wd26439 /wd26451 /wd26495 /wd26812 /wd26819)
endif()

# Library/Defines
target_compile_definitions(mumlib PUBLIC _USE_MATH_DEFINES)
if(WIN32)
    target_compile_definitions(mumlib PUBLIC _WIN32_WINNT=0x0601)
endif()
if(MSVC)
    target_compile_definitions(mumlib PUBLIC _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(mumlib PUBLIC _SILENCE_ALL_CXX20_DEPRECATION_WARNINGS)
endif()

# Library/IncludeDirs
target_include_directories(mumlib 
	PUBLIC
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
		"$<INSTALL_INTERFACE:include>"
)
target_include_directories(mumlib PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

# Library/Linkage
target_link_libraries(mumlib PRIVATE Boost::boost Boost::system)
target_link_libraries(mumlib PRIVATE OpenSSL::SSL)
target_link_libraries(mumlib PRIVATE Opus::opus)
target_link_libraries(mumlib PRIVATE protobuf::libprotobuf)

#Library/Properties
if(MSVC)
    set_target_properties(mumlib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Library/Package Config
configure_package_config_file(
    cmake/mumlib-config.cmake.in 
    mumlib-config.cmake
	INSTALL_DESTINATION "lib/cmake/mumlib")

#
# Example
#
add_executable(mumlib_example)
target_sources(mumlib_example PRIVATE "src_example/mumlib_example.cpp")
target_link_libraries(mumlib_example PRIVATE mumlib)

#
# Install
#
install(
    TARGETS mumlib
    EXPORT mumlib
    RUNTIME DESTINATION "bin"
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
)
install(
    EXPORT mumlib 
    NAMESPACE mumlib:: 
    FILE mumlib-targets.cmake
    DESTINATION "lib/cmake/mumlib"
)
install(
    DIRECTORY "include/"
    DESTINATION "include"
)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/mumlib-config.cmake"
    DESTINATION "lib/cmake/mumlib"
)
