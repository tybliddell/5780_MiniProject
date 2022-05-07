# PIXYCAM-BOT

## Diagram for project
![Pixy Diagram](https://github.com/tybliddell/5780_MiniProject/blob/main/Pinout_Project_Diagram.png)

## Pixy Settings Considerations

We seemed to have a decent amount of success using 115200 for the baud rate of the camera. This is easy to set using the PixyMon software you can obtain at the pixy website [here](https://pixycam.com/downloads-pixy1/). The specifics on how to navigate the software and change these settings can be located on the wiki page [here](https://docs.pixycam.com/wiki/doku.php?id=wiki:v1:porting_guide). 

### Lighting settings

The Pixycam has a lot of settings it holds internally and they can be modified/stored in the memory. There are a few bars that each make some changes to the way the hardware interprets and processes the video stream. Here are the settings for that tuning:


Within the signature tuning tab, you can find the following:
[Signature Tuining params](./signature_tune.png)

These parameters seem to have different uses for the preprocessing, but have a direct relationship to the amount of post processing that can be done. I was unable to label these with anything useful, so we just sort of played around with the values until we found something that worked well in the lab. Note* This kind of development was mostly due to time, but we quickly realized that the pixy is a camera that can't really be used in a dynamic environment all too well. Using a better camera sensor with beefier post processing techniques would allow for harder, better, faster, and stronger tracking feedback. 

### Capture settings

### Misc settings

#### 5780 Mini Project Authors
Tyler Liddell [owns the repo](https://github.com/tybliddell)

Miguel Gomez [Github Page](https://github.com/Mgomez-01)

Rich Baird 😎 [Github Page](https://github.com/richbai90)

Hyrum Saunders [Github Page](https://github.com/hyrum-saunders)

### Project Demonstration and Milestone Explanation
[Watch our demonstration here!](https://youtu.be/rSYqTkc3UDk)
