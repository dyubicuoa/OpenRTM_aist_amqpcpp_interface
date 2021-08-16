#ifndef AMQPCPPPRODUCER_H
#define AMQPCPPPRODUCER_H

#include <iostream>
#include <string>

#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>

#include "AmqpcppManager.h"

class AmqpcppProducer : public AmqpcppManager{
private:
  std::string aExchangeName;
  std::string aRoutingKey;

public:
  AmqpcppProducer();
  virtual ~AmqpcppProducer();

  void amqp_set_pub_attr(const std::string& exchangeName, const std::string& routingKey);
  void amqp_publish(const std::string& payload);
  void amqp_publish(const char* payload, const uint64_t length);
  void amqp_pub_transaction(const char* payload, const uint64_t length);
  void amqp_pub_confirm(const char* payload, const uint64_t length);
};

#endif  
