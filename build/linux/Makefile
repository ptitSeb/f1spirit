EXE = f1spirit
SRC = sources
ifeq ($(ODROID),1)
 PREFIX = /usr/local
else ifeq ($(LINUX),1)
 PREFIX = /usr/local
else
 PREFIX = /mnt/utmp/f1spirit
endif
EXTRADIR = ./build/linux

GAMEDIR = $(PREFIX)/games
STARTUP = $(GAMEDIR)/$(EXE)
BINDIR = $(PREFIX)/share/games/$(EXE)
ICNDIR = $(PREFIX)/share/pixmaps
APPDIR = $(PREFIX)/share/applications

DATA = demos graphics tracks sound readme.txt
ICON = f1spirit.png
DESKTOP = f1spirit.desktop

OBJS = \
	$(SRC)/2DCMC.o					$(SRC)/auxiliar.o				\
	$(SRC)/CCar.o					$(SRC)/CPlayer.o				\
	$(SRC)/debug.o					$(SRC)/EnemyCCar.o				\
	$(SRC)/F1SComputer.o			$(SRC)/F1Shttp.o				\
	$(SRC)/F1SpiritApp.o			$(SRC)/F1Spirit-auxiliar.o		\
	$(SRC)/F1SpiritGame.o			$(SRC)/F1SpiritTrackViewer.o	\
	$(SRC)/GameParameters.o			$(SRC)/geometrics.o				\
	$(SRC)/GLTile.o					$(SRC)/GLtile-f1.o				\
	$(SRC)/keyboardstate.o			$(SRC)/main.o					\
	$(SRC)/PlacedGLTile.o			$(SRC)/PlayerCCar.o				\
	$(SRC)/RacingCCar.o				$(SRC)/ranrotb.o				\
	$(SRC)/ReplayInfo.o				$(SRC)/RoadPiece.o				\
	$(SRC)/RotatedGLTile.o			$(SRC)/SDL_glutaux.o			\
	$(SRC)/sound.o					$(SRC)/state_disclaimer.o		\
	$(SRC)/state_endsequence.o		$(SRC)/state_gameoptions.o		\
	$(SRC)/state_gamestart.o		$(SRC)/state_hiscore.o			\
	$(SRC)/state_konami.o			$(SRC)/state_menu.o				\
	$(SRC)/state_menu_create_menu.o	$(SRC)/state_menu_draw.o		\
	$(SRC)/state_race.o				$(SRC)/state_race_result.o		\
	$(SRC)/state_replaymanager.o	$(SRC)/state_title.o			\
	$(SRC)/state_trackload.o		$(SRC)/track.o					\
	$(SRC)/Vector.o					$(SRC)/weather.o				\
	$(SRC)/eglport.o 								\
	$(SRC)/filehandling.o			$(SRC)/CarEngineSound.o

CC = gcc
ifeq ($(ODROID),1)
 CFLAGS = -mcpu=cortex-a9 -mfpu=neon -mfloat-abi=hard -g3 -O3 -Wall `sdl-config --cflags` `curl-config --cflags` -I/usr/X11R6/include -Ithirdparty -DHAVE_GLES -D__USE_SDL_GLES__ -DODROID -fdiagnostics-color=auto
 LDFLAGS = `sdl-config --libs` `curl-config --libs` -L/usr/X11R6/lib/ -lSDL_image -lSDL_mixer -lSDL_net -lm -lstdc++ -lGLESv1_CM -lEGL
else ifeq($(PYRA),1)
 CFLAGS = -mcpu=cortex-a15 -mfpu=neon-vfpv4 -marm -mfloat-abi=hard -g3 -O3 -Wall `sdl-config --cflags` `curl-config --cflags` -I/usr/X11R6/include -Ithirdparty -DPYRA 
 LDFLAGS = `sdl-config --libs` `curl-config --libs` -L/usr/X11R6/lib/ -lSDL_image -lSDL_mixer -lSDL_net -lm -lstdc++ -lGL
else ifeq ($(LINUX),1)
 CFLAGS = -g3 -O3 -Wno-write-strings `sdl-config --cflags` `curl-config --cflags` -I/usr/X11R6/include
 LDFLAGS = `sdl-config --libs` `curl-config --libs` -L/usr/X11R6/lib/ -lSDL_image -lSDL_mixer -lSDL_net -lm -lstdc++ -lGL
else
 CFLAGS = -g -gdwarf-2 -mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -ffast-math -O3 -Wno-write-strings -fsingle-precision-constant -fdiagnostics-color=auto -DPANDORA -DHAVE_GLES -DHAVE_C4A  -D__USE_SDL_GLES__ `sdl-config --cflags` `curl-config --cflags` -I/mnt/utmp/codeblocks/usr/X11R6/include
 LDFLAGS = `sdl-config --libs` `curl-config --libs` -L/usr/X11R6/lib/ -lSDL_image -lSDL_mixer -lSDL_net -lm -lstdc++ -lGLES_CM -lEGL
endif
CFLAGS += -Wno-write-strings
RM = rm -f
CP = cp -r
MD = mkdir -p
ECHO = echo
CHMOD = chmod
STRIP = strip

all: $(EXE)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(EXE): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)
#	@$(STRIP) $@
	@$(ECHO) " o If there are no errors, the game compiled succesfully"

clean:
	@$(ECHO) " o Cleaning up source directory"
	@$(RM) $(SRC)/*.o $(SRC)/*.bak core $(EXE)

install: all
	@$(ECHO) " o Creating install directory $(BINDIR)"
	@$(MD) "$(BINDIR)"
	@$(ECHO) " o Installing game and data to $(BINDIR)"
	@$(CP) "$(EXE)" $(DATA) "$(BINDIR)"
	@$(ECHO) " o Creating startup script $(STARTUP)"
	@$(MD) "$(GAMEDIR)"
	@$(ECHO) "#!/bin/sh" >"$(STARTUP)"
	@$(ECHO) "cd \"/usr/share/games/f1spirit/\"; ./$(EXE); cd -" >>"$(STARTUP)"
#	@$(ECHO) "cd \"$(BINDIR)\"; ./$(EXE); cd -" >>"$(STARTUP)"
	@$(CHMOD) 755 "$(STARTUP)"
	@$(ECHO) " o Creating application menu entry"
	@$(MD) "$(ICNDIR)"
	@$(CP) "$(EXTRADIR)/$(ICON)" "$(ICNDIR)"
	@$(MD) "$(APPDIR)"
	@$(CP) "$(EXTRADIR)/$(DESKTOP)" "$(APPDIR)"
	@$(ECHO) ""

uninstall:
	@$(ECHO) " o Removing game and data from $(BINDIR)"
	@$(RM) -r "$(BINDIR)"
	@$(ECHO) " o Removing startup script $(STARTUP)"
	@$(RM) "$(STARTUP)"
	@$(ECHO) " o Removing application menu entry"
	@$(RM) "$(ICNDIR)/$(ICON)" "$(APPDIR)/$(DESKTOP)"
	@$(ECHO) ""

