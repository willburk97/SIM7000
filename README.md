# SIM7000
Working Botletics (https://github.com/botletics) SIM7000A shield on Arduino Uno with Verizon to Adafruit IO.

Code had to be shortened considerably (especially the embedded status strings) to enable use on Arduino Uno.

Currently using a standard, activated Verizon 4G/LTE sim card out of an iPhone 6S.

Note: While doing initial testing I was unable to succesfully send a text or make a call.  I think Verizon may not be allowing the
SIM7000A on the voice/sms part of the network.  Internet access is working. I'm guessing this is because the SIM7000A connects to
the CAT-M1 part of the network.

Note: The SIM7000A is on Verizon's approved LTE Cat-M1 list.
https://opendevelopment.verizonwireless.com/design-and-build/approved-modules?count=0&lteCategories=Cat%20M1&gps=&gpio=&hwformfactor=&searchBox=&page=0&pageSize=12&loading=false&moreToShow=true&sms=&voicecapable=&developerkit=

Other random Verizon info:
https://www.verizon.com/about/news/verizon-launches-industrys-first-lte-category-m1-cat-m1-nationwide-network-iot

