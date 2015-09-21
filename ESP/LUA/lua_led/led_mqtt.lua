------------------------------------------------------------
gpio0  = 3 
gpio2  = 4 
gpio4  = 2
gpio5  = 1
gpio12 = 6
gpio13 = 7

mqtt_server_ip="192.168.1.100"
client_id     = "ESP01_"..node.chipid()
io_led        = {ledW=gpio13,ledR=gpio4,ledG=gpio0,ledB=gpio12}
led_status    = {ledW=false,ledR=false,ledG=false,ledB=false}
led_brightness= {ledW=0,ledR=0,ledG=0,ledB=0}
io_temp       = gpio5
temperature   = 0
mq_connected  = false


require("ds18b20")
ds18b20.setup(io_temp)
-------------------------------------------------------------
function turn_led(pin,flag)
   if not io_led[pin] then return end

   gpio.mode( io_led[pin], gpio.OUTPUT)
   gpio.write( io_led[pin], flag)
   led_status[pin]= gpio.HIGH==flag
   led_brightness[pin] = led_status[pin] and 1023 or 0
end

function set_brightness(pin,value)
   if not io_led[pin] then return end
   value = tonumber(value)
   if not value then return end 

   pwm.setup(io_led[pin],1000,256) 
   pwm.start(io_led[pin])
   pwm.setduty(io_led[pin],value)
   led_brightness[pin] = value > 1023 and 1023 or value
   led_status[pin] = true
end

function report_status()
   if mq_connected then 
        temperature=ds18b20.read() or 0;
	data={
	    led={
	        status=led_status,
                brightness=led_brightness,
	    },
	    temp={
		  temperature,
	    },
	    ip = wifi.sta.getip(),
            heap = node.heap(),
            client_id = client_id,
	}
   	print(cjson.encode(data))
   	m:publish("/status",cjson.encode(data),0,0, function(conn) print("sent") end)
   end
end

function handle_command(cmd)
   local _report = false

   if cmd.turn_off then 
        for _,pin in ipairs(cmd.turn_off) 
	do
	   turn_led(pin,gpio.LOW)
           _report = true
	end 
   end

   if cmd.turn_on then 
	for _,pin in ipairs(cmd.turn_on) 
	do
	   turn_led(pin,gpio.HIGH)
	   _report = true
	end 
   end

   if cmd.set_brightness then 
        for pin,value in pairs(cmd.set_brightness) 
	do
	   set_brightness(pin,value)
           _report = true
	end 
   end
   
   if cmd.report then _report=true end

   if _report then report_status() end 
end


-----------------------------------------------------------
--- init led , turn it off
for key,_ in pairs(io_led) 
do
    turn_led(key,gpio.LOW)
end


m = mqtt.Client(client_id, 10)
m:lwt("/lwt", "offline", 0, 0)

m:on("offline", function(con) 
   print ("offline") 
   mq_connected  = false
end)

m:on("message", function(conn, topic, data)
  print("message got"..tostring(data)..topic)
  if data then
    cmd=cjson.decode(data)
    if cmd then handle_command(cmd) end
  end
end)

m:connect(mqtt_server_ip, 1883, 0, 1, function(conn) 
   print("connected") 
   mq_connected = true
   m:subscribe("/cmd/"..client_id,0, function(conn) print("subscribe success") end)
   tmr.alarm(1,30000,1,function() report_status() end)
end)

------------------------------------------------------------