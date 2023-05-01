﻿#ifndef rbuf_h__
#define rbuf_h__

#include "rbase.h"
#include <stdio.h>
#include <string.h>

//#define r_round(d) ((uint)(d+0.5))
//#define r_log(a,b) (log(b)/log(a))

template<typename T>
struct rbuf
{
	enum
	{
		c_small=16
	};

	T* m_p;
	int m_count;
	int m_cmax;
	uchar m_block[c_small];
	//当分配小内存时，m_p指向m_block，无需new可提高效率

	~rbuf<T>()
	{
		free();
	}

	rbuf<T>()
	{
		init();
	}

	rbuf<T>(int num)
	{
		init();
		alloc(num);
	}

	rbuf<T>(int num,const T& a)
	{
		init();
		alloc(num);
		for(int i=0;i<num;i++)
		{
			m_p[i]=a;
		}
	}

	rbuf<T>(const rbuf<T>& a)
	{
		init();
		copy(a);
	}

	rbuf<T>(rbuf<T>&& a)
	{
		move(a);
	}

	void move(rbuf<T>& a)
	{
		if(a.m_p==(T*)a.m_block)
		{
			memcpy(this,&a,sizeof(a));
			m_p=(T*)m_block;
		}
		else
		{
			m_p=a.m_p;
			m_count=a.m_count;
			m_cmax=a.m_cmax;
		}
		a.init();//如果move后对象不再使用可以只置空a.m_p
	}

	//如果复制指针一定要处理好等于号，不支持自己复制自己
	void operator=(const rbuf<T>& a)
	{
		if(a.m_p==m_p)
		{
			return;
		}
		copy(a);
	}

	void operator=(rbuf<T>&& a)
	{
		free();
		move(a);
	}

	friend rbuf<T> operator+(const rbuf<T>& a,const rbuf<T>& b)
	{
		int total=a.count()+b.count();
		int max=r_max(a.m_cmax,b.m_cmax);
		rbuf<T> ret;
		if(total>max)
		{
			ret.alloc_not_change(extend_num(total));
		}
		else
		{
			ret.alloc_not_change(max);
		}
		int i;
		for(i=0;i<a.count();i++)
		{
			ret[i]=a[i];
		}
		for(i=0;i<b.count();i++)
		{
			ret[i+a.count()]=b[i];
		}
		ret.m_count=total;
		return r_move(ret);
	}

	void operator+=(const rbuf<T>& a)
	{
		int total=a.count()+count();
		if(total>m_cmax)
		{
			realloc_not_change(extend_num(total));
		}
		for(int i=count();i<total;i++)
		{
			m_p[i]=a[i-count()];
		}
		m_count=total;
	}

	void operator+=(rbuf<T>&& a)
	{
		int total=a.count()+count();
		if(total>m_cmax)
		{
			realloc_not_change(extend_num(total));
		}
		for(int i=count();i<total;i++)
		{
			m_p[i]=r_move(a[i-count()]);
		}
		m_count=total;
	}

	void operator+=(const T& a)
	{
		this->push(a);
	}

	void operator+=(T&& a)
	{
		this->push(a);
	}

	T& operator[](int num) const
	{
		return m_p[num];
	}

	friend rbool operator==(const rbuf<T>& a,const rbuf<T>& b)
	{
		if(a.count()!=b.count())
		{
			return false;
		}
		for(int i=0;i<a.count();++i)
		{
			if(a[i]!=b[i])
			{
				return false;
			}
		}
		return true;
	}

	friend rbool operator!=(const rbuf<T>& a,const rbuf<T>& b)
	{
		return !(a==b);
	}

	void init()
	{
		m_p=null;
		m_count=0;
		m_cmax=0;
	}

	static T* next(void* p)
	{
		return ((T*)p)+1;
	}

	static T* prev(void* p)
	{
		return ((T*)p)-1;
	}

	int size() const 
	{
		return m_count*r_size(T);
	}

	int count() const
	{
		return m_count;
	}

	T* pos(int num) const 
	{
		return begin()+num;
	}

	int pos(void* p) const 
	{
		return r_to_int((T*)p-begin());
	}

	T* begin() const 
	{
		return m_p;
	}

	T* end() const 
	{
		return m_p+count();
	}

	T* rbegin() const 
	{
		if(0==m_count)
		{
			return null;
		}
		else
		{
			return m_p+count()-1;
		}
	}

	T* rend() const 
	{
		if(0==m_count)
		{
			return null;
		}
		else
		{
			return m_p-1;
		}
	}

	rbool empty() const 
	{
		return 0==m_count;
	}

	void clear()
	{
		m_count=0;
	}

	static int extend_num(int num)
	{
		return r_cond(num<c_small,c_small,num*2);
	}
	
	//应判断m_count是否太大,否则在64位上m_count++有可能归零
	void push(const T& a)
	{
		if(m_count>=m_cmax)
		{
			realloc_not_change(extend_num(m_count));
		}
		m_p[count()]=a;
		m_count++;
	}

	void push(T&& a)
	{
		if(m_count>=m_cmax)
		{
			realloc_not_change(extend_num(m_count));
		}
		m_p[count()]=a;
		m_count++;
	}

	void push_move(T& a)
	{
		push(r_move(a));
	}

	T pop()
	{
		/*if(0==m_count)
			exit(0);*/
		m_count--;
		return m_p[count()];
	}

	T pop_front()
	{
		T ret=r_move(m_p[0]);
		erase(0);
		return r_move(ret);
	}

	//没必要增加push_front的move版本
	void push_front(const T& a)
	{
		insert(0,a);
	}

	T& top() const 
	{
		return m_p[count()-1];
	}

	T& bottom() const 
	{
		return m_p[0];
	}

