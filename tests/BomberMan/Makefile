ifeq ($(OS), Windows_NT)
APP_NAME			=	Bomberman.exe
else
APP_NAME			=	Bomberman
endif
APP_SRC				=	\
						src/Bomb.cpp \
						src/CrispyWall.cpp \
						src/Flame.cpp \
						src/Game.cpp \
						src/GameEntity.cpp \
						src/Level.cpp \
						src/Player.cpp \
						src/Wall.cpp \
						src/main.cpp

# APP_HEADERS			=	\
# 						include/Level.hpp
APP_INCLUDE_PATH	=	include
APP_RES_FILES		=	$(shell find ./res -type f)

APP_PATH			=	$(shell pwd)/

export

all:
	cd ../.. && $(MAKE) application

re: clean all

clean:
	rm -rf ./obj

ifeq ($(OS), Windows_NT)
debug: APP_NAME = BombermanD.exe
else
debug: APP_NAME = BombermanD
endif
debug: APP_CXXFLAGS = -g
debug:
	cd ../.. && $(MAKE) application DEBUG=1