# Olympus DSS Player Lite footswitch/hotkey support

The problem
-----------

Some dictation devices produce dictation recordings in proprietary 
[.dss file format](https://en.wikipedia.org/wiki/Digital_Speech_Standard)
If you want to play files in this format, you can use the free 
[Olympus DSS Player Lite](https://www.olympus.de/site/de/a/audio_software/transcription_software/dss_player/dss_player_lite_s_2/index.html).
However, the player does not have support for hotkeys for playback. 
Therefore, if you have i.e. a Phillips footswitch, you cannot use it to play
the files, because, even though you can configure the footswitch to send
specific key sequences to the DSS Player application, it would not have any
effect, as the application is not reacting to it.

The solution
------------

I wrote a little wrapper that adds hotkey support to the Olympus 
DSS Player Lite. The following hotkeys are used (analogous to the DSS 
Player Pro):

| Key | Function                   |
| --- | -------------------------- |
| F6  | Skip to next dictation     |
| F7  | <<  Rewind                 |
| F8  | >>  Forward                |
| F9  | >[]  Play/Stop             |

In order to get it working: 
1) Install the DSS Player Lite, if you haven't already.
2) Put the dssplystart.exe and dssplyhk.dll into a directory of you choice
3) Start the DSS Player by launching dssplystart.exe instead of DSSPly.exe

This should check for installation directory of DSSPly.exe and launch it 
with the hotkey wrapper DLL dssplyhk.dll injected. Now you should be able
to use the hotkey mentioned above in the player.
You can use the Phillips Footswitch program to assign the keys of the footswitch
to the hotkeys mentioned above and use the player.

If you wish to automatically open .dss files with DSS player Lite with hotkey
support, simply edit the registry Default key under:

`HKEY_CLASSES_ROOT\DSS-Lite\Shell\Open\Command`

and replace 

`C:\Program Files (x86)\Olympus\DSSPlayerLite\DSSPly.exe %1`

with (using correct path):

`C:\Path\to\DSSPlystart.exe %1`

Contact
-------
Should you have any questions, feel free to contact me at 
leecher [at] dose [dot] 0wnz [dot] at

