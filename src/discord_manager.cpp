#include "discord_manager.hpp"
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <ostream>
#include <thread>

namespace listenalong
{
    // Static instance for callbacks
    discord_manager* discord_manager::instance_ = nullptr;

    void discord_manager::initialize()
    {
        if (initialized_)
            return;

        // Set static instance for callbacks
        instance_ = this;

        DiscordEventHandlers handlers = {};
        handlers.ready = ready_callback;
        handlers.disconnected = disconnected_callback;
        handlers.errored = error_callback;
        handlers.joinGame = join_game_callback;
        handlers.spectateGame = spectate_game_callback;
        handlers.joinRequest = join_request_callback;

        Discord_Initialize(DISCORD_CLIENT_ID, &handlers, 1, nullptr);
        initialized_ = true;

        // Wait for connection
        const auto start_time = std::chrono::steady_clock::now();
        constexpr auto timeout = std::chrono::seconds(10);

        while (!connected_)
        {
            auto current_time = std::chrono::steady_clock::now();
            if (current_time - start_time > timeout)
            {
                throw std::runtime_error("Failed to connect to Discord within timeout period");
            }

            // IO thread handles connection updates automatically
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            Discord_RunCallbacks();
        }

        running_ = true;
    }

    void discord_manager::update_activity(const DiscordRichPresence& presence)
    {
        if (initialized_)
        {
            Discord_UpdatePresence(&presence);
            if (!connected_)
            {
                std::cout << "Discord presence updated (Discord connection status unknown)" << std::endl;
            }
        }
        else
        {
            std::cerr << "Cannot update Discord presence: Discord not initialized" << std::endl;
        }
    }

    void discord_manager::clear_activity()
    {
        if (initialized_ && connected_)
        {
            Discord_ClearPresence();
        }
    }

    void discord_manager::run() const
    {
        while (running_)
        {
            if (initialized_)
            {
                // IO thread handles connection, but we need to manually process callbacks
                Discord_RunCallbacks();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    // Static callback implementations
    void discord_manager::ready_callback(const DiscordUser* user)
    {
        if (instance_ && user)
        {
            instance_->current_user_.user_id = user->userId;
            instance_->current_user_.username = user->username;
            instance_->current_user_.discriminator = user->discriminator;
            instance_->current_user_.avatar = user->avatar;
            instance_->connected_ = true;

            std::cout << "Discord connected as " << user->username << "#" << user->discriminator << std::endl;
        }
    }

    void discord_manager::disconnected_callback(int error_code, const char* message)
    {
        if (instance_)
        {
            instance_->connected_ = false;
            std::cerr << "Discord disconnected (" << error_code << "): " << (message ? message : "Unknown error") << std::endl;
        }
    }

    void discord_manager::error_callback(int error_code, const char* message)
    {
        std::cerr << "Discord error (" << error_code << "): " << (message ? message : "Unknown error") << std::endl;

        // Common error codes and their meanings
        switch (error_code)
        {
        case 1000: // DISCORD_RESULT_NOT_RUNNING
            std::cerr << "  -> Discord is not running" << std::endl;
            break;
        case 1001: // DISCORD_RESULT_INVALID_PAYLOAD  
            std::cerr << "  -> Invalid payload sent to Discord" << std::endl;
            break;
        case 1002: // DISCORD_RESULT_INVALID_COMMAND
            std::cerr << "  -> Invalid command sent to Discord" << std::endl;
            break;
        case 1003: // DISCORD_RESULT_INVALID_GUILD
            std::cerr << "  -> Invalid guild ID" << std::endl;
            break;
        case 1004: // DISCORD_RESULT_INVALID_EVENT
            std::cerr << "  -> Invalid event" << std::endl;
            break;
        case 1005: // DISCORD_RESULT_INVALID_CHANNEL
            std::cerr << "  -> Invalid channel ID" << std::endl;
            break;
        default:
            std::cerr << "  -> Check Discord developer console for more details" << std::endl;
            break;
        }
    }

    void discord_manager::join_game_callback(const char* join_secret)
    {
        std::cout << "Discord join game request: " << (join_secret ? join_secret : "No secret") << std::endl;
        // Handle join game logic here
    }

    void discord_manager::spectate_game_callback(const char* spectate_secret)
    {
        std::cout << "Discord spectate game request: " << (spectate_secret ? spectate_secret : "No secret") << std::endl;
        // Handle spectate game logic here
    }

    void discord_manager::join_request_callback(const DiscordUser* request)
    {
        if (request)
        {
            std::cout << "Discord join request from " << request->username << "#" << request->discriminator << std::endl;
            // Handle join request logic here
            // You can call Discord_Respond() to accept/ignore the request
        }
    }
}