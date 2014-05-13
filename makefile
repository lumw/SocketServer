include $(ORACLE_HOME)/rdbms/lib/env_rdbms.mk
CC=`/bin/sh ./getopts.sh compiler`

COMMONLIBS=`/bin/sh ./getopts.sh options`
SOAPHOME=./soap
INCLUDE= -I. -I./soap -I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/rdbms/public -I$(ORACLE_HOME)/plsql/public -I$(ORACLE_HOME)/network/public -I$(SOAPHOME)
LDFLAGS=-L. -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/rdbms/lib -L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/network/lib -L/usr/lib -L/usr/local/lib -L$(SOAPHOME)
OCISHAREDLIBS=$(LLIBCLNTSH) ${COMMONLIBS}

CC=gcc -g

COMPILE= $(CC) $(INCLUDE) -c
MAKEEXE= $(CC) $(LDFLAGS) $(OCISHAREDLIBS)

.SUFFIXES: .o .cob .for .c

EXE=tux_cli_cus

#SOAP_OBJ=soap/soapC.o soap/soapServer.o soap/stdsoap2.o
MAIN_OBJ=TuxMain.o socket_api.o TuxClient.o log.o RecDeal.o plugin_base.o Transmit.o tools.o SemTools.o

OBJ=$(MAIN_OBJ)

all:	$(EXE)

$(EXE):	$(OBJ)
		$(MAKEEXE) $(OBJ) -o $(EXE) $(STATICLIBS)
.c.o:
		$(COMPILE) $< -o $@
clean:
		$(RM) -f *.o core $(EXE)
