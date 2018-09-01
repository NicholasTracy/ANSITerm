# <p align="center">ANSITerm Library</p>

### <p align="center"> ANSITerm is an Arduino library providing ANSI escape sequence control of a capable terminal using the stream class.</p>

---

### <p align="center">This is the master branch, this is where all stable releases will be generated. There <i>should not</i> be any issues with this build, however if you do find a bug, please check the [Issues](https://github.com/NicholasTracy/ANSITerm/issues) section to see if it has been brought to my attention. If it hasn't, please create a bug report and include as much detail as possible so that I can recreate it.</p>

---

<p align="center">Current Travis CI Build Status for the master branch:<a href="https://travis-ci.org/NicholasTracy/ANSITerm"><br>
<img src="https://travis-ci.org/NicholasTracy/ANSITerm.svg?branch=master" alt="Travis CI Build Status">
</a></p>

---

##### This code is loosely based on the work done in the *BasicTerm* Library
[BasicTerm GitHub](http://github.com/nottwo/BasicTerm).


##### Also, some inspiration was taken from the *VT100 library* by Rob Tillaart
[VT100 GitHub](https://github.com/RobTillaart/Arduino/tree/master/libraries/VT100)


##### ANSI Escape Sequences used in this library are found at:

[Assci-Table.com - ANSI escape sequences](http://ascii-table.com/ansi-escape-sequences.php)

**_and_**

[Wikipedia article on ANSI escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code#Escape_sequences)

---

### Installation:


Library Manager Method:

This method will be available after the 0.1.0 release is made ready.

Open the library manager in Arduino IDE 1.6 or newer and search for ANSITerm. Click `Install` and wait for the install to complete. After, the library should be ready to use. You can check the Examples menu to see how the library works.


Manual Method:

Downloading the ANSITerm repository and extracting the ANSITerm directory into
`~/sketchbook/libraries` and restarting the Arduino IDE should be
sufficient to make BasicTerm available in the Sketch | Import Library
menu.

If you are having trouble please refer to this link for an in-depth guide on both methods.
[Arduino Library Installation Guide](https://www.arduino.cc/en/Guide/Libraries#toc3)

---

### License:


BasicTerm is licensed under the terms of the GNU Lesser
General Public License v3.

No warranties or guarentees are made, the library is provided as is.
[License Information]( ANSITerm/LICENSE )


### Author:


Nicholas Tracy [Github](https://github.com/NicholasTracy)


Original Idea and bases for this library credited to:
Trannie Carter & <borys@nottwo.org>

---

### Things to come:


+ Complete ANSI escape Sequence support for all terminals - Allowing the user to select the terminal level at initialization
+ Baked in ANSI graphics and animations support, making it easier to create GUI and BBS style menus and titles.
+ .ANS file support for loading graphics and animations from SD card or other memory.

---

### Current release status:

Library is currently still in development. No releases have been made yet. 
