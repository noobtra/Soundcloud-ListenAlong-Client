#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <mutex>
#include <set>

using namespace websocketpp;
using json = nlohmann::json;

typedef std::set<connection_hdl, std::owner_less<connection_hdl>> con_list;

class SoundCloudServer {
public:
    SoundCloudServer() {
        m_server.init_asio();
        m_server.set_open_handler([this](auto hdl) {
            std::lock_guard<std::mutex> lock(m_connection_mutex);
            m_connections.insert(hdl);
            });
        m_server.set_close_handler([this](auto hdl) {
            std::lock_guard<std::mutex> lock(m_connection_mutex);
            m_connections.erase(hdl);
            });
        m_server.set_message_handler([this](auto hdl, auto msg) {
            handle_message(hdl, msg->get_payload());
            });
    }

    void broadcast(const json& message)
	{
        std::lock_guard<std::mutex> lock(m_connection_mutex);
        for (auto& hdl : m_connections) {
            try {
                m_server.send(hdl, message.dump(), frame::opcode::text);
            }
            catch (const std::exception& e) {
                // Handle error
            }
        }
    }

    void run(uint16_t port)
	{
        m_server.listen(port);
        m_server.start_accept();
        m_server.run();
    }

private:
    server<config::asio> m_server;
    con_list m_connections;
    std::mutex m_connection_mutex;

    void handle_message(connection_hdl hdl, const std::string& payload) {
        try {
            auto data = json::parse(payload);
            if (data["event"] == "trackChange") {
                // Handle track change from extension
            }
            else if (data["event"] == "syncRequest") {
                // Handle synchronization request
                json response = {
                    {"event", "syncResponse"},
                    {"data", {{"timestamp", 12345.678}}}
                };
                m_server.send(hdl, response.dump(), frame::opcode::text);
            }
        }
        catch (const std::exception& e) 
        {
            // Handle error
        }
    }
};