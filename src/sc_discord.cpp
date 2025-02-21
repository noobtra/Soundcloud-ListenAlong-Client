#include "sc_discord.hpp"
#include <stdexcept>
#include <chrono>

#define DISCORD_CLIENT_ID 1205361353188188221

void DiscordManager::initialize()
{
    discord::Core* core{};
    auto result = discord::Core::Create(DISCORD_CLIENT_ID, DiscordCreateFlags_Default, &core);
    core_.reset(core);

    if (!core_ || result != discord::Result::Ok) 
        throw std::runtime_error("Failed to initialize Discord core");

    running_ = true;
    callback_thread_ = std::thread(&DiscordManager::run_callbacks, this);
}

void DiscordManager::update_activity(const discord::Activity& activity)
{
    if (core_) 
    {
        core_->ActivityManager().UpdateActivity(activity, [](discord::Result result) 
            {
            if (result != discord::Result::Ok) 
            {
                // Handle error if needed
            }
            });
    }
}

void DiscordManager::run_callbacks()
{
    while (running_) 
    {
        if (core_) 
            core_->RunCallbacks();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void DiscordManager::shutdown()
{
    running_ = false;
    if (callback_thread_.joinable()) 
        callback_thread_.join();
    
    core_.reset();
}