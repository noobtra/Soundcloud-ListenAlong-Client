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

# --- Discord RPC Setup ---
FetchContent_Declare(discord-rpc
  GIT_REPOSITORY https://github.com/Kenny-Hui/discord-rpc.git
  GIT_TAG master
)

# Set discord-rpc options before making it available
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")
set(ENABLE_IO_THREAD ON CACHE BOOL "Start up a separate I/O thread, otherwise you must call Discord_UpdateConnection yourself")

# Make rapidjson available first, then discord-rpc
FetchContent_MakeAvailable(discord-rpc)


# --- Application Target ---
file(GLOB_RECURSE sources src/*.cpp)
add_executable(soundcloud_listenalong_app ${sources})

target_compile_features(soundcloud_listenalong_app PRIVATE cxx_std_20)

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
discord-rpc
)
target_include_directories(soundcloud_listenalong_app PRIVATE
  ${asio_SOURCE_DIR}/asio/include
  ${websocketpp_SOURCE_DIR}
  ${discord-rpc_SOURCE_DIR}/include
  ${rapidjson_SOURCE_DIR}/include
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