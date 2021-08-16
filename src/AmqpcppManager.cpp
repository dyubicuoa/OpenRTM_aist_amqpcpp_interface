#include <thread>
#include "AmqpcppManager.h"

AmqpcppManager::AmqpcppManager()
{
  loop = EV_DEFAULT;

  std::cout << "Manager constructor was called." << std::endl;
}

AmqpcppManager::~AmqpcppManager()
{
  delete channel;
  delete connection;
  delete handler;

  std::cout << "Manager destructor was called." << std::endl;
}

void AmqpcppManager::amqp_connect(const std::string& host, const std::string& port){
  aHost = host;
  aPort = port;
  std::string uri = "amqp://";
  uri.append(aHost);
  uri.push_back(':');
  uri.append(aPort);
  std::cout << "RabbitMQ host name: " << uri << std::endl;
  AMQP::Address address(uri);

  handler = new AMQP::LibEvHandler(loop);
  connection = new AMQP::TcpConnection(handler, address);
  channel = new AMQP::TcpChannel(connection);

  std::cout << "channel connected?: " << channel->connected() << std::endl;
}

void AmqpcppManager::amqp_disconnect(){
  channel->close();
  connection->close();
}

void AmqpcppManager::amqp_evloop(const int flag){
  if(flag == 0){
    ev_run(loop, 0);
  }
  else if(flag == 1){
    ev_run(loop, EVRUN_NOWAIT);
  }
  else if(flag == 2){
    ev_run(loop, EVRUN_ONCE);
  }
  else{
    ev_run(loop);
  }
}

void AmqpcppManager::amqp_evloop_thread(){
  std::thread th1([this]{amqp_evloop(3);});
  th1.detach();
  //th1.join();
}

void AmqpcppManager::amqp_declare_exchange(const std::string& exchangeName, const std::string& exchangeType){
  AMQP::ExchangeType amqpExchangeType;

  if(exchangeType == "0" || exchangeType == "d" || exchangeType == "D" || exchangeType == "direct" || exchangeType == "AMQP::direct"){
    amqpExchangeType = AMQP::direct;
  }
  else if(exchangeType =="1" || exchangeType == "f" || exchangeType == "F" || exchangeType == "fanout" || exchangeType == "AMQP::fanout"){
    amqpExchangeType = AMQP::fanout;
  }
  else if(exchangeType == "2" || exchangeType == "t" || exchangeType == "T" || exchangeType == "topic" || exchangeType == "AMQP::topic"){
    amqpExchangeType = AMQP::topic;
  }
  else if(exchangeType == "3" || exchangeType == "h" || exchangeType == "H" || exchangeType == "headers" || exchangeType == "AMQP::headers"){
    amqpExchangeType = AMQP::headers;
  }
  else{
    amqpExchangeType = AMQP::consistent_hash;
  }

  channel->declareExchange(exchangeName, amqpExchangeType).onSuccess([](){
    std::cout << "[Success] declared exchange: " << std::endl;
  }).onError([](const char* errmsg){
    std::cout << "[Error] not declared exchange: " << errmsg << std::endl;
  });
}

void AmqpcppManager::amqp_declare_queue(const std::string& queueName){
  channel->declareQueue(queueName).onSuccess([](const std::string& qname, int msgcnt, int consumercnt){
    std::cout << "[Success] declared queue: " << qname << std::endl;
    std::cout << "          stored messages: " << msgcnt << std::endl;
    std::cout << "          connected consumers: " << consumercnt << std::endl;
  }).onError([](const char* errmsg){
    std::cout << "[Error] not declared queue: " << errmsg << std::endl;
  });
}

void AmqpcppManager::amqp_bind(const std::string& exchangeName, const std::string& queueName, const std::string& bindingKey){
  channel->onReady([]() {
    std::cout << "onReady" << std::endl;
  });

  channel->bindQueue(exchangeName, queueName, bindingKey).onSuccess([](){
    std::cout << "[Success] binding between an exchange and a queue: " << std::endl;
  });
}

