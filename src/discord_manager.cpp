#define DISCORDPP_IMPLEMENTATION

#include "discord_manager.hpp"
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <ostream>

void listenalong::discord_manager::initialize()
{

	// Initialize the Discord client
	client_ = std::make_unique<discordpp::Client>();

    // Set up logging callback
    client_->AddLogCallback([](auto message, auto severity) {
        std::cout << "[" << EnumToString(severity) << "] " << message << std::endl;
        }, discordpp::LoggingSeverity::Info);


    // Set up status callback to monitor client connection
    client_->SetStatusChangedCallback([this](discordpp::Client::Status status, discordpp::Client::Error error, int32_t errorDetail) 
    {
        std::cout << "Status changed: " << discordpp::Client::StatusToString(status) << std::endl;

        if (status == discordpp::Client::Status::Ready) 
        {
			connected_ = true;
        }
        else if (error != discordpp::Client::Error::None) 
        {
			connected_ = false;
        }
    });

    // Generate OAuth2 code verifier for authentication
    auto codeVerifier = client_->CreateAuthorizationCodeVerifier();

    // Set up authentication arguments
    discordpp::AuthorizationArgs args{};
    args.SetClientId(DISCORD_CLIENT_ID);
    args.SetScopes(discordpp::Client::GetDefaultPresenceScopes());
    args.SetCodeChallenge(codeVerifier.Challenge());

    // Begin authentication process
    client_->Authorize(args, [this, codeVerifier](auto result, auto code, auto redirectUri) {
        if (!result.Successful()) {
            std::cerr << "Authentication Error: " << result.Error() << std::endl;
            return;
        }
        else {
            std::cout << "Authorization successful! Getting access token...\n";

            // Exchange auth code for access token
            client_->GetToken(DISCORD_CLIENT_ID, code, codeVerifier.Verifier(), redirectUri,
                [this](discordpp::ClientResult result,
                    std::string accessToken,
                    std::string refreshToken,
                    discordpp::AuthorizationTokenType tokenType,
                    int32_t expiresIn,
                    std::string scope) {
                        std::cout << "Access token received! Establishing connection...\n";
                        // Next Step: Update the token and connect
                        client_->UpdateToken(discordpp::AuthorizationTokenType::Bearer, accessToken, [this](discordpp::ClientResult result) {
                            if (result.Successful()) {
                                std::cout << "Token updated, connecting to Discord...\n";
                                client_->Connect();
                            }
                            });
                });
        }
        });

	while (!connected_)
	{
        discordpp::RunCallbacks();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

    running_ = true;
}

void listenalong::discord_manager::run() const
{
    while (running_)
    {
        discordpp::RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}
