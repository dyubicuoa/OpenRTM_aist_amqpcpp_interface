#include <unistd.h>
#include "AmqpcppProducer.h"

/* main for test */
int main(int argc, char* argv[]){
  const std::string HOST("localhost");
  const std::string PORT("5672");
  const std::string EX_NAME("ex1");
  const std::string EX_TYPE("0");
  const std::string Q_NAME("queue0");
  const std::string B_KEY("test");
  const std::string R_KEY("test");
  const std::string MESSAGE("test message !");

  AmqpcppProducer* rp = new AmqpcppProducer();

  rp->amqp_connect(HOST, PORT);
  rp->amqp_declare_exchange(EX_NAME, EX_TYPE);
  rp->amqp_declare_queue(Q_NAME);
  rp->amqp_bind(EX_NAME, Q_NAME, B_KEY);
  rp->amqp_evloop_thread();
  rp->amqp_set_pub_attr(EX_NAME, R_KEY);
  rp->amqp_publish(MESSAGE);
  sleep(2);
  rp->amqp_publish(MESSAGE);
  sleep(3);
  rp->amqp_publish(MESSAGE);
  rp->amqp_disconnect();

  delete rp;

  return 0;
}
