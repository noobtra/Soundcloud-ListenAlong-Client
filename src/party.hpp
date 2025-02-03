// party.hpp
#pragma once
#include <string>
#include <unordered_set>
#include <memory>

class Party {
public:
    Party(const std::string& hostId, const std::string& trackId, int maxSize = 5)
        : id_(generate_uuid()),
        secret_(generate_join_secret()),
        host_id_(hostId),
        track_id_(trackId),
        max_size_(maxSize) {
        members_.insert(hostId);
    }

    // Getters
    const std::string& id() const { return id_; }
    const std::string& secret() const { return secret_; }
    const std::string& host_id() const { return host_id_; }
    const std::unordered_set<std::string>& get_members() const { return members_; }

    int member_count() const { return static_cast<int>(members_.size()); }
    int max_size() const { return max_size_; }

    // Member management
    bool add_member(const std::string& userId) {
        if (members_.size() >= max_size_) return false;
        return members_.insert(userId).second;
    }

    bool remove_member(const std::string& userId) {
        return members_.erase(userId) > 0;
    }

    bool contains(const std::string& userId) const {
        return members_.count(userId) > 0;
    }

private:
    std::string generate_uuid();
    std::string generate_join_secret();

    std::string id_;
    std::string secret_;
    std::string host_id_;
    std::string track_id_;
    int max_size_;
    std::unordered_set<std::string> members_;
};