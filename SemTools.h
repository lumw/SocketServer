#ifndef _SEMTOOLS_INCLUDE_H_
#define _SEMTOOLS_INCLUDE_H_

#define SEM_ERROR 0
#define SEM_RIGHT 1

char gs_sem_err[256];

int sem_requ(int sem_key, int sem_num, int _value);
int sem_get(int sem_key, int sem_num);
int sem_free();
int sem_oper(int sem_no, int _values);

#endif

