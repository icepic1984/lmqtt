#ifndef MQTT_CONNECTION_HPP
#define MQTT_CONNECTION_HPP

#include <memory>
#include "mqtt_parser.hpp"
#include <boost/asio.hpp>
#include "mqtt_request_handler.hpp"

namespace lmqtt {

class mqtt_connection_manager;

class mqtt_connection : public std::enable_shared_from_this<mqtt_connection>
{
public:
   mqtt_connection(const mqtt_connection&) = delete;
   mqtt_connection& operator=(const mqtt_connection&) = delete;

   explicit mqtt_connection(boost::asio::ip::tcp::socket socket,
	   mqtt_connection_manager& manager);
   void start();
   void stop();

private:
   void read_mqtt_fixed_header();
   void read_header();
   void read_message();
   void do_write();

   boost::asio::ip::tcp::socket socket_;
   mqtt_connection_manager& manager_;
   boost::asio::deadline_timer timer_;
   std::array<char,1> buffer_header_;
   std::vector<char> buffer_message_;
   mqtt_header_parser parser_;
   mqtt_header header_;
   mqtt_request_handler handler_;
};

using mqtt_connection_ptr = std::shared_ptr<mqtt_connection>;
void log_remote_ip(const boost::asio::ip::tcp::socket& socket);

}

#endif
	
