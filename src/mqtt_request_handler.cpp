#include "mqtt_request_handler.hpp"
#include "mqtt_connection_manager.hpp"

namespace lmqtt {

mqtt_request_handler::mqtt_request_handler(mqtt_connection_manager& manager):
	manager_(manager)
{}

}
