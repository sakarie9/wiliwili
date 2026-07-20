//
// Created by fang on 2024/7/17.
//

#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace bilibili {

/// SponsorBlock segment returned by the API
class SponsorBlockSegment {
public:
    std::string UUID;
    std::string cid;
    std::string category;     // "sponsor"
    std::string actionType;   // "skip", "mute", "none"
    double segment[2] = {0, 0};  // [start, end] in seconds
    double videoDuration = 0;
    int locked = 0;
    int votes = 0;
    std::string description;
};

inline void from_json(const nlohmann::json& j, SponsorBlockSegment& s) {
    if (j.contains("UUID") && !j.at("UUID").is_null())
        j.at("UUID").get_to(s.UUID);
    if (j.contains("cid") && !j.at("cid").is_null())
        j.at("cid").get_to(s.cid);
    if (j.contains("category") && !j.at("category").is_null())
        j.at("category").get_to(s.category);
    if (j.contains("actionType") && !j.at("actionType").is_null())
        j.at("actionType").get_to(s.actionType);
    if (j.contains("segment") && j.at("segment").is_array() && j.at("segment").size() >= 2) {
        s.segment[0] = j.at("segment")[0].get<double>();
        s.segment[1] = j.at("segment")[1].get<double>();
    }
    if (j.contains("videoDuration") && !j.at("videoDuration").is_null())
        j.at("videoDuration").get_to(s.videoDuration);
    if (j.contains("locked") && !j.at("locked").is_null())
        j.at("locked").get_to(s.locked);
    if (j.contains("votes") && !j.at("votes").is_null())
        j.at("votes").get_to(s.votes);
    if (j.contains("description") && !j.at("description").is_null())
        j.at("description").get_to(s.description);
}

/// A video entry from the API response, containing videoID and its segments
class SponsorBlockVideoEntry {
public:
    std::string videoID;
    std::vector<SponsorBlockSegment> segments;
};

inline void from_json(const nlohmann::json& j, SponsorBlockVideoEntry& e) {
    if (j.contains("videoID") && !j.at("videoID").is_null())
        j.at("videoID").get_to(e.videoID);
    if (j.contains("segments") && j.at("segments").is_array())
        j.at("segments").get_to(e.segments);
}

typedef std::vector<SponsorBlockSegment> SponsorBlockSegmentList;
typedef std::vector<SponsorBlockVideoEntry> SponsorBlockVideoEntryList;

} // namespace bilibili
