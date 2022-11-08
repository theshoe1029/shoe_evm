CC := gcc
CFLAGS := -Wall -Wno-unused-function -Wno-missing-braces -fPIC
OBJDIR := ./build
OBJS := $(addprefix $(OBJDIR)/, evm.o keccak256.o)
TEST_OBJS := $(addprefix $(OBJDIR)/, keccak256_test.o)
INCLUDES := ./include
TEST_INCLUDES := ./tst/include

all: clean $(OBJS)
	$(CC) $(CFLAGS) -I $(INCLUDES) $(OBJS) -shared -o libshoe-evm.so
	
test: all $(TEST_OBJS)
	$(CC) $(CFLAGS) -I $(TEST_INCLUDES) $(OBJS) $(TEST_OBJS) tst/test.c -o test && ./test

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -I $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: tst/%.c
	$(CC) $(CFLAGS) -I $(INCLUDES) -c $< -o $@

$(OBJS): | $(OBJDIR)
$(TEST_OBJS): | $(OBJS)

$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -rf build