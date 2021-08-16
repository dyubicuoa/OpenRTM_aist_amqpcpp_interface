#ifndef AMQPCPPMANAGER_H
#define AMQPCPPMANAGER_H

#include <iostream>
#include <string>

#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>

class AmqpcppManager{
protected:
  struct ev_loop* loop;
  AMQP::LibEvHandler* handler;
  AMQP::TcpConnection* connection;
  AMQP::TcpChannel* channel;

private:
  std::string aHost;
  std::string aPort;

public:
  AmqpcppManager();
  virtual ~AmqpcppManager();

  void amqp_connect(const std::string& host, const std::string& port);
  void amqp_disconnect();
  void amqp_evloop(const int flag);
  void amqp_evloop_thread();
  void amqp_declare_exchange(const std::string& exchangeName, const std::string& exchangeType);
  void amqp_declare_queue(const std::string& queueName);
  void amqp_bind(const std::string& exchangeName, const std::string& queueName, const std::string& bindingKey);
};

#endif
