# CSci 4611 Project 2 - CarSoccer

## Information
* Name: Tiannan Zhou
* Student ID: 5232494
* Email: zhou0745@umn.edu

## Compile
This program is based on Linux and you just need to use GNU Makefile to compile the whole project.
```
make
```
The recommended environment for compiling this project is CSELAB Ubuntu Linux 16.04.

## Run
You can find a executive program which is named `CarSoccer` in the folder the source code located in. You also can simply run command `./CarSoccer` in terminal to start this game.

## Description
1. This project is based on OpenGl and GLM libiary.
2. I used fabs() function to help determine the current direction for balls' bounce in order to prevent unexpected movement.
3. I designed two drawing helper function to make draw the figures much easier.
4. I used sin and cos function to help determine the accelerate velocity's direction for the car. I also baned the moving functionality of car when the user just presses the left or right button. The car mush be controlled by forward and back button with or without left/right button. This design pattern can help the car in our game act like a real car.
5. I used the equipment provided by the handout pdf to determine the accelerate velocity for the ball. 
