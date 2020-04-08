#include <iostream>
#include "adda.h"
using namespace std;

int main()
{
    ADDA adda;
    int32_t volt[8];
    while(1){
        adda.SetDAC0(volt[0]);
        adda.SetDAC1(5000-volt[0]);
        volt[0] = adda.ReadChannel(0);
        cout << volt[0] << endl;


   }
    return 0;
}
