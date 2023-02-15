﻿#ifndef zcontrol_h__
#define zcontrol_h__

#include "tsh.h"
#include "zctl.h"
#include "zclass.h"

//替换控制结构并生成语句表，包括return
//tfunc.vword->vsent
struct zcontrol
{
	static rbool proc_func(tsh& sh,tfunc& tfi)
	{
		zclass::arrange_format(sh,tfi.vword,&tfi);
		insert_end_statement(sh,tfi,tfi.vword);
		if(!ifn_replace(sh,tfi.vword))
			return false;
		if(!switch_replace(sh,tfi.vword))
			return false;
		if(!auto_bbk(sh,tfi.vword,tfi))
			return false;
		if(!replace_for_to(sh,tfi.vword))
			return false;
		if(!proc_continue(sh,tfi.vword))
			return false;
		if(!proc_break(sh,tfi.vword))
			return false;
		if(!proc_for(sh,tfi.vword))
			return false;
		if(!proc_if(sh,tfi.vword))
			return false;
		if(!proc_bbk(sh,tfi.vword))
			return false;
		if(!obtain_sent(sh,tfi.vword,tfi.vsent))
			return false;
		proc_return(sh,tfi.vsent,tfi);
		part_vsent(tfi);
		tag_replace(sh,tfi);
		return true;
	}

