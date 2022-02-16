# DogTracker
__Authors: Kylee and Angie__

Bill of Materials can be found [here](https://github.com/krsandwich/DogTracker/tree/master/Hardware/BOM) \
KICAD Project can be found [here](https://github.com/krsandwich/DogTracker/tree/master/Hardware/Kicad)\
M4 Feather Express Schematic can be found [here](https://learn.adafruit.com/assets/57242)


### Purpose 

A common issue pet-owners face is discovering that their pet has escaped the house. To prevent this situation, our team is interested in designing a low-power dog tracker using LoRa. Unlike other pet-tracking collars, our system will implement a training technique by emitting a 25KHz sound wave if the pet is found outside of a user-defined geo fence. For example, if the user would like to train their dog to avoid a specific area of the backyard, our device will detect whether the dog is in this region and emit the high frequency sound if so.

### System diagram   
<p>
  <img src="https://github.com/krsandwich/DogTracker/blob/master/block_diagram.png" width="700">
  <p> Figure 1: System Level Block Diagram
</p>

### Proposed schedule
| Week  | Date | Tasks |Complete|
| ------------- | ------------- | ------------- |------------- |
| Week 7 | Feb 13-Feb 19  | Order parts <br />Review PCB and Schematic with Steve and Joanna   | In Progress|
| Week 8 | Feb 20-Feb 26  | Submit PCB for fabriciation | |
| Week 9 | Feb 27-March 5  | Bring up and test boards | | 
| Week 10 | March 6-March 12  | Demo | | 



### Verification
To verify our system, we will first verify the functionality of each module before methodically combining things into our final system. The steps are outlined below.

Before final PCB comes back: 
- [x] Upload basic Blink code to M4 Feather Board
- [ ] Write basic transmit/recieve code for SX1280 using this [reference](https://github.com/maholli/CircuitPython_SX1280) (In progress)
- [ ] Hook up SX1280 to SPI port of any available micro and run transmit code while probing antenna line with Spectrum Analyzer
- [ ] Connect additional SX1280 board and have both transmit and recieve data
After final PCB comes back:
- [ ] Again, verify transmit and recieve of SX1280 module on our PCB
- [ ] Connect SX1280 to ESP32 (the "base station") and try sending and recieving from our PCB
- [ ] Connect ESP32 to wifi and verify that there isn't any interference between Lora and WIFI
- [ ] Independently turn on GPS and test reading cordinates
- [ ] Turn on both SX1280 and GPS and run code to intermittently send GPS coordinates to "base station" 
- [ ] Configure pedometer to read step count on the microcontroller
- [ ] Read pedometer at the same time as reading GPS and sending Lora
- [ ] Confgure Ultrasonic piezo buzzer and try running at 25kHz, certain apps will verify sound since it's beyond the human range
- [ ] Upload code for runnng piezzo buzzer from the "base station" when dog is out of range
- [ ] Run all four modules at the same time 
- [ ] Have "base station"  upload information to a website hosted on the the Things network

### Demo Plan
Our demo will be putting the collar on a fellow classmate and having them run around the engineering quad to show the real time updates in GPS coordinates on the base station, which will be set up in the lab. Finally, we will show that when our classmate goes out of the user-defined range, the piezzo buzzer will turn on which can be verified with [this app](https://play.google.com/store/apps/details?id=com.microcadsystems.serge.ultrasounddetector&hl=en_US&gl=US)

In our write up, we will discuss the following
- importance and novelty of the idea 
- description of chosen features 
- high level system discussion
- schematic walkthrough 
- examination of different protocols SPI/I2C/UART 
- Review of RF components eg LoRa, WiFi 
- analysis of challenges or drawbacks
- improvements going forward 
- concluding remarks on our project 

### Challenges
The most difficult steps is getting the PCB right the first time around. We have been meticulously researching parts and going through datasheets to make sure we understand everything that we need for our final PCB. We also plan to have Joanna, Greig, and Steve verify the schematic before we submit it to JLCPCB. 

We have already gotten our parts list checked off and ordered. After submitting our final PCB, we will begin working on the software side of our project. There is a sufficient amount of code to write but none will be outside of our domain given that there's a lot of GPS/LORA/SPI/I2C libraries in Circuit python. 

