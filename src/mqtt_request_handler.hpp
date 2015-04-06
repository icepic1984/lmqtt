#ifndef REQUEST_HANDLER_HPP_
#define REQUEST_HANDLER_HPP_

namespace lmqtt {

class mqtt_connection_manager;
class mqtt_package_type;

class mqtt_request_handler
{
public:

   mqtt_request_handler(const mqtt_request_handler&) = delete;
   mqtt_request_handler& operator=(const mqtt_request_handler&) = delete;

   explicit mqtt_request_handler(mqtt_connection_manager& manager);
   void handle_request(const mqtt_package_type& type);

private:
   mqtt_connection_manager& manager_;
   
};

}

#endif
