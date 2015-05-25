#include <boost/log/trivial.hpp>
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
	   {
		   mqtt_connect_type *connect =
		      dynamic_cast<mqtt_connect_type*>(message);
		   BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__<< " --> Connect Message:"
		                           << connect->keep_alive;
		   BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__<< " --> Connect Message: "
		                           << connect->username;
		   mqtt_connack_type connack;
		   mqtt_header header;
		   header.type = mqtt_control_packet_type::connack;
		   header.flags = 0;
		   header.remaining_length = 2;
		   connack.header = header;
		   connack.ack_flag = 0;
		   connack.return_code = mqtt_return_code::accepted;
		   buffer_ = connack.serialize();
		   return mqtt_request_action::response;
	   }
	case mqtt_control_packet_type::publish:
	   {
		   mqtt_publish_type *publish =
		      dynamic_cast<mqtt_publish_type*>(message);
		   BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__<<" --> Publish Message: " <<
		      (*publish).topic_name;
		   return mqtt_request_action::next;
	   }
	case mqtt_control_packet_type::disconnect:
	   {
		   return mqtt_request_action::disconnect;
	   }
	}
}

std::vector<uint8_t> mqtt_request_handler::get_buffer() const 
{
	return buffer_;
}


}
