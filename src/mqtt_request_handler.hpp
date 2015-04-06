#ifndef REQUEST_HANDLER_HPP_
#define REQUEST_HANDLER_HPP_

namespace lmqtt {

class mqtt_connection_manager;

class mqtt_request_handler
{
public:

   mqtt_request_handler(const mqtt_request_handler&) = delete;
   mqtt_request_handler& operator=(const mqtt_request_handler&) = delete;

   explicit mqtt_request_handler(mqtt_connection_manager& manager);

private:
   mqtt_connection_manager& manager_;
   
};

}

#endif
