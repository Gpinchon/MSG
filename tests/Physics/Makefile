APP_NAME			=	Physics.exe
APP_SRC				=	src/main.cpp
APP_INCLUDE_PATH	=	include
APP_RES_FILES		=	$(shell find ./res -type f)

APP_PATH			=	$(shell pwd)/

export

all:
	cd ../.. && $(MAKE) application

debug: APP_NAME = PhysicsD.exe
debug: APP_CXXFLAGS = -g
debug:
	cd ../.. && $(MAKE) application DEBUG=1