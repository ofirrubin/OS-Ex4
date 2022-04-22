# OS-Ex4
OS Course Ex4 - Implementing Local &amp; TCP Thread Safe Stack

Recycled some of old code from OS Assignments.
The shell is based on Shell assignment,
The Server & Client is based on Multi-threaded TCP Server (+Its client).

Note: I'm not sure about the implementation of malloc & free, used based on tutorial and made a simpler version.
I've seen with valgrind that It shows as invalid reads - The question is wether it's because I read & write memory without the allocation or something is wrong with the implementation. 

Testing was performed using `test.txt` | Use pipes to set input and output, comparing it to the expected data.

# Makefile:


## Local Stack & Shell
`make local`

## Server &  Client (Multi threaded)
Server:
`make server`


Client:
`make client`


# Usage:

## Local
`./localShell`


## Server & Client
Start the server using `./server`

Then, create client instance using `./client <IP>` where by default (as the server is currently hard coded) you may use 127.0.0.1 as IP.
