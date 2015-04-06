#include "mqtt_request_handler.hpp"
#include "mqtt_connection_manager.hpp"
#include "mqtt_parser.hpp"

namespace lmqtt {

mqtt_request_handler::mqtt_request_handler(mqtt_connection_manager& manager):
	manager_(manager)
{}

void mqtt_request_handler::handle_request(const mqtt_package_type& message)
{
	switch(message.get_type()){
	case mqtt_control_packet_type::connect:
		mqtt_connect_type connect = dynamic_cast<const mqtt_connect_type&>(message);
		std::cout << "Connect:" << connect.keep_alive<< std::endl;
		std::cout << "Connect:" << connect.username<< std::endl;
		std::cout << "Connect:" << connect.password<< std::endl;
	}
}

}
