CC=gcc
CFLAGS=-Wall -Wextra -pthread #-Werror -pedantic
ODIR = build
TARGET = forensic
OBJS = $(ODIR)/server_util.o $(ODIR)/server.o $(ODIR)/request_queue.o

.PHONY: all clean

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

ifdef DEV_INFO
DEV_FLAGS = -save-temps
endif

#$(foreach TARGET, $(TARGETS), $(eval $(call make_target, $(TARGET))))

$(ODIR)/%.o: %.c
	$(CC) $(CFLAGS) $(DEV_FLAGS) -MMD -c $< -o $@

clean:
	rm -f $(TARGET) $(ODIR)/*.o $(ODIR)/*.d $(ODIR)/*.i $(ODIR)/*.s *.txt *.csv

-include $(TARGET:=.d)
