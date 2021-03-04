BIN_1 := main 
BIN_2 := client
SOURCES_1 := main.c
SOURCES_2 := client.c
DEPENDENCIES = output.c get_info.c network.c
CC := gcc
CFLAGS :=
LDFLAGS := -lpthread

$(BIN_1): $(SOURCES_1) $(DEPENDENCIES)
	clear && $(CC) $(SOURCES_1) $(DEPENDENCIES) $(LDFLAGS) -o $(BIN_1)
$(BIN_2): $(SOURCES_2) $(DEPENDENCIES)
	clear && $(CC) $(SOURCES_2) $(DEPENDENCIES) $(LDFLAGS) -o $(BIN_2)
clean:
	rm -rf $(BIN_1) $(BIN_2)