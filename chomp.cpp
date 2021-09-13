#include <cassert>
#include <iostream>
#include <vector>
#include <map>

const int MAXN = 10;
using State = std::vector<int>;

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
void print_state(const State& s) {
  for(int i = 0; i < r; i++) {
    for(int j = 0; j < c; j++) {
      printf("%c", " X"[j < s[i]]);
    }
    printf("\n");
  }
  fflush(stdout);
}

std::map<State, Move> vis;

Move dfs(const State& state) {
  auto it = vis.find(state);
  if (it != vis.end()) {
    return it->second;
  }
  if (state.back() == 0) {
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

int main(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "usage: %s r c\n", argv[0]);
    return -1;
  }
  r = atoi(argv[1]);
  c = atoi(argv[2]);
  if (r < 1 || r > MAXN || c < 1 || c > MAXN) {
    fprintf(stderr, "r,c should be in range [1,%d]\n", MAXN);
    return -1;
  }
  const State initial(r, c);
  printf("counting states... ");
  fflush(stdout);
  dfs(initial);
  printf("%lu\n", vis.size());
  auto mv = vis[initial];
  print_state(initial);
  printf("winning move %d %d\n", mv.r, mv.c);
  auto nx = mv.apply(initial);
}

