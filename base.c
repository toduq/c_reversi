#include "reversi.h"

const board_t INITIAL_STATE = {
  PLAYER_TURN_BLACK,
  GAME_STATE_CONTINUE,
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
const char ZENKAKU_NUMBER_CHARS[8][4] = {"１", "２", "３", "４", "５", "６", "７", "８"};

stone_count_t count_stones(const board_t *board) {
  stone_count_t count = {};
  FOR_Y {
    FOR_X {
      char stone = board->stones[XY_TO_POS(x, y)];
      if(stone == board->turn){
        count.self++;
      } else if(stone == board->turn * -1) {
        count.opponent++;
      }
    }
  }
  return count;
}

bool is_pass(const board_t *board) {
  FOR_Y {
    FOR_X {
      if(check_stone(board, XY_TO_POS(x, y)) > 0) return false;
    }
  }
  return true;
}

int check_stone(const board_t *board, int pos) {
  // そこに置くことによってひっくり返る石の個数を返す
  if(board->stones[pos] != 0) return 0;
  int count = 0;
  FOR_DIRECTION {
    for(int search=1; search<8; search++){
      int current_stones = board->stones[pos + DIRECTIONS[d]*search];
      if(current_stones == 0) {
        break;
      } else if(current_stones == board->turn) {
        if(search == 1) break;
        count += search - 1;
      }
    }
  }
  return count;
}

int take_stone(board_t *board, int pos) {
  // 実際に石をひっくり返してみて、そのひっくり返した石の個数を返す
  assert(board->stones[pos] == 0);
  int count = 0;
  FOR_DIRECTION {
    for(int search=1; search<8; search++){
      int current_stones = board->stones[pos + DIRECTIONS[d]*search];
      if(current_stones == 0) {
        // 空き地はひっくり返せない
        break;
      } else if(current_stones == board->turn) {
        // 実際に石をひっくり返す
        count += search - 1;
        for(int flip=1; flip<search; flip++) {
          board->stones[pos + DIRECTIONS[d]*flip] = board->turn;
        }
        break;
      }
    }
  }
  assert(count > 0);
  board->stones[pos] = board->turn;
  board->turn *= -1;
  if(is_pass(board)){
    board->state = GAME_STATE_PASSED;
    board->turn *= -1;
    if(is_pass(board)){
      board->state = GAME_STATE_FINISHED;
    }
  } else {
    board->state = GAME_STATE_CONTINUE;
  }
  return count;
}

void print_board(const board_t *board) {
  PRINT_SEPARATOR;
  printf("　ＡＢＣＤＥＦＧＨ\n");
  FOR_Y {
    printf("%s", ZENKAKU_NUMBER_CHARS[y-1 ]);
    FOR_X {
      printf("%s", BOARD_MARK(board->stones[XY_TO_POS(x, y)]));
    }
    printf("\n");
  }
  stone_count_t count = count_stones(board);
  printf("自分 : %d, 相手 : %d\n", count.self, count.opponent);
}
