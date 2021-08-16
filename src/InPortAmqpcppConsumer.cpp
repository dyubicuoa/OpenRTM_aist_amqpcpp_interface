// -*- C++ -*-
/*!
 * @file  InPortAmqpcppConsumer.cpp
 * @brief InPortAmqpcppConsumer class
 * @date  $Date: 2021-07-27 14:42:00 $
 * @author Daishi Yoshino
 *
 * Copyright (C) 2021
 *     Daishi Yoshino
 *     Revitalization Center
 *     University of Aizu, Japan
 * Originally under LGPL in OpenRTM-aist, http://www.openrtm.org/
 */

#include <InPortAmqpcppConsumer.h>
#include <cstdlib>

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

namespace RTC
{
/*****************************************************************************
 * messageReceiveCallback
 *****************************************************************************/
void InPortAmqpcppConsumer::amqp_consume(){
  channel->consume(amqp_get_qname())
  .onReceived([&](const AMQP::Message &msg, uint64_t deliveryTag, bool redelivered){
    const char* pld = msg.body();
    void* part = (void*)pld;
    const uint64_t len = msg.bodySize();

    ::OpenRTM::CdrData tmp((CORBA::ULong)(len), 
                           (CORBA::ULong)(len), 
                           static_cast<CORBA::Octet*>(part), 0);

    if (m_buffer == 0)
    {
      cdrMemoryStream cdr;
      cdr.put_octet_array(&(tmp[0]), tmp.length());
      onReceiverError(cdr);
    }

    RTC_PARANOID(("received data size: %d", tmp.length()))
    cdrMemoryStream cdr;
    // set endian type
    bool endian_type = m_connector->isLittleEndian();
    RTC_TRACE(("connector endian: %s", endian_type ? "little":"big"));
    cdr.setByteSwapFlag(endian_type);
    cdr.put_octet_array(&(tmp[0]), tmp.length());

    RTC_PARANOID(("converted CDR data size: %d", cdr.bufSize()));
    onReceived(cdr);
    BufferStatus::Enum ret = m_buffer->write(cdr);

    convertReturn(ret, cdr);

    channel->ack(deliveryTag);
  })
  .onSuccess([&](const std::string& tag){
    amqp_set_ctag(tag);

    std::cout << "[Success] consume operation started!" << std::endl;
    std::cout << "          consumer tag: " << amqp_get_ctag() << std::endl;
  })
  .onError([](const char *errmsg){
    std::cout << "[Error] consume operation failed: " << errmsg << std::endl;
  });
}

  /*!
   * @brief Constructor
   */
  InPortAmqpcppConsumer::InPortAmqpcppConsumer(void)
   : m_buffer(0) 
  {
    // PortProfile setting
    setInterfaceType("amqp_cdr");
  }
  
  /*!
   * @brief Destructor
   */
  InPortAmqpcppConsumer::~InPortAmqpcppConsumer(void)
  {
    // disconnect from AMQP Server
    std::cout << "[disconnecting from AMQP Server start]" << std::endl;
    amqp_cancel();
    amqp_disconnect();
    std::cout << "[disconnecting from AMQP Server end]" << std::endl;
  }

  void InPortAmqpcppConsumer::init(coil::Properties& prop)
  {
  }

  /*!
   * @brief Setting outside buffer's pointer
   */
  void InPortAmqpcppConsumer::
  setBuffer(BufferBase<cdrMemoryStream>* buffer)
  {
    m_buffer = buffer;
  }

  /*!
   * @brief Set the listener
   */
  void InPortAmqpcppConsumer::setListener(ConnectorInfo& info,
                                           ConnectorListeners* listeners)
  {
    m_profile = info;
    m_listeners = listeners;
  }

  /*!
   * @brief set Connector
   */
  void InPortAmqpcppConsumer::setConnector(InPortConnector* connector)
  {
    m_connector = connector;
  }

  /*!
   * @brief Return codes conversion
   */
  ::OpenRTM::PortStatus
  InPortAmqpcppConsumer::convertReturn(BufferStatus::Enum status,
                                        cdrMemoryStream& data)
  {
    switch(status)
      {
      case BufferStatus::BUFFER_OK:
        onBufferWrite(data);
        return ::OpenRTM::PORT_OK;
        break;

      case BufferStatus::BUFFER_ERROR:
        onReceiverError(data);
        return ::OpenRTM::PORT_ERROR;
        break;

      case BufferStatus::BUFFER_FULL:
        onBufferFull(data);
        onReceiverFull(data);
        return ::OpenRTM::BUFFER_FULL;
        break;

      case BufferStatus::BUFFER_EMPTY:
        return ::OpenRTM::BUFFER_EMPTY;
        break;

      case BufferStatus::PRECONDITION_NOT_MET:
        onReceiverError(data);
        return ::OpenRTM::PORT_ERROR;
        break;

      case BufferStatus::TIMEOUT:
        onBufferWriteTimeout(data);
        onReceiverTimeout(data);
        return ::OpenRTM::BUFFER_TIMEOUT;
        break;

      default:
        return ::OpenRTM::UNKNOWN_ERROR;
      }

    onReceiverError(data);
    return ::OpenRTM::UNKNOWN_ERROR;
  }

