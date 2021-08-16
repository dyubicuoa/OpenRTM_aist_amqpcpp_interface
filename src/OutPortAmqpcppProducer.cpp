// -*- C++ -*-
/*!
 * @file  OutPortAmqpcppProducer.h
 * @brief OutPortAmqpcppProducer class
 * @date  $Date: 2021-07-27 14:42:00 $
 * @author Daishi Yoshino
 *
 * Copyright (C) 2021
 *     Daishi Yoshino
 *     Revitalization Center
 *     University of Aizu, Japan
 * Originally under LGPL in OpenRTM-aist, http://www.openrtm.org/
 */

#include <rtm/NVUtil.h>
#include <OutPortAmqpcppProducer.h>
#include <cstdlib>
#include <string>

namespace RTC
{
  /*!
   * @brief Constructor
   */
  OutPortAmqpcppProducer::OutPortAmqpcppProducer(void)
    : rtclog("OutPortAmqpcppProducer")
  {
  }
  
  /*!
   * @brief Destructor
   */
  OutPortAmqpcppProducer::~OutPortAmqpcppProducer(void)
  {
    RTC_PARANOID(("~OutPortAmqpcppProducer()"));

    // Disconnect from AMQP Server
    std::cout << "[disconnecting from AMQP Server start]" << std::endl;
    amqp_disconnect();
    std::cout << "[disconnecting from AMQP Server end]" << std::endl;
  }

  /*!
   * @brief Initializing configuration
   */
  void OutPortAmqpcppProducer::init(coil::Properties& prop)
  {
    m_properties = prop;
  }

  /*!
   * @brief Write data into the buffer
   */
  InPortConsumer::ReturnCode OutPortAmqpcppProducer::
  put(cdrMemoryStream& data)
  {
    RTC_PARANOID(("put()"));

    const int bufleng = static_cast<int>(data.bufSize());
    const void* data_p = data.bufPtr();

    const char* cstr_data = static_cast<const char*>(data_p);

    try
      {
        //std::cout << "publsih start" << std::endl;
        amqp_publish(cstr_data, (uint64_t)bufleng);
        //std::cout << "publsih end" << std::endl;
        return InPortConsumer::PORT_OK;
      }
    catch (...)
      {
        return CONNECTION_LOST;
      }
    return UNKNOWN_ERROR;
  }
  
  /*!
   * @brief Publish InterfaceProfile information
   */
  void OutPortAmqpcppProducer::
  publishInterfaceProfile(SDOPackage::NVList& properties)
  {
    std::cout << "setting pub/sub information" << std::endl;
    return;
  }

  /*!
   * @brief Subscribe to the data sending notification
   */
  bool OutPortAmqpcppProducer::
  subscribeInterface(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("subscribeInterface()"));
    RTC_DEBUG_STR((NVUtil::toString(properties)));
    std::cout << "reading OutPort information" << std::endl;
    
    // getting Topic information
    if(subscribeAmqpcppProducer(properties)){
      return true;
    }

