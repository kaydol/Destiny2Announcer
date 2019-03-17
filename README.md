
### Destiny 2 Announcer by kaydol (c) 2019

This program is capable of recognizing three events:
1. A high-value target is nearby
2. A high-value target has dropped its plunder
3. A high-value target has departed
	
These are text messages displayed in the system chat in the left bottom corner, 
which is being monitored by this program. When one of these messages appears,
a corresponding audio file is played.

System Requirements:
- The program was tested on Windows 10 x64.
- Microsoft Visual C++ 2017 Redistributable (x64) is required.
	
FAQ:

	Q: Does it work when my game is alt-tabbed?
	A: No, your game has to be in full screen mode, because the program is
	making screenshots of the left bottom corner of the screen during work.
	
	Q: Can I replace the audio files with my own?
	A: You can replace the audio with your own as long as file format is .wav 
	and the name of the file is the same; the program will play it.	
	
	Q: I want to remove detection of some of the events.
	A: Just remove corresponding files in /templates/ and /sounds/ 
	folders and you should be fine. The program will write messages
	regarding the deleted files, and the corresponding checks will
	be disabled. The removal of each event should slightly reduce
	CPU load.
	
	Q: What resolution was this program designed for?
	A: You *must* run your game in 1980x1024.
	

Resources used to made female voices:
* http://clipliving.com/ -- IMO the best TTS resource to date (2019)
* http://text-to-speech.imtranslator.net/speech.asp 
	
Other resources:
* https://www.youtube.com/watch?v=PFyMhNZB-lc [01:00-01:08]
