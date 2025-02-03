// party_manager.cpp
#include "party_manager.hpp"

#include <random>
#include <stdexcept>

// Example implementation using C++11 random utilities
std::string Party::generate_uuid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    const char* hex_digits = "0123456789abcdef";
    std::string uuid(36, '-');
    // Generate a UUID-like string (simplified example)
    for (size_t i = 0; i < uuid.size(); ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) continue;
        uuid[i] = hex_digits[dis(gen)];
    }
    return uuid;
}

std::string Party::generate_join_secret() {
    // Implement a secure secret generation mechanism
    return "temp_secret_" + generate_uuid(); // Simplified example
}

Party* PartyManager::create_party(const std::string& hostId, const std::string& trackId, int maxSize)
{
    if (user_to_party_.count(hostId)) 
        return nullptr; // User already in a party

    auto party = std::make_unique<Party>(hostId, trackId, maxSize);
    auto* ptr = party.get();

    user_to_party_[hostId] = party->id();
    parties_[party->id()] = std::move(party);

    return ptr;
}

Party* PartyManager::get_party(const std::string& userId) {
    auto it = user_to_party_.find(userId);
    if (it == user_to_party_.end()) return nullptr;

    auto partyIt = parties_.find(it->second);
    return (partyIt != parties_.end()) ? partyIt->second.get() : nullptr;
}

bool PartyManager::join_party(const std::string& partyId, const std::string& userId) {
    if (user_to_party_.count(userId)) return false; // Already in a party

    auto partyIt = parties_.find(partyId);
    if (partyIt == parties_.end()) return false;

    if (!partyIt->second->add_member(userId)) return false;

    user_to_party_[userId] = partyId;
    return true;
}

bool PartyManager::leave_party(const std::string& userId)
{
    auto party = get_party(userId);
    if (!party) return false;

    user_to_party_.erase(userId);
    if (!party->remove_member(userId)) return false;

    if (userId == party->host_id()) 
    {
        for (const auto& member : party->get_members())
            user_to_party_.erase(member);
        parties_.erase(party->id());
    }
    else if (party->member_count() == 0)
    {
	    parties_.erase(party->id());
    }
    
    return true;
}
