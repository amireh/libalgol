#!/usr/bin/env python

import algol_py
import time

algol_py.algol_init("test-py", "0", "1", "0", "a1")

class Comlink(algol_py.communicator):
  def __init__(self):
     algol_py.communicator.__init__(self)

  def on_message_received(self, msg):
     print("Received a message from python! %s" % msg.dump_str())

c = Comlink()

print("Publishing 1000 messages")
for i in range(1000):
  m = algol_py.message("hello world!")
  c.publish(m, "test_exchange", "test_queue")
  del m


m = algol_py.message("quit")
c.publish(m, "test_exchange", "test_queue")
del m

print("Accepting messages for 5 seconds")

c.subscribe("test_exchange", "test_queue")
time.sleep(5)

print("Test passed!")

del c

algol_py.algol_cleanup()
