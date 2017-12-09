#include "reversi.h"

int get_user_input_pos(board_t *board) {
  while(true) {
    PRINT_SEPARATOR;
    printf("[%s] 置く場所を入力してください[a-h][1-8] : ", BOARD_MARK(board->turn));
    char buf[3];
    scanf("%2s", buf);
    if ('a' <= buf[0] && buf[0] <= 'h' && '1' <= buf[1] && buf[1] <= '8') {
      int pos = (buf[1] - '0') * 10 + (buf[0] - 'a' + 1);
      if(check_stone(board, pos) > 0) return pos;
      printf("そこには置けません。\n");
    } else {
      printf("入力が不正です。\n");
    }
  }
}

void play_game(board_t *board, pos_choice_function black, pos_choice_function white) {
  while(true) {
    print_board(board);
    int pos = (board->turn == 1 ? black : white)(board);;
    take_stone(board, pos);
    if(board->state == GAME_STATE_PASSED) {
      printf("PASS.\n");
    } else if (board->state == GAME_STATE_FINISHED){
      break;
    }
  }
  print_board(board);
  printf("END GAME.\n");
}

void play_game_silently(board_t *board, pos_choice_function black, pos_choice_function white) {
  while(true) {
    int pos = (board->turn == 1 ? black : white)(board);;
    take_stone(board, pos);
    if (board->state == GAME_STATE_FINISHED) return;
  }
}

void evaluate_cpu(pos_choice_function black, pos_choice_function white) {
  int black_win = 0, white_win = 0;
  board_t board;
  for(int i=0; i<1; i++) {
    memcpy(&board, &INITIAL_STATE, sizeof(board));
    play_game(&board, black, white);
    board.turn = PLAYER_TURN_BLACK;
    stone_count_t count = count_stones(&board);
    if(count.self > count.opponent) {
      black_win++;
    } else {
      white_win++;
    }
  }
  printf("Black : %d, White : %d\n", black_win, white_win);
}

int main(int argc, char *argv[]) {
  srand((unsigned)time(NULL));
  if(argc == 3 && strcmp(argv[1], "play") == 0 && strcmp(argv[2], "black") == 0) {
    printf("Player vs CPU\n");
    board_t board;
    memcpy(&board, &INITIAL_STATE, sizeof(board));
    play_game(&board, get_user_input_pos, cpu_recursive_taker);
  } else if(argc == 3 && strcmp(argv[1], "play") == 0 && strcmp(argv[2], "white") == 0) {
    printf("CPU vs Player\n");
    board_t board;
    memcpy(&board, &INITIAL_STATE, sizeof(board));
    play_game(&board, cpu_recursive_taker, get_user_input_pos);
  } else if(argc == 2 && strcmp(argv[1], "cpu") == 0) {
    printf("CPU vs CPU\n");
    evaluate_cpu(cpu_recursive_taker, cpu_random_taker);
  } else {
    printf("main play black -- play with black\n");
    printf("main play white -- play with white\n");
    printf("main cpu        -- cpu vs cpu\n");
  }
  return 0;
}
