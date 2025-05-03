#pragma once
#include <atomic>
#include <thread>
#include <memory>
#include <string>

#include <discord.h>

namespace listenalong
{
    class discord_manager
    {
    public:
        discord_manager() = default;
        ~discord_manager()
    	{
            running_ = false;
            core_.reset();
        }

        void initialize();
        void run() const;

		discord::User get_user() const { return current_user_; }
        void update_activity(const discord::Activity& activity);

    private:
        discord::User current_user_;
        std::unique_ptr<discord::Core> core_;
		std::atomic<bool> connected_{ false };
        std::atomic<bool> running_{ false };
    };
}