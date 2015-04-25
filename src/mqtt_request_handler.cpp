#include "mqtt_request_handler.hpp"
#include "mqtt_connection_manager.hpp"
#include "mqtt_parser.hpp"

namespace lmqtt {

mqtt_request_handler::mqtt_request_handler(mqtt_connection_manager& manager):
	manager_(manager)
{}

mqtt_request_action mqtt_request_handler::handle_request(mqtt_package_type *message)
{
	switch(message->get_type()){
	case mqtt_control_packet_type::connect:
		mqtt_connect_type *connect =
		   dynamic_cast<mqtt_connect_type*>(message);
		std::cout << "Connect:" << connect->keep_alive<< std::endl;
		std::cout << "Connect:" << connect->username<< std::endl;
		std::cout << "Connect:" << connect->password<< std::endl;
		
		return mqtt_request_action::response;
	}
}

std::vector<uint8_t> mqtt_request_handler::get_buffer() const 
{
	return buffer_;
}


}
