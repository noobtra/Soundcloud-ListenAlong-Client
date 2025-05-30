#include "party.hpp"
#include <sstream>
#include <iomanip>
#include <format>
#include <cstring>

listenalong::party::party(const uint64_t host_id) :
    host_id_(host_id)
{
    std::random_device rd;
    id_ = generate_uuid(rd);
    secret_ = generate_uuid(rd);
    match_id_ = generate_uuid(rd);
    members_.insert(host_id);
}

DiscordRichPresence listenalong::party::get_rich_presence() const
{
    DiscordRichPresence presence {};

    // Convert UTF-8 strings to regular strings
    auto artist_utf8 = std::string(
        reinterpret_cast<const char*>(track_.artist.c_str()),
        track_.artist.size());
    auto title_utf8 = std::string(
        reinterpret_cast<const char*>(track_.title.c_str()),
        track_.title.size());

    // Create details string - need to store it statically since DiscordRichPresence holds pointers
    thread_local std::string details_str;
    thread_local std::string state_str;
    thread_local std::string large_image_key_str;
    thread_local std::string party_id_str;
    thread_local std::string join_secret_str;
    thread_local std::string match_secret_str;

    details_str = title_utf8;
    state_str = artist_utf8;
    large_image_key_str = track_.artwork_url;
    party_id_str = id_;
    join_secret_str = secret_;
    match_secret_str = match_id_;

    // Set basic presence info
    presence.details = details_str.c_str();
    presence.state = state_str.c_str();

    // Set assets
    presence.largeImageKey = large_image_key_str.c_str();
    presence.largeImageText = "SoundCloud";
    presence.smallImageKey = "small_logo";
    presence.smallImageText = "SoundCloud";

    // Set party info
    presence.partyId = party_id_str.c_str();
    presence.partySize = get_member_count();
    presence.partyMax = max_size_;

    // Set secrets for joining
    presence.joinSecret = join_secret_str.c_str();
    presence.matchSecret = match_secret_str.c_str();

    // Set timestamps (discord-rpc expects seconds, not milliseconds)
    presence.startTimestamp = track_.start_time / 1000;
    presence.endTimestamp = track_.end_time / 1000;
    presence.type = 2;

    return presence;
}

std::string listenalong::party::generate_uuid(std::random_device& rd)
{
    // Combine two 32-bit outputs for a 64-bit value.
    const uint64_t value = (static_cast<uint64_t>(rd()) << 32) | rd();
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << value;
    return ss.str();
}