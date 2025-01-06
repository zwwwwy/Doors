#ifndef C_LIST_H
#define C_LIST_H

typedef struct task_list
{
	struct task_list* next;
	struct task_list* prev;
} task_list;

typedef struct DoubleLinkList
{
	unsigned long data;

	struct DoubleLinkList* next;
	struct DoubleLinkList* prev;
} DoubleLinkList;

#endif
