# Inspiration: https://spin.atomicobject.com/2016/08/26/makefile-c-projects/

# The package name (i.e. its folder name)
PACKAGE_NAME 		:= $(notdir $(CURDIR))
# Hard-coded in Dockerfile, do not change
BINARY				:= rps-cpp

BUILD_DIR ?= build
SRC_DIRS ?= src
LIB_DIR ?= lib

SRCS := $(shell find $(SRC_DIRS) -not -name *_test* -name *.cpp -or -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d) $(shell find $(INC_DIR) -type d) 
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) -I$(LIB_DIR)

LDFLAGS := -pthread

CC := gcc
CXX := g++
# C flags
CFLAGS := -std=c11
# C++ flags
CXXFLAGS := -std=c++11
# C/C++ flags
CPPFLAGS := $(INC_FLAGS) -MMD -MP -g -Wall -Wextra -Wpedantic

# Project Build Image
BUILDER_DOCKERFILE	:= Dockerfile.builder
BUILDER_IMAGE_NAME 	:= $(PACKAGE_NAME)-builder

# Target Deployed Image
REGISTRY_URL 		:= 
DOCKERFILE			:= Dockerfile
IMAGE_NAME 			:= $(REGISTRY_URL)$(PACKAGE_NAME)-img

# -----------------------------------------------------------------------------

.PHONY: all build

all: build

build: $(BUILD_DIR)/$(BINARY)

# Executable
$(BUILD_DIR)/$(BINARY): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# -----------------------------------------------------------------------------

# Testing 
TEST_EXEC := rps-cpp_test
#INC_TEST := $(INC_FLAGS) -Ilib/catch
#LD_TEST := -pthread
SRCS_TEST := $(shell find $(SRC_DIRS) -not -name main.cpp -name *.cpp -or -name *.c)
OBJS_TEST := $(SRCS_TEST:%=$(BUILD_DIR)/%.o)

.PHONY: test
test: $(BUILD_DIR)/$(TEST_EXEC)

$(BUILD_DIR)/$(TEST_EXEC): $(OBJS_TEST)
	$(CXX) $(OBJS_TEST) -o $@ $(LDFLAGS) $(LD_TEST)

# -----------------------------------------------------------------------------

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# -----------------------------------------------------------------------------

.PHONY: clean
clean:
	$(RM) -rf $(BUILD_DIR)/*

.DELETE_ON_ERROR:

# -----------------------------------------------------------------------------
# Docker 

# Create and push the image (IMAGE_NAME needs to be a repo url)
docker-push: docker 
	docker push $(IMAGE_NAME)

# Create the project image
docker: docker-build
	docker build -f $(DOCKERFILE) -t $(IMAGE_NAME) $(CURDIR)

# Builds the project inside the builder image
docker-build: 
	docker run --rm -v "$(CURDIR):/src/" $(BUILDER_IMAGE_NAME) \
		make build BUILD_DIR=$(BUILD_DIR)/docker

# Launch this rule if you modify Dockerfile.builder
docker-builder:
	docker build -f $(BUILDER_DOCKERFILE) -t $(BUILDER_IMAGE_NAME) \
		$(CURDIR)

# -----------------------------------------------------------------------------

-include $(DEPS)

MKDIR_P ?= mkdir -p