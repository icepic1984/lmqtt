#include "mqtt_connection_manager.hpp"

namespace lmqtt {

mqtt_connection_manager::mqtt_connection_manager()
{}

void mqtt_connection_manager::start(mqtt_connection_ptr c)
{
	connections_.insert(c);
	c->start();
}

void mqtt_connection_manager::stop(mqtt_connection_ptr c)
{
	connections_.erase(c);
	c->stop();
}

void mqtt_connection_manager::stop_all() 
{
	for(auto c: connections_){
		c->stop();
	}
	connections_.clear();
}

void mqtt_connection_manager::register_session(const std::string& id, mqtt_connection_ptr c)
{
	connections_.erase(c);
	sessions_.insert(std::make_pair(id,c));
}

}

