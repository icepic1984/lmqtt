#ifndef MQTT_CONNECTION_HPP
#define MQTT_CONNECTION_HPP

#include <memory>
#include <boost/asio.hpp>

namespace lmqtt {

class mqtt_connection : public std::enable_shared_from_this<mqtt_connection>
{
   mqtt_connection(const mqtt_connection&) = delete;
   mqtt_connection& operator=(const mqtt_connection&) = delete;

   explicit mqtt_connection(boost::asio::ip::tcp::socket socket);
   void start();
   void stop();

private:
   void do_read();
   void do_write();

   boost::asio::ip::tcp::socket socket_;
   std::array<char,8129> buffer_;
};

using mqtt_connection_ptr = std::shared_ptr<mqtt_connection>;

}

#endif
	
