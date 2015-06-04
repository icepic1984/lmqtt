#ifndef BROKER_HPP_
#define BROKER_HPP_


#include <string>
#include <vector>
#include <memory>

namespace lmqtt {

class mqtt_connection;
class mqtt_topic_parser;
class mqtt_subtree_node;

using mqtt_connection_weakptr = std::weak_ptr<int>;
using mqtt_subtree_node_ptr = std::unique_ptr<mqtt_subtree_node>;
using mqtt_connection_ptr = std::shared_ptr<int>;

struct mqtt_subtree_node 
{
   std::string topic_;
   std::vector<mqtt_connection_weakptr> leaves_;
   std::vector<mqtt_subtree_node_ptr> branches_;
};

class mqtt_subtree 
{
public:
   mqtt_subtree();
   void subscribe(const std::string& topic, const mqtt_connection_ptr connection);
   void subscribe(const std::vector<std::string>& topics, const mqtt_connection_ptr& connection);
   void unsubscribe(const std::vector<std::string>& topics, const mqtt_connection_ptr& connection);
   void unsubscribe(const std::string& topic, const mqtt_connection_ptr& connection);
   
   std::vector<mqtt_connection_ptr> publish(const std::string& topic);
private:
   std::vector<mqtt_connection_weakptr> publish(mqtt_subtree_node* node, mqtt_topic_parser& parser);
   void subscribe(mqtt_subtree_node* node ,
                  mqtt_topic_parser& parser,
                  const mqtt_connection_ptr& connection);
   void unsubscribe(mqtt_subtree_node* node, mqtt_topic_parser& parser, const mqtt_connection_ptr& connection);
   
   mqtt_subtree_node_ptr root_;
};

class mqtt_topic_parser 
{
public:
   mqtt_topic_parser(const std::string& topic);
   bool has_next();
   std::string next();
private:
   std::string topic_;
   std::size_t curr_pos_;
};

}

#endif
