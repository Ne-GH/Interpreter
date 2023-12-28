#include <stdlib.h>
int func(int n) {

    printf("当前值为%d\n",n);

    if (n <= 0) 
        return n;
    else
        return func(n-1);
}

int main() {

    func(5);

    return 0;
}
