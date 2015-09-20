CSS_Sheet=[[<html>
<head>
<meta http-equiv="refresh" content="10">
<style type="text/css">
#customers
  {
  font-family:"Trebuchet MS", Arial, Helvetica, sans-serif;
  width:100%;
  border-collapse:collapse;
  }
#customers td, #customers th 
  {
  font-size:1em;
  border:1px solid #98bf21;
  padding:3px 7px 2px 7px;
  }
#customers th 
  {
  font-size:1.1em;
  text-align:left;
  padding-top:5px;
  padding-bottom:4px;
  background-color:#A7C942;
  color:#ffffff;
  }
#customers tr.alt td 
  {
  color:#000000;
  background-color:#EAF2D3;
  }
</style>
</head>
<body>
<table id="customers">
<tr>
<th>Input Source</th>
<th>Temperature</th>
<th>Humidity</th>
</tr>
<tr>
<td>DHT Temp&Humi Sensor</td>
<td>]]
CSS_2=[[ deg C</td>
<td>]]
CSS_3=[[ %</td>
</tr>
</table>
</body>
</html>
]]


pin = 4 -- The pin you connected the DHTXX

dht = require("dht_lib")



print("NodeMcu's IP Address:"..wifi.sta.getip())

srv=net.createServer(net.TCP) 
srv:listen(80,function(conn) 
    conn:on("receive",function(client,payload) 
      print(payload) 

     dht.init(pin)

    client:send(CSS_Sheet..dht.getTemp()..CSS_2..dht.getHumidity()..CSS_3)
    client:close()
    collectgarbage();
    end) 
end)