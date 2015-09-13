function bitmap_picture(state)
     disp:setFont(u8g.font_6x10)
     disp:drawStr( 0, 10, "Bitmap picture")

     disp:drawBitmap( 0 + (state * 10), 20 + (state * 4), 10, 80, bm_data )
end

-- the draw() routine
function draw(draw_state)
     local component = bit.rshift(draw_state, 3)
     if (component == 1) then
          bitmap_picture(bit.band(draw_state, 7))
     end
end


function bitmap_test(delay)

     -- read Bitmap picture
     file.open("logo.bmp", "r")
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
--init_spi_display()
bitmap_test(50000)