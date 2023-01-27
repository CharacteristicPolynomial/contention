#include "scheme.h"

int main() {
    Scheme s(3, 3);
    s.suppress[1] = true;
    // s.suppress[2] = true;
    // s.suppress[4] = true;
    // s.suppress[7] = true;
    // s.p[0] = 0;
    // s.p[1] = 0.45562;
    // s.p[2] = 0.54438;
    // s.p[3] = 0;
    // s.update_value();
    // s.print_value();

    s.randomize();
    s.update_value();
    // s.print_value();
    for(int i=0; i<10000; i++) {
        if(i%100 == 0) 
            cout << "step " << i << endl;
        if(s.stable_fix()) 
            break;
        s.update_value();
        // s.print_value();
    }
    s.print_value();
    return 0;
}