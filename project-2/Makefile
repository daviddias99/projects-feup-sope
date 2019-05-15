CC=gcc
CFLAGS=-Wall -Wextra -pthread -Werror #-pedantic
ODIR = build

SRC_SHARED = src
SRC_SERVER = src/server
SRC_USER = src/user

TARGET_SERVER = server
TARGET_USER = user

OBJS_SERVER = $(ODIR)/server_util.o $(ODIR)/server.o $(ODIR)/request_queue.o $(ODIR)/log.o  $(ODIR)/server_sync.o $(ODIR)/server_accounts.o $(ODIR)/server_operations.o $(ODIR)/server_bank_office.o
OBJS_USER = $(ODIR)/user_util.o $(ODIR)/user_validation.o $(ODIR)/user.o $(ODIR)/log.o


.PHONY: all clean

all: $(TARGET_SERVER) $(TARGET_USER) logfiles

logfiles: 
	touch slog.txt
	touch ulog.txt

$(TARGET_SERVER) : $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER) $(OBJS_SERVER)

$(TARGET_USER) : $(OBJS_USER)
	$(CC) $(CFLAGS) -o $(TARGET_USER) $(OBJS_USER)


ifdef DEV_INFO
DEV_FLAGS = -save-temps
endif


$(ODIR)/%.o: $(SRC_SHARED)/%.c
	$(CC) $(CFLAGS) $(DEV_FLAGS) -MMD -c $< -o $@

$(ODIR)/%.o: $(SRC_SERVER)/%.c
	$(CC) $(CFLAGS) $(DEV_FLAGS) -MMD -c $< -o $@

$(ODIR)/%.o: $(SRC_USER)/%.c
	$(CC) $(CFLAGS) $(DEV_FLAGS) -MMD -c $< -o $@


clean:
	rm -f $(TARGET_SERVER) $(ODIR)/*.o $(ODIR)/*.d $(ODIR)/*.i $(ODIR)/*.s *.txt *.csv
	rm -f $(TARGET_USER) $(ODIR)/*.o $(ODIR)/*.d $(ODIR)/*.i $(ODIR)/*.s *.txt *.csv

-include $(TARGET:=.d)
