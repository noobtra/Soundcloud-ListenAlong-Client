#include <iostream>
#include "core.hpp"

int main()
{
    auto core = std::make_unique<listenalong::core>();
    try
    {
		// Initialize listenalong core, pass discord client as a dependency
        core->initialize();
        std::cout << "Soundcloud Manager and Discord Manager initialized" << std::endl;
        core->run();

        // Explicitly clean up before returning
        core.reset();
    }
    catch (const std::exception& e)
    {
        // Handle initialization errors
        std::cout << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
