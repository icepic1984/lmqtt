#ifndef MQTT_CONNECTION_MANAGER_HPP
#define MQTT_CONNECTION_MANAGER_HPP

#include <set>
#include <unordered_map>
#include "mqtt_connection.hpp"

namespace lmqtt {

class mqtt_connection_manager
{
public:
   mqtt_connection_manager(const mqtt_connection_manager&) = delete;
   mqtt_connection_manager& operator=(const mqtt_connection_manager&) = delete;

   mqtt_connection_manager();

   void start(mqtt_connection_ptr c);
   void stop(mqtt_connection_ptr c);
   void register_session(const std::string& id, mqtt_connection_ptr c);
   void stop_all();
private:
   std::set<mqtt_connection_ptr> connections_;
   std::unordered_map<std::string,mqtt_connection_ptr> sessions_;
};
}

#endif   
   
   
   
