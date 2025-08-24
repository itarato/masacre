#include <iostream>

#include "../common.h"
#include "../path_finder.h"

using namespace std;

int main() {
  PathFinder pf{};
  pf.cells_w = 3;
  pf.cells_h = 3;

  for (int y = 0; y <= 3; y++) {
    for (int x = 0; x <= 3; x++) {
      pf.cells[y * pf.cells_w + x] = 0b1;
    }
  }

  pf.cells[1 * pf.cells_w + 2] = 0;
  pf.cells[2 * pf.cells_w + 1] = 0;
  pf.cells[2 * pf.cells_w + 2] = 0;

  for (int y = 0; y <= 3; y++) {
    for (int x = 0; x <= 3; x++) {
      cout << (int)pf.cells[y * pf.cells_w + x];
    }
    cout << endl;
  }
  cout << endl;

  // Cells:
  // . . . .
  // . . X .
  // . X X .
  // . . . .

  pf.find_path(IntVector2{0, 0}, IntVector2{3, 3});
}