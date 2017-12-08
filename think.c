#include "reversi.h"

int cpu_first_taker(board_t *board) {
  FOR_X {
    FOR_Y {
      int pos = XY_TO_POS(x, y);
      if(check_stone(board, pos) > 0) return pos;
    }
  }
  assert(false);
}

int cpu_random_taker(board_t *board) {
  int places[30] = {};
  int place_count = 0;
  FOR_X {
    FOR_Y {
      int pos = XY_TO_POS(x, y);
      if(check_stone(board, pos) == 0) continue;
      places[place_count++] = pos;
    }
  }
  assert(place_count > 0);
  return places[rand() % place_count];
}

int cpu_much_taker(board_t *board) {
  int max_pos = -1;
  int max_count = 0;
  FOR_X {
    FOR_Y {
      int pos = XY_TO_POS(x, y);
      int count = check_stone(board, pos);
      if(count == 0 || max_count >= count) continue;
      max_pos = pos;
      max_count = count;
    }
  }
  assert(max_count > 0);
  return max_pos;
}

typedef struct pos_score {
  int pos;
  int value;
  int searched_board_count; // for debug
} pos_score_t;

static const int POS_EVALUATION[100] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 30, -12, 0 -1, -1, 0, -12, 30, 0,
  0, -12, -15, -3, -3, -3, -3, -15, -12, 0,
  0, 0, -3, 0, -1, -1, 0, -3, 0, 0,
  0, 0, -3, -1, -1, -1, -1, -3, 0, 0,
  0, 0, -3, -1, -1, -1, -1, -3, 0, 0,
  0, 0, -3, 0, -1, -1, 0, -3, 0, 0,
  0, -12, -15, -3, -3, -3, -3, -15, -12, 0,
  0, 30, -12, 0 -1, -1, 0, -12, 30, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int _evaluate_board(const board_t *board) {
  if(board->state == GAME_STATE_FINISHED) {
    stone_count_t count = count_stones(board);
    return count.self > count.opponent ? 1000 : -1000;
  }
  int score = 0;
  FOR_X {
    FOR_Y {
      int pos = XY_TO_POS(x, y);
      if(board->stones[pos] == 0) continue;
      score += POS_EVALUATION[pos] * (board->stones[pos]==board->turn ? 1 : -1);
    }
  }
  return score;
}

pos_score_t _recursive_much_taker(const board_t *board, int depth, int upper_limit) {
  // 現状のボードの、置けるすべての場所に石を置いてみて、
  // そのうち最大の評価値の場所のとその評価値
  // 評価値とはdepthが0の時は、自分と相手の石の数の差
  // depth>0の時は、再帰探索したときに相手の評価値が最小となる場所の、評価値
  if(depth == 0) {
    int value = _evaluate_board(board);
    pos_score_t score = {-1, value, 1};
    return score;
  }
  pos_score_t best = {-1, -10000, 0};
  FOR_X {
    FOR_Y {
      // そこが置ける場所か確認する
      int count = check_stone(board, XY_TO_POS(x, y));
      if(count == 0) continue;
      // 実際に置いてみる
      board_t next_board;
      memcpy(&next_board, board, sizeof(next_board));
      take_stone(&next_board, XY_TO_POS(x, y));
      // 評価値を決める
      pos_score_t score;
      switch(next_board.state) {
        case GAME_STATE_FINISHED:
          score = _recursive_much_taker(&next_board, 0, 0);
          break;
        case GAME_STATE_CONTINUE:
          // [FIXME] best.value*-1ってあってる？
          score = _recursive_much_taker(&next_board, depth-1, best.value*-1);
          score.value *= -1;
          break;
        case GAME_STATE_PASSED:
          score = _recursive_much_taker(&next_board, depth-1, upper_limit);
          break;
      }
      best.searched_board_count += score.searched_board_count;
      // upper limitより大きい場合は、その上位の探索で選ばれる可能性がなくなるので、探索を打ち切る
      if(score.value >= upper_limit) {
        best.pos = -1;
        best.value = upper_limit + 1;
        return best;
      }
      if(score.value > best.value) {
        best.pos = XY_TO_POS(x, y);
        best.value = score.value;
      }
    }
  }
  assert(best.pos != -1);
  return best;
}

int cpu_recursive_taker(board_t *board) {
  pos_score_t score = _recursive_much_taker(board, 5, 10000);
  return score.pos;
}
