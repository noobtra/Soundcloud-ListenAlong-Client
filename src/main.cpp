#include <iostream>

#include "sc_discord.hpp"

int run_tests()
{
    try 
    {
        discord::Activity activity{};
        activity.SetDetails("Playing Through the Fire and Flames");
        activity.SetState("By DragonForce");
        activity.GetAssets().SetLargeImage("song_cover");
        activity.GetTimestamps().SetStart(std::time(nullptr));

        DiscordManager::get().update_activity(activity);

        std::cout << "Rich Presence is active. Press Enter to exit...\n";
        std::cin.get();
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

int main() {
    try 
    {
        DiscordManager::get().initialize();
    	run_tests();
    }
    catch (const std::exception& e)
    {
        // Handle initialization errors
        return 1;
    }
	DiscordManager::get().shutdown();
    return 0;
}