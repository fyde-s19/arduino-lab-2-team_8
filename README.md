Questions - Answer these in your README. 
1.   What is happening with the dimmer code pattern in part D step 3? 
	it gives you wave funciton of when its on and off (high and low)
2.   What does the decoded message say in part D step 4? 
	just a some numbers in hex
3.   How long does ESP8266_to_Mega() take in part D step 5? 
	around 600 msecs
4.   How long does  ReadSensors() take in part D step 5?
	around 2 seconds
5.   How long does ESP8266_to_Mega() take in part D step 6? 
	around 600 msecs
6.   How long does  ReadSensors() take in part D step 6?
	around 1.5 seconds
7.   Compare serial monitor debugging with logic analyzer debugging. What are the pros and 
cons of each?
	the serial monitor prints what ever you tell the ardino to print while the logic analyzer measures information at the pins.
the serial monitor's use for debugging although easier, is more intrusive in the program than one would think depending on what the program
does. The logic analyzer is non-intrusive to the program but you end up having to wire things in parallel, making the circuit more complex.

8.   Why do we need to connect the logic analyzer to the same ground as the Arduino?
	Because the logic analyzer measures voltages realtive to a ground. so having the same ground would get you more accurate information 
coming out of the arduino.