/*
拆分宏的另一种用法：
括号里如果是类名将展开为该类的所有数据成员
如果是取地址符号加类名将展开为该类的所有函数成员的指针
*/

void main()
{
	A a
	a.print
	
	#func(&B)
}

func(void* p)
{
	putsl int[p,10]
}


class A
{
	int m_a=1
	rstr m_b="abc"
	int m_c(2)
	
	print
	{
		#putsl(A)
	}
}

namespace B
{
	int fmul(int a)
	{
		return a*2
	}
	
	int fdiv(int a)
	{
		return a/2
	}
	
	int fsquare(int a)
	{
		return a*a
	}
}