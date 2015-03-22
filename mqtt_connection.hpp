#ifndef MQTT_CONNECTION_HPP
#define MQTT_CONNECTION_HPP

#include <memory>
#include <boost/asio.hpp>

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
   void do_read();
   void do_write();

   boost::asio::ip::tcp::socket socket_;
   mqtt_connection_manager& manager_;
   std::array<char,5> buffer_;
   
};

using mqtt_connection_ptr = std::shared_ptr<mqtt_connection>;
void log_remote_ip(const boost::asio::ip::tcp::socket& socket);

}

#endif
	
