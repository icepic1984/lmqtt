#define BOOST_TEST_MODULE broker

#include <iostream>
#include <boost/test/unit_test.hpp>
#include <memory>
#include <stdexcept>
#include "mqtt_broker.hpp"

BOOST_AUTO_TEST_CASE(topic_parser)
{
	lmqtt::mqtt_topic_parser parser("a/b/c");
	bool hn = parser.has_next();
	std::string topic = parser.next();
	BOOST_CHECK_EQUAL(hn,true);
	BOOST_CHECK_EQUAL(topic,std::string("a"));
	hn = parser.has_next();	
	topic = parser.next();
	BOOST_CHECK_EQUAL(hn,true);
	BOOST_CHECK_EQUAL(topic,std::string("b"));
	hn = parser.has_next();	
	topic = parser.next();
	BOOST_CHECK_EQUAL(hn,true);
	BOOST_CHECK_EQUAL(topic,std::string("c"));
	BOOST_CHECK_THROW(lmqtt::mqtt_topic_parser bla("") , std::length_error );
}

BOOST_AUTO_TEST_CASE(subtree)
{
	lmqtt::mqtt_subtree tree;
	std::shared_ptr<int> a =
	   std::make_shared<int>();
	std::shared_ptr<int> b =
	   std::make_shared<int>();
	tree.subscribe("test/fuck/you",a);
	tree.subscribe("test/fuck/me",a);
	tree.subscribe("test/fuck/me",a);
	tree.subscribe("test/fuck/me",a);
	auto ptrs = tree.publish("test/fuck/him");
	BOOST_CHECK_EQUAL(ptrs.size(),0);
	ptrs = tree.publish("test/fuck/you");
	BOOST_CHECK_EQUAL(ptrs.size(),1);
	ptrs = tree.publish("test/fuck/me");
	BOOST_CHECK_EQUAL(ptrs.size(),3);
	tree.unsubscribe("test/fuck/me",a);
	ptrs = tree.publish("test/fuck/me");
	BOOST_CHECK_EQUAL(ptrs.size(),0);	
	tree.subscribe({"test/fuck/me","test/fuck/you","test/fuck/him"},a);
	tree.unsubscribe({"test/fuck/me","test/fuck/you","test/fuck/him"},a);
	ptrs = tree.publish("test/fuck/me");
	BOOST_CHECK_EQUAL(ptrs.size(),0);
	ptrs = tree.publish("test/fuck/you");
	BOOST_CHECK_EQUAL(ptrs.size(),0);
	ptrs = tree.publish("test/fuck/him");
	BOOST_CHECK_EQUAL(ptrs.size(),0);
	tree.subscribe("test/yeah",b);
	{
		auto ptr1 = tree.publish("test/yeah");
		BOOST_CHECK_EQUAL(ptr1.size(),1);
	}
	b.reset(new int);
	{
		auto ptr1 = tree.publish("test/yeah");
		BOOST_CHECK_EQUAL(ptr1.size(),0);
	}
	
}

	
