#pragma once
#include <string>
#include <unordered_set>
#include <random>
#include <cstdint>
#include "track.hpp"
#include "discord_manager.hpp"

namespace listenalong
{
    class party
    {
    public:

        explicit party(const uint64_t host_id);
        party() = default;

        // General getters
        const std::string& get_id() const { return id_; }
        const std::string& get_secret() const { return secret_; }
		uint64_t get_host_id() const { return host_id_; } // if 0, not a valid party

        // Member management
        bool add_member(const uint64_t user_id) { return members_.insert(user_id).second; }
        bool remove_member(const uint64_t user_id) { return members_.erase(user_id) > 0; }
        bool contains(const uint64_t user_id) const { return members_.contains(user_id); }
        const std::unordered_set<uint64_t>& get_members() const { return members_; }
        int get_member_count() const { return static_cast<int>(members_.size()); }

        // Track management
        track get_track() const { return track_; }
		void set_track(const track& new_track) { track_ = new_track; }
        double get_duration() const { return track_.end_time - track_.start_time; } // Returns the duration of the track in ms

		// Activity management
        discord::Activity get_activity() const;


    private:
        static std::string generate_uuid(std::random_device& rd);

        std::string id_{};
        std::string match_id_{};
        std::string secret_{};

		uint64_t local_id_ = 0;
        uint64_t host_id_ = 0;
        track track_{};
        std::unordered_set<uint64_t> members_{};
        uint8_t max_size_ = 16;
    };
}
