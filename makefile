include $(ORACLE_HOME)/rdbms/lib/env_rdbms.mk
CC=`/bin/sh ./getopts.sh compiler`

COMMONLIBS=`/bin/sh ./getopts.sh options`
INCLUDE= -I. -I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/rdbms/public -I$(ORACLE_HOME)/plsql/public -I$(ORACLE_HOME)/network/public -I$(OCI_LIB_HOME)/include
LDFLAGS=-L. -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/rdbms/lib -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/network/lib -L/usr/lib -L/usr/local/lib -L$(OCI_LIB_HOME)/lib -locilib
OCISHAREDLIBS=$(LLIBCLNTSH) ${COMMONLIBS}

CC=gcc -g

COMPILE= $(CC) $(INCLUDE) -c
MAKEEXE= $(CC) $(LDFLAGS) $(OCISHAREDLIBS)

.SUFFIXES: .o .cob .for .c

EXE=sock_server

MAIN_OBJ=TuxMain.o socket_api.o TuxClient.o log.o RecDeal.o OracleDB.o Tools.o SemTools.o

OBJ=$(MAIN_OBJ)

all:	$(EXE)

$(EXE):	$(OBJ)
		$(MAKEEXE) $(OBJ) -o $(EXE) $(STATICLIBS)
.c.o:
		$(COMPILE) $< -o $@
clean:
		$(RM) -f *.o core $(EXE)
