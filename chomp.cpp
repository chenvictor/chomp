#include <cassert>
#include <iostream>
#include <vector>
#include <map>

// curses
#include <ncurses.h>

// sleep
#include <chrono>
#include <thread>

// getopt
#include <unistd.h>

using namespace std::chrono_literals;

const int MAXN = 10;

struct State: public std::vector<int> {
  using vector::vector;
  bool is_empty() const {
    return back() == 0;
  }
};

struct Move {
  int r, c;
  // number of followers
  int nx_cnt = 0;
  // number of winning followers
  int nx_win = 0;
  State apply(State from) const {
    assert(from[r] > c);
    for (int k = 0; k <= r; ++k) {
      from[k] = std::min(c, from[k]);
    }
    return from;
  }
  bool operator==(const Move& b) const {
    return r==b.r && c==b.c;
  }
  bool is_winning() const {
    return nx_win == 0;
  }
};
const Move LOSING = {-1,-1};

namespace args {
  struct {
    bool is_computer = false;
    std::string name = "Player";
    void set_computer() {
      is_computer = true;
      name = "Computer";
    }
  } player[2];
  int rows, cols;
  void usage(char* cmd) {
    fprintf(stderr, "usage: %s [-12] r c\n", cmd);
    exit(-1);
  }
  void parse(int argc, char** argv) {
    int c;
    while ((c = getopt(argc, argv, "12")) != -1) {
      switch(c) {
        case '1':
          player[0].set_computer();
          break;
        case '2':
          fprintf(stderr, "computer 2 is not implemented, skipping -2\n");
//          player[1].set_computer();
          break;
      }
    }
    if (argc-optind != 2) {
      usage(argv[0]);
    }
    rows = atoi(argv[optind++]);
    cols = atoi(argv[optind++]);
    // Add numbers if both player or computer
    if (player[0].is_computer == player[1].is_computer) {
      player[0].name += " 1";
      player[1].name += " 2";
    }
  }
}

std::map<State, Move> strategy;

Move dfs(const State& state) {
  auto it = strategy.find(state);
  if (it != strategy.end()) {
    return it->second;
  }
  if (state.is_empty()) {
    return strategy[state] = {-1, 0};
  }
  strategy[state] = LOSING;
  Move mv;
  for (int i = 0; i < args::rows; ++i) {
    for (int j = 0; j < state[i]; ++j) {
      mv = {i,j};
      const auto nx = mv.apply(state);
      if (dfs(mv.apply(state)) == LOSING) {
        strategy[state] = mv;
      }
    }
  }
  return strategy[state];
}

void calc() {
  const State initial(args::rows, args::cols);
  printf("calculating states... ");
  fflush(stdout);
  dfs(initial);
  printf("%lu\nEnter to continue", strategy.size());
  getchar();
}

std::pair<int,int> get_click() {
  MEVENT event;
  int ch;
  while(1) {
    ch = getch();
    if (ch != KEY_MOUSE) continue;
    if (getmouse(&event) != OK) continue;
    if (!(event.bstate & BUTTON1_CLICKED)) continue;
    const int y = event.y;
    const int x = event.x;
    if (y >= args::rows) continue;
    if (x >= args::cols) continue;
    return {y,x};
  }
}

void print_state(const State& s) {
  move(0,0);
  for(int i = 0; i < args::rows; i++) {
    for(int j = 0; j < args::cols; j++) {
      printw("%c", " X"[j < s[i]]);
    }
    printw("\n");
  }
}

bool computer_move[2] = {true, false};

void turn(State& state, int t) {
  print_state(state);
  mvprintw(args::rows,0,"turn %d: ", t);
  State nx;
  auto player = args::player[t%2];
  mvprintw(args::rows,0,"%s's move", player.name.c_str());
  clrtoeol();
  refresh();
  if (player.is_computer) {
    std::this_thread::sleep_for(1s);
    nx = strategy[state].apply(state);
  } else {
    int y,x;
    do {
      std::tie(y,x) = get_click();
    } while(x >= state[y]);
    Move move = {y,x};
    nx = move.apply(state);
  }
  state.swap(nx);
}

void game() {
  {
    // curses setup
    initscr();
    raw();
    cbreak();
    noecho();
    keypad(stdscr, true);
    curs_set(0);
    mousemask(BUTTON1_CLICKED, NULL);
  }
  State state(args::rows, args::cols);
  int t = 0;
  while(!state.is_empty()) {
    turn(state, t);
    t++;
  }
  endwin();
}

int main(int argc, char** argv) {
  args::parse(argc, argv);
  const int r = args::rows;
  const int c = args::cols;
  if (r < 1 || c < 1) {
    fprintf(stderr, "r,c should positive");
    return -1;
  }
  if (r > MAXN || c > MAXN) {
    fprintf(stderr, "WARNING: grid is possibly large. Calculation may take a while.\n");
  }
  calc();
  game();
}

