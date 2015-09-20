function xbm_picture()
     disp:setFont(u8g.font_6x10)
     disp:drawXBM( 0, -5, 60, 60, xbm_data)
end
 
function bitmap_test(delay)
     file.open("gmc.xbm", "r")
     xbm_data = file.read()
     file.close()
 
      disp:firstPage()
      repeat
           xbm_picture()
           tmr.wdclr()
      until disp:nextPage() == false
 
      
end
 
bitmap_test()