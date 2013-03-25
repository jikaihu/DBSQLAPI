#ifndef H_NAMEVALUE_H
#define H_NAMEVALUE_H


extern "C" {
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <ctype.h>
}


class NameValue {


public:
	char	  *name;
	char	  *value;
	int		dataType;
	NameValue *next;
};


class  NameValueList{

public:
	NameValue	*head_ptr;
	NameValue	*tail_ptr;
	NameValue	*curr_ptr;

public:
	NameValueList();
	~NameValueList();

	void init();

	char *str_dup(const char *src);

	void free_dup(char *&src);

	void	free(); 

	void	clean();

	long	mem();

	int	add(const char *name, int dataType);
	int	add(const char *name, const char *value);

	int	put(const char *name, const char *value);

	int	copy(NameValueList *dest);

	char	*get(const char *name);

	NameValue *find(const char *name);

	char *get_fname();

	char *get_nname();

	int allCount();	

	long	mem_all();

	int get_fitem(char **pcName,char **pcValue,int *piDataType);

	int get_nitem(char **pcName,char **pcValue,int *piDataType);
};



typedef struct {
	int group;
	NameValueList data_list;
}Data;

typedef struct {
    unsigned char num;
	unsigned char current;
    Data *data[100];
}DataList;

int ClearList(DataList* datasource);

//把输入字符串加入到namevalue队列中
int  Convert2nvlist(char *inputNVList,NameValueList *NVListValue);//sigma 
#endif
