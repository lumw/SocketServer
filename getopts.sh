#!/bin/sh 
###########################################################################
#
#  Usage:
#  getopts.sh
#
#  getopts.sh compiler
#  getopts.sh options
#
#     APP_SPEC_LIBS -- application-specific libaries 
#	  NETWORK_LIBS -- network communication libaries
#
###########################################################################

PATH=${PATH:=/usr/bin}
export PATH

#GCCOPTIONS="-g"
#CCOPTIONS="--"

if [ $# -eq 0 ]
then
	echo "Syntax Error: getopts.sh compiler/options"
	exit 0
fi

USEGCC="yes"

if [ -x /usr/bin/which ]
then 
	GCCEXIST=`which gcc | awk '{ print $1 }'`
else
	GCCEXIST="no"
fi
if [ ${GCCEXIST} != 'no' ]
then
	RETSTRING="gcc ${GCCOPTIONS}"
	USEGCC="yes"
elif [ -x /usr/local/bin/gcc ]
then
	RETSTRING="/usr/local/bin/gcc ${GCCOPTIONS}"
	USEGCC="yes"
else
	RETSTRING="cc ${CCOPTIONS}"
	USEGCC="no"
fi

if [ $1 = "compiler" ]
then
	echo ${RETSTRING}
	exit 0
fi

RETSTRING=""
if [ $1 = "options" ]
then
	UNAME_SYSTEM=`(uname -s) 2>/dev/null`  || UNAME_SYSTEM=unknown
	case ${UNAME_SYSTEM} in	
		SunOS)	
				APP_SPEC_LIBS="" 
				NETWORK_LIBS=" -lnsl -lsocket -ldl"
				;;
		Linux)	
				APP_SPEC_LIBS="-lm"
				NETWORK_LIBS=" -lnsl -ldl"
				;;
		OSF1)	
				APP_SPEC_LIBS="" 
				NETWORK_LIBS=""
				;;
		SCO_SV)	
				APP_SPEC_LIBS="" 
				NETWORK_LIBS=" -lnsl -lsocket"
				;;
		HP-UX)	
				APP_SPEC_LIBS=""
				NETWORK_LIBS=" -lnsl"
				;;
		AIX)	
				if [ ${USEGCC} = 'no' ]
				then
					APP_SPEC_LIBS=" -bmaxdata:0x30000000 -bdynamic -brtl" 
				else
					APP_SPEC_LIBS=" -Xlinker -brtl -Xlinker -bmaxdata:0x30000000" 
				fi
				NETWORK_LIBS=" -lnsl -ldl"
				;;
		*)		
				APP_SPEC_LIBS=""
				NETWORK_LIBS=" -lnsl -lsocket -ldl "
				;;
	esac;
	ECHOSTRING="${APP_SPEC_LIBS} ${NETWORK_LIBS}"
	echo "${ECHOSTRING}"
	exit 0
fi

echo "Syntax Error: getopts.sh compiler/options"
exit 0
