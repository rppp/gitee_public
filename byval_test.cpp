#include <stdio.h>
#include <windows.h>

/*

cpu i7-10700 win7 x64

g++ -o2

-1450759936
765
-1450759936
985
-1450759936
1000

vs2019 -o2

-1450759936
735
-1450759936
890
-1450759936
860

clang11 -o2

-1450759936
718
-1450759936
844
-1450759936
891

-o0有很类似的结果，C++98或最新C++17有很类似的结果，malloc或new有很类似的结果

因为例子2和3的耗时基本相同，都慢于例子1，证明byval并不会损失性能，
也许C++的二进制标准就是byval

*/

struct string
{
	char* p;
	
	string(){
		p = NULL;
	}
	
	string(int a){
		//p=new char[1024];
		p=(char*)malloc(1024);
		sprintf(p, "%d", a);
	}
	
	string(const string& a){
		if(this != &a){
			//p=new char[1024];
			p=(char*)malloc(1024);
			memcpy(p, a.p, 1024);
		}
	}
	
	void operator=(const string& a){
		if(this != &a){
			if(p!=NULL){
				//delete [] p;
				free(p);
			}
			//p=new char[1024];
			p=(char*)malloc(1024);
			memcpy(p, a.p, 1024);
		}
	}
	
	~string(){
		if(p!=NULL){
			//delete [] p;
			free(p);
		}
	}
	
	int toint(){
		if(p==NULL){
			return 0;
		}
		int ret;
		sscanf(p, "%d",&ret);
		return ret;
	}
};

int f(string a,string b,string c,string d){
	return a.toint()+b.toint()+c.toint()+d.toint();
}

int f2(string* a,string* b,string* c,string* d){
	return a->toint()+b->toint()+c->toint()+d->toint();
}

int main(){
	int sum;
	int start;
	
	sum=0;
	start = GetTickCount();
	for(int i=0;i<1000000;i++){
		sum+=f(string(i),string(i+1),string(i+2),string(i+3));
	}
	printf("%d\n",sum);
	printf("%d\n",GetTickCount()-start);
	
	sum=0;
	start = GetTickCount();
	for(int i=0;i<1000000;i++){
		string a(i);
		string b(i+1);
		string c(i+2);
		string d(i+3);
		sum+=f(a,b,c,d);
	}
	printf("%d\n",sum);
	printf("%d\n",GetTickCount()-start);
	
	sum=0;
	start = GetTickCount();
	for(int i=0;i<1000000;i++){
		string a(i);
		string b(i+1);
		string c(i+2);
		string d(i+3);
		string a2(a);
		string b2(b);
		string c2(c);
		string d2(d);
		sum+=f2(&a2,&b2,&c2,&d2);
	}
	printf("%d\n",sum);
	printf("%d\n",GetTickCount()-start);
	
	return 0;
}