    return false;
  }
  
  /*!
   * @brief Unsubscribe the data send notification
   */
  void OutPortAmqpcppProducer::
  unsubscribeInterface(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("unsubscribeInterface()"));
    RTC_DEBUG_STR((NVUtil::toString(properties)));
    std::cout << "unsbscribing OutPort" << std::endl;

    return;
  }
  
  //----------------------------------------------------------------------
  // private functions

  /*!
   * @brief Find index of the properties
   */
  CORBA::Long OutPortAmqpcppProducer::
  findProp(const SDOPackage::NVList& properties, const char* key)
  {
    CORBA::Long index;

    index = NVUtil::find_index(properties, key);
    if(index >= 0){
      return index;
    }

    std::string part_key = "dataport.";
    part_key.append(key);
    index = NVUtil::find_index(properties, part_key.c_str());
    if(index >= 0){
      return index;
    }

    std::string full_key = part_key.append("outport.");
    full_key.append(key);
    index = NVUtil::find_index(properties, full_key.c_str());
    if(index >= 0){
      return index;
    }

    return -1;
  }

  /*!
   * @brief Settings to connect to AMQP broker
   */
  bool OutPortAmqpcppProducer::
  subscribeAmqpcppProducer(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("subscribeAmqpcppProducer()"));

    const char* PN_HOST = "host";
    const char* PN_EXNAME = "exname";
    const char* PN_EXTYPE = "extype";
    const char* PN_QNAME = "qname";
    const char* PN_BKEY = "bkey";
    const char* PN_BIND = "bind";
    const char* PN_RKEY = "rkey";
    const char* PN_PORT = "msport";

    CORBA::Long index0, index1, index2, index3, index4, index5, index6, index7;
    index0 = findProp(properties, PN_HOST);
    index1 = findProp(properties, PN_EXNAME);
    index2 = findProp(properties, PN_EXTYPE);
    index3 = findProp(properties, PN_QNAME);
    index4 = findProp(properties, PN_BKEY);
    index5 = findProp(properties, PN_BIND);
    index6 = findProp(properties, PN_RKEY);
    index7 = findProp(properties, PN_PORT);

    const char* tmp_host = "localhost";
    const char* tmp_exname = "exchange1";
    const char* tmp_extype = "0";
    const char* tmp_qname = "queue1";
    const char* tmp_bkey = "test";
    const char* cstr_bind ="false";
    bool tmp_bind = false;
    const char* tmp_rkey = "test";
    const char* tmp_port ="5672";

    if(index0 < 0)
    {
      //RTC_ERROR(("AMQP Host not found"));
      std::cout << "AMQP Host not found. Default Host(localhost) is used." << std::endl;
      //return false;
    }
    else{
      if(!(NVUtil::isString(properties, PN_HOST)))
      {
        //RTC_ERROR(("Host Name is not string"));
        std::cout << "Host Name is not string. Default Host(localhost) is used." << std::endl;
        //return false;
      }
      else{
        properties[index0].value >>= tmp_host;
      }
    }

    std::cout << "Host: " << tmp_host << std::endl;

    if(index1 < 0)
    {
      //RTC_ERROR(("AMQP Exchange name not found"));
      std::cout << "AMQP Exchange name not found. Default Exchange name is used." << std::endl;
      //return false;
    }
    else{
      if(!(NVUtil::isString(properties, PN_EXNAME)))
      {
        //RTC_ERROR(("Exchange Name is not string"));
        std::cout << "Exchange Name is not string. Default Exchange name is used." << std::endl;
        //return false;
      }
      else{
        properties[index1].value >>= tmp_exname;
      }
    }

    std::cout << "Exchange name: " << tmp_exname << std::endl;

    if(index2 < 0)
    {
      //RTC_ERROR(("AMQP Exchange type not found"));
      std::cout << "AMQP Exchange type not found. Default Exchange type(AMQP::direct) is used." << std::endl;
      //return false;
    }
    else{
      if(!(NVUtil::isString(properties, PN_EXTYPE)))
      {
        //RTC_ERROR(("Exchange type is not string"));
        std::cout << "Exchange type is not string. Default Exchange type(AMQP::direct) is used." << std::endl;
        //return false;
      }
      else{
        properties[index2].value >>= tmp_extype;
      }
    }

    std::cout << "Exchange type: " << tmp_extype << std::endl;

    if(index3 < 0)
    {
      //RTC_ERROR(("AMQP Queue name not found"));
      std::cout << "AMQP Queue name not found. Default Queue name is used." << std::endl;
      //return false;
    }
    else{
      if(!(NVUtil::isString(properties, PN_QNAME)))
      {
        //RTC_ERROR(("Queue Name is not string"));
        std::cout << "Queue Name is not string. Default Queue name is used." << std::endl;
        //return false;
      }
      else{
        properties[index3].value >>= tmp_qname;
      }
    }

    std::cout << "Queue name: " << tmp_qname << std::endl;

    if(index4 < 0)
    {
      //RTC_ERROR(("AMQP binding Key not found"));
      std::cout << "AMQP binding Key not found. Default binding Key is used." << std::endl;
      //return false;
    }
    else{
      if(!(NVUtil::isString(properties, PN_BKEY)))
      {
        //RTC_ERROR(("Binding Key is not string"));
        std::cout << "Binding Key is not string. Default binding Key is used." << std::endl;
        //return false;
      }
      else{
        properties[index4].value >>= tmp_bkey;
      }
    }

    std::cout << "Binding Key: " << tmp_bkey << std::endl;

    if(index5 < 0)
    {
      //RTC_ERROR(("AMQP binding true or false not found"));
      std::cout << "AMQP binding true or false not found. Binding is true." << std::endl;
      //return false;
    }
    else{
      if(!(NVUtil::isString(properties, PN_BIND)))
      {
        //RTC_ERROR(("Binding selection is not string"));
        std::cout << "Binding selection is not string. Binding is true." << std::endl;
        //return false;
      }
      else{
        properties[index5].value >>= cstr_bind;
        std::string str_bind = std::string(cstr_bind);
        if(str_bind == "true" || str_bind == "t" || str_bind == "True" || str_bind == "TRUE" || str_bind == "T" || str_bind == "1"){
          tmp_bind = true;
        }
      }
    }

    if(tmp_bind == true){
      std::cout << "binding: true" << std::endl;
    }
    else{
      std::cout << "binding: false" << std::endl;
    }

    if(index6 < 0)
    {
      //RTC_ERROR(("AMQP Routing Key not found"));
      std::cout << "AMQP Routing Key not found. Default Routing Key is used." << std::endl;
      //return false;
    }
    else{
      if(!(NVUtil::isString(properties, PN_RKEY)))
      {
        //RTC_ERROR(("Routing Key is not string"));
        std::cout << "Routing Key is not string. Default Routing Key is used." << std::endl;
        //return false;
      }
      else{
        properties[index6].value >>= tmp_rkey;
      }
    }

    std::cout << "Routing Key: " << tmp_rkey << std::endl;

    if(index7 < 0)
    {
      //RTC_ERROR(("AMQP Port not found"));
      std::cout << "AMQP Port not found. Default Port(5672) is used." << std::endl;
      //return false;
    }
    else{
      if(!(NVUtil::isString(properties, PN_PORT)))
      {
        //RTC_ERROR(("Port number is not string"));
        std::cout << "Port number is not string. Default Port(5672) is used." << std::endl;
        //return false;
      }
      else{
        properties[index7].value >>= tmp_port;
      }
    }

    std::cout << "Port: " << tmp_port << std::endl;

    std::cout << "[connecting to AMQP server start]" << std::endl;
    amqp_connect(std::string(tmp_host), std::string(tmp_port));
    amqp_declare_exchange(std::string(tmp_exname), std::string(tmp_extype));
    if(tmp_bind == true){
      amqp_declare_queue(std::string(tmp_qname));
      amqp_bind(std::string(tmp_exname), std::string(tmp_qname), std::string(tmp_bkey));
    }
    //amqp_evloop_thread();
    amqp_set_pub_attr(std::string(tmp_exname), std::string(tmp_rkey));
    std::cout << "[connecting to AMQP server end]" << std::endl;

    return true;
  }

  /*!
   * @brief Return codes conversion
   */
  InPortConsumer::ReturnCode
  OutPortAmqpcppProducer::convertReturnCode(OpenRTM::PortStatus ret)
  {
    switch (ret)
      {
      case OpenRTM::PORT_OK:
        return InPortConsumer::PORT_OK;
        break;
      case OpenRTM::PORT_ERROR:
        return InPortConsumer::PORT_ERROR;
        break;
      case OpenRTM::BUFFER_FULL:
        return InPortConsumer::SEND_FULL;
        break;
      case OpenRTM::BUFFER_TIMEOUT:
        return InPortConsumer::SEND_TIMEOUT;
        break;
      case OpenRTM::UNKNOWN_ERROR:
        return InPortConsumer::UNKNOWN_ERROR;
        break;
      default:
        return InPortConsumer::UNKNOWN_ERROR;
        break;
      }
    return InPortConsumer::UNKNOWN_ERROR;
  }
  
};     // namespace RTC

extern "C"
{ 
  /*!
   * @brief Module initialization
   */
  void OutPortAmqpcppProducerInit(void)
  {
    RTC::InPortConsumerFactory& factory(RTC::InPortConsumerFactory::instance());
    factory.addFactory("amqp_cdr",
                       ::coil::Creator< ::RTC::InPortConsumer,
                                        ::RTC::OutPortAmqpcppProducer>,
                       ::coil::Destructor< ::RTC::InPortConsumer,
                                           ::RTC::OutPortAmqpcppProducer>);
  }
};

void registerModule(void)
{
  std::cout << "[AMQPCPP Producer initialization start]" << std::endl;
  OutPortAmqpcppProducerInit();
  std::cout << "[AMQPCPP Producer initialization end]" << std::endl;
};
