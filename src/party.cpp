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

    // Set basic presence info
    presence.details = title_utf8.c_str();
    presence.state = artist_utf8.c_str();

    // Set assets
    presence.largeImageKey = track_.artwork_url.c_str();
    presence.largeImageText = "SoundCloud";
    presence.smallImageKey = "small_logo";
    presence.smallImageText = "SoundCloud";

    // Set party info
    presence.partyId = id_.c_str();
    presence.partySize = get_member_count();
    presence.partyMax = max_size_;

    // Set secrets for joining
    presence.joinSecret = secret_.c_str();
    presence.matchSecret = match_id_.c_str();

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