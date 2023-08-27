#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define FPS 10

// snake is a linked list of nodes with each node
// having a x and y coordinate

#define UP 'w'
#define LEFT 'a'
#define DOWN 's'
#define RIGHT 'd'
#define QUIT 'q'

#define COL 60
#define ROW 40
// space with yellow background
#define YELLOW_SQUARE "\033[0;43m \033[0m"

typedef struct {
  int x;
  int y;
} TailSegment;

TailSegment tail[100];
int tail_length = 0;

int head_x, head_y = 0;
int food_x, food_y = 0;
char flag = 1; // char has 8 bits so its equal to 1 byte // this is to be
int score = 0;

void update_tail() {
  for (int i = tail_length - 1; i > 0; i--) {
    tail[i].x = tail[i - 1].x;
    tail[i].y = tail[i - 1].y;
  }
  tail[0].x = head_x;
  tail[0].y = head_y;
}

int kbhit() {
  struct termios oldt, newt;
  int ch;
  int oldf;

  // take the current terminal i/o settings
  tcgetattr(STDIN_FILENO, &oldt);
  // copy the settings into the new struct
  newt = oldt;
  // make the necessary changes to the terminal
  newt.c_lflag &= ~(ICANON | ECHO);
  // apply the new settings immediatly
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  // get the current settings of stdin
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  // set stdin to be non-blocking
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  // reapply the old settings
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  // reapply the old settings of stdin
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

void draw() {
  system("clear");

  for (int i = 0; i < COL + 2; ++i) {
    printf("%s", YELLOW_SQUARE);
  }
  printf("\n");

  for (int i = 0; i < ROW; ++i) {
    printf("%s", YELLOW_SQUARE);
    for (int j = 0; j < COL; ++j) {
      if (i == head_y && j == head_x) {
        printf("\033[0;31m");
        printf("O");
        printf("\033[0m");
      } else if (i == food_y && j == food_x) {
        printf("\033[0;32m");
        printf("X");
        printf("\033[0m");
      } else {
        int is_tail_segment = 0;
        for (int k = 0; k < tail_length; k++) {
          if (tail[k].x == j && tail[k].y == i) {
            printf("\033[0;34m");
            printf("o");
            printf("\033[0m");
            is_tail_segment = 1;
            break;
          }
        }
        if (!is_tail_segment) {
          printf(" ");
        }
      }
    }
    printf("%s\n", YELLOW_SQUARE);
  }

  for (int i = 0; i < COL + 2; ++i) {
    printf("%s", YELLOW_SQUARE);
  }
  printf("\n");
  printf("Score: %d\n", score);
}

void input() {
  char ch = getchar();
  if (ch == QUIT) {
    exit(0);
  } else if (ch == UP && flag != DOWN) {
    flag = UP;
  } else if (ch == DOWN && flag != UP) {
    flag = DOWN;
  } else if (ch == LEFT && flag != RIGHT) {
    flag = LEFT;
  } else if (ch == RIGHT && flag != LEFT) {
    flag = RIGHT;
  }
}

void logic() {

  update_tail();

  switch (flag) {
  case UP:
    head_y--;
    break;
  case DOWN:
    head_y++;
    break;
  case LEFT:
    head_x--;
    break;
  case RIGHT:
    head_x++;
    break;
  }

  // Check if snake hits walls
  if (head_x < 0 || head_x >= COL || head_y < 0 || head_y >= ROW) {
    printf("Game Over\n");
    exit(0);
  }

  // Check if snake has eaten itself
  for (int i = 0; i < tail_length; i++) {
    if (head_x == tail[i].x && head_y == tail[i].y) {
      printf("Game Over\n");
      exit(0);
    }
  }

  // Check if snake has eaten food
  if (head_x == food_x && head_y == food_y) {
    srand(time(NULL));
    food_x = rand() % COL;
    food_y = rand() % ROW;
    score++;
    tail_length++;
  }
}

int main() {
  srand(time(NULL));

  head_x = COL / 2;
  head_y = ROW / 2;
  food_x = rand() % COL;
  food_y = rand() % ROW;

  // Initialize the tail to follow the head's initial position
  for (int i = 0; i < tail_length; i++) {
    tail[i].x = head_x;
    tail[i].y = head_y;
  }

  // Main game loop
  while (1) {
    // remove the curser from the screen
    printf("\033[?25l");

    draw();
    if (kbhit()) {
      input();
    }
    logic();
    usleep(1000000 / FPS);
  }

  return 0;
}
