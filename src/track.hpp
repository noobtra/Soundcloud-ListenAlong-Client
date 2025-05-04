#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace listenalong
{
    struct track
    {
        std::u8string artist;
        std::u8string title;
        int64_t start_time;
        int64_t end_time;
        std::string artwork_url;
        std::string track_url;

        // Deserialize from JSON
        static track from_json(const nlohmann::json& j)
        {
            track track;

            track.artist = std::u8string(reinterpret_cast<const char8_t*>(j["artist"].get<std::string>().c_str()));
            track.title = std::u8string(reinterpret_cast<const char8_t*>(j["title"].get<std::string>().c_str()));

            // set timestamps
            track.start_time = j["start_time"];
            track.end_time = j["end_time"];

            // hack to fix artwork_url
            track.artwork_url = j["artwork_url"];
            track.artwork_url = j["artwork_url"].get<std::string>();
            size_t pos = track.artwork_url.find("large.jpg");
            if (pos != std::string::npos) 
                track.artwork_url.replace(pos, 9, "t500x500.jpg");

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