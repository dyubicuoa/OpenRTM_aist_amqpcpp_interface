#ifndef AMQPCPPCONSUMER_H
#define AMQPCPPCONSUMER_H

#include <iostream>
#include <string>

#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>

#include "AmqpcppManager.h"

class AmqpcppConsumer : public AmqpcppManager{
private:
  std::string aQueueName;
  std::string aConsumerTag;

public:
  AmqpcppConsumer();
  virtual ~AmqpcppConsumer();

  void amqp_set_sub_attr(const std::string& queueName);
  virtual void amqp_consume();
  void amqp_cancel();
  std::string amqp_get_qname();
  std::string amqp_get_ctag();
  void amqp_set_ctag(const std::string& consumerTag);
};

#endif
