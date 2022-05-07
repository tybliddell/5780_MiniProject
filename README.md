# PIXYCAM-BOT

## Explanation

The Pixycam-Bot is a ping pong ball tracking robot that contains a camera that sits on a pan and tilt-controllable assembly to continuously point itself at the ball. The project itself does little more than entertain us, but it could easily be expanded into many different applications including an automatic referee, an auto face-tracking webcam, and more.

## Diagram for project
![Pixy Diagram](https://github.com/tybliddell/5780_MiniProject/blob/main/Pinout_Project_Diagram.png)

## Pixy Settings Considerations

We seemed to have a decent amount of success using 115200 for the baud rate of the camera. This is easy to set using the PixyMon software you can obtain at the pixy website [here](https://pixycam.com/downloads-pixy1/). The specifics on how to navigate the software and change these settings can be located on the wiki page [here](https://docs.pixycam.com/wiki/doku.php?id=wiki:v1:porting_guide). 

### Lighting settings

The Pixycam has a lot of settings it holds internally and they can be modified/stored in the memory. There are a few bars that each make some changes to the way the hardware interprets and processes the video stream. Here are the settings for that tuning:


Within the signature tuning tab, you can find the following:
![Signature Tuining params](https://github.com/tybliddell/5780_MiniProject/blob/main/signature_tune.png)

These parameters seem to have different uses for the preprocessing, but have a direct relationship to the amount of post processing that can be done. I was unable to label these with anything useful, so we just sort of played around with the values until we found something that worked well in the lab. Note* This kind of development was mostly due to time, but we quickly realized that the pixy is a camera that can't really be used in a dynamic environment all too well. Using a better camera sensor with beefier post processing techniques would allow for harder, better, faster, and stronger tracking feedback. 

### Capture settings

To capture a single item in frame, we needed to make some mods to the blocks tab. These changes are shown below:

![Changes for Blocks settings](https://github.com/tybliddell/5780_MiniProject/blob/main/BlocksVals.png)

The changes here allow you to capture a specified size, number of captures, and max number of blocks per image captured. Since the ball we were planning to use could be fairly small when farther away, we wanted to make sure the size was still picked up. The 20 min seemed like a decent choice for that value, but we saw some issues with picking up stray signatures around the ball or from reflections in the area. More testing needed to find out what the best way to minimize that would be. 

### Misc settings

The next thing is the baud rate for the UART, and the expert settings are last pieces needed to be configured. Uart settings as well as I2C settings can be seen below and found in the Interface tab, and the Expert tab contains some settings for ...uuhhh experts :) . 

![Interface for baud rate](https://github.com/tybliddell/5780_MiniProject/blob/main/uart.png)

#### Expert stuff

The expert settings are shown below and mostly stay as they are. The LED brightness and the Teaching Threshold are the only ones you likely want to mess with unless you know what you are doing with the others. 

![Expert Settings](https://github.com/tybliddell/5780_MiniProject/blob/main/expert.png)


#### 5780 Mini Project Authors
Tyler Liddell [owns the repo](https://github.com/tybliddell)

Miguel Gomez [Github Page](https://github.com/Mgomez-01)

Rich Baird 😎 [Github Page](https://github.com/richbai90)

Hyrum Saunders [Github Page](https://github.com/hyrum-saunders)

### Project Demonstration and Milestone Explanation
[Watch our demonstration here!](https://youtu.be/rSYqTkc3UDk)
