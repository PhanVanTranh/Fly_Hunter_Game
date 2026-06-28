CFLAGS		+= -I./sources/app/game/fly_hunter_game
CPPFLAGS	+= -I./sources/app/game/fly_hunter_game

VPATH += sources/app/game/fly_hunter_game

# CPP source files 
# fly_hunter game 
SOURCES_CPP += sources/app/game/fly_hunter_game/ar_game_fly_hunter.cpp
SOURCES_CPP += sources/app/game/fly_hunter_game/ar_game_arrow.cpp
SOURCES_CPP += sources/app/game/fly_hunter_game/ar_game_bang.cpp
SOURCES_CPP += sources/app/game/fly_hunter_game/ar_game_border.cpp
SOURCES_CPP += sources/app/game/fly_hunter_game/ar_game_meteoroid.cpp
SOURCES_CPP += sources/app/game/fly_hunter_game/ar_game_bee.cpp
SOURCES_CPP += sources/app/game/fly_hunter_game/ar_game_boss.cpp

