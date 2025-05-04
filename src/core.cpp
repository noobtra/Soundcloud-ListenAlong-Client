#include "core.hpp"

#include <codecvt>

listenalong::core::core() = default;

listenalong::core::~core()
{
	running_ = false;

	browser_extension_server_.stop();
	api_sync_client_.stop();

	if (api_sync_thread_.joinable())
		api_sync_thread_.join();
	if (browser_extension_thread_.joinable())
		browser_extension_thread_.join();
	
}

void listenalong::core::initialize()
{
	running_ = true;

	discord_manager_.initialize();
	party_ = party(discord_manager_.get_user().Id());

	// Initialize the extension server
	initialize_extension_server();

	// Initialize the sync client
	initialize_sync_client();
}

void listenalong::core::run() const
{
	discord_manager_.run();
}

void listenalong::core::initialize_extension_server()
{
    browser_extension_server_.init_asio();

    browser_extension_server_.set_open_handler([this](const connection_hdl& hdl)
    {
        browser_extension_connected_ = true;
        browser_extension_connection_ = hdl;
        std::cout << "Connected to extension\n";
    });

    browser_extension_server_.set_close_handler([this](const connection_hdl& hdl)
    {
        browser_extension_connected_ = false;
        std::cout << "Extension disconnected\n";
    });

    browser_extension_server_.set_fail_handler([this](const connection_hdl& hdl)
    {
        auto con = browser_extension_server_.get_con_from_hdl(hdl);
        std::cerr << "Extension connection error: " << con->get_ec().message() << std::endl;
        browser_extension_connected_ = false;
    });

    browser_extension_server_.set_message_handler([this](const connection_hdl& hdl, const ws_server::message_ptr& msg)
    {
        receive_extension_message(msg->get_payload());
    });

    try 
    {
        browser_extension_server_.listen(9005);
        browser_extension_server_.start_accept();
        browser_extension_thread_ = std::thread([this] { browser_extension_server_.run(); });
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Failed to start extension server: " << e.what() << std::endl;
    }
}

void listenalong::core::initialize_sync_client()
{
    api_sync_client_.init_asio();

    api_sync_client_.set_tls_init_handler([](const connection_hdl&)
    {
        auto ctx = std::make_shared<tls_context>(tls_context::tlsv12);
        return ctx;
    });
    api_sync_client_.set_open_handler([this](const connection_hdl& hdl)
    {
        api_sync_connected_ = true;
        api_sync_connection_ = hdl;
        std::cout << "Connected to Sync server" << '\n';
    });
    api_sync_client_.set_close_handler([this](const connection_hdl& hdl)
    {
        api_sync_connected_ = false;
        std::cout << "Disconnected from Sync server" << '\n';
    });
    api_sync_client_.set_fail_handler([this](const connection_hdl& hdl)
    {
        auto con = api_sync_client_.get_con_from_hdl(hdl);
        std::string error_msg = "Connection failed: " + con->get_ec().message();
        std::cout << error_msg << std::endl;
        api_sync_connected_ = false;
    });
    api_sync_client_.set_message_handler([this](const connection_hdl& hdl, const ws_client::message_ptr& msg)
    {
        receive_sync_message(msg->get_payload());
    });

    attempt_sync_connection();

    // Start the API client in its own thread
    api_sync_thread_ = std::thread([this] 
    {
	    while (running_) 
	    {
	        try 
            {
	            api_sync_client_.run();
	        }
	        catch (const std::exception& e) 
            {
	            std::cerr << "API sync client error: " << e.what() << std::endl;
	        }

            std::this_thread::sleep_for(std::chrono::seconds(5));

	        if (running_) 
	            attempt_sync_connection();
	        
	    }
    });
}

void listenalong::core::attempt_sync_connection()
{
    try 
    {
        api_sync_client_.reset();

        websocketpp::lib::error_code ec;
        const auto con = api_sync_client_.get_connection("wss://ws.listenalong.tv", ec);
        if (ec) 
        {
            std::cerr << "Failed to create connection: " << ec.message() << std::endl;
            return;
        }

        api_sync_client_.connect(con);
        std::cout << "Attempting to connect to sync server..." << std::endl;
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Connection attempt failed: " << e.what() << std::endl;
    }
}

void listenalong::core::send_play_request(const std::string& track_url)
{
	const json msg = 
	{
		{"type", "WS_PLAY_TRACK"},
		{"data",{{"trackUrl", track_url}}}
	};

	send_extension_message(msg);
	std::cout << "Sent play request for track: " << track_url << "\n";
}

void listenalong::core::send_extension_message(const json& message)
{
	try
	{
		browser_extension_server_.send(browser_extension_connection_, message.dump(), websocketpp::frame::opcode::text);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Send error: " << e.what() << "\n";
	}
}

void listenalong::core::receive_extension_message(const std::string& payload)
{
	try 
	{
		auto message = json::parse(payload);
		const std::string event = message["type"];
        if (event == "WS_SEND_TRACK") 
        {
            const auto track_data = track::from_json(message["data"]);
            const auto party_host_id = party_.get_host_id();
			if (party_host_id && party_host_id == discord_manager_.get_user().Id())
				party_.set_track(track_data);
        }
	}
	catch (const std::exception& e) 
	{
		std::cerr << "Error parsing message: " << e.what() << "\n";
	}
}

void listenalong::core::send_sync_message(const json& message) const
{
	// TODO: Send every new song and heartbeat to the sync server
}

void listenalong::core::receive_sync_message(const std::string& payload) const
{
	// TODO: Handle new track requests from the sync server, when too out of sync with host -- resync with host
}
