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

#ifndef RTC_OUTPORTAMQPCPPPRODUCER_H
#define RTC_OUTPORTAMQPCPPPRODUCER_H


#include <rtm/idl/DataPortSkel.h>
#include <rtm/InPortConsumer.h>
#include <rtm/Manager.h>

#include "AmqpcppProducer.h"

namespace RTC
{
  /*!
   * @class OutPortAmqpcppProducer
   * @brief OutPortAmqpcppProducer class
   */
  class OutPortAmqpcppProducer
    : public InPortConsumer, public AmqpcppProducer
  {
  public:
    DATAPORTSTATUS_ENUM
    /*!
     * @brief Constructor
     */
    OutPortAmqpcppProducer(void);
    
    /*!
     * @brief Destructor
     */
    virtual ~OutPortAmqpcppProducer(void);

    /*!
     * @brief Initializing configuration
     */
    virtual void init(coil::Properties& prop);

    /*!
     * @brief Send data to the destination port
     */
    virtual ReturnCode put(cdrMemoryStream& data);

    /*!
     * @brief Publish InterfaceProfile information
     */
    virtual void publishInterfaceProfile(SDOPackage::NVList& properties);

    /*!
     * @brief Subscribe to the data sending notification
     */
    virtual bool subscribeInterface(const SDOPackage::NVList& properties);
    
    /*!
     * @brief Unsubscribe the data send notification
     */
    virtual void unsubscribeInterface(const SDOPackage::NVList& properties);

  private:
    /*!
     * @brief Find index of the properties
     */
    CORBA::Long findProp(const SDOPackage::NVList& properties, const char* key);

    /*!
     * @brief Settings to connect to AMQP broker
     */
    bool subscribeAmqpcppProducer(const SDOPackage::NVList& properties);
    
  private:
    /*!
     * @brief Return codes conversion
     */
    InPortConsumer::ReturnCode convertReturnCode(OpenRTM::PortStatus ret);

    mutable Logger rtclog;
    coil::Properties m_properties;
  };
};     // namespace RTC

extern "C"
{
  /*!
   * @brief Module initialization
   */
  void OutPortAmqpcppProducerInit(void);
};

#endif // RTC_OUTPORTAMQPCPPPRODUCER_H
