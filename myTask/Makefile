all:
	clear && gcc Server-Thread.c DieWithError.c get_info.c -o server -lpthread && gcc Client.c DieWithError.c -o client

clean: 
	rm -rf server client