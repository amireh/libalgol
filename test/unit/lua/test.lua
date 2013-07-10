require 'algol_lua'

function sleep(n)
  os.execute("sleep " .. tonumber(n))
end

algol.algol_init("lua_test", "0", "1", "0", "a1")

logger = algol.logger:new("Lua")

station = algol.station:singleton()
-- chan = station:open_channel("test_exchange")
-- sleep(1)

comm = algol.communicator:new()
-- comm:subscribe("test_exchange", "test_queue")

-- for i,v in ipairs(comm) do print(i,v) end
print(comm.on_message_received)

logger:log():info("consuming...")

-- sleep(3)

logger:log():info("exiting")

-- station:close_channel(chan:id())

algol.algol_cleanup()