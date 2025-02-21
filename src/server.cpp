#include "server.hpp"

SoundCloudServer::SoundCloudServer() = default;

void SoundCloudServer::initialize(const uint16_t port)
{
	m_server_.init_asio();
	m_server_.set_open_handler([this](const connection_hdl& hdl)
		{
			std::lock_guard<std::mutex> lock(m_connection_mutex_);
			m_connections_.insert(hdl);
			std::cout << "Client connected\n";
		});
	m_server_.set_close_handler([this](const connection_hdl& hdl)
		{
			std::lock_guard lock(m_connection_mutex_);
			m_connections_.erase(hdl);
			std::cout << "Client disconnected\n";
		});
	m_server_.set_message_handler([this](const connection_hdl& hdl, const server::message_ptr& msg)
		{
			handle_message(hdl, msg->get_payload());
		});

	m_server_.listen(port);
	m_server_.start_accept();
	m_server_.run();
}

void SoundCloudServer::broadcast(const json& message)
{
	std::lock_guard lock(m_connection_mutex_);
	for (auto& hdl : m_connections_) 
	{
		try 
		{
			m_server_.send(hdl, message.dump(), websocketpp::frame::opcode::text);
		}
		catch (const std::exception& e) 
		{
			std::cerr << "Send error: " << e.what() << "\n";
		}
	}
}

void SoundCloudServer::send_play_request(const std::string& track_id)
{
	json msg = 
	{
		{"event", "playRequest"},
		{"data", {{"trackId", track_id}}}
	};

	broadcast(msg);
	std::cout << "Sent play request for track: " << track_id << "\n";
}

void SoundCloudServer::handle_message(connection_hdl hdl, const std::string& payload)
{
	try 
	{
		auto data = json::parse(payload);
		std::string event = data["event"];
		if (event == "currentTrack") 
		{
			std::cout << "Now playing: " << data.dump() << "\n";
			// Process currently playing track info
		}
		else if (event == "stopTrack")
		{
			std::cout << "Track stopped: " << data.dump() << "\n";
			// Process stop event
		}
		else 
		{
			std::cout << "Unknown event: " << event << "\n";
		}
	}
	catch (const std::exception& e) 
	{
		std::cerr << "Error parsing message: " << e.what() << "\n";
	}
}
