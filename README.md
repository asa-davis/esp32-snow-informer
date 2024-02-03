# esp32-snow-informer

This was my first ever esp32 project. It alternates every two seconds or so between displaying the current snow depth and reported fresh snow at winter park. Every hour, the data is fetched via the ski-resort-forecast api from rapidapi.com. I salvaged a seven segment display from an alarm clock, and after some head-scratching managed to write a driver for it to display the data.
![alt text](https://github.com/asa-davis/esp32-snow-informer/blob/main/pics/PXL_20240203_021249926.jpg)
![alt text](https://github.com/asa-davis/esp32-snow-informer/blob/main/pics/PXL_20240203_021311848.jpg)

To use this, you'll have to first figure out which pins (and therefore which bits in the data sent to the shift registers) control which segments of the display, and then fill in the segmentBits array in display.c

You'll also have to get a rapid api key and put it in the request headers in snow_api.c

You'll also have to fill in your wifi name and password in wifi_creds.h
