cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
      "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(soundcloud_listenalong_app)

if(MSVC)
  add_compile_options(/bigobj)
endif()

add_compile_options(-std=c++20)
option(USE_STANDALONE_ASIO "Use standalone Asio" ON)

if(APPLE)
  # Detect host architecture automatically
  execute_process(COMMAND uname -m OUTPUT_VARIABLE HOST_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)

  # Set architecture based on host system
  if(HOST_ARCH STREQUAL "arm64")
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architecture for macOS" FORCE)
  else()
    set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architecture for macOS" FORCE)
  endif()

  message(STATUS "Building for macOS architecture: ${CMAKE_OSX_ARCHITECTURES}")
endif()

# --- Fetch Dependencies ---
include(FetchContent)
find_package(OpenSSL REQUIRED)

if(USE_STANDALONE_ASIO)
  FetchContent_Declare(asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-28-0
  )
  FetchContent_MakeAvailable(asio)
  add_definitions(-DASIO_STANDALONE)
else()
  find_package(Boost REQUIRED COMPONENTS system)
endif()

FetchContent_Declare(websocketpp
  GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
  GIT_TAG 0.8.2
)
FetchContent_MakeAvailable(websocketpp)

FetchContent_Declare(nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# --- Application Target ---
file(GLOB_RECURSE sources src/*.cpp)
add_executable(soundcloud_listenalong_app ${sources})

if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_target_properties(soundcloud_listenalong_app PROPERTIES
          CXX_STANDARD 20
          CXX_STANDARD_REQUIRED ON
  )
endif()

target_compile_definitions(soundcloud_listenalong_app
  PRIVATE
    ASIO_STANDALONE
    _WEBSOCKETPP_CPP11_INTERNAL_
    _WEBSOCKETPP_CPP11_TYPE_TRAITS_
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
)

target_link_libraries(soundcloud_listenalong_app PRIVATE 
nlohmann_json::nlohmann_json
OpenSSL::SSL OpenSSL::Crypto
)
target_include_directories(soundcloud_listenalong_app PRIVATE
  ${asio_SOURCE_DIR}/asio/include
  ${websocketpp_SOURCE_DIR}
)

# Define some handy Social SDK variables
set(DISCORD_SDK_ROOT "${CMAKE_SOURCE_DIR}/lib/discord_social_sdk")
set(DISCORD_SDK_LIB_DIR "${DISCORD_SDK_ROOT}/lib/release")
set(DISCORD_SDK_BIN_DIR "${DISCORD_SDK_ROOT}/bin/release")
set(DISCORD_SDK_INCLUDE_DIR "${DISCORD_SDK_ROOT}/include")

# Include for Social SDK headers
target_include_directories(soundcloud_listenalong_app PRIVATE ${DISCORD_SDK_INCLUDE_DIR})

# Platform-specific Social SDK library paths
if(WIN32)
    set(DISCORD_LIB_PATH "${DISCORD_SDK_LIB_DIR}/discord_partner_sdk.lib")
    set(DISCORD_SHARED_LIB "${DISCORD_SDK_BIN_DIR}/discord_partner_sdk.dll")
elseif(APPLE)
    set(DISCORD_LIB_PATH "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.dylib")
    set(DISCORD_SHARED_LIB "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.dylib")
else() # Linux
    set(DISCORD_LIB_PATH "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.so")
    set(DISCORD_SHARED_LIB "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.so")
endif()

# Link the Social SDK library
target_link_libraries(soundcloud_listenalong_app PRIVATE ${DISCORD_LIB_PATH})

# Set the runtime search path (RPATH) for Linux and macOS
if(UNIX)
    # Use RPATH when building
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    # Set the RPATH to use the lib directory relative to the executable
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
    if(APPLE)
        set(CMAKE_INSTALL_RPATH "@executable_path")
    endif()
endif()

# Copy Social SDK shared library to output directory, so it's available at runtime.
add_custom_command(TARGET soundcloud_listenalong_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${DISCORD_SHARED_LIB}"
    $<TARGET_FILE_DIR:soundcloud_listenalong_app>
)