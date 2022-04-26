# OS-Ex4
OS Course Ex4 - Implementing Local &amp; TCP Thread Safe Stack

Recycled some of old code from OS Assignments.
The shell is based on Shell assignment,
The Server & Client is based on Multi-threaded TCP Server (+Its client).

Note: I'm not sure about the implementation of malloc & free, used based on tutorial and made a simpler version.
I've seen with valgrind that It shows as invalid reads - The question is wether it's because I read & write memory without the allocation or something is wrong with the implementation. 

Testing was performed using `test.txt` | Use pipes to set input and output, comparing it to the expected data.

# Makefile:
`make all`

`make clean`

Note: cleanup is used to also remove unwanted test trash.

## All & Cleanup


## Testing
`make test`

## Local Stack & Shell
`make local`

## Server &  Client (Multi threaded)
Server:
`make server`


Client:
`make client`


# Usage:
After using `make` command, follow the instructions to run:


## Testing

Make sure that there is no other server running.
Run using: `./test`

Results: If the output shows that the stack is empty, thus it's valid.
Note: Terminate the server if wanted by CTRL + C

## Local
`./localShell`
In local, the stack is saved locally, used mainly for debugging & testing.

## Server & Client
Start a server using `./server` (or by testing).

Then, create client instance using `./client <IP>` where by default (as the server is currently hard coded) you may use 127.0.0.1 as IP.
