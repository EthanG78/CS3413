#ifndef GLOBALS_H
#define GLOBALS_H

#define MOVE_LEFT 'a'
#define MOVE_RIGHT 'd'
#define MOVE_UP 'w'
#define MOVE_DOWN 's'
#define SHOOT ' '
#define QUIT 'q'

#define GAME_ROWS 24
#define GAME_COLS 80

/**** DIMENSIONS MUST MATCH the ROWS/COLS */
char *GAME_BOARD[] = {
"                   Score:          Lives:",
"=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-centipiede!=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"",
"",
"",
"",
"",
"",
"", 
"" };

#define ENEMY_HEIGHT 2
#define ENEMY_BODY_ANIM_TILES 8 

// MODIFY MODEL
char* ENEMY_BODY[ENEMY_BODY_ANIM_TILES][ENEMY_HEIGHT] = 
{
  {"1",
   "1"},
  {"2",
   "2"},
  {"3",
   "3"},
  {"4",
   "4"},
  {"5",
   "5"},
  {"6",
   "6"},
  {"7",
   "7"},
  {"8",
   "8"}
};

#define PLAYER_HEIGHT 3
#define PLAYER_WIDTH 3
#define PLAYER_BODY_ANIM_TILES 2 

// There are two annimation frames for the player
// The player has width three and height threee
//   A   M
//  <x> <X>
//   V   V
char* PLAYER_BODY[PLAYER_BODY_ANIM_TILES][PLAYER_WIDTH][PLAYER_HEIGHT] = 
{
  {{" ",
    "<",
    " "},
    {"A",
    "x",
    "V"},
    {" ",
    ">",
    " "}},
  {{" ",
    "<",
    " "},
    {"M",
    "X",
    "W"},
    {" ",
    ">",
    " "}},
};

#endif // GLOBALS_H