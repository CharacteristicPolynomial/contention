#include "scheme.h"

int main() {
    Scheme s(5, 3);
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