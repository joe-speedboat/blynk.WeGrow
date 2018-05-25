# blynk.WeGrow
Wemos D1 mini plant grow box project


This is a POC to get familiar with Blynk.
## What does it do:
- Read DHT sensor and push humidity and temperatur to blynk
- Read soil with A0 and push to blynk
- turn lights on when power on

## What I have done in Blynk app:
- alert if soil is to low
- turn fan on when temperatur rises above 30 C
- turn lights on/off with eventor timer
- added manual buttons for: light, fan, water
- added history graph
![BlynkSS1](https://github.com/joe-speedboat/blynk.WeGrow/blob/master/Screenshot_20180525-101539.png)
![BlynkSS2](https://github.com/joe-speedboat/blynk.WeGrow/blob/master/Screenshot_20180525-102956.png)


## Hardware I use:
- WeMos D1 mini
- 4 channel Arduino relay block
- 5v water pump
- 2 chrome soil electrodes
- DHT sensor
- LED grow lights

## Soil analog read
It's worth mentioning how I have done   
- Pull up Digital Output
- Sleep 0.1s
- Read Analog Sensor A0
- do some math
- Shut down Digital out

## Soil analog sensor setup
- Digital Out (3.3v, pulled up)
- 100 000 ohm potentiometer
- Soil Sensor1 ==> A0
- Soil Sensor2 ==> Gnd
```
Digital out == potentiometer (0-100k) == A0, SoilProbe1
Gnd == SoilProbe2
```

Thats it so far

- all the IOs are documented in code
