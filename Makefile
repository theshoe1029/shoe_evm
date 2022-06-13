CC := gcc
CFLAGS := -Wall -Wno-unused-function -fPIC
OBJDIR := ./build
OBJS := $(addprefix $(OBJDIR)/, evm.o hex_prefix.o keccak256.o rlp.o trie.o)
TEST_OBJS := $(addprefix $(OBJDIR)/, hex_prefix_test.o rlp_test.o trie_test.o keccak256_test.o)
INCLUDES := ./include
TEST_INCLUDES := ./tst/include

all: clean $(OBJS)
	$(CC) $(CFLAGS) -I $(INCLUDES) $(OBJS) main.c -shared -o cevm.so cevm
	
test: all $(TEST_OBJS)
	$(CC) $(CFLAGS) -I $(TEST_INCLUDES) $(OBJS) $(TEST_OBJS) test.c -o test && ./test

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -I $(INCLUDES) -c $< -o $@

$(OBJDIR)/%.o: tst/%.c
	$(CC) $(CFLAGS) -I $(INCLUDES) -c $< -o $@

$(OBJS): | $(OBJDIR)
$(TEST_OBJS): | $(OBJS)

$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -rf build