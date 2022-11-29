#include "scheme.h"

int main() {
    Scheme s(4, 3);
    s.update_value();
    s.print_value();
    for(int i=0; i<10000; i++) {
        cout << "step " << i << endl;
        s.fix();
        s.update_value();
        s.print_value();
    }
    return 0;
}