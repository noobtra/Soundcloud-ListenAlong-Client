#pragma once
#include "discordpp.h"
#include <atomic>
#include <thread>
#include <memory>
#include <string>

#define DISCORD_CLIENT_ID 1205361353188188221

namespace listenalong
{
    class discord_manager
    {
    public:
        discord_manager() = default;
        ~discord_manager()
    	{
            running_ = false;
            client_.reset();
        }

        void initialize();
		discordpp::UserHandle get_user() const { return client_->GetCurrentUser(); }
        void run() const;

    private:
        std::unique_ptr<discordpp::Client> client_;
		std::atomic<bool> connected_{ false };
        std::atomic<bool> running_{ false };
    };
}