ECHO=@echo
RM=rm
MKDIR=mkdir
LINK=gcc
COMP=gcc
CCFLAGS=-pedantic -Wall
RMFLAGS=-rf
MKDIRFLAGS=-p
LDFLAGS=$(CCFLAGS)
LIBS=
CLEAN=clean
BIN=bin

COMMUNICATION=communication
GAME=game

SERVER=server
SERVER_INT=$(addprefix $(GAME)/, awale.h) $(addprefix $(COMMUNICATION)/, communication.h) 
SERVER_REAL=$(SERVER_INT:.h=.c)
SERVER_OBJ=$(SERVER_INT:.h=.o)
SERVER_MAINFILE=$(SERVER)/server

CLIENT=client
CLIENT_INT=$(addprefix $(CLIENT)/, display.h) $(addprefix $(COMMUNICATION)/, communication.h) 
CLIENT_REAL=$(CLIENT_INT:.h=.c)
CLIENT_OBJ=$(CLIENT_INT:.h=.o)
CLIENT_MAINFILE=$(CLIENT)/client

.PHONY:$(CLEAN)

$(SERVER): $(SERVER_OBJ) $(SERVER_MAINFILE).o
	$(ECHO) "Linking of $(SERVER)"
	$(MKDIR) $(MKDIRFLAGS) $(BIN)
	$(LINK) -o $(BIN)/$(SERVER) $(SERVER_MAINFILE).o $(SERVER_OBJ) $(LIBS) $(LDFLAGS)

$(CLIENT): $(CLIENT_OBJ) $(CLIENT_MAINFILE).o
	$(ECHO) "Linking of $(CLIENT)"
	$(MKDIR) $(MKDIRFLAGS) $(BIN)
	$(LINK) -o $(BIN)/$(CLIENT) $(CLIENT_MAINFILE).o $(CLIENT_OBJ) $(LIBS) $(LDFLAGS)

%.o: %.c
	$(ECHO) "Compiling <$<>"
	$(COMP) -o $@ -c $< $(CCFLAGS)

$(CLEAN):
	$(ECHO) "Cleaning"
	$(RM) $(RMFLAGS) $(BIN) $(SERVER_OBJ) $(SERVER_MAINFILE).o $(CLIENT_OBJ) $(CLIENT_MAINFILE).o core
