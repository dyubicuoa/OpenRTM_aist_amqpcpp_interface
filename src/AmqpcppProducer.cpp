#include "AmqpcppProducer.h"

AmqpcppProducer::AmqpcppProducer()
{
  std::cout << "Producer constructor was called." << std::endl;
}

AmqpcppProducer::~AmqpcppProducer()
{
  std::cout << "Producer destructor was called." << std::endl;
}

void AmqpcppProducer::amqp_set_pub_attr(const std::string& exchangeName, const std::string& routingKey){
  aExchangeName = exchangeName;
  aRoutingKey = routingKey;

  std::cout << "- setting producer attributes done." << std::endl;
}

void AmqpcppProducer::amqp_publish(const std::string& payload){
  channel->publish(aExchangeName, aRoutingKey, payload.c_str(), payload.size());
  std::cout << "publish OK" << std::endl;
}

void AmqpcppProducer::amqp_publish(const char* payload, const uint64_t length){
  channel->publish(aExchangeName, aRoutingKey, payload, length);
}

void AmqpcppProducer::amqp_pub_transaction(const char* payload, const uint64_t length){
  channel->startTransaction();

  amqp_publish(payload, length);

  channel->commitTransaction()
  .onSuccess([](){
    // all messages were successfully published.
  })
  .onError([](const char* errmsg){
    std::cout << "[Commit Error] None of the messages were published: " << errmsg << std::endl;
  });
}

void AmqpcppProducer::amqp_pub_confirm(const char* payload, const uint64_t length){
  channel->confirmSelect()
  .onSuccess([&](){
    amqp_publish(payload, length);
  })
  .onAck([&](uint64_t deliverTag, bool multiple){
    // multiple will be set to true if all messages up to the deliverTag have been processed.
  })
  .onNack([&](uint64_t deliverTag, bool multiple, bool requeue){
    // multiple will be set to true if all messages up to the deliverTag have not been processed.
    std::cout << "[Negative Ack] Server is unable to process the message." << std::endl;
  });
}
