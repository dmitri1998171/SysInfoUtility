BIN_1 := main 
BIN_2 := client
SOURCES_1 := src/main.c
SOURCES_2 := src/client.c
DEPENDENCIES = src/output.c src/get_info.c src/network.c
CC := gcc
CFLAGS := -g
LDFLAGS := -lpthread

$(BIN_1): $(SOURCES_1) $(DEPENDENCIES)
	clear && mkdir -p bin && $(CC) $(SOURCES_1) $(DEPENDENCIES) $(CFLAGS) $(LDFLAGS) -o bin/$(BIN_1)
$(BIN_2): $(SOURCES_2) $(DEPENDENCIES)
	clear && mkdir -p bin && $(CC) $(SOURCES_2) $(DEPENDENCIES) $(CFLAGS) $(LDFLAGS) -o bin/$(BIN_2)
clean:
	rm -rf bin