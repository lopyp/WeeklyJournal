CXX = g++

CXXFLAGS = -std=c++17 -Wall -Werror -Wextra

TARGET = daily_planner

SRCS = journal.cc

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
