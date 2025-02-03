// party_manager.hpp
#pragma once
#include "party.hpp"
#include <memory>
#include <unordered_map>

class PartyManager {
public:
    static PartyManager& get() {
        static PartyManager instance;
        return instance;
    }

    // Main interface
    Party* create_party(const std::string& hostId, const std::string& trackId, int maxSize = 5);
    Party* get_party(const std::string& userId);
    bool leave_party(const std::string& userId);

    // Join functionality
    bool join_party(const std::string& partyId, const std::string& userId);

private:
    PartyManager() = default;

    std::unordered_map<std::string, std::unique_ptr<Party>> parties_;  // Party ID to Party
    std::unordered_map<std::string, std::string> user_to_party_;       // User ID to Party ID
};