  /*!
   * @brief Publish InterfaceProfile information
   */
  void InPortAmqpcppConsumer::publishInterfaceProfile(SDOPackage::NVList& properties){
    std::cout << "setting pub/sub information" << std::endl;
    return;
  }

  /*!
   * @brief Publish Interface information
   */
  bool InPortAmqpcppConsumer::publishInterface(SDOPackage::NVList& properties)
  {
    RTC_TRACE(("publishInterface()"));
    RTC_DEBUG_STR((NVUtil::toString(properties)));
    std::cout << "reading InPort infomation" << std::endl;

   // getting Topic information
   if(subscribeAmqpcppConsumer(properties)){
     return true;
   }

   return false;
  }

  /*!
   * @brief Find index of the properties
   */
  CORBA::Long InPortAmqpcppConsumer::
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

    std::string full_key = part_key.append("inport.");
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
  bool InPortAmqpcppConsumer::
  subscribeAmqpcppConsumer(const SDOPackage::NVList& properties)
  {
    RTC_TRACE(("subscribeAmqpcppConsumer()"));

    const char* PN_HOST = "host";
    const char* PN_EXNAME = "exname";
    const char* PN_EXTYPE = "extype";
    const char* PN_QNAME = "qname";
    const char* PN_BKEY = "bkey";
    const char* PN_BIND = "bind";
    const char* PN_PORT = "msport";

    CORBA::Long index0, index1, index2, index3, index4, index5, index6;
    index0 = findProp(properties, PN_HOST);
    index1 = findProp(properties, PN_EXNAME);
    index2 = findProp(properties, PN_EXTYPE);
    index3 = findProp(properties, PN_QNAME);
    index4 = findProp(properties, PN_BKEY);
    index5 = findProp(properties, PN_BIND);
    index6 = findProp(properties, PN_PORT);

    const char* tmp_host = "localhost";
    const char* tmp_exname = "exchange0";
    const char* tmp_extype = "0"; // 0 = direct, 1 = fanout, 2 = topic
    const char* tmp_qname = "queue0";
    const char* tmp_bkey = "test";
    const char* cstr_bind ="true";
    bool tmp_bind = true;
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
      //std::cout << "AMQP binding true or false not found. Binding is false." << std::endl;
      //return false;
    }
    else{
      if(!(NVUtil::isString(properties, PN_BIND)))
      {
        //RTC_ERROR(("Binding selection is not string"));
        std::cout << "Binding selection is not string. Binding is true." << std::endl;
        //std::cout << "Binding selection is not string. Binding is false." << std::endl;
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
        properties[index6].value >>= tmp_port;
      }
    }

    std::cout << "Port: " << tmp_port << std::endl;

    std::cout << "[connecting to AMQP Server start]" << std::endl;
    amqp_connect(std::string(tmp_host), std::string(tmp_port));
    amqp_declare_queue(std::string(tmp_qname));
    if(tmp_bind == true){
      amqp_declare_exchange(std::string(tmp_exname), std::string(tmp_extype));
      amqp_bind(std::string(tmp_exname), std::string(tmp_qname), std::string(tmp_bkey));
    }
    amqp_set_sub_attr(std::string(tmp_qname));
    amqp_consume();
    amqp_evloop_thread();
    std::cout << "[connecting to AMQP Server end]" << std::endl;

    return true;
  }
};     // namespace RTC


extern "C"
{
  /*!
   * @brief Module initialization
   */
  void InPortAmqpcppConsumerInit(void)
  {
    RTC::InPortProviderFactory& factory(RTC::InPortProviderFactory::instance());
    factory.addFactory("amqp_cdr",
                       ::coil::Creator< ::RTC::InPortProvider,
                                        ::RTC::InPortAmqpcppConsumer>,
                       ::coil::Destructor< ::RTC::InPortProvider,
                                           ::RTC::InPortAmqpcppConsumer>);
  }
};

void registerModule(void)
{
  std::cout << "[AMQPCPP Consumer initialization start]" << std::endl;
  InPortAmqpcppConsumerInit();
  std::cout << "[AMQPCPP Consumer initialization end]" << std::endl;
};
