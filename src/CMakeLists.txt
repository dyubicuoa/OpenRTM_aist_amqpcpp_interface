add_library(OutPortAmqpcppProducer
  SHARED
    OutPortAmqpcppProducer.cpp
    AmqpcppProducer.cpp
    AmqpcppManager.cpp
)

target_include_directories(OutPortAmqpcppProducer
  PUBLIC
    /usr/include/coil-1.2
    /usr/include/openrtm-1.2
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(OutPortAmqpcppProducer
  PUBLIC
    RTC-1.2.2
    coil-1.2.2
    omnithread
    omniORB4
    omniDynamic4
    ev
    amqpcpp
)

set_target_properties(OutPortAmqpcppProducer PROPERTIES PREFIX "")

add_library(InPortAmqpcppConsumer
  SHARED
    InPortAmqpcppConsumer.cpp
    AmqpcppConsumer.cpp
    AmqpcppManager.cpp
)

target_include_directories(InPortAmqpcppConsumer
  PUBLIC
    /usr/include/coil-1.2
    /usr/include/openrtm-1.2
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(InPortAmqpcppConsumer
  PUBLIC
    RTC-1.2.2
    coil-1.2.2
    omnithread
    omniORB4
    omniDynamic4
    ev
    amqpcpp
)

set_target_properties(InPortAmqpcppConsumer PROPERTIES PREFIX "")
