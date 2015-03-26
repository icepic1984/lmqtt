#ifndef MQTT_PARSER_HPP
#define MQTT_PARSER_HPP

#include <iostream>
#include <bitset>
#include <iterator>
#include <stdexcept>
#include <tuple>

namespace lmqtt {

enum class mqtt_control_packet_type 
{
   connect = 1,
   connack,
   publish,
   puback,
   pubrec,
   pubrel,
   pubcomp,
   subscribe,
   suback,
   unsubscribe,
   unsuback,
   pingreq,
   pingresp,
   disconnect
};

static const std::array<std::string,static_cast<int>(mqtt_control_packet_type::disconnect)>
	mqtt_control_packet_type_strings{
	"connect",
	"connack",
	"publish",
	"puback",
	"pubrec",
	"pubrel",
	"pubcomp",
	"subscribe",
	"suback",
	"unsubscribe",
	"unsuback",
	"pingreq",
	"pingresp",
	"disconnect"
};

std::string type_string(mqtt_control_packet_type type);

struct mqtt_header 
{
   mqtt_control_packet_type type;
   std::bitset<4> flags;
   uint32_t remaining_length;
};
	

class mqtt_header_parser
{
public:
   mqtt_header_parser();
   void reset();

   enum result_type {good,bad,indeterminate};

   template <typename InputIterator>
   std::tuple<result_type, InputIterator> parse(mqtt_header& header,
                                                InputIterator begin, InputIterator end){
	   while(begin != end){
		   result_type result = consume(header,*begin++);
		   if(result == good || result == bad){
			   return std::make_tuple(result,begin);
		   }
	   }
	   return std::make_tuple(indeterminate,begin);
   }
   
private:
   result_type consume(mqtt_header& header, uint8_t byte);
   int multiplier_;
   enum state {type,length} state_;
   
};

   

// mqtt_control_packet_type get_control_packet_type(uint8_t byte)
// {return(static_cast<mqtt_control_packet_type>(byte >>4));}

// template<typename InputIterator>
// uint32_t get_remaining_bytes(InputIterator begin, InputIterator end)
// {
// 	uint32_t multiplier = 1;
// 	uint32_t result = 0;
// 	begin++;
// 	do{
// 		result += ((*begin++) & 127) * multiplier;
// 		multiplier *= 128;
// 		if(multiplier > 128*128*128)
// 		   throw std::length_error("Invalid header size");
// 	} while((begin != end) && (*begin & 128) != 0);
// 	return result;
// }


// template <typename InputIterator>
// std::tuple<mqtt_control_packet_type,InputIterator>
// parse_fixed_header(InputIterator begin, InputIterator end)
// {
// 	header.type = get_control_packet_type(*begin);
// 	header.flags = std::bitset<4>(*begin & 0xF);
// 	header.remaining_length = get_remaining_bytes(begin,end);
// 	return std::make_tuple(header,begin);
// }

// template <typename InputIterator>
// bool is_header_complete(InputIterator begin, InputIterator end)
// {
// 	if(std::distance(begin,end) < 2) return false;
// 	while(begin != end) {
// 		if(((*begin++) & 128)  == 0) return true;
// 	}
// 	return false;
// };

// template <typename InputIterator>
// bool is_message_complete(InputIterator begin, InputIterator end)
// {
// 	if(std::distance(begin,end) < 3) return false;
// 	while(begin != end) {
// 		if(((*begin++) & 128)  == 0) return true;
// 	}
// 	return false;

// }
}


#endif
   
   
   