	T get_top() const 
	{
		if(count()>0)
		{
			return m_p[count()-1];
		}
		return T();
	}

	T get_bottom() const 
	{
		if(count()>0)
		{
			return m_p[0];
		}
		return T();
	}

	rbool erase(int num)
	{
		if(num>=count()||num<0)
		{
			return false;
		}
		for(int i=num;i<count()-1;++i)
		{
			*(m_p+i)=r_move(*(m_p+i+1));
		}
		m_count--;
		return true;
	}

	rbool erase(int begin,int end)
	{
		if(begin<0||end>count()||begin>=end)
		{
			return false;
		}
		for(int i=0;i<count()-end;i++)//count()-(end-begin)-begin
		{
			m_p[i+begin]=r_move(m_p[end+i]);//删除的元素稍后析构
		}
		m_count-=end-begin;
		return true;
	}

	rbool insert(int pos,const T& a)
	{
		if(pos>count()||pos<0)
		{
			return false;
		}
		if(m_count>=m_cmax)
		{
			realloc_not_change(extend_num(m_count));
		}
		for(int i=count();i>pos;--i)
		{
			m_p[i]=r_move(m_p[i-1]);
		}
		m_p[pos]=a;
		m_count++;
		return true;
	}

	rbool insert(int pos,const rbuf<T>& a)
	{
		if(pos>count()||pos<0||0==a.count())
		{
			return false;
		}
		if(m_cmax<m_count+a.count())
		{
			realloc_not_change(a.count()+count());
		}
		for(int i=0;i<count()-pos;i++)
		{
			m_p[a.count()+count()-1-i]=r_move(m_p[count()-1-i]);
		}
		for(int i=0;i<a.count();i++)
		{
			m_p[pos+i]=a[i];
		}
		m_count+=a.count();
		return true;
	}

	T* v_new(int num)
	{
		return r_cond(num*sizeof(T)<=c_small,(T*)m_block,r_new<T>(num));
	}

	void v_delete()
	{
		if(m_p!=(T*)m_block)
			r_delete<T>(m_p);
	}

	//这里的重复代码可用宏或者模板简化
	void alloc(int num)
	{
		if(m_p!=null)
		{
			return;
		}
		if(num<=0)
		{
			init();
			return;
		}
		m_p=v_new(num);
		m_cmax=num;
		m_count=m_cmax;
	}

	void alloc_not_change(int num)
	{
		if(m_p!=null)
		{
			return;
		}
		if(num<=0)
		{
			init();
			return;
		}
		m_p=v_new(num);
		m_cmax=num;
	}

	void realloc(int num)
	{
		if(null==m_p)
		{
			alloc(num);
			return;
		}
		if(num<=0)
		{
			free();
			return;
		}
		realloc_not_change_in(num);
		m_count=m_cmax;
	}

	void realloc_not_change(int num)
	{
		if(null==m_p)
		{
			alloc_not_change(num);
			return;
		}
		if(num<=0)
		{
			free();
			return;
		}
		realloc_not_change_in(num);
	}

	void realloc_not_change_in(int num)
	{
		T* p=v_new(num);
		int copy_size=r_min(num,m_count);
		for(int i=0;i<copy_size;i++)
		{
			p[i]=r_move(m_p[i]);
		}
		v_delete();
		m_p=p;
		m_cmax=num;
	}
	
	//不复制原有的元素
	void realloc_n(int num)
	{
		if(null==m_p)
		{
			alloc(num);
			return;
		}
		if(num<=0)
		{
			free();
			return;
		}
		v_delete();
		m_p=v_new(num);
		m_cmax=num;
		m_count=m_cmax;
	}

	void realloc_n_not_change(int num)
	{
		if(null==m_p)
		{
			alloc_not_change(num);
			return;
		}
		if(num<=0)
		{
			free();
			return;
		}
		v_delete();
		m_p=v_new(num);
		m_cmax=num;
	}

	void free()
	{
		if(null==m_p)//m_p为空时rbuf必须保证m_count和m_cmax也为空
		{
			return;
		}
		v_delete();
		init();
	}

	void copy(const rbuf<T>& a)
	{
		if(m_cmax<a.count())
		{
			realloc_n_not_change(a.m_cmax);
		}
		for(int i=0;i<a.count();i++)
		{
			m_p[i]=a[i];
		}
		m_count=a.count();
	}

	T get(int i) const
	{
		if((uint)i>=(uint)count())
		{
			return T();
		}
		else 
		{
			return m_p[i];
		}
	}

	rbuf<T> sub(int begin,int end) const 
	{
		if(end>count())
		{
			end=count();
		}
		if(begin<0)
		{
			begin=0;
		}
		rbuf<T> ret;
		int temp=end-begin;
		ret.alloc(temp);//alloc会判断temp<0的情况，所以不需要判断begin>=end
		for(int i=0;i<temp;i++)
		{
			ret[i]=m_p[begin+i];
		}
		return r_move(ret);
	}

	rbuf<T> sub(int begin) const 
	{
		return sub(begin,count());
	}

	rbool exist(const T& a)
	{
		for(int i=0;i<count();i++)
		{
			if(a==m_p[i])
			{
				return true;
			}
		}
		return false;
	}

	int find(const T& a,int begin=0) const
	{
		for(int i=begin;i<count();i++)
		{
			if(a==m_p[i])
			{
				return i;
			}
		}
		return count();
	}

	int find_last(const T& a) const
	{
		for(int i=count()-1;i>=0;i--)
		{
			if(a==m_p[i])
			{
				return i;
			}
		}
		return count();
	}

	void print() const 
	{
		for(int i=0;i<count();i++)
		{
			m_p[i].print();
		}
	}

	void printl() const 
	{
		for(int i=0;i<count();i++)
		{
			m_p[i].printl();
		}
	}
};

#endif