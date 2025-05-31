# SoundCloud Listen Along - Desktop Application

A cross-platform C++ desktop application that integrates SoundCloud playback with Discord Rich Presence, enabling synchronized group listening experiences. Originally designed to work with Discord's listen-along features before discovering their closed-source limitations.

## ⚠️ Project Status

This project has been **discontinued** due to Discord's activity sync and listen-along features being strictly closed source, making it impossible to interact with Discord's RPC to emulate the desired functionality.

## Features

- **Discord Rich Presence Integration**: Real-time Discord activity updates showing current SoundCloud track
- **WebSocket Communication**: Bidirectional communication with browser extension
- **Party System**: Group listening with host/member management 
- **Cross-Platform Support**: Windows, macOS (Intel/Apple Silicon), and Linux
- **Sync Server Ready**: Infrastructure for multi-user synchronization (incomplete)
- **Modern C++20**: Leverages latest C++ features and best practices

## Related Components

This desktop application works in conjunction with a browser extension:

**🌐 Browser Extension**: [SoundCloud Listen Along - Chrome Extension](https://github.com/noobtra/Soundcloud-ListenAlong-Extension)

The extension monitors SoundCloud playback and sends track information to this desktop app via WebSocket. **Both components are required** for the system to function.

## Architecture

```
┌─────────────────────┐    ┌──────────────────────┐    ┌─────────────────────┐
│   Browser Extension │◄──►│   Desktop App Core   │◄──►│   Discord Client    │
│   (Port 9005)       │    │                      │    │   (Rich Presence)   │
└─────────────────────┘    └──────────────────────┘    └─────────────────────┘
                                       │
                                       ▼
                           ┌──────────────────────┐
                           │   Sync Server        │
                           │ (wss://listenalong.tv)│
                           └──────────────────────┘
```

## Project Structure

```
├── src/
│   ├── main.cpp              # Application entry point
│   ├── core.{cpp,hpp}        # Main application logic & WebSocket handling
│   ├── discord_manager.{cpp,hpp}  # Discord RPC integration
│   ├── party.{cpp,hpp}       # Group listening party management
│   └── track.hpp             # Track data structure & JSON serialization
├── CMakeLists.txt            # Build configuration
└── README.md                 # This file
```

## Dependencies

### Core Libraries
- **Discord RPC** - Discord Rich Presence integration
- **WebSocketPP** - WebSocket server/client implementation  
- **nlohmann/json** - JSON parsing and serialization
- **Asio** - Asynchronous I/O operations
- **OpenSSL** - TLS/SSL support for secure WebSocket connections

### Platform Libraries
- **Windows**: `ws2_32`, `crypt32`
- **macOS**: `Cocoa` framework
- **Linux**: `pthread`, `dl`

All dependencies are automatically fetched via CMake's FetchContent.

## Building

### Prerequisites

- **CMake 3.15+**
- **C++20 compatible compiler**:
  - Visual Studio 2019+ (Windows)
  - Xcode 12+ (macOS) 
  - GCC 10+ or Clang 10+ (Linux)
- **OpenSSL** (system-installed)

### Windows (Visual Studio)

```bash
# Clone repository
git clone <repository-url>
cd soundcloud-listenalong-desktop

# Configure and build
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

### macOS

```bash
# Install OpenSSL via Homebrew
brew install openssl

# Configure and build
mkdir build && cd build
cmake .. -DOPENSSL_ROOT_DIR=/opt/homebrew
make -j$(sysctl -n hw.ncpu)
```

### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt update
sudo apt install build-essential cmake libssl-dev

# Configure and build  
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Build Options

- `USE_STANDALONE_ASIO=ON` - Use standalone Asio instead of Boost.Asio (default)
- `CMAKE_BUILD_TYPE=Release` - Build optimized release version

## Usage

### Basic Operation

1. **Start the application**:
   ```bash
   ./soundcloud_listenalong_app
   ```

2. **Install and configure** the browser extension

3. **Open Discord** - The app will connect automatically

4. **Play SoundCloud tracks** - Rich presence updates automatically

### Console Output

The application provides detailed logging:

```
Soundcloud Manager and Discord Manager initialized
Discord connected as YourUsername#1234
Connected to extension
Attempting to connect to sync server...
```

### Discord Rich Presence

When tracks are playing, Discord will show:

- **Details**: Track title
- **State**: Artist name
- **Large Image**: Track artwork (high resolution)
- **Timestamps**: Real-time progress bar
- **Party Info**: Listen-along party details

## API Reference

### WebSocket Messages (Port 9005)

#### Incoming (Extension → Desktop)
```json
{
  "type": "WS_SEND_TRACK",
  "data": {
    "artist": "Artist Name",
    "title": "Track Title", 
    "start_time": 1640995200000,
    "end_time": 1640995380000,
    "artwork_url": "https://...",
    "track_url": "https://soundcloud.com/..."
  }
}
```

#### Outgoing (Desktop → Extension)
```json
{
  "type": "WS_PLAY_TRACK",
  "data": {
    "trackUrl": "https://soundcloud.com/track-url"
  }
}
```

### Core Classes

#### `listenalong::core`
Main application coordinator managing all subsystems.

**Key Methods:**
- `initialize()` - Start Discord connection and WebSocket servers
- `run()` - Main application loop  
- `send_play_request(url)` - Request extension to play specific track

#### `listenalong::discord_manager`
Discord RPC integration with automatic connection handling.

**Key Methods:**
- `initialize()` - Connect to Discord with 10-second timeout
- `update_activity(presence)` - Update Discord Rich Presence
- `get_user_id()` - Get current Discord user ID

#### `listenalong::party`
Group listening session management.

**Key Methods:**
- `add_member(user_id)` - Add user to listening party
- `set_track(track)` - Update current playing track
- `get_rich_presence()` - Generate Discord presence data

#### `listenalong::track`
Track data structure with JSON serialization.

**Key Features:**
- UTF-8 string support for international characters
- Automatic artwork URL optimization (large.jpg → t500x500.jpg)
- Unix timestamp precision for synchronization

## Configuration

### Discord Application Setup

The application uses Discord Client ID: `1205361353188188221`

To use your own Discord application:
1. Create application at [Discord Developer Portal](https://discord.com/developers/applications)
2. Update `DISCORD_CLIENT_ID` in `discord_manager.hpp`
3. Configure Rich Presence assets (optional)

### Network Configuration

- **Extension Server**: `localhost:9005` (WebSocket)
- **Sync Server**: `wss://ws.listenalong.tv` (WebSocket over TLS)

## Development

### Code Style

- **Modern C++20** with RAII and smart pointers
- **Thread-safe** operations using atomics
- **Exception safety** with proper cleanup
- **Cross-platform** compatibility considerations

### Threading Model

```
Main Thread
├── Discord Manager (Discord_RunCallbacks loop)
├── Extension Server Thread (WebSocket server)
└── Sync Client Thread (WebSocket client with reconnection)
```

### Debugging

**Enable verbose logging** by modifying console output in:
- `discord_manager.cpp` - Discord connection events
- `core.cpp` - WebSocket message handling
- Build in Debug mode for additional assertions

### Extension Points

The codebase includes placeholders for future features:

- `send_sync_message()` - Multi-user track synchronization
- `receive_sync_message()` - Handle sync server commands  
- Join/spectate callbacks - Discord party integration
- Custom artwork processing - Local image caching

## Known Limitations

- **Discord Limitations**: Cannot access Discord's closed-source listen-along features
- **SoundCloud Dependencies**: Relies on extension for track detection
- **Single Party Host**: Only party host can control playback
- **No Persistence**: Party state lost on application restart
- **Network Dependencies**: Requires stable WebSocket connections

## Troubleshooting

### Discord Connection Issues

```
Error: Failed to connect to Discord within timeout period
```
**Solutions:**
- Ensure Discord is running before starting the application
- Check Discord isn't in administrator mode (Windows)
- Verify Discord Client ID is valid

### WebSocket Connection Issues

```
Failed to start extension server: Address already in use
```
**Solutions:**
- Kill existing instances: `pkill -f soundcloud_listenalong_app`
- Change port in `core.cpp` if 9005 is occupied
- Check firewall settings

### Build Issues

**Windows - Missing OpenSSL:**
```bash
# Install via vcpkg
vcpkg install openssl
cmake .. -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
```

**macOS - Architecture Issues:**
```bash
# Force specific architecture
cmake .. -DCMAKE_OSX_ARCHITECTURES=arm64  # or x86_64
```

**Linux - Missing Dependencies:**
```bash
# Install development packages
sudo apt install libssl-dev build-essential cmake
```

## Performance Notes

- **Memory Usage**: ~15-30MB typical usage
- **CPU Usage**: Minimal when idle, brief spikes during track changes
- **Network**: Low bandwidth WebSocket communication only
- **Discord Updates**: 60Hz callback processing for smooth presence updates

## License

MIT License

Copyright (c) 2025 noobtra

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## Contributing

This project is discontinued, but the code serves as a reference for:
- Discord RPC integration patterns
- Cross-platform WebSocket applications
- Modern C++ project structure
- CMake dependency management
- Real-time audio application synchronization

---

**Note**: This application demonstrates advanced real-time communication patterns and should be used responsibly in accordance with Discord's and SoundCloud's Terms of Service.