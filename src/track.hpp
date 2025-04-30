#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace listenalong
{
    struct track
    {
        std::u8string artist;  // UTF-8 string
        std::u8string title;   // UTF-8 string
        double start_time;
        double end_time;
        std::string artwork_url;
        std::string track_url;
        // Deserialize from JSON
        static track from_json(const nlohmann::json& j)
        {
            track track;

            track.artist = std::u8string(reinterpret_cast<const char8_t*>(j["artist"].get<std::string>().c_str()));
            track.title = std::u8string(reinterpret_cast<const char8_t*>(j["title"].get<std::string>().c_str()));

            track.start_time = j["start_time"];
            track.end_time = j["end_time"];
            track.artwork_url = j["artwork_url"];
            track.track_url = j["track_url"];
            return track;
        }

        // Serialize to JSON
        nlohmann::json to_json() const
        {
            return {
                {"type", "WS_SEND_TRACK"},
                {"data", {
                    {"artist", std::string(reinterpret_cast<const char*>(artist.c_str()))},
                    {"title", std::string(reinterpret_cast<const char*>(title.c_str()))},
                    {"start_time", start_time},
                    {"end_time", end_time},
                    {"artwork_url", artwork_url},
                    {"track_url", track_url}
                }}
            };
        }
    };
}