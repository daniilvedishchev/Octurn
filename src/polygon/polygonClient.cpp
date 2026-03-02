#include "polygonClient.hpp"

polygonClient::polygonClient(const std::string& api_key):
            api_key(std::move(api_key)){}

nlohmann::json polygonClient::fetchData(const std::string& ticker,int multiplier,
        const std::string& from,
        const std::string& to,
        const std::string& timespan)
    {
    std::string url = "https://api.polygon.io/v2/aggs/ticker/" + ticker +
                      "/range/" + std::to_string(multiplier) + "/" + timespan +
                      "/" + from + "/" + to;
    cpr::Response response = cpr::Get(cpr::Url{url},
                            cpr::Parameters{{"apiKey",api_key}});
    
    auto json = nlohmann::json::parse(response.text);
    
    if (!json.contains("results") || !json["results"].is_array()) {
        throw std::runtime_error("Polygon response has no results[]: " + json.dump());
    }

    return json;
}

