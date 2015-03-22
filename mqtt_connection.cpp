#include "mqtt_connection.hpp"
#include "mqtt_connection_manager.hpp"
#include "mqtt_parser.hpp"
#include <iostream>
#include <boost/log/trivial.hpp>

namespace lmqtt {

mqtt_connection::mqtt_connection(boost::asio::ip::tcp::socket socket,
	mqtt_connection_manager& manager)
	: socket_(std::move(socket)),
	  manager_(manager)
{

}

void mqtt_connection::start() 
{
	log_remote_ip(socket_);
	do_read();
}

void mqtt_connection::stop()
{socket_.close();}

void mqtt_connection::read_mqtt_fixed_header() 
{auto self(shared_from_this());}

void mqtt_connection::do_read()
{
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(buffer_),
	                        [this,self](boost::system::error_code ec,
	                                    std::size_t bytes_transferred)
	                        {
		                        if(!ec) {
			                        if(!fixed_header_complete(buffer_.begin(),
			                                                 buffer_.begin()+bytes_transferred)){
				                        do_read();
			                        } else {
				                        try{
					                        parse_fixed_header(buffer_.begin(),
					                                           buffer_.begin()+bytes_transferred);
				                        }catch(std::exception& e){
					                        manager_.stop(shared_from_this()); 
				                        }
				                       std::cout << "done" << std::endl; 
			                        }
		                        } else if(ec == boost::asio::error::operation_aborted){
			                        manager_.stop(shared_from_this());
		                        }
	                        });
}

void mqtt_connection::do_write()
{std::cout << "Write" << std::endl;}

void log_remote_ip(const boost::asio::ip::tcp::socket& socket)
{
	auto remote_endp = socket.remote_endpoint();
	auto remote_address = remote_endp.address().to_string();
	BOOST_LOG_TRIVIAL(info) << "New Connection: "<<remote_address;
}

}
