#include "GeminiClient.h"
#include <cpr/cpr.h>
#include <iostream>



GeminiClient::GeminiClient(const std::string& key) : apiKey(key) {
    // The v1beta model is good for general purpose use.
    apiUrl = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash-latest:generateContent?key=" + apiKey;
}

std::string GeminiClient::generateContent(const std::string& prompt) {
    // 1. Construct the JSON request body
    nlohmann::json requestBody = {
        {"contents", {{
            {"parts", {{
                {"text", prompt}
            }}}
        }}}
    };

    // 2. Make the HTTPS POST request
    cpr::Response r = cpr::Post(cpr::Url{apiUrl},
                                cpr::Header{{"Content-Type", "application/json"}},
                                cpr::Body{requestBody.dump()});

    // 3. Check for errors
    if (r.status_code != 200) {
        std::cerr << "Error: " << r.status_code << " - " << r.text << std::endl;
        return "Error communicating with Gemini API.";
    }

    // 4. Parse the JSON response
    try {
        nlohmann::json responseJson = nlohmann::json::parse(r.text);
        // Navigate the JSON to find the generated text
        if (responseJson.contains("candidates") && responseJson["candidates"].is_array() && !responseJson["candidates"].empty()) {
            const auto& firstCandidate = responseJson["candidates"][0];
            if (firstCandidate.contains("content") && firstCandidate["content"].contains("parts") && firstCandidate["content"]["parts"].is_array() && !firstCandidate["content"]["parts"].empty()) {
                return firstCandidate["content"]["parts"][0].value("text", "No text found in response.");
            }
        }
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return "Error parsing Gemini response.";
    }

    return "Received an unexpected response format from Gemini.";
}