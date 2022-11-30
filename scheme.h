#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <algorithm>
#include <iomanip>
using namespace std;

#define PRECISION 0.00001
#define DIS_PRECISION 1E-8
#define STEP_FACTOR 0.1

void print_binary(size_t a) {
    while(a>0) {
        cout << a%2;
        a /= 2;
    }
    cout << endl;
}

class Scheme {
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
    public:
    // bool value_comparator(int a, int b) const { return value_list[a] > value_list[b]; }
    bool stable_fix() {
        /*
            1. order rows by values (high to low)
            2. calculate the distance between the highest value with probability at least PRECISION and the lowest value
            3. set the amount of probability to shift proportional to the distance above
            4. from high values to low values, deduct probabilities till reaches the shift amount above
            5. distribute the shift probability evenlyg amon the remaining rows.
        */
        vector<size_t> index(size);
        for(size_t i=0; i<size; i++) 
            index[i] = i;
        sort(index.begin(), index.end(), [&](size_t i1, size_t i2) {return value_list[i1] > value_list[i2];});
        // for(size_t i=0; i<size; i++) {
        //     cout << "@@1 " <<  value_list[index[i]] << " : " << p[index[i]] << " : ";
        //     print_binary(index[i]);
        // }
        {
            size_t i = 0;
            while(p[index[i]] < PRECISION) {
                i++;
            }
            double dis = value_list[index[i]] - value_list[index[size - 1]];
            double step_size = dis * STEP_FACTOR;
            if(dis < DIS_PRECISION) 
                return true;
            assert(step_size<=1);
            assert(step_size>=0);
            // cout << "dis: " << dis << "; step_size: " << step_size << endl;

            i = 0;
            double temp = step_size;
            while(true) {
                if (temp >= p[index[i]]) {
                    temp -= p[index[i]];
                    p[index[i]] = 0;
                    i++;
                } else {
                    p[index[i]] -= temp;
                    i++;
                    break;
                }
            }
            double share = step_size/(size - i);
            // cout << "share: " << share << "; i: " << i << endl;
            while(i<size) {
                p[index[i]] += share;
                i++;
            }
            // for(size_t i=0; i<size; i++) {
            //     cout << "@@2 " <<  value_list[index[i]] << " : " << p[index[i]] << " : ";
            //     print_binary(index[i]);
            // }
        }
        
        normalize();
        return false;
    }
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
            cout << "(" << setw(10) << p[i] << ") "
                 << setw(10) << ettr_list[i] << " " 
                 << setw(10) << ptr_list[i] << " " 
                 << setw(10) << value_list[i] << " : ";
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
            if (p[i]>PRECISION && value_list[i] > hv) {
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
        return et + (1-pt) * (period+9.0/4);
    }
};