#include "party.hpp"
#include <sstream>
#include <iomanip>
#include <format>

listenalong::party::party(const uint64_t host_id) :
	host_id_(host_id)
{
	std::random_device rd;
	id_ = generate_uuid(rd);
	secret_ = generate_uuid(rd);
	match_id_ = generate_uuid(rd);
	members_.insert(host_id);
}

discord::Activity listenalong::party::get_activity() const
{
	// Set up the name, state, and details. We have to convert the UTF-8 strings to std::string
	discord::Activity activity{};
	activity.SetName("SoundCloud");
	activity.SetState("In a party");
    auto artist_utf8 = std::string(
        reinterpret_cast<const char*>(track_.artist.c_str()),
        track_.artist.size());
    auto title_utf8 = std::string(
        reinterpret_cast<const char*>(track_.title.c_str()),
        track_.title.size());
	std::string details = std::format(
		"Listening to {} - {}", artist_utf8, title_utf8);
    activity.SetDetails(details.c_str());
	activity.SetType(discord::ActivityType::Listening);
	activity.SetInstance(true);

    // Set up assets.
    discord::ActivityAssets& activity_assets = activity.GetAssets();
	activity_assets.SetLargeImage(track_.artwork_url.c_str());
	activity_assets.SetLargeText("SoundCloud");
	activity_assets.SetSmallImage("small_logo");
	activity_assets.SetSmallText("Soundcloud");

    // Set up party.
	discord::ActivityParty& activity_party = activity.GetParty();
	activity_party.SetId(id_.c_str());
	activity_party.GetSize().SetCurrentSize(get_member_count());
	activity_party.GetSize().SetMaxSize(max_size_);
	activity_party.SetPrivacy(discord::ActivityPartyPrivacy::Public);

    // Set up secrets.
	discord::ActivitySecrets& activity_secrets = activity.GetSecrets();
	activity_secrets.SetMatch(match_id_.c_str());
	activity_secrets.SetJoin(secret_.c_str());

	// Set up timestamps.
	discord::ActivityTimestamps& activity_timestamps = activity.GetTimestamps();
    activity_timestamps.SetStart(track_.start_time);
	activity_timestamps.SetEnd(track_.end_time);

    return activity;
}


std::string listenalong::party::generate_uuid(std::random_device& rd)
{
    // Combine two 32-bit outputs for a 64-bit value.
    const uint64_t value = (static_cast<uint64_t>(rd()) << 32) | rd();
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << value;
    return ss.str();
}
