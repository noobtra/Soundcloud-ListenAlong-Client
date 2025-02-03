#pragma once
#include <atomic>
#include <thread>
#include <memory>
#include <string>

#include <discord.h>

class DiscordManager {
public:
    static DiscordManager& get()
	{
        static DiscordManager instance;
        return instance;
    }

    void initialize();
    void update_activity(const discord::Activity& activity);
    void shutdown();

    ~DiscordManager() { shutdown(); }

private:
    DiscordManager() = default;
    std::unique_ptr<discord::Core> core_;
    std::atomic<bool> running_{ false };
    std::thread callback_thread_;

    void run_callbacks();
};