Submitters: 
	Matan Weiss - 208722264, Zohar Simhon - 211871868
Installation: 
	1. extract the zip file,
	2. make sure you have the following files: server.c, hashmap.c, hashmap.h, headers.h, reactor.c, makefile
	3. run ‘make’ command
Usage:
	1. run './server'
	2. in other terminal, run 'telnet [hostname] 4000' (to get your hostname, run 'hostname')
	3. type a message and press enter (the maximum message size is 1024)
	4. repeat steps 2-3
	5. exit from a client with CTRL+] and then run 'quit'
	6. exit the program and free allocated resources with CTRL+C
