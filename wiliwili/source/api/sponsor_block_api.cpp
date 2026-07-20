//
// Created by fang on 2024/7/17.
//

#include <nlohmann/json.hpp>

#include <borealis/core/logger.hpp>

#include "bilibili.h"
#include "bilibili/api.h"
#include "bilibili/util/http.hpp"
#include "bilibili/result/sponsor_block_result.h"

namespace bilibili {

void BilibiliClient::get_sponsor_block_segments(const std::string& bvid,
                                                 const std::function<void(SponsorBlockSegmentList)>& callback,
                                                 const ErrorCallback& error) {
    auto session = HTTP::createSession();
    session->SetUrl(cpr::Url{HTTP::PROTOCOL + Api::SponsorBlockSkipSegments});
    session->SetParameters(cpr::Parameters({
        {"videoID", bvid},
        {"categories", "[\"sponsor\"]"},
    }));
    session->SetHeader(cpr::Header{
        {"User-Agent", "wiliwili"},
        {"Origin", "wiliwili"},
    });
    session->SetTimeout(cpr::Timeout{5000});

    brls::Logger::debug("[SponsorBlock] requesting segments for bvid={}", bvid);

    session->GetCallback<>(
        [callback, error, bvid](const cpr::Response& r) {
            if (r.error) {
                brls::Logger::error("[SponsorBlock] network error: {}", r.error.message);
                ERROR_MSG(r.error.message, -1);
                return;
            }
            if (r.status_code == 404) {
                brls::Logger::debug("[SponsorBlock] no segments for bvid={} (404)", bvid);
                if (callback) callback({});
                return;
            }
            if (r.status_code != 200) {
                brls::Logger::error("[SponsorBlock] http error: {} for bvid={}", r.status_code, bvid);
                ERROR_MSG("SponsorBlock error. [Status code: " + std::to_string(r.status_code) + " ]", r.status_code);
                return;
            }
            try {
                nlohmann::json res = nlohmann::json::parse(r.text);
                brls::Logger::debug("[SponsorBlock] raw response: {}", r.text);
                if (res.is_array()) {
                    auto segments = res.get<SponsorBlockSegmentList>();
                    brls::Logger::debug("[SponsorBlock] got {} segments for bvid={}", segments.size(), bvid);
                    for (auto& seg : segments) {
                        brls::Logger::debug("[SponsorBlock]   seg uuid={} cid={} category={} [{}, {}]",
                               seg.UUID, seg.cid, seg.category,
                               seg.segment[0], seg.segment[1]);
                    }
                    if (callback) callback(segments);
                } else {
                    brls::Logger::debug("[SponsorBlock] response is not an array, type: {}", res.type_name());
                    if (callback) callback({});
                }
            } catch (const std::exception& e) {
                brls::Logger::error("[SponsorBlock] parse error: {} raw: {}", e.what(), r.text);
                ERROR_MSG(std::string(e.what()), -1);
            }
        });
}

} // namespace bilibili
