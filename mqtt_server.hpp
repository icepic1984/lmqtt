#ifndef MQTT_SERVER_HPP
#define MQTT_SERVER_HPP

#include <boost/asio.hpp>
#include "mqtt_connection.hpp"
#include "mqtt_connection_manager.hpp"

namespace lmqtt {
class mqtt_server 
{
public:
   mqtt_server(const mqtt_server&) = delete;
   mqtt_server& operator=(const mqtt_server) = delete;

   explicit mqtt_server(const std::string& address, const std::string& port);
   void run();

private:
   void do_accept();
   boost::asio::io_service io_service_;
   boost::asio::ip::tcp::acceptor acceptor_;
   mqtt_connection_manager manager_;
   boost::asio::ip::tcp::socket socket_;
};

}


#endif
