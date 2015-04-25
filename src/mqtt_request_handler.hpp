#ifndef REQUEST_HANDLER_HPP_
#define REQUEST_HANDLER_HPP_

#include <vector>
#include <cstdint>

namespace lmqtt {

class mqtt_connection_manager;
class mqtt_package_type;

enum class mqtt_request_action {nothing, disconnect, response};
   
class mqtt_request_handler
{
public:

   mqtt_request_handler(const mqtt_request_handler&) = delete;
   mqtt_request_handler& operator=(const mqtt_request_handler&) = delete;

   explicit mqtt_request_handler(mqtt_connection_manager& manager);
   mqtt_request_action handle_request(mqtt_package_type *message);
   std::vector<uint8_t> get_buffer() const;

private:
   mqtt_connection_manager& manager_;
   std::vector<uint8_t> buffer_;
};

}

#endif
