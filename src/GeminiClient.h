#pragma once
#include <string>
#include <nlohmann/json.hpp>

class GeminiClient {
public:
    /**
     * @brief Constructs the client with your API key.
     * @param key Your Google Gemini API key.
     */
    explicit GeminiClient(const std::string& key);

    /**
     * @brief Sends a prompt to the Gemini API and gets a response.
     * @param prompt The text prompt to send to the model.
     * @return The generated text content from the model.
     */
    std::string generateContent(const std::string& prompt);

private:
    std::string apiKey;
    std::string apiUrl;
};