#include "AmqpcppConsumer.h"

/* main for test */
int main(int argc, char* argv[]){
  const std::string HOST("localhost");
  const std::string PORT("5672");
  const std::string Q_NAME("queue0");

  AmqpcppConsumer* rc = new AmqpcppConsumer();

  rc->amqp_connect(HOST, PORT);
  rc->amqp_declare_queue(Q_NAME);
  rc->amqp_set_sub_attr(Q_NAME);
  rc->amqp_consume();
  rc->amqp_evloop(3);
  rc->amqp_cancel();
  rc->amqp_disconnect();

  delete rc;

  return 0;
}
