<p align="center">
  <img src="https://i.imgur.com/iFLDs6C.png">
  <br />
  A group of functions, that allows you to bypass the Windows Multimedia API, thus getting rid of the lag caused by its slow buffer system.
</p>

## What does this do?
Windows Multimedia Wrapper (as the name says), wraps around the default multimedia library in Windows, **winmm.dll**, and allows users to circumvent the extremely slow buffer that Microsoft has set in Windows 8.1 and Windows 10, when using the **midiOutShortMsg** call.

## Why did they make the function slower?
I have no idea really.<br/>
Aside from making it slower, the output latency *(MIDI app -> MIDI driver)* also got worse, since the function seems to make some useless checks on the message, which adds overhead.<br/>
I don't know if it's for safety purposes, or they just did something wrong in the function itself.

# Was the patch really necessary?
For me and other people, it was.<br/>
I really like Black MIDIs, which is a genre of music characterised by its extreme note arts and melodies.<br/>
Performance when playing them were okay, when using Windows XP/Vista/7, but Microsoft decided to **ruin** them on 8.x/10, so we had to find a way around it.<br/>
The first iteration of the *"wrapper"* was literally just **copying winmm.dll from Windows 7**, and using it on Windows 8.x. That worked fine on everything, until Microsoft decided to make that *impossible* on Windows 10. Everytime an app tries to load 7's winmm.dll on 10, it'd just throw an error, complaining about a missing ordinal.<br/>
For Windows 10, the second iteration was using **ReactOS' winmm.dll** on Windows 10. That also worked fine, aside from minor issues with the *timeGetTime* function (Which I fixed in later releases of the file).<br/>
Then, I decided to work on a *wrapper*, which would import the functions from the **real** Windows Multimedia library *(Allowing users to use both Microsoft GS and MIDI input devices)*, while also getting rid of the lag caused by some weird overhead in **midiOutShortMsg** *(When using my MIDI driver, that is ;P)*.<br/>
Long story short, it was a success. Everything works like it should, and people are happy.

# License
See [LICENSE](LICENSE.md).
