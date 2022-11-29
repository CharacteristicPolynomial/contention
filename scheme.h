#include <iostream>
#include <vector>
#include <cmath>
#include<cstdlib>
using namespace std;

#define precision 0.00001

void print_binary(size_t a) {
    while(a>0) {
        cout << a%2;
        a /= 2;
    }
    cout << endl;
}

class Scheme {
    public:
    Scheme(int _p, int _n) : period(_p), n(_n) {
        size = 1<<period;
        p.resize(size);
        // initialize sending probability
        for (int i=0; i< size; i++) {
            int a = i;
            p[i] = 1.0;
            for (int j =0; j<period; j++) {
                if(a%2) {
                    p[i] *= 1.0/n;
                } else {
                    p[i] *= 1 - 1.0/n;
                }
                a /= 2;
            }
        }
        ptr_list.resize(size);
        ettr_list.resize(size);
        value_list.resize(size);
    }
    void update_value() {
        // calculate ettr and ptr based on current vector p
        vector<size_t> cursor(n-1);
        ptr = 0;
        ettr = 0;
        for(size_t pattern = 0; pattern<size; pattern++){
            fill(cursor.begin(), cursor.end(), 0);
            ptr_list[pattern] = 0;
            ettr_list[pattern] = 0;
            do {
                // for(int j=0; j<n-1; j++) {
                //     cout << cursor[j] << "\t";
                // }
                // cout << endl;
                int k = 0;
                vector<size_t> temp = cursor;
                bool flag = false;
                size_t temp_pattern = pattern;
                while(k<period) {
                    k++;
                    int cnt = 0;
                    cnt += temp_pattern%2;
                    temp_pattern /= 2;
                    for(int i=0; i<n-1; i++) {
                        cnt += temp[i]%2;
                        temp[i] /= 2;
                    }
                    if(cnt == 1) {
                        flag = true;
                        break;
                    }
                }
                // for(int j =0; j<n-1; j++) {
                //     cout << cursor[j] << " ";
                // }
                // cout << pattern << " " << flag << " " << prob(cursor) << endl;
                if(flag) {
                    ptr_list[pattern] += prob(cursor);
                    ettr_list[pattern] += k * prob(cursor);
                }
            } while(step(cursor, 0));
            ptr += ptr_list[pattern] * p[pattern];
            ettr += ettr_list[pattern] * p[pattern];
            // cout << ptr << " " <<  ptr_list[pattern] << " " << p[pattern] << endl;
        }
        value = get_value(ettr, ptr);
        for(int i=0; i< size; i++) {
            value_list[i] = get_value(ettr_list[i], ptr_list[i]);
        }
    }
    bool step(vector<size_t>& v, int i) {
        size_t eff_size = 1 << period;
        if(v[i] < eff_size - 1) {
            v[i] ++;
            return true;
        } else {
            if (i < v.size()-1){
                v[i] = 0;
                return step(v, i+1);
            } else return false;
        }
    }
    double prob(const vector<size_t>& v) {
        double temp = 1;
        for(int i=0; i<v.size(); i++) {
            temp *= p[v[i]];
        }
        return temp;
    }
    void print_value() {
        for(size_t i=0; i<size; i++) {
            cout << "(" << p[i] << ") " << ettr_list[i] << " " << ptr_list[i] << " " << value_list[i] << " : ";
            print_binary(i);
        }
        cout << ettr << endl << ptr << endl;
        cout << value << endl;
    }
    void randomize() {
        srand((unsigned) time(NULL));
        for(int i=0; i<size; i++) {
            p[i] = (rand()/(double)RAND_MAX);
        }
        normalize();
    }
    void normalize() {        
        double sum = 0;
        for(int i=0; i<size; i++) {
            sum += p[i];
        }
        for(int i=0; i<size; i++) {
            p[i] /= sum;
        }
    }
    void fix() {
        // find the highest non-zero entry and set it to zero
        int k = 0;
        double hv = 0;
        for(int i=0; i<size; i++) {
            if (p[i]>precision && value_list[i] > hv) {
                k = i;
                hv = value_list[i];
            }
        }
        for(int i=0; i<size; i++) {
            if (i != k) {
                p[i] += p[k] / (double)(size-1);
            }
        }
        p[k] = 0;
        normalize(); 
    }
    double get_value(double et, double pt) {
        return et + (1-pt) * (3+9.0/4);
    }
    int period;
    int n;
    size_t size;
    vector<double> p; // current sending probabilities
    double ettr; // expection of first success restricted to first period
    double ptr; // probability that the first success happens in the first period
    double value;
    vector<double> ettr_list; // set the first player to each pattern, what is the ettr now?
    vector<double> ptr_list; // set the first player to each pattern, what is the ptr now?
    vector<double> value_list;
};