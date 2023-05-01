#include <stdio.h>

struct A
{
	int a;
	double b;
	char c[1024];
	
	A(){
		printf("%d\n",__LINE__);
	}
	
	~A(){
		printf("%d\n",__LINE__);
	}
	
	A(const A& a){
		printf("%d\n",__LINE__);
	}
};

void f(A a){
	
}

int main(){
	((void (*)(A*))f)(NULL); //msvc和clang11会调用析构，而g++不会，证明g++的参数析构是由调用方执行
	
	//printf("%d\n",__clang__);
	
	//f(A()); //这行代码3个编译器都只调用了一次构造和一次析构
	
	return 0;
}