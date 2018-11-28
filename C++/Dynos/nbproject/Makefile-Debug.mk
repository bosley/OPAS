#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/blocker/Blocker.o \
	${OBJECTDIR}/ckey/ckey.o \
	${OBJECTDIR}/cmac/NetAdapter/NetAdapter_Shared.o \
	${OBJECTDIR}/cmac/NetAdapter/NetAdapter_Stub.o \
	${OBJECTDIR}/cmac/NetAdapter/NetAdapter_Unix.o \
	${OBJECTDIR}/cmac/NetAdapter/NetAdapter_Win32.o \
	${OBJECTDIR}/cmac/NetAdapter/NetAdapter_WinRT.o \
	${OBJECTDIR}/cmac/cmac.o \
	${OBJECTDIR}/core.o \
	${OBJECTDIR}/host/DydnoDB.o \
	${OBJECTDIR}/host/helpers.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/net/RPCClient.o \
	${OBJECTDIR}/net/RPCServer.o \
	${OBJECTDIR}/net/dnetmsg.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-std=c++14
CXXFLAGS=-std=c++14

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/local/lib -lboost_filesystem -lboost_signals -lboost_system -lboost_thread-mt /usr/local/lib/libboost_program_options.a -lcryptopp -lrpc -ldb-6.2 -ldb_sql -ldb_stl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynos

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynos: /usr/local/lib/libboost_program_options.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynos: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynos ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/blocker/Blocker.o: blocker/Blocker.cpp
	${MKDIR} -p ${OBJECTDIR}/blocker
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/blocker/Blocker.o blocker/Blocker.cpp

${OBJECTDIR}/ckey/ckey.o: ckey/ckey.cpp
	${MKDIR} -p ${OBJECTDIR}/ckey
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ckey/ckey.o ckey/ckey.cpp

${OBJECTDIR}/cmac/NetAdapter/NetAdapter_Shared.o: cmac/NetAdapter/NetAdapter_Shared.cpp
	${MKDIR} -p ${OBJECTDIR}/cmac/NetAdapter
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/cmac/NetAdapter/NetAdapter_Shared.o cmac/NetAdapter/NetAdapter_Shared.cpp

${OBJECTDIR}/cmac/NetAdapter/NetAdapter_Stub.o: cmac/NetAdapter/NetAdapter_Stub.cpp
	${MKDIR} -p ${OBJECTDIR}/cmac/NetAdapter
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/cmac/NetAdapter/NetAdapter_Stub.o cmac/NetAdapter/NetAdapter_Stub.cpp

${OBJECTDIR}/cmac/NetAdapter/NetAdapter_Unix.o: cmac/NetAdapter/NetAdapter_Unix.cpp
	${MKDIR} -p ${OBJECTDIR}/cmac/NetAdapter
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/cmac/NetAdapter/NetAdapter_Unix.o cmac/NetAdapter/NetAdapter_Unix.cpp

${OBJECTDIR}/cmac/NetAdapter/NetAdapter_Win32.o: cmac/NetAdapter/NetAdapter_Win32.cpp
	${MKDIR} -p ${OBJECTDIR}/cmac/NetAdapter
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/cmac/NetAdapter/NetAdapter_Win32.o cmac/NetAdapter/NetAdapter_Win32.cpp

${OBJECTDIR}/cmac/NetAdapter/NetAdapter_WinRT.o: cmac/NetAdapter/NetAdapter_WinRT.cpp
	${MKDIR} -p ${OBJECTDIR}/cmac/NetAdapter
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/cmac/NetAdapter/NetAdapter_WinRT.o cmac/NetAdapter/NetAdapter_WinRT.cpp

${OBJECTDIR}/cmac/cmac.o: cmac/cmac.cpp
	${MKDIR} -p ${OBJECTDIR}/cmac
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/cmac/cmac.o cmac/cmac.cpp

${OBJECTDIR}/core.o: core.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/core.o core.cpp

${OBJECTDIR}/host/DydnoDB.o: host/DydnoDB.cpp
	${MKDIR} -p ${OBJECTDIR}/host
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/host/DydnoDB.o host/DydnoDB.cpp

${OBJECTDIR}/host/helpers.o: host/helpers.cpp
	${MKDIR} -p ${OBJECTDIR}/host
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/host/helpers.o host/helpers.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/net/RPCClient.o: net/RPCClient.cpp
	${MKDIR} -p ${OBJECTDIR}/net
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/net/RPCClient.o net/RPCClient.cpp

${OBJECTDIR}/net/RPCServer.o: net/RPCServer.cpp
	${MKDIR} -p ${OBJECTDIR}/net
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/net/RPCServer.o net/RPCServer.cpp

${OBJECTDIR}/net/dnetmsg.o: net/dnetmsg.cpp
	${MKDIR} -p ${OBJECTDIR}/net
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/local/include -I/usr/local/include/boost -I/usr/local/Cellar/berkeley-db/6.2.32/include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/net/dnetmsg.o net/dnetmsg.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
