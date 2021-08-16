// -*- C++ -*-
/*!
 * @file  InPortAmqpcppConsumer.h
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

#ifndef RTC_INPORTAMQPCPPCONSUMER_H
#define RTC_INPORTAMQPCPPCONSUMER_H

#include <rtm/idl/DataPortSkel.h>
#include <rtm/BufferBase.h>
#include <rtm/InPortProvider.h>
#include <rtm/CORBA_SeqUtil.h>
#include <rtm/Manager.h>
#include <rtm/ConnectorListener.h>
#include <rtm/ConnectorBase.h>

#include "AmqpcppConsumer.h"

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif


namespace RTC
{
  /*!
   * @class InPortAmqpcppConsumer
   * @brief InPortAmqpcppConsumer class
   */
  class InPortAmqpcppConsumer
    : public InPortProvider, public AmqpcppConsumer
  {
  public:
    /*!
     * @brief Constructor
     */
    InPortAmqpcppConsumer(void);
    
    /*!
     * @brief Destructor
     */
    virtual ~InPortAmqpcppConsumer(void);

    /*!
     * @brief Initializing configuration
     */
    virtual void init(coil::Properties& prop);

    /*!
     * @brief Set buffer
     */
    virtual void setBuffer(BufferBase<cdrMemoryStream>* buffer);

    /*!
     * @brief Set listener. 
     */
    virtual void setListener(ConnectorInfo& info,
                             ConnectorListeners* listeners);

    /*!
     * @brief set Connector
     */
    virtual void setConnector(InPortConnector* connector);

    /*!
     * @brief Publish InterfaceProfle information
     */
    virtual void publishInterfaceProfile(SDOPackage::NVList& properties);

    /*!
     * @brief Publish interface information
     */
    virtual bool publishInterface(SDOPackage::NVList& properties);

    /*************************************************************************
     * messageReceiveCallback
     *************************************************************************/
     virtual void amqp_consume();

  private:
    /*!
     * @brief Return codes conversion
     */
    ::OpenRTM::PortStatus
    convertReturn(BufferStatus::Enum status,
                  cdrMemoryStream& data);

    
    /*!
     * @brief Notify an ON_BUFFER_WRITE event to listeners
     */
    inline void onBufferWrite(cdrMemoryStream& data)
    {
      m_listeners->
        connectorData_[ON_BUFFER_WRITE].notify(m_profile, data);
    }

    /*!
     * @brief Notify an ON_BUFFER_FULL event to listeners
     */
    inline void onBufferFull(cdrMemoryStream& data)
    {
      m_listeners->
        connectorData_[ON_BUFFER_FULL].notify(m_profile, data);
    }

    /*!
     * @brief Notify an ON_BUFFER_WRITE_TIMEOUT event to listeners
     */
    inline void onBufferWriteTimeout(cdrMemoryStream& data)
    {
      m_listeners->
        connectorData_[ON_BUFFER_WRITE_TIMEOUT].notify(m_profile, data);
    }

    /*!
     * @brief Notify an ON_BUFFER_WRITE_OVERWRITE event to listeners
     */
    inline void onBufferWriteOverwrite(cdrMemoryStream& data)
    {
      m_listeners->
        connectorData_[ON_BUFFER_OVERWRITE].notify(m_profile, data);
    }

    /*!
     * @brief Notify an ON_RECEIVED event to listeners
     */
    inline void onReceived(cdrMemoryStream& data)
    {
      m_listeners->
        connectorData_[ON_RECEIVED].notify(m_profile, data);
    }

    /*!
     * @brief Notify an ON_RECEIVER_FULL event to listeners
     */
    inline void onReceiverFull(cdrMemoryStream& data)
    {
      m_listeners->
        connectorData_[ON_RECEIVER_FULL].notify(m_profile, data);
    }

    /*!
     * @brief Notify an ON_RECEIVER_TIMEOUT event to listeners
     */
    inline void onReceiverTimeout(cdrMemoryStream& data)
    {
      m_listeners->
        connectorData_[ON_RECEIVER_TIMEOUT].notify(m_profile, data);
    }

    /*!
     * @Brief Notify an ON_RECEIVER_ERROR event to listeners
     */
    inline void onReceiverError(cdrMemoryStream& data)
    {
      m_listeners->
        connectorData_[ON_RECEIVER_ERROR].notify(m_profile, data);
    }

    /*!
     * @brief Find index of the properties
     */
    CORBA::Long findProp(const SDOPackage::NVList& properties, const char* key);

    /*!
     * @brief Settings to connect to AMQP broker
     */
    bool subscribeAmqpcppConsumer(const SDOPackage::NVList& properties);

  private:
    CdrBufferBase* m_buffer;
    ::OpenRTM::InPortCdr_var m_objref;
    ConnectorListeners* m_listeners;
    ConnectorInfo m_profile;
    InPortConnector* m_connector;

  };  // class InPortAmqpcppConsumer
};    // namespace RTC

extern "C"
{
  /*!
   * @brief Module initialization
   */
  void InPortAmqpcppConsumerInit(void);
};

#ifdef WIN32
#pragma warning( default : 4290 )
#endif

#endif // RTC_INPORTAMQPCPPCONSUMER_H
