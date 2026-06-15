# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Source and Object Files
SRCS = src/User.cpp src/UserManager.cpp src/Post.cpp src/PostManager.cpp src/Feed.cpp src/SocialGraph.cpp src/UIComponent.cpp src/TextInput.cpp src/GlassButton.cpp src/LoginScreen.cpp src/RegisterScreen.cpp src/PostCard.cpp src/FeedScreen.cpp src/App.cpp src/main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = PakistanHub.exe

# Default target
all: $(TARGET)

# Link executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts using PowerShell for safety on Windows
clean:
	powershell -Command "Get-ChildItem -Path src/*.o -ErrorAction SilentlyContinue | Remove-Item; if (Test-Path $(TARGET)) { Remove-Item $(TARGET) }"

# Run target
run: all
	./$(TARGET)
