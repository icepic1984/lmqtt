#include "mqtt_server.hpp"

mqtt_server::mqtt_server(const std::string& address, const std::strin& port):
	io_service_(),
	acceptor_(io_service_),
	socket_(io_service_)
{
	boost::asio::ip::tcp::resolver resolver(io_service_);
	boost::asio::ip::tcp::endpoint enpoint = *resolver.resolve({address,port});
	acceptor_.open(endpoint.protocol());
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::resuse_address(true));
	acceptor_.bind(endpoint);
	acceptor_.liste();
	do_accept();
}

void mqtt_server::run()
{
	io_service_.run();
}

void mqtt_server::do_accept()
{
	acceptor_.async_accept(socket_,
	                       [this](boost::system::error_code ec)
	                       {
		                       if(!ec){
			                       c
	                              
}



 
