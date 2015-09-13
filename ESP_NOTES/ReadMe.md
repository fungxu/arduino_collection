### ESP8266 使用记录

1. 最小接线方式, CH_PD 口需要接 VCC 可以开机

  接 usb 的图![p1](https://raw.githubusercontent.com/fungxu/esp8266-co-uk/master/images/esp-01-connections.png)

2. GPIO 口与代码的对应关系
| GPIO pin | IO index |
| -------- | -------- |
| GPIO0    | 3        |
| GPIO1    | 10       |
| GPIO2    | 4        |
| GPIO3    | 9        |
| GPIO4    | 2        |
| GPIO5    | 1        |
| GPIO6    | N/A      |
| GPIO7    | N/A      |
| GPIO8    | N/A      |
| GPIO9    | 11       |
| GPIO10   | 12       |
| GPIO11   | N/A      |
| GPIO12   | 6        |
| GPIO13   | 7        |
| GPIO14   | 5        |
| GPIO15   | 8        |
| GPIO16   | 0        |

3. 有两个口开机时要注意
| GPIO PIN | 电平 | 作用 |
|----------|-----|------|
| GPIO15   | 高  | 从SD卡启动 |
| GPIO00   | 低  | 写入程序 |

4. 使用IO口时需要初始化
| Mode      | Reference   | Description                                                                     |
| --------- | ----------- | ------------------------------------------------------------------------------- |
| Input     | gpio.INPUT  | Poll a pin to get its value.                                                    |
| Output    | gpio.OUTPUT | Assign a pin its value.                                                         |
| Interrupt | gpio.INT    | Same as input + set a callback to be executed every time a pin's value changes. |
如：  

``` lua
gpio.mode(3, gpio.OUTPUT)
gpio.write(3, gpio.HIGH)

gpio.mode(4, gpio.OUTUT)
gpio.write(4, gpio.LOW)

gpio.mode(4, gpio.INPUT)
local pinValue = gpio.read(4)

if pinValue == gpio.LOW then
	print 'GPIO2 is low'
else
	print 'GPIO2 is high'
end

```

使用中断模式,这也是充分利 lua 接语言特点

```lua
local pin = 4	--> GPIO2

function onChange ()
	print('The pin value has changed to '..gpio.read(pin))
end

gpio.mode(pin, gpio.INT)
gpio.trig(pin, 'both', onChange)
```

有几个事件可以监听:


| Event name | Description                              |
| ---------- | ---------------------------------------- |
| up         | Occurs when the pin moves high.          |
| down       | Occurs when the pin moves low.           |
| both       | Occurs when the pin moves high or low.   |
| low        | Occurs repeatedly while the pin is low.  |
| high       | Occurs repeatedly while the pin is high. |

软件去抖:

```lua
local pin = 4	--> GPIO2

function debounce (func)
	local last = 0
	local delay = 5000　--- 5000 代表 us 即 5ms

	return function (...)
		local now = tmr.now()
		if now - last < delay then return end

		last = now
		return func(...)
	end
end

function onChange ()
	print('The pin value has changed to '..gpio.read(pin))
end

gpio.mode(pin, gpio.INT)
gpio.trig(pin, 'both', debounce(onChange))
```


5. 为了在开机时进行重试，重试时显示进度，失败后停止，需要在init.lua 中写：

```lua
if true then  --change to if true
	print("set up wifi mode")
	wifi.setmode(wifi.STATION)
	--please config ssid and password according to settings of your wireless router.
	wifi.sta.config("ssid","password")
	wifi.sta.connect()
	cnt = 0
	tmr.alarm(1, 1000, 1, function() 
	    if (wifi.sta.getip() == nil) and (cnt < 20) then 
	    	print("IP unavaiable, Waiting...")
	    	cnt = cnt + 1 
	    else 
	    	tmr.stop(1)
	    	if (cnt < 20) then print("Config done, IP is "..wifi.sta.getip())
	    	dofile("dht11.lua")
	    	else print("Wifi setup time more than 20s, Please verify wifi.sta.config() function. Then re-download the file.")
	    	end
	    end 
	 end)
else
	print("\n")
	print("Please edit 'init.lua' first:")
	print("Step 1: Modify wifi.sta.config() function in line 5 according settings of your wireless router.")
	print("Step 2: Change the 'if false' statement in line 1 to 'if true'.")
end

```

6. i2c oled 的初始化

```lua
sda = 5
scl = 6
sla = 0x3c
i2c.setup(0, sda, scl, i2c.SLOW)
disp = u8g.ssd1306_128x64_i2c(sla)
```

7. 显示字符

```lua
disp:setFont(u8g.font_6x10)
disp:drawStr( 0, 10, lines[1]);
```
8. 画个盒子

```lua 
     disp:drawBox(0, 0, 3, 3)
     disp:drawBox(disp:getWidth()-6, 0, 6, 6)
     disp:drawBox(disp:getWidth()-9, disp:getHeight()-9, 9, 9)
     disp:drawBox(0, disp:getHeight()-12, 12, 12)
```

9. SPI 方式

```lua
-- setup SPI and connect display
function init_spi_display()
     -- Hardware SPI CLK  = GPIO14
     -- Hardware SPI MOSI = GPIO13
     -- Hardware SPI MISO = GPIO12 (not used)
     -- CS, D/C, and RES can be assigned freely to available GPIOs
     local cs  = 8 -- GPIO15, pull-down 10k to GND
     local dc  = 4 -- GPIO2
     local res = 0 -- GPIO16

     spi.setup(1, spi.MASTER, spi.CPOL_LOW, spi.CPHA_LOW, spi.DATABITS_8, 0)
     disp = u8g.ssd1306_128x64_spi(cs, dc, res)
end
```

10. 旋转

```lua
function rotate()
     if (next_rotation < tmr.now() / 1000) then
          if (dir == 0) then
               disp:undoRotation()
          elseif (dir == 1) then
               disp:setRot90()
          elseif (dir == 2) then
               disp:setRot180()
          elseif (dir == 3) then
               disp:setRot270()
          end

          dir = dir + 1
          dir = bit.band(dir, 3)
          -- schedule next rotation step in 1000ms
          next_rotation = tmr.now() / 1000 + 1000
     end
end
```

11. 显示图片

```lua 
-- setup I2c and connect display
function init_i2c_display()
     -- SDA and SCL can be assigned freely to available GPIOs
     local sda = 5 -- GPIO14
     local scl = 6 -- GPIO12
     local sla = 0x3c
     i2c.setup(0, sda, scl, i2c.SLOW)
     disp = u8g.ssd1306_128x64_i2c(sla)
end

-- setup SPI and connect display
function init_spi_display()
     -- Hardware SPI CLK  = GPIO14
     -- Hardware SPI MOSI = GPIO13
     -- Hardware SPI MISO = GPIO12 (not used)
     -- CS, D/C, and RES can be assigned freely to available GPIOs
     local cs  = 8 -- GPIO15, pull-down 10k to GND
     local dc  = 4 -- GPIO2
     local res = 0 -- GPIO16

     spi.setup(1, spi.MASTER, spi.CPOL_LOW, spi.CPHA_LOW, spi.DATABITS_8, 0)
     disp = u8g.ssd1306_128x64_spi(cs, dc, res)
end


function xbm_picture()
     disp:setFont(u8g.font_6x10)
     disp:drawStr( 0, 10, "XBM picture")

     disp:drawXBM( 0, 20, 38, 24, xbm_data )
end

function bitmap_picture(state)
     disp:setFont(u8g.font_6x10)
     disp:drawStr( 0, 10, "Bitmap picture")

     disp:drawBitmap( 0 + (state * 10), 20 + (state * 4), 1, 8, bm_data )
end

-- the draw() routine
function draw(draw_state)
     local component = bit.rshift(draw_state, 3)

     if (component == 0) then
          xbm_picture(bit.band(draw_state, 7))
     elseif (component == 1) then
          bitmap_picture(bit.band(draw_state, 7))
     end
end


function bitmap_test(delay)
     -- read XBM picture
     file.open("u8glib_logo.xbm", "r")
     xbm_data = file.read()
     file.close()

     -- read Bitmap picture
     file.open("u8g_rook.bm", "r")
     bm_data = file.read()
     file.close()

     print("--- Starting Bitmap Test ---")
     dir = 0
     next_rotation = 0

     local draw_state
     for draw_state = 1, 7 + 1*8, 1 do
          disp:firstPage()
          repeat
               draw(draw_state)
          until disp:nextPage() == false

          tmr.delay(delay)
          tmr.wdclr()
     end

     print("--- Bitmap Test done ---")
end

--init_i2c_display()
init_spi_display()
bitmap_test(50000)

```

12. pwm 例子

```lua
-- setup I2c and connect display
function init_i2c_display()
     -- SDA and SCL can be assigned freely to available GPIOs
     local sda = 5 -- GPIO14
     local scl = 6 -- GPIO12
     local sla = 0x3c
     i2c.setup(0, sda, scl, i2c.SLOW)
     disp = u8g.ssd1306_128x64_i2c(sla)
end

-- setup SPI and connect display
function init_spi_display()
     -- Hardware SPI CLK  = GPIO14
     -- Hardware SPI MOSI = GPIO13
     -- Hardware SPI MISO = GPIO12 (not used)
     -- CS, D/C, and RES can be assigned freely to available GPIOs
     local cs  = 8 -- GPIO15, pull-down 10k to GND
     local dc  = 4 -- GPIO2
     local res = 0 -- GPIO16

     spi.setup(1, spi.MASTER, spi.CPOL_LOW, spi.CPHA_LOW, spi.DATABITS_8, 0)
     disp = u8g.ssd1306_128x64_spi(cs, dc, res)
end


function xbm_picture()
     disp:setFont(u8g.font_6x10)
     disp:drawStr( 0, 10, "XBM picture")

     disp:drawXBM( 0, 20, 38, 24, xbm_data )
end

function bitmap_picture(state)
     disp:setFont(u8g.font_6x10)
     disp:drawStr( 0, 10, "Bitmap picture")

     disp:drawBitmap( 0 + (state * 10), 20 + (state * 4), 1, 8, bm_data )
end

-- the draw() routine
function draw(draw_state)
     local component = bit.rshift(draw_state, 3)

     if (component == 0) then
          xbm_picture(bit.band(draw_state, 7))
     elseif (component == 1) then
          bitmap_picture(bit.band(draw_state, 7))
     end
end


function bitmap_test(delay)
     -- read XBM picture
     file.open("u8glib_logo.xbm", "r")
     xbm_data = file.read()
     file.close()

     -- read Bitmap picture
     file.open("u8g_rook.bm", "r")
     bm_data = file.read()
     file.close()

     print("--- Starting Bitmap Test ---")
     dir = 0
     next_rotation = 0

     local draw_state
     for draw_state = 1, 7 + 1*8, 1 do
          disp:firstPage()
          repeat
               draw(draw_state)
          until disp:nextPage() == false

          tmr.delay(delay)
          tmr.wdclr()
     end

     print("--- Bitmap Test done ---")
end

--init_i2c_display()
init_spi_display()
bitmap_test(50000)

```

13. 字体变大

```lua
disp:setScale2x2();
```

14. 改变字体

```C
// nodemcu-firmware/app/include/u8g_config.h

#define U8G_FONT_TABLE \
    U8G_FONT_TABLE_ENTRY(font_6x10)  \
    U8G_FONT_TABLE_ENTRY(font_chikita)
#undef U8G_FONT_TABLE_ENTRY
```

