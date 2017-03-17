#include <iostream>

using namespace std;

int main() {
    FILE* f;
    fopen_s(&f, "a.txt", "w");
    for (int i = 0; i < 96; i++) {
        if (i%4 == 0) {
            fprintf(f,"\n%d", 109+(i/4));
        }
        fprintf(f,"\tT_%d_H\tT_%d_L", i, i);
    }
    getchar();
}