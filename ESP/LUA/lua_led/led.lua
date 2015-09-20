header=[[<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=2">
<style type="text/css">
body
  {
  font-family:"Trebuchet MS", Arial, Helvetica, sans-serif;
  width:100%;
  border-collapse:collapse;
  }

.on{
    background-color: red;
}
</style>
</head>
<body>]]

footer=[[
</body>
</html>
]]

gpio0 = 4 -- LED
gpio2 = 3 -- ds18b20


led2_status=0;
class="class='on'"

require("ds18b20")
ds18b20.setup(gpio2)

gpio.mode(gpio0, gpio.OUTPUT)
gpio.write(gpio0, gpio.LOW);

srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
    conn:on("receive", function(client,request)
        local buf = "";
        local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP");
        if(method == nil)then
            _, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP");
        end
        local _GET = {}
        if (vars ~= nil)then
            for k, v in string.gmatch(vars, "(%w+)=(%w+)&*") do
                _GET[k] = v
            end
        end

t=ds18b20.read();
t=t or 0;
local s3="";
local s4="";


        if(_GET.pin == "ON2")then
              gpio.mode(gpio0, gpio.OUTPUT)
              gpio.write(gpio0, gpio.HIGH);
              led2_status=1;
        elseif(_GET.pin == "OFF2")then
              gpio.mode(gpio0, gpio.OUTPUT)
              gpio.write(gpio0, gpio.LOW);
             led2_status=0;
        end

b=tonumber(_GET.b);
print(_GET);
if b then
   print("got . "..b);
   pwm.setup(gpio0,1000,256) 
   pwm.start(gpio0)
   pwm.setduty(gpio0,b)
end

if (led2_status==1) then
      s3=class;
else
      s4=class;
end

        buf = header ; 
        buf = buf.."<p>Temperature: ".. t .."C</p>";
        buf = buf.."<p>GPIO2 <a href=\"?pin=ON2\"><button "..s3..">ON</button></a> <a href=\"?pin=OFF2\"><button "..s4..">OFF</button></a></p>";
        buf = buf..footer;
        local _on,_off = "",""

        client:send(buf);
        client:close();
        collectgarbage();
    end)
end)
