#include "party.hpp"
#include <sstream>
#include <iomanip>
#include <format>

/*
discord::Activity listenalong::party::get_activity() const
{
	// Set up the name, state, and details. We have to convert the UTF-8 strings to std::string
	discord::Activity activity;
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

    // Set up assets.
    discord::ActivityAssets& activity_assets = activity.GetAssets();
	activity_assets.SetLargeImage(track_.artwork_url.c_str());
	activity_assets.SetLargeText("SoundCloud");

    // Set up assets.
    strcpy_s(activity_assets.large_image, sizeof(activity_assets.large_image), "soundcloud");
    strcpy_s(activity_assets.large_text, sizeof(activity_assets.large_text), "SoundCloud");
    strcpy_s(activity_assets.small_image, sizeof(activity_assets.small_image), "");
    strcpy_s(activity_assets.small_text, sizeof(activity_assets.small_text), "");

    // Set up party.
    DiscordActivityParty activity_party;
    strcpy_s(activity_party.id, sizeof(activity_party.id), "");
    activity_party.size.current_size = member_count();
    activity_party.size.max_size = max_size_;
    activity_party.privacy = DiscordActivityPartyPrivacy_Private;
    strcpy_s(activity_party.id, sizeof(activity_party.id), id_.c_str());

    // Set up secrets.
    DiscordActivitySecrets activity_secrets;
    strcpy_s(activity_secrets.join, sizeof(activity_secrets.join), "");
    strcpy_s(activity_secrets.join, sizeof(activity_secrets.join), secret_.c_str());

    // Set up the activity.
    DiscordActivity activity;
    activity.type = DiscordActivityType_Listening;
    activity.application_id = DISCORD_CLIENT_ID;
    strcpy_s(activity.name, sizeof(activity.name), "SoundCloud");
    strcpy_s(activity.state, sizeof(activity.state), "In a party");
    strcpy_s(activity.details, sizeof(activity.details), "");
    activity.timestamps.start = track_.start_time;
    activity.timestamps.end = track_.end_time;
    activity.assets = activity_assets;
    activity.party = activity_party;
    activity.secrets = activity_secrets;
    activity.instance = true;
	activity.supported_platforms = 0;

    return activity;
}
*/

std::string listenalong::party::generate_uuid()
{
    std::random_device rd;
    // Combine two 32-bit outputs for a 64-bit value.
    const uint64_t value = (static_cast<uint64_t>(rd()) << 32) | rd();
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << value;
    return ss.str();
}
