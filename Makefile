# 通用Makefile：编译 src/ 下所有 .cpp 文件和主目录下的 test*.cpp 文件


CXX = g++
 CXXFLAGS = -std=c++11 -Wall
 INCLUDES = -I./include
SRC_DIR = src
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
MAIN_TEST = $(wildcard test*.cpp)
OBJS = $(SRC_FILES) $(MAIN_TEST)
TARGET = testApp

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) -llog4cpp -lmysqlclient -lavformat -lavcodec -lavutil -lswscale -lcrypto -lcurl

clean:
	rm -f $(TARGET)
