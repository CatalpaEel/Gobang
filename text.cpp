#include <iostream>
using namespace std;
void abc(int a,int b,int *c=NULL){
    if(!c)return;
    printf("%d",c);
}
int main(){
    abc(1,2);
}