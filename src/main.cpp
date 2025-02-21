#include <iostream>

#include "sc_discord.hpp"
#include "server.hpp"

int main()
{
    DiscordManager discord_manager{};
    SoundCloudServer sc_server{};
    try 
    {
        discord_manager.initialize();
        sc_server.initialize(9002);
    }
    catch (const std::exception& e)
    {
        // Handle initialization errors
        return 1;
    }
    discord_manager.shutdown();
    return 0;
}