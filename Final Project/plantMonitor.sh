#!/bin/bash

# Checks to see if the pin has already been exported
if [ ! -e "/sys/class/gpio/gpio17/" ]; then
echo 17 > /sys/class/gpio/export
fi

# Sets the pin direction to be input
echo in > /sys/class/gpio/gpio17/direction

# Fetches previous state from a file, collects new state from pin
PREVSTATE=`cat lastState.txt`
SIGNAL=`cat /sys/class/gpio/gpio17/value`

# Checks if the signal is high, and if the last time around the signal was low
if [ $SIGNAL -eq 1 ]; then
	if [ !  $PREVSTATE -eq 1 ]; then
		# Send an email to the plant owner, letting them know their plant will need their attention
		MSG="Hello, this is your plant. I need attention! Please check on me at your earliest convience - the lights on my monitor will tell you what to do!"
		echo $MSG | mail -s "A Message from your Plant" jbelang@bu.edu
	fi
fi

# Modify previous state
echo $SIGNAL > "lastState.txt"

# Unexport the pin
echo 17 > /sys/class/gpio/unexport