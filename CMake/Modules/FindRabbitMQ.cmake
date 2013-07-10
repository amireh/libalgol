# FindRabbitMQ.cmake
# --
# Find the RabbitMQ library
#
# This module defines:
#   RabbitMQ_INCLUDE_DIRS - where to find amqp.h
#   RabbitMQ_LIBRARIES    - the rabbitmq library
#   RabbitMQ_FOUND        - True if RabbitMQ was found

Include(FindModule)
FIND_MODULE(RabbitMQ amqp.h "" "" rabbitmq "" "")