	static rbool ifn_replace(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_ifn))
				continue;
			rbuf<tword> vcond;
			int pos=get_condition_end(sh,v,i,&vcond);
			if(pos>=v.count())
			{
				sh.error(v[i]);
				return false;
			}
			sh.clear_word_val(v,i,pos);
			v[i].multi.push("if");
			v[i].multi.push("!");
			v[i].multi.push("(");
			v[i].multi+=sh.vword_to_vstr(vcond);
			v[i].multi.push(")");
		}
		zpre::arrange(v);
		return true;
	}

	static rbool switch_replace(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_switch))
				continue;
			rbuf<tword> vcond;
			int pos=get_condition_end(sh,v,i,&vcond);
			if(pos>=v.count())
			{
				sh.error(v[i]);
				return false;
			}
			sh.clear_word_val(v,i,pos);
			if(v[pos]==rppoptr(c_bbk_l))//只跳过不清除
				pos++;
			rbool first=true;
			int left;
			int right;
			for(;pos<v.count();)
			{
				if(v[pos]!=rppkey(c_case)&&v[pos]!=rppkey(c_else))
					break;
				left=get_condition_end(sh,v,pos);
				if(left>=v.count())
				{
					sh.error(v[pos]);
					return false;
				}
				if(v.get(left)==rppoptr(c_bbk_l))
				{
					right=sh.find_symm_bbk(v,left);
					if(right>=v.count())
					{
						sh.error(v[pos]);
						return false;
					}
				}
				else
				{
					right=get_low_end(v,left,pos);
					if(right>=v.count())
					{
						sh.error(v[pos]);
						return false;
					}
					right--;
					if(left>right)
					{
						sh.error(v[pos]);
						return false;
					}
				}
				rbuf<tword> val;
				if(v[left-1]==rppoptr(c_colon))
				{
					val=v.sub(pos+1,left-1);
				}
				else
					val=v.sub(pos+1,left);
				if(v[pos]==rppkey(c_case))
				{
					if(first)
					{
						v[pos].multi.push(rppkey(c_if));
						first=false;
					}
					else
					{
						v[pos].multi.push(rppkey(c_else));
						v[pos].multi.push(rppkey(c_if));
					}
					v[pos].multi+=sh.vword_to_vstr(val);
					v[pos].multi.push(rppoptr(c_equalequal));
					v[pos].multi.push(rppoptr(c_sbk_l));
					v[pos].multi+=sh.vword_to_vstr(vcond);
					v[pos].multi.push(rppoptr(c_sbk_r));
				}
				else
				{
					v[pos].multi.push(rppkey(c_else));
				}
				sh.clear_word_val(v,pos,left);
				pos=right+1;
			}
		}
		zpre::arrange(v);
		return true;
	}

	static rbool auto_bbk(tsh& sh,rbuf<tword>& v,tfunc& tfi)
	{
		if(!rppconf(c_auto_bbk))
		{
			v.top().pos.line=tfi.last_pos.line;//what?
			return true;
		}
		for(int i=0;i<c_rpp_deep;i++)
		{
			if(!auto_bbk_one(sh,v))
				return false;
			rbool need;
			zpre::arrange(v,&need);
			zclass::arrange_format(sh,v,&tfi);
			if(!need)
			{
				return true;
			}
			v.top().pos.line=tfi.last_pos.line;
		}
		sh.error(tfi,"auto bbk too deep");
		return false;
	}

	static rbool auto_bbk_one(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			int begin;
			if(v[i].val==rppkey(c_else)&&
				v.get(i+1).val!=rppkey(c_if))
			{
				begin=i+1;
			}
			elif(v[i].val==rppkey(c_if)||
				v[i].val==rppkey(c_for))
			{
				begin=get_condition_end(sh,v,i);
			}
			else
			{
				continue;
			}
			if(begin>=v.count())
			{
				sh.error(v[i],"miss { or miss tab");
				return false;
			}
			if(v[begin].val==rppoptr(c_bbk_l))
				continue;
			int end=get_low_end(v,begin,i);
			if(end>=v.count())
			{
				sh.error(v[i],"miss { or miss tab");
				return false;
			}
			if(begin>=end)
			{
				sh.error(v[i],"miss { or miss tab");
				return false;
			}
			v[begin].multi.push(rppoptr(c_bbk_l));
			v[begin].multi.push(v[begin].val);
			v[begin].val.clear();
			v[end].multi.push(rppoptr(c_bbk_r));
			v[end].multi.push(v[end].val);
			v[end].val.clear();
			return true;
		}
		return true;
	}

	static void tag_replace(tsh& sh,tfunc& tfi)
	{
		rbuf<tword> vtag;
		for(int i=0;i<tfi.vsent.count();i++)
		{
			if(tfi.vsent[i].vword.get(1).val!=rppoptr(c_colon)||
				!tfi.vsent[i].vword.get(0).is_name())
			{
				continue;
			}
			tword word=tfi.vsent[i].vword.get(0);
			word.pos.line=tfi.vsent[i].pos.line;
			vtag.push(word);
			tfi.vsent[i].vword[0].clear();
			tfi.vsent[i].vword[1].clear();
			zpre::arrange(tfi.vsent[i].vword);
		}
		for(int i=0;i<tfi.vsent.count();i++)
			for(int j=0;j<tfi.vsent[i].vword.count();j++)
				tag_replace(tfi.vsent[i].vword[j],vtag);
	}

	static void tag_replace(tword& word,rbuf<tword>& vtag)
	{
		for(int i=0;i<vtag.count();i++)
			if(word.val==vtag[i].val)
				word.val=rstr(vtag[i].pos.line);
	}

	static void part_vsent(tfunc& tfi)
	{
		part_vsent(tfi.vsent);
	}

	static void part_vsent(rbuf<tsent>& vsent)
	{
		if(!need_part(vsent))
			return;
		rbuf<tsent> result(vsent.count()*2);
		result.m_count=0;
		for(int i=0;i<vsent.count();++i)
		{
			rbuf<rbuf<tword> > temp=r_split(
				vsent[i].vword,tword(rstr(";")));
			if(temp.empty())
			{
				continue;
			}
			tsent sent;
			sent.pos=vsent[i].pos;
			for(int j=0;j<temp.count();j++)
			{
				sent.vword=r_move(temp[j]);
				result.push_move(sent);
			}
		}
		vsent=r_move(result);
	}

	static rbool need_part(rbuf<tsent>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].vword.count()==0)
				return true;
			for(int j=0;j<v[i].vword.count();j++)
				if(v[i].vword[j].val==rstr(";"))
					return true;
		}
		return false;
	}

	static void proc_return(tsh& sh,rbuf<tsent>& vsent,tfunc& tfi)
	{
		for(int i=0;i<vsent.count();i++)
		{
			proc_return_v(sh,vsent[i].vword,tfi);
		}
	}

	static void proc_return_v(tsh& sh,rbuf<tword>& v,tfunc& tfi)
	{
		if(v.get_bottom().val!=rppkey(c_return))
			return;
		tword twi;
		twi.pos_src=tfi.last_pos;
		twi.pos=tfi.last_pos;
		if(1==v.count())
		{
			v.clear();
			sh.push_twi(v,twi,rppkey(c_jmp));
			sh.push_twi(v,twi,tfi.last_pos.line);
			return;
		}
		if(sh.is_quote(tfi.retval.type))
		{
			rbuf<tword> vtemp;
			sh.push_twi(vtemp,twi,rppoptr(c_addr));
			sh.push_twi(vtemp,twi,rppoptr(c_sbk_l));
			vtemp+=v.sub(1);
			sh.push_twi(vtemp,twi,rppoptr(c_sbk_r));
			sh.push_twi(vtemp,twi,rppoptr(c_semi));
			sh.push_twi(vtemp,twi,rppkey(c_mov));
			sh.push_twi(vtemp,twi,rppoptr(c_mbk_l));
			sh.push_twi(vtemp,twi,rppkey(c_ebp));
			sh.push_twi(vtemp,twi,rppoptr(c_plus));
			sh.push_twi(vtemp,twi,rppkey(c_s_off));
			sh.push_twi(vtemp,twi,tfi.retval.name);
			sh.push_twi(vtemp,twi,rppoptr(c_mbk_r));
			sh.push_twi(vtemp,twi,rppoptr(c_comma));
			sh.push_twi(vtemp,twi,rppkey(c_ebx));
			sh.push_twi(vtemp,twi,rppoptr(c_semi));
			sh.push_twi(vtemp,twi,rppkey(c_jmp));
			sh.push_twi(vtemp,twi,tfi.last_pos.line);
			v=r_move(vtemp);
		}
		else
		{
			rbuf<tword> vtemp;
			sh.push_twi(vtemp,twi,tfi.retval.name);
			sh.push_twi(vtemp,twi,rppoptr(c_sbk_l));
			sh.push_twi(vtemp,twi,rppoptr(c_sbk_l));
			vtemp+=v.sub(1);
			sh.push_twi(vtemp,twi,rppoptr(c_sbk_r));
			sh.push_twi(vtemp,twi,rppoptr(c_sbk_r));
			sh.push_twi(vtemp,twi,rppoptr(c_semi));
			sh.push_twi(vtemp,twi,rppkey(c_jmp));
			sh.push_twi(vtemp,twi,tfi.last_pos.line);
			v=r_move(vtemp);
		}
	}

	static rbool obtain_sent(tsh& sh,rbuf<tword>& v,rbuf<tsent>& vsent)
	{
		int start=0;
		for(int i=0;i<v.count();++i)
		{
			if(v[i].val==rppoptr(c_semi))
			{
				tsent sent;
				sent.vword=v.sub(start,i);
				if(!sent.vword.empty())
				{
					sent.pos=sent.vword.get_bottom().pos;
					vsent.push_move(sent);
				}
				start=i+1;
			}
			elif(i>0&&v[i-1].pos.line!=v[i].pos.line)
			{
				tsent sent;
				sent.vword=v.sub(start,i);
				if(!sent.vword.empty())
				{
					sent.pos=sent.vword.get_bottom().pos;
					vsent.push_move(sent);
				}
				v[i-1].pos.line=v[i].pos.line;
				start=i;
				--i;
			}
		}
		return true;
	}

	static rbool proc_bbk(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppoptr(c_bbk_l))
			{
				continue;
			}
			int left=i;
			int right=sh.find_symm_bbk(v,left);
			if(right>=v.count())
			{
				sh.error(v.get(i),"miss }");
				return false;
			}
			v[left].val.clear();
			v[right].val.clear();
			v[right].multi.push(rppoptr(c_semi));
			v[right].multi.push(rppkey(c_nop));
		}
		zpre::arrange(v);
		return true;
	}

	static int find_else_end(tsh& sh,rbuf<tword>& v,int begin)
	{
		while(true)
		{
			if(begin>=v.count())
				return v.count();
			if(v.get(begin).val==rppkey(c_else)&&
				v.get(begin+1).val==rppkey(c_if))
			{
				int left=r_find_pos(v,tword(rppoptr(c_bbk_l)),begin+2);
				if(left==v.count())
					return v.count();
				int right=sh.find_symm_bbk(v,left);
				if(right>=v.count())
					return v.count();
				begin=right+1;
			}
			elif(v.get(begin).val==rppkey(c_else))
			{
				int left=r_find_pos(v,tword(rppoptr(c_bbk_l)),begin+1);
				if(left==v.count())
					return v.count();
				int right=sh.find_symm_bbk(v,left);
				if(right>=v.count())
					return v.count();
				return right+1;
			}
			else
				return begin;
		}
	}

	static rbool proc_if(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_if))
			{
				continue;
			}
			int cond_end;
			rbuf<tword> vcond;
			cond_end=get_condition_end(sh,v,i,&vcond);
			if(vcond.empty())
			{
				sh.error(v.get(i),"miss cond");
				return false;
			}
			int left=cond_end;
			if(v.get(left).val!=rppoptr(c_bbk_l))
			{
				sh.error(v.get(i),"miss {");
				return false;
			}
			int right=sh.find_symm_bbk(v,left);
			if(right>=v.count())
			{
				sh.error(v.get(i),"miss }");
				return false;
			}
			int else_end=find_else_end(sh,v,right+1);
			if(else_end==v.count())
			{
				sh.error(v.get(i),"miss else end");
				return false;
			}
			if(v.get(right+1).val==rppkey(c_else))
			{
				insert_jmp_asm(sh,v[right],v[else_end]);
				v[right+1].val.clear();
			}
			v[left].multi=sh.vword_to_vstr(vcond);
			insert_cond_false_asm(sh,v[left].multi,v.get(right+1));
			sh.clear_word_val(v,i,cond_end);
			v[left].val.clear();
			v[right].val.clear();
			v[right].multi.push(rppoptr(c_semi));
			v[right].multi.push(rppkey(c_nop));
		}
		return true;
	}

	static rbool proc_for(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_for))
				continue;
			int cond_end;
			rbuf<tword> vcond;
			cond_end=get_condition_end(sh,v,i,&vcond);
			if(vcond.empty())
			{
				tword twi=v[i];
				twi.val=1;
				vcond.push(twi);
			}
			rbool is_for=true;
			if(r_find_pos(vcond,tword(rppoptr(c_semi)))>=vcond.count())
			{
				is_for=false;
			}
			int left=cond_end;
			if(v.get(left).val!=rppoptr(c_bbk_l))
			{
				sh.error(v.get(i),"miss {");
				return false;
			}
			int right=sh.find_symm_bbk(v,left);
			if(right>=v.count())
			{
				sh.error(v.get(i),"miss }");
				return false;
			}
			if(is_for)
			{
				rbuf<rbuf<tword> > forcond=r_split_e(
					vcond,tword(rppoptr(c_semi)));
				if(forcond.count()!=3)
				{
					sh.error(v.get(i),"miss for cond");
					return false;
				}
				if(forcond[1].empty())
				{
					tword twi=v[i];
					twi.val=1;
					forcond[1].push(twi);
				}
				v[i].val.clear();
				v[i].multi=sh.vword_to_vstr(forcond[0]);
				v[left].val.clear();
				v[left].multi=sh.vword_to_vstr(forcond[1]);
				insert_cond_false_asm(sh,v[left].multi,v.get(right+1));
				v[right].val.clear();
				v[right].multi=sh.vword_to_vstr(forcond[2]);
				insert_jmp_asm(sh,v[right],v[left]);
				sh.clear_word_val(v,i,cond_end);
			}
			else
			{
				v[left].multi=sh.vword_to_vstr(vcond);
				insert_cond_false_asm(sh,v[left].multi,v.get(right+1));
				v[left].val.clear();
				insert_jmp_asm(sh,v[right],v[left]);
				v[right].val.clear();
				sh.clear_word_val(v,i,cond_end);
			}
		}
		return true;
	}

	static void insert_jmp_asm(tsh& sh,tword& word,const tword& posword)
	{
		word.multi.push(rppoptr(c_semi));
		word.multi.push(rppkey(c_jmp));
		word.multi.push(rstr(posword.pos.line));
	}

	static void insert_cond_true_asm(tsh& sh,rbuf<rstr>& multi,
		const tword& posword)
	{
		multi.push(rppoptr(c_semi));
		multi.push(rppkey(c_jebxnz));
		multi.push(rstr(posword.pos.line));
	}

	static void insert_cond_false_asm(tsh& sh,rbuf<rstr>& multi,
		const tword& posword)
	{
		multi.push(rppoptr(c_semi));
		multi.push(rppkey(c_jebxz));
		multi.push(rstr(posword.pos.line));
	}

	static int find_jump_out(tsh& sh,rbuf<tword>& v,int i)
	{
		int j;
		for(j=i-1;j>=0;--j)
		{
			if(v[j].val!=rppkey(c_for))
			{
				continue;
			}
			int left=get_condition_end(sh,v,j);
			if(left>=v.count())
				return -2;
			if(v[left]!=rppoptr(c_bbk_l))
				return -2;
			int right=sh.find_symm_bbk(v,left);
			if(right>=v.count())
				return -2;
			if(i>left&&i<right)
				break;
		}
		return j;
	}

	static rbool proc_continue(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_continue)&&
				v[i].val!=rppkey(c_continued))
			{
				continue;
			}
			int j=find_jump_out(sh,v,i);
			if(j<0)
			{
				sh.error(v.get(i),"no loop sturct");
				return false;
			}
			rbuf<tword> vcond;
			int cond_end=get_condition_end(sh,v,j,&vcond);
			rbool is_for=true;
			if(r_find_pos(vcond,tword(rppoptr(c_semi)))>=vcond.count())
			{
				is_for=false;
			}
			int left=r_find_pos(v,tword(rppoptr(c_bbk_l)),cond_end);
			if(left>=v.count())
			{
				sh.error(v.get(j),"miss {");
				return false;
			}
			int right=sh.find_symm_bbk(v,left);
			if(right==0||right+1>=v.count())
			{
				sh.error(v.get(j),"miss }");
				return false;
			}
			if(is_for)
			{
				if(v[i].val==rppkey(c_continue))
				{
					insert_jmp_asm(sh,v[i],v[right]);
					v[i].val.clear();
				}
				else
				{
					insert_jmp_asm(sh,v[i],v[left]);
					v[i].val.clear();
				}
			}
			else
			{
				insert_jmp_asm(sh,v[i],v[left]);
				v[i].val.clear();
			}
		}
		return true;
	}

	static rbool proc_break(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_break))
			{
				continue;
			}
			int j=find_jump_out(sh,v,i);
			if(j<0)
			{
				sh.error(v.get(i),"no loop sturct");
				return false;
			}
			int left=r_find_pos(v,tword(rppoptr(c_bbk_l)),j);
			int right=sh.find_symm_bbk(v,left);
			if(right==0||right+1>=v.count())
			{
				sh.error(v.get(j),"miss { or }");
				return false;
			}
			insert_jmp_asm(sh,v[i],v[right+1]);
			v[i].val.clear();
		}
		return true;
	}

	static int get_word_tab(tword& word)
	{
		tpos pos=word.pos_src;
		if(pos.file==null)
		{
			rf::printl("pos file null");
			return -1;
		}
		if(pos.line>=pos.file->tab_list.count())
		{
			rf::printl("pos file null");
			return -1;
		}
		return pos.file->tab_list[pos.line];
	}

	//获取低一层结尾语句的位置
	static int get_low_end(rbuf<tword>& v,int begin,int head)
	{
		if(v.get(begin).val.empty())
			return v.count();
		for(int i=begin;i<v.count();i++)
		{
			if(get_word_tab(v[i])<=get_word_tab(v[head]))
			{
				return i;
			}
		}
		return v.count();
	}

	//begin应该是条件关键字的位置
	static int get_condition_end(tsh& sh,rbuf<tword>& v,
		int begin,rbuf<tword>* pret=null)
	{
		if(v.get(begin).val.empty())
			return v.count();
		int i;
		for(i=begin+1;i<v.count();i++)
		{
			if(v[i].pos!=v[begin].pos)
				break;
		}
		rbuf<tword> ret;
		if(i>=v.count())
		{
			if(pret!=null)
			{
				*pret=r_move(ret);
			}
			return v.count();
		}
		if(pret!=null)
		{
			ret=v.sub(begin+1,i);
			if(ret.get_bottom()==rppoptr(c_sbk_l)&&
				ret.get_top()==rppoptr(c_sbk_r))
			{
				ret.pop_front();
				ret.pop();
			}
			*pret=r_move(ret);
		}
		return i;
	}

	//fixme fot..to条件带括号有问题
	static rbool replace_for_to(tsh& sh,rbuf<tword>& v)
	{
		for(int i=0;i<v.count();i++)
		{
			if(v[i].val!=rppkey(c_for))
			{
				continue;
			}
			rbuf<tword> vcond;
			int cond_end=get_condition_end(sh,v,i,&vcond);
			int topos=r_find_pos(vcond,tword(rppkey(c_to)));
			if(topos<vcond.count())
			{
				int assignpos=r_find_pos(
					vcond,tword(rppoptr(c_equal)));
				if(assignpos==vcond.count())
				{
					sh.error(v.get(i),"for to miss assign optr");
					vcond.print();
					return false;
				}
				rstr name=vcond.get(assignpos-1).val;
				if(name.empty())
				{
					sh.error(v.get(i),"for to miss assign name");
					return false;
				}
				v[i].multi.push(rstr("for"));
				v[i].multi.push(rstr("("));
				for(int j=0;j<topos;j++)
					v[i].multi.push(vcond[j].val);
				v[i].multi.push(rppoptr(c_semi));
				v[i].multi.push(name);
				v[i].multi.push(rstr("<="));
				for(int j=topos+1;j<vcond.count();j++)
					v[i].multi.push(vcond[j].val);
				v[i].multi.push(rppoptr(c_semi));
				v[i].multi.push(rstr("++"));
				v[i].multi.push(name);
				v[i].multi.push(rstr(")"));
				sh.clear_word_val(v,i,cond_end);
				continue;
			}
			int inpos=r_find_pos(vcond,tword(rppkey(c_in)));
			if(inpos<vcond.count())
			{
				rstr var=vcond.get(0).val;
				rbuf<rstr> vset=sh.vword_to_vstr(vcond.sub(2));
				if(var.empty()||vset.empty())
				{
					sh.error(v.get(i),"for in miss name or set");
					return false;
				}
				sh.clear_word_val(v,i,cond_end);
				v[i].multi.push(rstr("for"));
				v[i].multi.push(rstr("("));
				v[i].multi+=var;
				v[i].multi+=rstr("=");
				v[i].multi+=rstr("0");
				v[i].multi+=rstr(";");
				v[i].multi+=var;
				v[i].multi+=rstr("<");
				v[i].multi+=vset;
				v[i].multi+=rstr(".");
				v[i].multi+=rstr("count");
				v[i].multi+=rstr("(");
				v[i].multi+=rstr(")");
				v[i].multi+=rstr(";");
				v[i].multi+=rstr("++");
				v[i].multi+=var;
				v[i].multi+=rstr(")");
			}
		}
		zpre::arrange(v);
		return true;
	}

	static void insert_end_statement(tsh& sh,tfunc& tfi,rbuf<tword>& v)
	{
		tword item;
		item.pos=tfi.last_pos;
		item.pos_src=tfi.last_pos;
		item.pos_src.line=0;
		item.val=rppoptr(c_semi);
		v.push_move(item);
	}
};

#endif