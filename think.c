#include "reversi.h"
#include "graphtree.h"

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
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0, 30,-12,  0, -1, -1,  0,-12, 30,  0,
  0,-12,-15, -3, -3, -3, -3,-15,-12,  0,
  0,  0, -3,  0, -1, -1,  0, -3,  0,  0,
  0,  0, -3, -1, -1, -1, -1, -3,  0,  0,
  0,  0, -3, -1, -1, -1, -1, -3,  0,  0,
  0,  0, -3,  0, -1, -1,  0, -3,  0,  0,
  0,-12,-15, -3, -3, -3, -3,-15,-12,  0,
  0, 30,-12,  0, -1, -1,  0,-12, 30,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

graphtree_t *current_graph;

void graphtree_exit_board(const board_t *board, char *format, ...) {
  char buf[1000] = {'\0'};
  va_list arg;
  va_start(arg, format);
  vsprintf(buf, format, arg);
  va_end(arg);
  strcat(buf, "\\n");
  FOR_Y {
    FOR_X {
      strcat(buf, BOARD_MARK(board->stones[XY_TO_POS(x,y)]));
    }
    strcat(buf, "\\n");
  }
  graphtree_exit(current_graph, buf);
}

int _evaluate_board(const board_t *board) {
  stone_count_t count = count_stones(board);
  if(board->state != GAME_STATE_FINISHED) {
    // 序盤は石の場所の重み付けにより評価する
    int score = 0;
    FOR_X {
      FOR_Y {
        int pos = XY_TO_POS(x, y);
        if(board->stones[pos] == 0) continue;
        score += POS_EVALUATION[pos] * (board->stones[pos]==board->turn ? 1 : -1);
      }
    }
    return score;
  } else {
    // 終盤は石の多さのみを評価にする
    return (count.self - count.opponent) * 100;
  }
}

pos_score_t _recursive_taker(const board_t *board, int depth, int upper_limit, int lower_limit) {
  int search_order = graphtree_enter(current_graph);
  if(depth == 0 || board->state == GAME_STATE_FINISHED) {
    int value = _evaluate_board(board);
    pos_score_t score = {-1, value, 1};
    graphtree_exit_board(board, "%s#%d => %d, [%d, %d]", BOARD_MARK(board->turn), search_order, score.value, upper_limit, lower_limit);
    return score;
  }
  pos_score_t best = {-1, -88888, 0};
  FOR_X {
    FOR_Y {
      // そこが置ける場所か確認する
      int pos = XY_TO_POS(x, y);
      int count = check_stone(board, pos);
      if(count == 0) continue;
      // 実際に置いてみる
      board_t next_board;
      memcpy(&next_board, board, sizeof(next_board));
      take_stone(&next_board, pos);
      // 置いてみた盤面を評価する
      // next_board自分が置き終わって相手の番になっている
      pos_score_t score;
      switch(next_board.state) {
        case GAME_STATE_CONTINUE:
          score = _recursive_taker(&next_board, depth-1, (lower_limit > best.value ? lower_limit : best.value)*-1, upper_limit*-1);
          score.value *= -1;
          break;
        case GAME_STATE_PASSED:
          score = _recursive_taker(&next_board, depth-1, upper_limit, lower_limit);
          break;
        case GAME_STATE_FINISHED:
          score = _recursive_taker(&next_board, 0, 0, 0);
          score.value *= -1;
          break;
      }
      best.searched_board_count += score.searched_board_count;
      if(upper_limit <= score.value) {
        // 探索打ち切り
        graphtree_enter(current_graph);
        graphtree_exit(current_graph, "cut");
        graphtree_exit_board(board, "%s[%d] => %d @ %d, [%d, %d]", BOARD_MARK(board->turn), search_order, best.value, best.pos, upper_limit, lower_limit);
        return score;
      } else if(score.value > best.value) {
        // 最善値の更新
        best.pos = pos;
        best.value = score.value;
      }
    }
  }
  assert(best.pos != -1);
  graphtree_exit_board(board, "%s[%d] => %d @ %d, [%d, %d]", BOARD_MARK(board->turn), search_order, best.value, best.pos, upper_limit, lower_limit);
  return best;
}

int cpu_recursive_taker(board_t *board) {
  graphtree_t graph;
  current_graph = &graph;
  char filename[30] = {'\0'};
  stone_count_t count = count_stones(board);
  sprintf(filename, "visualize-%d.dot", count.self + count.opponent);
  graphtree_init(current_graph, filename);
  graphtree_enter(current_graph);
  pos_score_t score = _recursive_taker(board, 4, 999999, -999999);
  graphtree_exit_board(board, "[root] evaluated : %d node", score.searched_board_count);
  graphtree_save(current_graph);
  return score.pos;
}
