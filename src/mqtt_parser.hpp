#ifndef MQTT_PARSER_HPP
#define MQTT_PARSER_HPP

#include <iostream>
#include <bitset>
#include <iterator>
#include <memory>
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
	
struct mqtt_package_type
{
   virtual mqtt_control_packet_type get_type() const = 0;
   mqtt_header header;
};

struct mqtt_connect_type : mqtt_package_type
{
   std::string protocol_name;
   uint8_t version;
   uint8_t flags;
   uint16_t keep_alive;
   std::string client_id;
   std::string will_topic;
   std::vector<uint8_t> will_message;
   std::string username;
   std::string password;

   mqtt_control_packet_type get_type() const override {
	   return header.type;
   }

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

struct mqtt_publish_type : mqtt_package_type 
{
   std::string topic_name;
   uint16_t packet_id;
   std::vector<uint8_t> payload;
   
   mqtt_control_packet_type get_type() const override {
	   return header.type;
   }
};

struct mqtt_puback_type : mqtt_package_type 
{
   uint16_t packet_id;
   mqtt_control_packet_type get_type() const override {
	   return header.type;
   }
};

using topic_t = std::tuple<uint8_t,std::string>;

struct mqtt_subscribe_type : mqtt_package_type
{
   
   mqtt_control_packet_type get_type() const override {
	   return header.type;
   }
   uint16_t packet_id;
   std::vector<topic_t> topics;
   
};

struct mqtt_unsubscribe_type : mqtt_package_type
{
   mqtt_control_packet_type get_type() const override {
	   return header.type;
   }
   uint16_t packet_id;
   std::vector<std::string> topics;
};

struct mqtt_pingreq_type : mqtt_package_type
{
   mqtt_control_packet_type get_type() const override {
	   return header.type;
   }
   
};

struct mqtt_disconnect_type : mqtt_package_type
{
   mqtt_control_packet_type get_type() const override {
	   return header.type;
   }
   
};


bool verify_string(const std::string& name);

template<typename InputIterator>
std::tuple<result_type,std::unique_ptr<mqtt_package_type>> parse_message(InputIterator begin, InputIterator end,
	  const mqtt_header& header)
{
	switch(header.type) {
	case mqtt_control_packet_type::connect:
	   {
		   mqtt_connect_type connect;
		   auto state = parse_buffer(begin,end,header,connect);
		   return(std::make_tuple(state,std::unique_ptr<mqtt_package_type>(
			                          std::make_unique<mqtt_connect_type>(connect))));
	   }
	case mqtt_control_packet_type::publish:
	   {
		   mqtt_publish_type publish;
		   auto state = parse_buffer(begin,end,header,publish);
		   return(std::make_tuple(state,std::unique_ptr<mqtt_package_type>(
			                         std::make_unique<mqtt_publish_type>(publish))));
	   } 
	case mqtt_control_packet_type::puback:
	   {
		   mqtt_puback_type puback;
		   auto state = parse_buffer(begin,end,header,puback);
		   return(std::make_tuple(state,std::unique_ptr<mqtt_package_type>(
			                         std::make_unique<mqtt_puback_type>(puback))));
	   } 
	case mqtt_control_packet_type::subscribe:
	   {
		   mqtt_subscribe_type subscribe;
		   auto state = parse_buffer(begin,end,header,subscribe);
		   return(std::make_tuple(state,std::unique_ptr<mqtt_package_type>(
			                         std::make_unique<mqtt_subscribe_type>(subscribe))));
	   } 
	case mqtt_control_packet_type::unsubscribe:
	   {
		   mqtt_unsubscribe_type unsubscribe;
		   auto state = parse_buffer(begin,end,header,unsubscribe);
		   return(std::make_tuple(state,std::unique_ptr<mqtt_package_type>(
			                          std::make_unique<mqtt_unsubscribe_type>(unsubscribe))));
	   } 
	case mqtt_control_packet_type::pingreq:
	   {
		   mqtt_pingreq_type ping;
		   return(std::make_tuple(result_type::good,
		                          std::unique_ptr<mqtt_package_type>(
			                          std::make_unique<mqtt_pingreq_type>(ping))));
	   }
	case mqtt_control_packet_type::disconnect:
	   {
		   mqtt_disconnect_type disconnect;
		   return(std::make_tuple(result_type::good,
		                          std::unique_ptr<mqtt_package_type>(
			                          std::make_unique<mqtt_disconnect_type>(disconnect))));
	   }
	default:
		return(std::make_tuple(result_type::bad,nullptr));
	} 
}

template<typename InputIterator>
result_type parse_buffer(InputIterator begin, InputIterator end,
                         const mqtt_header& header, mqtt_unsubscribe_type& message)
{
	try{
		message.header = header;
		begin = deserialize(begin,end,message.packet_id);
		while(begin != end) {
			uint16_t length;
			begin = deserialize(begin,end,length);
			std::string topic(' ',length);
			begin = deserialize(begin,end,topic);
		}
		return result_type::good;
	} catch(const std::out_of_range& ex){
		return result_type::bad;
	}
		
}

template<typename InputIterator>
result_type parse_buffer(InputIterator begin, InputIterator end,
                         const mqtt_header& header, mqtt_subscribe_type& message)
{
	try{
		message.header = header;
		begin = deserialize(begin,end,message.packet_id);
		while(begin != end) {
			uint16_t length;
			begin = deserialize(begin,end,length);
			std::string topic(' ',length);
			begin = deserialize(begin,end,topic);
			uint8_t qos;
			begin = deserialize(begin,end,qos);
			message.topics.emplace_back(qos,topic);
		}
		return result_type::good;
	} catch(const std::out_of_range& ex){
		return result_type::bad;
	}
		
}

template<typename InputIterator>
result_type parse_buffer(InputIterator begin, InputIterator end,
                         const mqtt_header& header, mqtt_puback_type& message)
{
	try{
		message.header = header;
		begin = deserialize(begin,end,message.packet_id);
		if(begin == end){
			return result_type::good;
		} else {
			return result_type::bad;
		}
	} catch(const std::out_of_range& ex){
		return result_type::bad;
	}
		
}

template<typename InputIterator>
result_type parse_buffer(InputIterator begin, InputIterator end,
                         const mqtt_header& header, mqtt_publish_type& message)
{

	try{
		message.header = header;
		uint16_t length;
		begin = deserialize(begin,end,length);
		message.topic_name = std::string(' ',length);
		begin = deserialize(begin,end,message.topic_name);
		begin = deserialize(begin,end,message.packet_id);
		std::size_t length_var_header = length + sizeof(length) * 2;
		std::size_t length_payload = message.header.remaining_length -
		   length_var_header;
		if(length_payload > 0){
			message.payload = std::vector<uint8_t>(length_payload);
			begin = deserialize(begin,end,message.payload);
		} else if (length_payload < 0) {
			return result_type::bad;
		}
		if(begin == end){
			return result_type::good;
		} else {
			return result_type::bad;
		}
	} catch(const std::out_of_range& ex){
		return result_type::bad;
	}
		
}
	
template<typename InputIterator>
result_type parse_buffer(InputIterator begin, InputIterator end,
                         const mqtt_header& header, mqtt_connect_type& message)
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
   
   
   
