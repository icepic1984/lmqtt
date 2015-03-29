#include "mqtt_parser.hpp"
#include <boost/log/trivial.hpp>

namespace lmqtt{

mqtt_header_parser::mqtt_header_parser():
	multiplier_(1),
	state_(type)
{}

void mqtt_header_parser::reset()
{
	state_ = type;
	multiplier_ = 1;
}

mqtt_header_parser::result_type mqtt_header_parser::consume(mqtt_header& header, uint8_t byte)
{
	switch(state_) {
	case type:
		if(((byte >> 4) == 15) || ((byte >> 4 ) == 0)) {
			return bad;
		}
		header.type = static_cast<mqtt_control_packet_type>(byte >>4);
		header.flags = (byte & 15);
		switch(header.type) {
		case mqtt_control_packet_type::connect:
		case mqtt_control_packet_type::connack:
		case mqtt_control_packet_type::puback:
		case mqtt_control_packet_type::pubrec:
		case mqtt_control_packet_type::pubcomp:
		case mqtt_control_packet_type::suback:
		case mqtt_control_packet_type::unsuback:
		case mqtt_control_packet_type::pingreq:
		case mqtt_control_packet_type::pingresp:
		case mqtt_control_packet_type::disconnect:
			if(header.flags != 0){
				return bad;
			}
			break;
		case mqtt_control_packet_type::pubrel:
		case mqtt_control_packet_type::subscribe:
		case mqtt_control_packet_type::unsubscribe:
			if(header.flags  != 2) {
			   return bad;
			}
			break;
		}	
		header.remaining_length = 0;
		state_ = length;
		return indeterminate;
	case length:
		if(multiplier_ > (128 * 128 * 128)){
			return bad;
		}
		header.remaining_length += (byte & 127) * multiplier_;
		multiplier_ *= 128;
		if((byte & 128) == 0)
			return good;
		 else 
		   return indeterminate;
	}
}

std::string type_string(mqtt_control_packet_type type) {
	uint32_t field = static_cast<int>(type);
	return mqtt_control_packet_type_strings[field-1];
}

}


