# AOpenGPS(v4.3.10)
https://github.com/farmerbriantee/AgOpenGPS/

# Libraries

https://github.com/m5stack/M5Stack

https://github.com/FastLED/FastLED

# Parts

- M5stickC (with LCD ,Battery)

https://m5stack.com/products/stick-c

https://www.switch-science.com/catalog/6350/

- ATOM Lite (no LCD)

https://m5stack.com/products/atom-lite-esp32-development-kit

https://www.switch-science.com/catalog/6262/

- M5Stack Official RGB LEDs Cable SK6812 with GROVE Port 

https://aliexpress.com/item/32950831315.html

https://www.switch-science.com/catalog/5209/


- M5Stack Official Universal 4Pin Buckled Grove Cable 

https://aliexpress.com/item/32949298454.html

https://www.switch-science.com/catalog/5217/

# default setting
```
  #define USE_BLUETOOTH_SERIAL 1         // 0:use USB Serial   1:use Bluetooth Serial
  #define NUMPIXELS   30                 //How many pixels
  #define Neopixel_Pin 32                //GPIP32:M5stickC  GPIO26:ATOMLite
  #define mmPerLightbarPixel  40         // 40 = 4cm
  #define NIGHT_BRIGHTNESS 50            //night mode brightness
  #define USE_LCD 1                      //1:LCD display(onlyM5stickC)
```

**AOG send distanceFromLine data only in Autosteer mode**

**Press the front button to switch to night mode**
<blockquote class="twitter-tweet"><p lang="en" dir="ltr">External Light Bar for <a href="https://twitter.com/hashtag/AgOpenGPS?src=hash&amp;ref_src=twsrc%5Etfw">#AgOpenGPS</a> <br>on <a href="https://twitter.com/hashtag/M5StickC?src=hash&amp;ref_src=twsrc%5Etfw">#M5StickC</a> <br>BluetoothSerial connection <a href="https://t.co/haLmXN42JO">pic.twitter.com/haLmXN42JO</a></p>&mdash; mnl_t (@mnlt18) <a href="https://twitter.com/mnlt18/status/1281016090550652928?ref_src=twsrc%5Etfw">July 9, 2020</a></blockquote> <script async src="https://platform.twitter.com/widgets.js" charset="utf-8"></script>
