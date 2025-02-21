#pragma once
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <mutex>
#include <set>
#include <iostream>
#include <thread>
#include <chrono>

using websocketpp::connection_hdl;
using json = nlohmann::json;
typedef websocketpp::server<websocketpp::config::asio> server;
typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;

class SoundCloudServer 
{
public:
    SoundCloudServer();
    void initialize(uint16_t port);

    // Sends a play request to connected clients
    void send_play_request(const std::string& track_id);

private:
    void broadcast(const json& message);
    void handle_message(connection_hdl hdl, const std::string& payload);

    server m_server_;
    con_list m_connections_;
    std::mutex m_connection_mutex_;
};
