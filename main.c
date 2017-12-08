#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define FOR_X for(int x=1; x<9; x++)
#define FOR_Y for(int y=1; y<9; y++)
#define FOR_DIRECTION for(int d=0; d<8; d++)
#define PRINT_SEPARATOR printf("---------\n")

#define BOARD_MARK(c) (c==-1 ? '*' : c==0 ? ' ' : '#')
#define XY_TO_POS(x, y) (y*10+x)
#define POS_TO_X(pos) (x%10)
#define POS_TO_Y(pos) (y/10)

typedef char board_t[100];
typedef struct board_state {
  int turn;
  board_t board;
} board_state_t;

const board_state_t INITIAL_STATE = {
  1,
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, -1, 0, 0, 0, 0,
    0, 0, 0, 0, -1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  }
};

const int DIRECTIONS[8] = {-11, -10, -9, -1, 1, 9, 10, 11};

void print_board(const board_t board) {
  PRINT_SEPARATOR;
  printf(" abcdefgh\n");
  FOR_Y {
    printf("%d", y);
    FOR_X {
      printf("%c", BOARD_MARK(board[XY_TO_POS(x, y)]));
    }
    printf("\n");
  }
}

int get_user_input_pos(int turn) {
  PRINT_SEPARATOR;
  while(true) {
    printf("[%c] 置く場所を入力してください[a-h][1-8] : ", BOARD_MARK(turn));
    char buf[3];
    scanf("%2s", buf);
    if ('a' <= buf[0] && buf[0] <= 'h' && '1' <= buf[1] && buf[1] <= '8') {
      return (buf[1] - '0') * 10 + (buf[0] - 'a' + 1);
    } else {
      printf("入力が不正です。\n");
    }
  }
}

int put_stone(board_state_t *state, int pos, bool only_check) {
  if(state->board[pos] != 0) return -1;
  int flipped_stones = 0;
  FOR_DIRECTION {
    for(int search=1; search<8; search++){
      int current_stone = state->board[pos + DIRECTIONS[d]*search];
      if(current_stone == state->turn) {
        flipped_stones += search - 1;
        if (only_check) break;
        for(int flip=1; flip<search; flip++) {
          state->board[pos + DIRECTIONS[d]*flip] = state->turn;
        }
        break;
      } else if(current_stone == 0) {
        break;
      }
    }
  }
  if(flipped_stones > 0){
    state->board[pos] = state->turn;
    state->turn *= -1;
  }
  return flipped_stones;
}

int main() {
  board_state_t current_state;
  memcpy(&current_state, &INITIAL_STATE, sizeof(current_state));
  while(true) {
    print_board(current_state.board);
    int pos = get_user_input_pos(current_state.turn);
    put_stone(&current_state, pos, false);
  }
  return 0;
}
