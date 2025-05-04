#pragma once
#include "discord.h"
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>
#include <asio/ssl/context.hpp>
#include <nlohmann/json.hpp>
#include "party.hpp"
#include "discord_manager.hpp"

typedef websocketpp::server<websocketpp::config::asio> ws_server;
typedef websocketpp::client<websocketpp::config::asio_tls_client> ws_client;
using tls_context = asio::ssl::context;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using websocketpp::connection_hdl;
using json = nlohmann::json;

namespace listenalong
{
    class core
    {
    public:
        core();
        ~core();

        void initialize();
        void run() const;

        bool is_extension_connected() const { return browser_extension_connected_; }
        bool is_server_connected() const { return api_sync_connected_; }

        void send_play_request(const std::string& track_url);

    private:
        void initialize_extension_server();
        void initialize_sync_client();
        void attempt_sync_connection();

        void send_extension_message(const json& message);
        void receive_extension_message(const std::string& payload);
        void send_sync_message(const json& message) const;
        void receive_sync_message(const std::string& payload) const;

        discord_manager discord_manager_;
        std::atomic<bool> running_{ false };

        party party_{};

        ws_server browser_extension_server_{};
        std::thread browser_extension_thread_{};
        connection_hdl browser_extension_connection_{};
        std::atomic<bool> browser_extension_connected_{ false };

        ws_client api_sync_client_{};
        std::thread api_sync_thread_{};
        connection_hdl api_sync_connection_{};
        std::atomic<bool> api_sync_connected_{ false };
    };
}
