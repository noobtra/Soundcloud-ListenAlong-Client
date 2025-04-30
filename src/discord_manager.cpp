#include "discord_manager.hpp"
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <ostream>

#define DISCORD_CLIENT_ID 1205361353188188221

void listenalong::discord_manager::initialize()
{
    discord::Core* core{};
    auto result = discord::Core::Create(DISCORD_CLIENT_ID, DiscordCreateFlags_Default, &core);
    core_.reset(core);

    if (!core_ || result != discord::Result::Ok)
        throw std::runtime_error("Failed to initialize Discord core");

	// Install callback to wait for user to be ready
    core_->UserManager().OnCurrentUserUpdate.Connect([this]
    {
    	core_->UserManager().GetCurrentUser(&current_user_);
		std::cout << "User ID: " << current_user_.GetId() << std::endl;
        running_ = true;
    });

	while (!running_)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		core_->RunCallbacks();
	}
}

void listenalong::discord_manager::update_activity(const discord::Activity& activity)
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

void listenalong::discord_manager::run()
{
    while (running_)
    {
        if (core_)
            core_->RunCallbacks();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}