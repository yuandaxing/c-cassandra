/*=============================================================================
*
* Author:ydx - mryuan0@gmail.com
*
* QQ:1179923349
*
* Last modified:2011-08-29 11:28
*
* Filename:auxiRBtree.h
*
* Description: 
*
=============================================================================*/
#include "tree.h"
#include "stdio.h"

/*
 *这个文件用来解决的问题是tree.h中定义不是很集中，比较麻烦看懂的问题
 *这里主要生成一个结构体struct opsSet， 该结构体封装了
 *红黑树的头部和红黑树的操作，并且封装了红黑树的析构函数
 */
#define AUXIL_RB_TREE(data_type, head_type, link, cmp)	\
						RB_HEAD(head_type, data_type);	\
						RB_PROTOTYPE(head_type, data_type, link, cmp);\
						RB_GENERATE(head_type, data_type, link, cmp);

#define GENERATE_OPS_DATA(opsSet_type, head_type, head_name, data_type, link, cmp)  \
		typedef struct opsSet_type{	\
			struct head_type head_name; 	\
			struct data_type *(*remove)(struct head_type *, struct data_type *);		\
			struct data_type *(*insert)(struct head_type *, struct data_type *);		\
			struct data_type *(*find)(struct head_type *, struct data_type *);		\
			struct data_type *(*next)(struct data_type *);				\
			struct data_type *(*minmax)(struct head_type *, int);			\
		}opsSet_type;

#define RB_TREE_GENERATE(opsSet_type, opsSet_name, data_type, head_type, head_name, link, cmp)	\
						AUXIL_RB_TREE(data_type, head_type, link, cmp)   \
						GENERATE_OPS_DATA(opsSet_type, head_type, head_name, data_type, link, cmp)	\
						opsSet_type opsSet_name = { {NULL}, \
													head_type##_RB_REMOVE, 	\
                                                    head_type##_RB_INSERT,\
                                                    head_type##_RB_FIND, \
                                                    head_type##_RB_NEXT,\
                                                    head_type##_RB_MINMAX,\
						}
//这里需要说明remove中的data_type *必须指向红黑树中的节点
/* 使用方法如下
struct Int{
	int i;
	RB_ENTRY(Int) link;
};
int cmp(struct Int *x, struct Int *y)
{
	return x->i - y->i;
}

RB_TREE_GENERATE(Iops_type, Iops, Int, head, head, link, cmp); 
*/
