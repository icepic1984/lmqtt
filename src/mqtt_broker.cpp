#include "mqtt_broker.hpp"
#include <iostream>
#include <algorithm>

namespace lmqtt {

mqtt_subtree::mqtt_subtree():
	root_(std::make_unique<mqtt_subtree_node>())
{}

void mqtt_subtree::subscribe(const std::vector<std::string>& topics,
                             const mqtt_connection_ptr& connection)
{
	for(auto& iter :  topics){
		mqtt_topic_parser parser(iter);
		subscribe(root_.get(),parser,connection);
	}
}

void mqtt_subtree::subscribe(const std::string& topic, const mqtt_connection_ptr connection)
{
	mqtt_topic_parser parser(topic);
	subscribe(root_.get(),parser,connection);
}

std::vector<mqtt_connection_ptr>  mqtt_subtree::publish(const std::string& topic)
{
	std::vector<mqtt_connection_ptr> result;
	mqtt_topic_parser parser(topic);
	auto ptrs =  publish(root_.get(),parser);
	for(auto& iter : ptrs) {
		if(auto sp = iter.lock()){
			result.emplace_back(sp);
		}
	}
	return result;
}

std::vector<mqtt_connection_weakptr> mqtt_subtree::publish(mqtt_subtree_node* node,
                                                           mqtt_topic_parser& parser)
{
	if(parser.has_next()){
		auto tmp = parser.next();
		auto iter = std::find_if(node->branches_.begin(),
		                         node->branches_.end(),[tmp](const mqtt_subtree_node_ptr& ptr)
		          {
			          if(ptr == nullptr) return false;
			          if(ptr->topic_ == tmp) return true;
			          return false;
		          });
		if( iter != node->branches_.end()){
			return publish((*iter).get(),parser);
		}	
	} else {
		return node->leaves_;
	}
	return std::vector<mqtt_connection_weakptr>();
}

void mqtt_subtree::subscribe(mqtt_subtree_node* node ,
                             mqtt_topic_parser& parser,
                             const mqtt_connection_ptr& connection)
{
	if(parser.has_next()){
		auto tmp = parser.next();
		auto iter = std::find_if(node->branches_.begin(),
		                         node->branches_.end(),[tmp](const mqtt_subtree_node_ptr& ptr)
		          {
			          if(ptr == nullptr) return false;
			          if(ptr->topic_ == tmp) return true;
			          return false;
		          });
		if( iter != node->branches_.end()){
			subscribe((*iter).get(),parser,connection);
		} else {
			node->branches_.push_back(std::make_unique<mqtt_subtree_node>());
			node->branches_.back()->topic_ = tmp;
			subscribe(node->branches_.back().get(),parser,connection);
		}
	} else {
		node->leaves_.push_back(std::weak_ptr<int>(connection));
	}
}

void mqtt_subtree::unsubscribe(const std::string& topic, const mqtt_connection_ptr& connection)
{
	mqtt_topic_parser parser(topic);
	unsubscribe(root_.get(),parser,connection);
}

void mqtt_subtree::unsubscribe(const std::vector<std::string>& topics,
                               const mqtt_connection_ptr& connection)
{
	for(auto& iter : topics){
		mqtt_topic_parser parser(iter);
		unsubscribe(root_.get(),parser,connection);
	}
}


void mqtt_subtree::unsubscribe(mqtt_subtree_node* node,
                               mqtt_topic_parser& parser,
                               const mqtt_connection_ptr& connection)
{
	if(parser.has_next()){
		auto tmp = parser.next();
		auto iter = std::find_if(node->branches_.begin(),
		                         node->branches_.end(),[tmp](const mqtt_subtree_node_ptr& ptr)
		          {
			          if(ptr == nullptr) return false;
			          if(ptr->topic_ == tmp) return true;
			          return false;
		          });
		if( iter != node->branches_.end()){
			unsubscribe((*iter).get(),parser,connection);
		}
	} else {
		node->leaves_.erase(std::remove_if(node->leaves_.begin(),
		               node->leaves_.end(),[&connection](const mqtt_connection_weakptr& weak)
		               {
			               if(!weak.expired()){
				               if(weak.lock() == connection){
					               return true;
				               }
			               }
			               return false;
		               }),node->leaves_.end());
		
	}
}


mqtt_topic_parser::mqtt_topic_parser(const std::string& topic) :
	topic_(topic),
	curr_pos_(0)
{
	if(topic_.size() == 0){
		throw std::length_error("Invalid size");
	}
}

bool mqtt_topic_parser::has_next()
{
	if(curr_pos_ == topic_.size() )
		return false;
	return true;
}

std::string mqtt_topic_parser::next()
{
	auto pos = topic_.find("/",curr_pos_);
	if(pos != std::string::npos){
		auto tmp = topic_.substr(curr_pos_,pos-curr_pos_);
		curr_pos_ = pos + 1;
		return tmp;
	} else {
		auto tmp = topic_.substr(curr_pos_,topic_.size()-curr_pos_);
		curr_pos_ = topic_.size();
		return tmp;
	}
}
 
}
