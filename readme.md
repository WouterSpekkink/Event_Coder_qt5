# Event Coder
This is the code repository for the Event Coder program, which is a tool I built to facilitate (part of) the qualitative coding of event data. I wrote the program for personal use, but anyone who's interested in the program or the code is free to use it. I therefore release the code under the GLP 3.0 license. 

# Building the program from source
The program was built in C++ and Qt5, so if you want to build the program from the source code yourself, you'll need to have the C++ standard libraries and the Qt5 libraries installed on your machine. If you have the proper libraries installed on your machine, the following process will allow you to build the program from source.

Open a terminal and navigate to the folder with the source code. Then use the following commands.

```sh
qmake -project
qmake -makefile
```


