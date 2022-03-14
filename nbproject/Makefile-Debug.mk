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
CC=clang
CCC=clang++
CXX=clang++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=CLang-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/Addition.o \
	${OBJECTDIR}/src/Advection.o \
	${OBJECTDIR}/src/Diffusion.o \
	${OBJECTDIR}/src/Divergence.o \
	${OBJECTDIR}/src/FileUtils.o \
	${OBJECTDIR}/src/Pressure.o \
	${OBJECTDIR}/src/Projection.o \
	${OBJECTDIR}/src/Renderer.o \
	${OBJECTDIR}/src/ShaderUtils.o \
	${OBJECTDIR}/src/TextureUtils.o \
	${OBJECTDIR}/src/Visualization.o \
	${OBJECTDIR}/src/glad/glad.o \
	${OBJECTDIR}/src/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=`pkg-config --libs glfw3` `pkg-config --libs gl` -ldl  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cppnaviersimulation

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cppnaviersimulation: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cppnaviersimulation ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/Addition.o: src/Addition.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags glfw3` `pkg-config --cflags gl`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Addition.o src/Addition.cpp

${OBJECTDIR}/src/Advection.o: src/Advection.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags glfw3` `pkg-config --cflags gl`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Advection.o src/Advection.cpp

${OBJECTDIR}/src/Diffusion.o: src/Diffusion.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags glfw3` `pkg-config --cflags gl`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Diffusion.o src/Diffusion.cpp

${OBJECTDIR}/src/Divergence.o: src/Divergence.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags glfw3` `pkg-config --cflags gl`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Divergence.o src/Divergence.cpp

${OBJECTDIR}/src/FileUtils.o: src/FileUtils.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags glfw3` `pkg-config --cflags gl`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/FileUtils.o src/FileUtils.cpp

${OBJECTDIR}/src/Pressure.o: src/Pressure.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags glfw3` `pkg-config --cflags gl`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Pressure.o src/Pressure.cpp

${OBJECTDIR}/src/Projection.o: src/Projection.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags glfw3` `pkg-config --cflags gl`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Projection.o src/Projection.cpp

${OBJECTDIR}/src/Renderer.o: src/Renderer.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags glfw3` `pkg-config --cflags gl`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Renderer.o src/Renderer.cpp

${OBJECTDIR}/src/ShaderUtils.o: src/ShaderUtils.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags glfw3` `pkg-config --cflags gl`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ShaderUtils.o src/ShaderUtils.cpp

${OBJECTDIR}/src/TextureUtils.o: src/TextureUtils.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags glfw3` `pkg-config --cflags gl`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/TextureUtils.o src/TextureUtils.cpp

${OBJECTDIR}/src/Visualization.o: src/Visualization.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags glfw3` `pkg-config --cflags gl`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Visualization.o src/Visualization.cpp

${OBJECTDIR}/src/glad/glad.o: src/glad/glad.C
	${MKDIR} -p ${OBJECTDIR}/src/glad
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags glfw3` `pkg-config --cflags gl`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/glad/glad.o src/glad/glad.C

${OBJECTDIR}/src/main.o: src/main.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g `pkg-config --cflags glfw3` `pkg-config --cflags gl`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.cpp

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
