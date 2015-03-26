#include <iostream>
#include <boost/asio.hpp>
#include "mqtt_server.hpp"

int main(int argc, char *argv[])
{
	try {
		lmqtt::mqtt_server server("0.0.0.0","12306");
		server.run();
	} catch(std::exception& e){
		std::cerr<<"exception: "<<e.what() <<"\n";
	}

	return 0;
}
