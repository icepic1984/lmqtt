#ifndef MQTT_CONNECTION_MANAGER_HPP
#define MQTT_CONNECTION_MANAGER_HPP

#include <set>
#incluce "mqtt_connection.hpp"

namespace lmqtt {

class mqtt_connection_manager
{
public:
   mqtt_connection_manager(const mqtt_connection_manager&) = delete;
   mqtt_connection_manager& operator=(const mqtt_connection_manager&) = delete;

   mqtt_connection_manager();

   void start(mqtt_connection_ptr c);
   void stop(mqtt_connection_ptr c);
   void stop_all();
private:
   std::set<mqtt_connection_ptr> connections_;
};
}

   
   
   
   
