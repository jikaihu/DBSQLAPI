#include "namevalue.h"

NameValueList::NameValueList()
{	
	head_ptr = NULL;
	tail_ptr = NULL;
	curr_ptr = NULL;
}


NameValueList::~NameValueList()
{	
	free();
}

//init
void NameValueList::init()
{	
	head_ptr = NULL;
	tail_ptr = NULL;
	curr_ptr = NULL;
}

//free
void NameValueList::free_dup(char *&src)
{
	if (src)
	{
		delete [] src;
		src = NULL;
	}

	return;
}

//copy
char *NameValueList::str_dup(const char *src)
{
	if (!src)
	{
 		return NULL;
	}

	char *dst = (char *) new char[strlen(src)+1];
	if (!dst)
	{
		return NULL;
	}
	strcpy(dst, src);

	return(dst);
}

//得到链表第一个单元的name
char *NameValueList::get_fname()
{
		curr_ptr = head_ptr;
		if(!curr_ptr)
		{
			return NULL;
		}

		char *name = curr_ptr->name;

		return(name);
}

//得到链表当前单元的下一单元的name
char *NameValueList::get_nname()
{
	if(curr_ptr) curr_ptr = curr_ptr->next;

	if(!curr_ptr)
	{
		return NULL;
	}

	return(curr_ptr->name);
}


//得到链表第一个单元的信息并把链表指针后移
int NameValueList::get_fitem(char **pcName,char **pcValue,int *piDataType)
{
	curr_ptr = head_ptr;
	if(!curr_ptr)
	{
		return -3;
	}

	*pcName = curr_ptr->name;
	*pcValue = curr_ptr->value;
	if(piDataType) 	*piDataType = curr_ptr->dataType;

	curr_ptr = curr_ptr->next;
	return 0;
}

//得到当前单元的信息，并把链表指针后移
int NameValueList::get_nitem(char **pcName,char **pcValue,int *piDataType)
{
	if(!curr_ptr)
	{
		return -3;
	}

	*pcName = curr_ptr->name;
	*pcValue = curr_ptr->value;

	if(piDataType)	*piDataType = curr_ptr->dataType;

	curr_ptr = curr_ptr->next;
	return 0;
}

//给指定的name付值
int NameValueList::put(const char *name, const char *value)
{
	NameValue *ptr;
	
	if(!name)
	{
		return -2;
	}

	ptr = find(name);
	if (!ptr)
	{
		return(-2); 
	}
	
	char *pcValue = NULL;

	if(value) pcValue=str_dup(value);
	if(ptr->value ) free_dup(ptr->value);

	ptr->value = pcValue;

	return(0);
}

//得到指定name 的value 
char *NameValueList::get(const char *name)
{
	NameValue *ptr;
		
	if(!name)
	{
		return NULL;
	}
	
	//else find next value
	ptr = find(name);
	if (!ptr)
	{
		return(NULL);
	}
	
	return(ptr->value);
}

//在链表的当前单元中添加name 和datatype 
int NameValueList::add(const char *name, int dataType)
{
	if (!name)
	{
 		return -2;
	}

	NameValue* ptr = new NameValue;
	memset(ptr, '\0', sizeof(NameValue));

	if (!head_ptr)
		curr_ptr = head_ptr = ptr;
	else
		tail_ptr->next = ptr;

	ptr->next = NULL;
	tail_ptr = ptr;

	ptr->name	  = str_dup(name);
	ptr->dataType = dataType;
	ptr->value    = NULL;

	return(0);
}

//在链表的当前单元中添加name 和value 
int NameValueList::add(const char *name, const char *value)
{
	if(!name)
	{
		return -2;
	}

	NameValue *ptr = new NameValue;
	memset(ptr, '\0', sizeof(NameValue));

	if (!head_ptr)
		curr_ptr = head_ptr = ptr;
	else
		tail_ptr->next = ptr;

	ptr->next = NULL;
	tail_ptr = ptr;

	ptr->name	  = str_dup(name);
	ptr->value    = str_dup(value);
	ptr->dataType = -1;

	return(0);
}

//copy 整个链表到 *dest
int NameValueList::copy(NameValueList *dest)
{
	if(!dest)
	 {
		return -1;
	}

	NameValue	*p = head_ptr;
	NameValue	*destHead = dest->head_ptr;
	if(destHead)
	{
		dest->free();
	}
	else
	{
		dest->init();
	}

	for(;p;)
	{
		int rtn = dest->add(p->name,p->dataType);
		if(rtn != 0){
			dest->free();
			return (rtn);
		}

		if(p->value) dest->put(p->name,p->value);
		p = p->next;
	}

	return(0);
}


//free整个链表
void NameValueList::free()
{
	NameValue *ptr=head_ptr, *ptmp;
	for(;ptr;)
	{
		if (ptr->name)	free_dup(ptr->name);

		ptr->name = NULL;
		if (ptr->value)
			free_dup(ptr->value);
		ptr->value = NULL;

		ptmp = ptr->next;
		delete ptr;
		ptr = NULL;
		ptr = ptmp;
	} 

	head_ptr = NULL;
	tail_ptr = NULL;
	curr_ptr = NULL;
}

//--------------------------------------------------------------------------
//删除链表中的value值
void NameValueList::clean()
{
	NameValue * ptr = head_ptr;
	for(;ptr;)
	{
		if (ptr->value)
		{
			free_dup(ptr->value);
			ptr->value = NULL;
		}
		ptr = ptr->next;
	}
}


//从链表中查找符合name条件的单元
NameValue *NameValueList::find(const char *name)
{
	if(!name)	return NULL;

	NameValue *ptr = head_ptr;
	for(;ptr;)
	{

		if(!(ptr->name))	return NULL;

		//if( !(_stricmp(ptr->name, name) ) ) {
		if( !(strcmp(ptr->name, name)))	//modify in 20080206
			break;
		
		if(ptr != tail_ptr)
			ptr = ptr->next;
		else 
			return NULL;
	}

	return(ptr);
}

//计算所有链表的单元格数
int NameValueList::allCount()
{
	int cnt=0;
	
	NameValue *p = head_ptr;
	for(;p;)
	{
		cnt++;		
		p = p->next;
	}

	return(cnt);
}

//计算value所占用的存储空间的长度
long NameValueList::mem()
{
	long cnt=0;
	
	for(NameValue *p=head_ptr; p; p=p->next)
	{
		if(p->value)  cnt+=strlen(p->value);
	}

	return(cnt);
}

//计算整个存储空间的长度，包括name和value 
long NameValueList::mem_all()
{
	long cnt=0;
	
	for(NameValue *p=head_ptr; p; p=p->next)
	{
		if(p->value)  cnt += strlen(p->value);

		cnt+=strlen(p->name);
	}

	return(cnt);
}


