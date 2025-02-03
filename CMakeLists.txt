cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
      "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(soundcloud_listenalong_app)

option(USE_STANDALONE_ASIO "Use standalone Asio" ON)

if(APPLE)
  set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "" FORCE)
endif()

# --- Fetch Dependencies ---
include(FetchContent)

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
add_executable(soundcloud_listenalong_app
  src/main.cpp
  src/server.h
  src/party_manager.hpp
  src/sc_discord.cpp 
  src/party_manager.cpp
)

if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET soundcloud_listenalong_app PROPERTY CXX_STANDARD 20)
endif()

target_compile_definitions(soundcloud_listenalong_app
  PRIVATE
    ASIO_STANDALONE
    _WEBSOCKETPP_CPP11_INTERNAL_
    _WEBSOCKETPP_CPP11_TYPE_TRAITS_
)

target_link_libraries(soundcloud_listenalong_app PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(soundcloud_listenalong_app PRIVATE
  ${asio_SOURCE_DIR}/asio/include
  ${websocketpp_SOURCE_DIR}
)

# --- Discord SDK Setup ---
set(discord_sdk_dir "${CMAKE_SOURCE_DIR}/third_party/discord-sdk")

# (1) Build the C++ Interface from SDK sources (using files in the "include" folder)
set(discord_cpp_dir "${discord_sdk_dir}/include")
file(GLOB discord_sources "${discord_cpp_dir}/*.cpp")
file(GLOB discord_headers "${discord_cpp_dir}/*.h")
add_library(DiscordSDKLibrary SHARED ${discord_sources} ${discord_headers})
if(WIN32)
  # Ensure that export symbols are generated even if __declspec(dllexport) isn't used.
  set_target_properties(DiscordSDKLibrary PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()
add_library(core::DiscordSDKLibrary ALIAS DiscordSDKLibrary)
target_include_directories(DiscordSDKLibrary
  PUBLIC
    $<BUILD_INTERFACE:${discord_cpp_dir}>
    $<INSTALL_INTERFACE:include>
)

# (2) Create an imported target for the binary SDK library
if(WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(DISCORD_LIB_DIR "${discord_sdk_dir}/lib/x86_64")
  else()
    set(DISCORD_LIB_DIR "${discord_sdk_dir}/lib/x86")
  endif()
  set(DISCORD_DLL    "${DISCORD_LIB_DIR}/discord_game_sdk.dll")
  set(DISCORD_IMPLIB "${DISCORD_LIB_DIR}/discord_game_sdk.dll.lib")
elseif(APPLE)
  if("${CMAKE_OSX_ARCHITECTURES}" MATCHES "arm64")
    set(DISCORD_LIB_DIR "${discord_sdk_dir}/lib/aarch64")
  else()
    set(DISCORD_LIB_DIR "${discord_sdk_dir}/lib/x86_64")
  endif()
  set(DISCORD_LIB "${DISCORD_LIB_DIR}/discord_game_sdk.dylib")
elseif(UNIX)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(DISCORD_LIB_DIR "${discord_sdk_dir}/lib/x86_64")
  else()
    set(DISCORD_LIB_DIR "${discord_sdk_dir}/lib/aarch64")
  endif()
  set(DISCORD_LIB "${DISCORD_LIB_DIR}/discord_game_sdk.so")
endif()

add_library(DiscordGameSDKLibrary SHARED IMPORTED)
add_library(core::DiscordGameSDKLibrary ALIAS DiscordGameSDKLibrary)
if(WIN32)
  set_target_properties(DiscordGameSDKLibrary PROPERTIES
    IMPORTED_LOCATION    "${DISCORD_DLL}"
    IMPORTED_IMPLIB      "${DISCORD_IMPLIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${discord_sdk_dir}/include"
  )
else()
  set_target_properties(DiscordGameSDKLibrary PROPERTIES
    IMPORTED_LOCATION    "${DISCORD_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "${discord_sdk_dir}/include"
  )
endif()

# (3) Link the binary SDK into the interface target so that DiscordCreate is resolved.
target_link_libraries(DiscordSDKLibrary PRIVATE core::DiscordGameSDKLibrary)

# (4) Link the Discord interface into your executable.
target_link_libraries(soundcloud_listenalong_app
  PRIVATE
    core::DiscordSDKLibrary
)

# --- Platform-Specific System Libraries ---
if(WIN32)
  target_link_libraries(soundcloud_listenalong_app PRIVATE ws2_32 crypt32)
elseif(APPLE)
  find_library(COCOA_LIBRARY Cocoa)
  target_link_libraries(soundcloud_listenalong_app PRIVATE ${COCOA_LIBRARY})
elseif(UNIX)
  find_package(Threads REQUIRED)
  target_link_libraries(soundcloud_listenalong_app PRIVATE Threads::Threads dl)
endif()

# Post build for windows
if(WIN32)
  add_custom_command(TARGET soundcloud_listenalong_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${DISCORD_LIB_DIR}/discord_game_sdk.dll"
      $<TARGET_FILE_DIR:soundcloud_listenalong_app>)
endif()
