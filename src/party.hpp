// party.hpp
#pragma once
#include <string>
#include <unordered_set>
#include <memory>

class Party {
public:
    Party(const std::string& host_id, const std::string& track_id, int max_size = 5)
        : id_(generate_uuid()),
        secret_(generate_join_secret()),
        host_id_(host_id),
        track_id_(track_id),
        max_size_(max_size) {
        members_.insert(host_id);
    }

    // Getters
    [[nodiscard]] const std::string& id() const { return id_; }
    [[nodiscard]] const std::string& secret() const { return secret_; }
    [[nodiscard]] const std::string& host_id() const { return host_id_; }
    [[nodiscard]] const std::unordered_set<std::string>& get_members() const { return members_; }

    [[nodiscard]] int member_count() const { return static_cast<int>(members_.size()); }
    [[nodiscard]] int max_size() const { return max_size_; }

    // Member management
    bool add_member(const std::string& user_id)
	{
        if (members_.size() >= max_size_) 
            return false;

        return members_.insert(user_id).second;
    }

    bool remove_member(const std::string& user_id) { return members_.erase(user_id) > 0; }

    bool contains(const std::string& user_id) const { return members_.contains(user_id); }

private:
    static std::string generate_uuid();
    std::string generate_join_secret();

    std::string id_;
    std::string secret_;
    std::string host_id_;
    std::string track_id_;
    int max_size_;
    std::unordered_set<std::string> members_;
};