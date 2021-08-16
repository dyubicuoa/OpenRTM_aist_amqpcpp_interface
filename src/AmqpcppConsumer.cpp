#include "AmqpcppConsumer.h"

AmqpcppConsumer::AmqpcppConsumer()
{
  std::cout << "Consumer constructor was called." << std::endl;
}

AmqpcppConsumer::~AmqpcppConsumer()
{
  std::cout << "Consumer destructor was called." << std::endl;
}

void AmqpcppConsumer::amqp_set_sub_attr(const std::string& queueName){
  aQueueName = queueName;

  std::cout << "- setting consumer attributes done." << std::endl;
}

void AmqpcppConsumer::amqp_consume(){
  channel->consume(aQueueName)
  .onReceived([&](const AMQP::Message &msg, uint64_t deliveryTag, bool redelivered){
    std::string originalMsg(msg.body(), msg.bodySize());
    std::cout << "[Received] message: " << originalMsg << std::endl;

    // acknowlegde the message to RabbitMQ server
    channel->ack(deliveryTag);
  })
  .onSuccess([&](const std::string& tag){
    aConsumerTag = tag;

    std::cout << "[Success] consume operation started!" << std::endl;
    std::cout << "          consumer tag: " << aConsumerTag << std::endl;
  })
  .onError([](const char *errmsg){
    std::cout << "[Error] consume operation failed: " << errmsg << std::endl;
  });
}

void AmqpcppConsumer::amqp_cancel(){
  channel->cancel(aConsumerTag)
  .onSuccess([](){
    std::cout << "[Success] canceled consume operation." << std::endl;
  })
  .onError([](const char* errmsg){
    std::cout << "[Error] canceling consume operation failed: " << errmsg << std::endl;
  });
}

std::string AmqpcppConsumer::amqp_get_qname(){
  return aQueueName;
}

std::string AmqpcppConsumer::amqp_get_ctag(){
  return aConsumerTag;
}

void AmqpcppConsumer::amqp_set_ctag(const std::string& consumerTag){
  aConsumerTag = consumerTag;
}
