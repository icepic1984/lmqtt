#ifndef MQTT_PARSER_HPP
#define MQTT_PARSER_HPP

#include <iostream>
#include <bitset>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <boost/asio.hpp> 
#include "mqtt_serialize.hpp"

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

enum class qos {qos_0,qos_1,qos_2};

struct mqtt_header 
{
   mqtt_control_packet_type type;
   uint8_t flags;
   uint32_t remaining_length;

   bool retain_flag() const {
	   return(flags&1);
   }
   qos qos_flag() const  {
	   return static_cast<qos>(flags&6 >> 1);
   }
   bool dup_flag() const {
	   return flags&8;
   }
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
   uint32_t multiplier_;
   enum state {type,length} state_;
   
};


enum class result_type {good,bad};
	
struct mqtt_connect_type
{
   mqtt_header header;
   std::string protocol_name;
   uint8_t version;
   uint8_t flags;
   uint16_t keep_alive;
   std::string client_id;
   std::string will_topic;
   std::vector<uint8_t> will_message;
   std::string username;
   std::string password;

   bool username_flag() const {
	   return flags&128;
   }
   bool password_flag() const {
	   return flags&64;
   }
   bool will_retain_flag() const {
	   return flags&32;
   }
   qos will_qos_flag() const {
	   return static_cast<qos>((flags&24) >> 3);
   }
   bool will_flag() const {
	   return flags&4;
   }
   bool clean_session_flag() const {
	   return flags&2;
   }
   bool reserved_flag() const {
	   return flags&1;
   } 
};

bool verify_string(const std::string& name);

template<typename InputIterator>
result_type parse_buffer(InputIterator begin, InputIterator end, const mqtt_header& header, mqtt_connect_type& message)
{
	try{
		message.header = header;
		uint16_t length;
		begin = deserialize(begin,end,length);
		if(length != 4) return result_type::bad;
		message.protocol_name = std::string(4,' ');
		begin = deserialize(begin,end,message.protocol_name);
		if(!verify_string(message.protocol_name) || message.protocol_name != "MQTT")
		   return result_type::bad;
		begin = deserialize(begin,end,message.version);
		begin = deserialize(begin,end,message.flags);	
		// MQTT-3.1.1 3.1.2.3:  Verify reserved flag is zero
		if(message.reserved_flag()) return result_type::bad;
		// MQTT-3.1.1 3.1.2.6: Verify qos flag
		if(static_cast<int>(message.will_qos_flag()) == 3){
			return result_type::bad;
		}
		// MQTT-3.1.1 3.1.2.5: Verify will flag
		if(!message.will_flag() &&
		   (message.will_qos_flag() != qos::qos_0 || message.will_retain_flag())){
			return result_type::bad;
		}
		// MQTT-3.1.1 3.1.2.9: Verify user flag
		if(!message.username_flag() && message.password_flag())
		   return  result_type::bad;

		// Data are encoded as big endian 
		begin = deserialize(begin,end,message.keep_alive);
		begin = deserialize(begin,end,length);
		if(length == 0 && !message.clean_session_flag())
		   return result_type::bad;
		message.client_id = std::string(length,' ');
		begin = deserialize(begin,end,message.client_id);
		if(message.will_flag()){
			begin = deserialize(begin,end,length);
			message.will_topic = std::string(length,' ');
			begin = deserialize(begin,end,message.will_topic);
			begin = deserialize(begin,end,length);
			message.will_message = std::vector<uint8_t>(length);
			begin = deserialize(begin,end,message.will_message);
		}
		if(message.username_flag()){
			begin = deserialize(begin,end,length);
			message.username = std::string(length,' ');
			begin = deserialize(begin,end,message.username);
		}
		if(message.password_flag()){
			begin = deserialize(begin,end,length);
			message.password = std::string(length,' ');
			begin = deserialize(begin,end,message.password);
		}
		if(begin == end){
			return result_type::good;
		} else {
			return result_type::bad;
		}
	}catch(const std::out_of_range& ex){
		return result_type::bad;
	}
}
}


#endif
   
   
   
