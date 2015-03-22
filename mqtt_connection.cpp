#include "mqtt_connection.hpp"
#include <iostream>

namespace lmqtt {

mqtt_connection::mqtt_connection(boost::asio::ip::tcp::socket socket)
	: socket_(std::move(socket))
{

}

void mqtt_connection::start() 
{do_read();}

void mqtt_connection::stop()
{socket_.close();}

void mqtt_connection::do_read()
{std::cout << "Read" << std::endl;}

void mqtt_connection::do_write()
{std::cout << "Write" << std::endl;}

}


