APP_SRC			=	\
					main.cpp			\
					callbacks.cpp

APP_RES_FILES	=	$(shell find ./res -type f)

APP_PATH		=	$(PWD)/

APP_NAME		=	Terrain.exe

export

all:
	cd ../.. && $(MAKE) application USE_GDAL=1

debug: APP_NAME = TerrainD.exe
debug:
	cd ../.. && $(MAKE) application USE_GDAL=1 DEBUG=1