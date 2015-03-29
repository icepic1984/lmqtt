#ifndef MQTT_PARSER_HPP
#define MQTT_PARSER_HPP

#include <iostream>
#include <bitset>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <boost/asio.hpp> 

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

   bool user_name_flag() const {
	   return flags&128;
   }
   bool password_flag() const {
	   return flags&64;
   }
   bool will_retain_flag() const {
	   return flags&32;
   }
   qos will_qos_flag() const {
	   return static_cast<qos>(flags&24 >> 4);
   }
   bool will_flag() const {
	   return flags&4;
   }
   bool clean_session_flag() const {
	   return flags&2;
   }
   bool reserverd_flag() const {
	   return flags&1;
   } 
};

template<typename T,typename InputIterator>
result_type parse_buffer(const InputIterator begin, const InputIterator end, const mqtt_header& header, T& message);

bool verify_string(const std::string& name);

template<typename InputIterator>
result_type parse_buffer(InputIterator begin, InputIterator end, const mqtt_header& header, mqtt_connect_type& message)
{
	if(distance(begin,end) < 10 ) return result_type::bad;
	message.header = header;
	begin = begin + 2;
	std::string name(4,' ');
	for(unsigned i = 0; i < 4; ++i){
		name[i] = *(begin++);
	}
	if(!verify_string(name) || name != "MQTT")
	   return result_type::bad;
	message.protocol_name = name;
	message.version = *(begin++);
	message.flags = *(begin++);
	if(message.reserverd_flag()) return result_type::bad;
	if(!message.user_name_flag() && message.password_flag())
	   return  result_type::bad;;
	// Data are encoded as big endian 
	message.keep_alive = (*(begin+1) << 0 ) | *(begin) << 8;
	return result_type::good;
}


}


#endif
   
   
   
