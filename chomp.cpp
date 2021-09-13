#include <ncurses.h>

#include <cassert>
#include <iostream>
#include <vector>
#include <map>

const int MAXN = 10;

struct State: public std::vector<int> {
  using vector::vector;
  bool is_empty() const {
    return back() == 0;
  }
};

struct Move {
  int r, c;
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
};

const Move LOSING = {-1,-1};

int r, c;

std::map<State, Move> vis;

Move dfs(const State& state) {
  auto it = vis.find(state);
  if (it != vis.end()) {
    return it->second;
  }
  if (state.is_empty()) {
    return vis[state] = {-1, 0};
  }
  vis[state] = LOSING;
  for (int i = 0; i < r; ++i) {
    for (int j = 0; j < state[i]; ++j) {
      Move mv = {i,j};
      const auto nx = mv.apply(state);
      if (dfs(mv.apply(state)) == LOSING) {
        vis[state] = mv;
      }
    }
  }
  return vis[state];
}

void calc() {
  const State initial(r, c);
  printf("calculating states... ");
  fflush(stdout);
  dfs(initial);
  printf("%lu\nEnter to continue", vis.size());
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
    if (y >= r) continue;
    if (x >= c) continue;
    return {y,x};
  }
}

void print_state(const State& s) {
  move(0,0);
  for(int i = 0; i < r; i++) {
    for(int j = 0; j < c; j++) {
      printw("%c", " X"[j < s[i]]);
    }
    printw("\n");
  }
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
  State state(r, c);
  while(!state.is_empty()) {
    print_state(state);
    int y,x;
    do {
      std::tie(y,x) = get_click();
    } while(x >= state[y]);
    Move move = {y,x};
    state = move.apply(state);
  }
  endwin();
}

int main(int argc, char** argv) {
  if (argc < 3) {
    fprintf(stderr, "usage: %s r c\n", argv[0]);
    return -1;
  }
  r = atoi(argv[1]);
  c = atoi(argv[2]);
  if (r < 1 || r > MAXN || c < 1 || c > MAXN) {
    fprintf(stderr, "r,c should be in range [1,%d]\n", MAXN);
    return -1;
  }
  calc();
  game();
}

