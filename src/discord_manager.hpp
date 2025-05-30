#pragma once
#include <atomic>
#include <thread>
#include <memory>
#include <string>
#include <functional>

#include <discord_rpc.h>

#define DISCORD_CLIENT_ID "1205361353188188221"

namespace listenalong
{
    struct discord_user
    {
        std::string user_id;
        std::string username;
        std::string discriminator;
        std::string avatar;
    };

    class discord_manager
    {
    public:
        discord_manager() = default;
        ~discord_manager()
        {
            running_ = false;
            if (initialized_)
            {
                Discord_Shutdown();
            }
        }

        void initialize();
        void run() const;

        discord_user get_user() const { return current_user_; }
		int64_t get_user_id() const
		{
			return std::stoll(current_user_.user_id);
		}
        void update_activity(const DiscordRichPresence& presence);
        void clear_activity();

        bool is_connected() const { return connected_; }
        bool is_initialized() const { return initialized_; }

    private:
        static void ready_callback(const DiscordUser* user);
        static void disconnected_callback(int error_code, const char* message);
        static void error_callback(int error_code, const char* message);
        static void join_game_callback(const char* join_secret);
        static void spectate_game_callback(const char* spectate_secret);
        static void join_request_callback(const DiscordUser* request);

        discord_user current_user_;
        std::atomic<bool> connected_{ false };
        std::atomic<bool> running_{ false };
        std::atomic<bool> initialized_{ false };

        // Static instance for callbacks
        static discord_manager* instance_;
    };
}