
NAME = run_tests

all: $(NAME)

$NAME: $(NAME).cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

coverage: $(NAME).cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) --coverage

clean:
	rm -f $(NAME) *.gcno *.gcda

.PHONY: all clean coverage