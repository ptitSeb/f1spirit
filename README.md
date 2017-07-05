f1spirit
====

![f1spirit build status](https://api.travis-ci.org/ptitSeb/f1spirit.png "f1spirit build status")

This version of F1 Spirit as been modified. It now support a GLES renderer, 
has C4A support (enable only on the Pandora) and works on the Pandora and ODROID.
To compile on the Pandora, using the Codeblocks Command Line PND, just type
`make`

To compile on the ODROID, type
`make ODROID=1`

To compile on x86 Linux, type
`make LINUX=1`

Other platform can probably be added easily (like RPi), but will need some slight 
changes in the Makefile (contact me or do a pull request if you want / have a new platform)

Here is a video of it running on an gigahertz OpenPandora
[![Play on Youtube](https://img.youtube.com/vi/ObQXqjt7518/0.jpg)](https://www.youtube.com/watch?v=ObQXqjt7518)

And here running on an ODroid XU4
[![Play on Youtube](https://img.youtube.com/vi/M7I4K3dlW34/0.jpg)](https://www.youtube.com/watch?v=M7I4K3dlW34)
