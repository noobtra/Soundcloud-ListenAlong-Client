#include "discord_manager.hpp"
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <ostream>

void listenalong::discord_manager::initialize()
{
    discord::Core* core{};
    auto result = discord::Core::Create(DISCORD_CLIENT_ID, DiscordCreateFlags_Default, &core);
    core_.reset(core);

    if (!core_ || result != discord::Result::Ok)
        throw std::runtime_error("Failed to initialize Discord core");

	
    core_->UserManager().OnCurrentUserUpdate.Connect([this]
    {
    	core_->UserManager().GetCurrentUser(&current_user_);
		connected_ = true;
    });

	while (!connected_)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		core_->RunCallbacks();
	}

    running_ = true;
}

void listenalong::discord_manager::update_activity(const discord::Activity& activity)
{
    if (core_)
    {
        core_->ActivityManager().UpdateActivity(activity, [](discord::Result result)
            {
                if (result != discord::Result::Ok)
                {
					std::cerr << "Failed to update activity: " << static_cast<int>(result) << std::endl;
                }
            });
    }
}

void listenalong::discord_manager::run() const
{
    while (running_)
    {
        if (core_)
            core_->RunCallbacks();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}
