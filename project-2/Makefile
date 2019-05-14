CC=gcc
CFLAGS=-Wall -Wextra -pthread #-Werror -pedantic
ODIR = build
TARGET_SERVER = server
TARGET_USER = user
OBJS_SERVER = $(ODIR)/server_util.o $(ODIR)/server.o $(ODIR)/request_queue.o $(ODIR)/log.o $(ODIR)/server_acc_managing.o $(ODIR)/server_office_managing.o $(ODIR)/server_sync.o $(ODIR)/server_dispatcher.o
OBJS_USER = $(ODIR)/user_util.o $(ODIR)/user.o $(ODIR)/log.o

.PHONY: all clean

all: $(TARGET_SERVER) $(TARGET_USER)

$(TARGET_SERVER) : $(OBJS_SERVER)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER) $(OBJS_SERVER)

$(TARGET_USER) : $(OBJS_USER)
	$(CC) $(CFLAGS) -o $(TARGET_USER) $(OBJS_USER)

	touch slog.txt
	touch ulog.txt

ifdef DEV_INFO
DEV_FLAGS = -save-temps
endif

#$(foreach TARGET, $(TARGETS), $(eval $(call make_target, $(TARGET))))

$(ODIR)/%.o: %.c
	$(CC) $(CFLAGS) $(DEV_FLAGS) -MMD -c $< -o $@

clean:
	rm -f $(TARGET_SERVER) $(ODIR)/*.o $(ODIR)/*.d $(ODIR)/*.i $(ODIR)/*.s *.txt *.csv
	rm -f $(TARGET_USER) $(ODIR)/*.o $(ODIR)/*.d $(ODIR)/*.i $(ODIR)/*.s *.txt *.csv

-include $(TARGET:=.d)
