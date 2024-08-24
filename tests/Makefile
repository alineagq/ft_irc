NAME = run_tests
GTEST_DIR = googletest
GTEST_BUILD_DIR = $(GTEST_DIR)/build
GTEST_LIB_DIR = $(GTEST_BUILD_DIR)/lib
GTEST_INCLUDE_DIR = $(GTEST_DIR)/googletest/include
GMOCK_INCLUDE_DIR = $(GTEST_DIR)/googlemock/include

all: $(GTEST_BUILD_DIR)/libgtest.a $(NAME)

$(GTEST_BUILD_DIR)/libgtest.a:
	if [ ! -d "$(GTEST_DIR)" ]; then \
		git clone https://github.com/google/googletest.git $(GTEST_DIR); \
	fi
	cd $(GTEST_DIR) && mkdir -p build && cd build && cmake .. && make

$(NAME): $(NAME).cpp ../src/network/Socket.cpp ../src/network/TcpConnection.cpp
	$(CXX) -o $@ $^ -I$(GTEST_INCLUDE_DIR) -I$(GMOCK_INCLUDE_DIR) \
	$(GTEST_LIB_DIR)/libgtest.a \
	$(GTEST_LIB_DIR)/libgtest_main.a \
	$(GTEST_LIB_DIR)/libgmock.a \
	$(GTEST_LIB_DIR)/libgmock_main.a -pthread

coverage: $(NAME).cpp ../src/network/Socket.cpp ../src/network/TcpConnection.cpp
	$(CXX) -o $(NAME) $^ -I$(GTEST_INCLUDE_DIR) -I$(GMOCK_INCLUDE_DIR) \
	$(GTEST_LIB_DIR)/libgtest.a \
	$(GTEST_LIB_DIR)/libgtest_main.a \
	$(GTEST_LIB_DIR)/libgmock.a \
	$(GTEST_LIB_DIR)/libgmock_main.a -pthread --coverage
	./$(NAME)
	gcovr -r .. --filter='../src/'

integrations: ./integrations/TestReceiveMessage.cpp ../src/network/Socket.cpp ../src/network/TcpConnection.cpp
	$(CXX) -o $(NAME) $^ -I$(GTEST_INCLUDE_DIR) -I$(GMOCK_INCLUDE_DIR) \
	$(GTEST_LIB_DIR)/libgtest.a \
	$(GTEST_LIB_DIR)/libgtest_main.a \
	$(GTEST_LIB_DIR)/libgmock.a \
	$(GTEST_LIB_DIR)/libgmock_main.a -pthread
	./$(NAME)

clean:
	rm -f $(NAME) *.gcno *.gcda coverage
	rm -rf $(GTEST_DIR)

.PHONY: all clean coverage