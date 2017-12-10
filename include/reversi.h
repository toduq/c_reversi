#ifndef _REVERSI_H_
#define _REVERSI_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#define FOR_X for(int x=1; x<9; x++)
#define FOR_Y for(int y=1; y<9; y++)
#define FOR_DIRECTION for(int d=0; d<8; d++)

#define XY_TO_POS(x, y) ((y)*10+(x))

#define PRINT_SEPARATOR printf("\n------------------\n\n")
#define BOARD_MARK(c) (c==-1 ? "⬜" : c==0 ? "　" : "⬛")

typedef enum player_turn {
  PLAYER_TURN_BLACK = 1,
  PLAYER_TURN_WHITE = -1
} player_turn_t;

typedef enum board_state {
  GAME_STATE_CONTINUE,
  GAME_STATE_PASSED,
  GAME_STATE_FINISHED
} board_state_t;

typedef struct board {
  player_turn_t turn;
  board_state_t state;
  char stones[100];
  int last_pos;
} board_t;

typedef struct stone_count {
  int self;
  int opponent;
} stone_count_t;

typedef int (*pos_choice_function)(board_t *board);

extern const board_t INITIAL_STATE;
extern const int DIRECTIONS[8];

stone_count_t count_stones(const board_t *board);
void print_board(const board_t *board);
int check_stone(const board_t *board, int pos);
int take_stone(board_t *board, int pos);

int cpu_first_taker(board_t *board);
int cpu_random_taker(board_t *board);
int cpu_much_taker(board_t *board);
int cpu_recursive_taker(board_t *board);

#endif
