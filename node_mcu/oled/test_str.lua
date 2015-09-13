file.open("logo.bmp", "r")
bm_data = file.read()
file.close()


function draw_str()
  disp:setFont(u8g.font_6x10)
--  disp:setScale2x2()
    disp:drawStr( 0, 7,"Initializing...")
--  disp:drawBox(10,12,20,30)
--  disp:undoScale()
    disp:drawBitmap( 0 , 24, 5, 40, bm_data )
  --disp:drawStr( 0, 24,"FUNG.XU123")
end 

function drawloop_str()
     disp:firstPage()
     repeat
          draw_str()
          tmr.wdclr()
     until (disp:nextPage() == false)
end

drawloop_str()