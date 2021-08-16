set(AMQPCPP_PUBLISH_SRC
  ${PROJECT_SOURCE_DIR}/src/AmqpcppManager.cpp
  ${PROJECT_SOURCE_DIR}/src/AmqpcppProducer.cpp
)

add_executable(publish_test publish_test.cpp ${AMQPCPP_PUBLISH_SRC})

target_include_directories(publish_test
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(publish_test
  PUBLIC
    ev
    amqpcpp
    pthread
    dl
)

set(AMQPCPP_CONSUME_SRC
  ${PROJECT_SOURCE_DIR}/src/AmqpcppManager.cpp
  ${PROJECT_SOURCE_DIR}/src/AmqpcppConsumer.cpp
)

add_executable(consume_test consume_test.cpp ${AMQPCPP_CONSUME_SRC})

target_include_directories(consume_test
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(consume_test
  PUBLIC
    ev
    amqpcpp
    pthread
    dl
)

