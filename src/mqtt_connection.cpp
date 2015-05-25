#include "mqtt_connection.hpp"
#include "mqtt_connection_manager.hpp"
#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace lmqtt {

mqtt_connection::mqtt_connection(boost::asio::ip::tcp::socket socket,
	mqtt_connection_manager& manager)
	: socket_(std::move(socket)),
	  timer_(socket_.get_io_service(),boost::posix_time::seconds(10)),
	  manager_(manager),
	  handler_(manager)
{}

void mqtt_connection::start() 
{
	// auto self(shared_from_this());
	// timer_.async_wait([this,self](const boost::system::error_code& err)
	//                   {
	// 	                  if(!err){
	// 		                  manager_.stop(shared_from_this()); 
	// 	                  }
	//                   });
	log_remote_ip(socket_);
	read_header();
}

void mqtt_connection::stop()
{
	socket_.close();;
}

void mqtt_connection::read_mqtt_fixed_header() 
{
	auto self(shared_from_this());
}

void mqtt_connection::read_message() 
{
	auto self(shared_from_this());
	boost::asio::async_read(socket_,boost::asio::buffer(buffer_message_),
	                        [this,self](boost::system::error_code ec,
	                                    std::size_t bytes_transferred) {
		                        if(!ec) {
			                        BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__<< " --> Message received: "
			                                                << bytes_transferred;
			                        auto message = parse_message(buffer_message_.cbegin(), buffer_message_.cend(),header_);
			                        if(std::get<0>(message) == result_type::good){
				                        auto action  = handler_.handle_request(std::get<1>(message).get());
				                        switch(action) {
				                        case mqtt_request_action::response:
					                        BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__<< " --> Send reponse!";
					                        do_write();
					                        break;
				                        case mqtt_request_action::next:
					                        BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__<< " --> Read next package!";
					                        read_header();
					                        break;
				                        case mqtt_request_action::disconnect:
					                        BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__<< " --> Disconnect!";
					                        manager_.stop(shared_from_this()); 
					                        break;
				                        }
			                        } else {
				                        BOOST_LOG_TRIVIAL(debug) << __PRETTY_FUNCTION__<< " --> Received bad data.";
				                        manager_.stop(shared_from_this()); 
			                        }
		                        }
	                        });
}

void mqtt_connection::read_header()
{
	auto self(shared_from_this());
	boost::asio::async_read(socket_,boost::asio::buffer(buffer_header_),
	                        [this,self](boost::system::error_code ec, std::size_t bytes_transferred) {
		                        if(!ec) {
			                        auto result = parser_.parse(header_,buffer_header_.begin(),
			                                                    buffer_header_.end());
			                        if(std::get<0>(result) == mqtt_header_parser::good) {
				                        BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__<<" --> Received header ";
				                        BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__<<" --> Type: "
				                                                << type_string(header_.type); 
				                        BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__<<" --> Message length: "
				                                                << header_.remaining_length;
				                        buffer_message_.resize(header_.remaining_length);
				                        //timer_.cancel();
				                        parser_.reset();
				                        read_message();
			                        } else if (std::get<0>(result) == mqtt_header_parser::bad) {
				                        BOOST_LOG_TRIVIAL(debug) << __PRETTY_FUNCTION__<<" --> Bad header received";
				                        manager_.stop(shared_from_this());
			                        } else {
				                        read_header();
			                        }
		                        }
	                        });
}

void mqtt_connection::do_write()
{
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__<< " --> Write data";
	auto self(shared_from_this());
	boost::asio::async_write(socket_,boost::asio::buffer(handler_.get_buffer()),
	                         [this,self](boost::system::error_code ec, std::size_t) {
		                         if(!ec){
			                         read_header();
		                         }
	                         });
}

void log_remote_ip(const boost::asio::ip::tcp::socket& socket)
{
	auto remote_endp = socket.remote_endpoint();
	auto remote_address = remote_endp.address().to_string();
	BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__<<" --> New Connection: "<<remote_address;
}

}
