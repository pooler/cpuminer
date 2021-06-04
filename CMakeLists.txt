# The MIT License (MIT)
# 
# Copyright (c) 2021 github.com/Pan7
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# 
cmake_minimum_required(VERSION 2.8.12)
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "Not building in source directory. Please create an extra build directory and run cmake there. I.e. commands: mkdir build;cd build;cmake ..")
endif()
get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT IS_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
    message(WARNING "Single-configuration generators require CMAKE_BUILD_TYPE to be set. I.e. -DCMAKE_BUILD_TYPE=Release")
endif()

set(PACKAGE_NAME "cpuminer")
set(PACKAGE_TARGET "minerd")
set(PACKAGE_STATIC_TARGET "${PACKAGE_TARGET}-static")
project(${PACKAGE_NAME})

#read PACKAGE_VERSION from configure.ac
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/configure.ac")
	file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/configure.ac" AC_INIT LIMIT_COUNT 1)
	string(REGEX MATCH "\\[[0-9]+\\.[0-9]+\\.[0-9]+(-[A-Za-z0-9]+)?\\]" PACKAGE_VERSION_STR "${AC_INIT}")
	string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+(-[A-Za-z0-9]+)?" PACKAGE_VERSION "${PACKAGE_VERSION_STR}")
endif()
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
message(STATUS "Package ${PACKAGE_STRING}")

include(CheckCSourceCompiles)
include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)

option(USE_ASM "Use assembly routines" ON)
option(USE_AVX "Use AVX assembly" ON)
option(USE_AVX2 "Use AVX2 assembly" ON)
option(USE_XOP "Use XOP assembly" ON)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpuminer-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/include/cpuminer-config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

list(APPEND SRC_FILES
cpu-miner.c
scrypt.c
sha2.c
util.c
)
list(APPEND HEADER_FILES
compat.h
elist.h
miner.h
)
list(APPEND ARM_FILES
scrypt-arm.S
sha2-arm.S
)
list(APPEND PPC_FILES
scrypt-ppc.S
sha2-ppc.S
)
list(APPEND X86_FILES
scrypt-x86.S
sha2-x86.S
)
list(APPEND X64_FILES
scrypt-x64.S
sha2-x64.S
)

CHECK_INCLUDE_FILES(alloca.h HAVE_ALLOCA_H)
CHECK_INCLUDE_FILES(getopt.h HAVE_GETOPT_H)
CHECK_INCLUDE_FILES(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
CHECK_INCLUDE_FILES(strings.h HAVE_STRINGS_H)
CHECK_INCLUDE_FILES(syslog.h HAVE_SYSLOG_H)
CHECK_INCLUDE_FILES(sys/endian.h HAVE_SYS_ENDIAN_H)
CHECK_INCLUDE_FILES(sys/param.h HAVE_SYS_PARAM_H)
CHECK_INCLUDE_FILES(sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)

if(HAVE_ALLOCA_H)
	list(APPEND EXTRA_DEFS "-DHAVE_ALLOCA_H")
	CHECK_SYMBOL_EXISTS(alloca alloca.h HAVE_ALLOCA)
else()
	CHECK_FUNCTION_EXISTS(alloca HAVE_ALLOCA)
endif()
if(HAVE_GETOPT_H)
	list(APPEND EXTRA_DEFS "-DHAVE_GETOPT_H")
	CHECK_SYMBOL_EXISTS(getopt_long getopt.h HAVE_GETOPT_LONG)
else()
	CHECK_FUNCTION_EXISTS(getopt_long HAVE_GETOPT_LONG)
endif()
if(HAVE_GETOPT_LONG)
	list(APPEND EXTRA_DEFS "-DHAVE_GETOPT_LONG")
endif()
if(HAVE_STRINGS_H)
	list(APPEND EXTRA_DEFS "-DHAVE_STRINGS_H")
	CHECK_SYMBOL_EXISTS(strncasecmp strings.h HAVE_STRNCASECMP)
	CHECK_SYMBOL_EXISTS(strcasecmp strings.h HAVE_STRCASECMP)
else()
	CHECK_FUNCTION_EXISTS(strncasecmp HAVE_STRNCASECMP)
	CHECK_FUNCTION_EXISTS(strcasecmp HAVE_STRCASECMP)
endif()
if(HAVE_SYS_ENDIAN_H)
	list(APPEND EXTRA_DEFS "-DHAVE_SYS_ENDIAN_H")
	CHECK_SYMBOL_EXISTS(be32dec sys/endian.h HAVE_DECL_BE32DEC)
	CHECK_SYMBOL_EXISTS(le32dec sys/endian.h HAVE_DECL_LE32DEC)
	CHECK_SYMBOL_EXISTS(be32enc sys/endian.h HAVE_DECL_BE32ENC)
	CHECK_SYMBOL_EXISTS(le32enc sys/endian.h HAVE_DECL_LE32ENC)
else()
	CHECK_FUNCTION_EXISTS(be32dec HAVE_DECL_BE32DEC)
	CHECK_FUNCTION_EXISTS(le32dec HAVE_DECL_LE32DEC)
	CHECK_FUNCTION_EXISTS(be32enc HAVE_DECL_BE32ENC)
	CHECK_FUNCTION_EXISTS(le32enc HAVE_DECL_LE32ENC)
endif(HAVE_SYS_ENDIAN_H)
if(HAVE_DECL_BE32DEC)
	list(APPEND EXTRA_DEFS "-DHAVE_DECL_BE32DEC")
endif()
if(HAVE_DECL_LE32DEC)
	list(APPEND EXTRA_DEFS "-DHAVE_DECL_LE32DEC")
endif()
if(HAVE_DECL_BE32ENC)
	list(APPEND EXTRA_DEFS "-DHAVE_DECL_BE32ENC")
endif()
if(HAVE_DECL_LE32ENC)
	list(APPEND EXTRA_DEFS "-DHAVE_DECL_LE32ENC")
endif()
if(HAVE_SYS_PARAM_H)
	list(APPEND EXTRA_DEFS "-DHAVE_SYS_PARAM_H")
	CHECK_INCLUDE_FILES("sys/types.h sys/param.h sys/sysctl.h" HAVE_SYS_SYSCTL_H)
else()
	CHECK_INCLUDE_FILES("sys/types.h sys/sysctl.h" HAVE_SYS_SYSCTL_H)
endif()
if(HAVE_SYS_TIME_H)
	list(APPEND EXTRA_DEFS "-DHAVE_SYS_TIME_H")
	CHECK_SYMBOL_EXISTS(gettimeofday sys/time.h HAVE_GETTIMEOFDAY)
else()
	CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
endif()
if(HAVE_GETTIMEOFDAY)
	list(APPEND EXTRA_DEFS "-DHAVE_GETTIMEOFDAY")
endif()
if(HAVE_UNISTD_H)
	list(APPEND EXTRA_DEFS "-DHAVE_UNISTD_H")
	CHECK_SYMBOL_EXISTS(getopt unistd.h HAVE_GETOPT)
else()
	CHECK_FUNCTION_EXISTS(getopt HAVE_GETOPT)
endif()

#CHECK_C_SOURCE_COMPILES("int foo __attribute__((aligned(16))); int main() {return 0;}" HAVE_ATTRIBUTE_ALIGNED)
CHECK_C_SOURCE_COMPILES("int __attribute__((aligned(16))) foo; int main() {return 0;}" HAVE_ATTRIBUTE_ALIGNED)
CHECK_C_SOURCE_COMPILES("int __declspec(align(16)) foo; int main() {return 0;}" HAVE_DECLSPEC_ALIGN)
CHECK_C_SOURCE_COMPILES("inline void foo(void) { } int main() {return 0;}" HAVE_INLINE)
if(NOT HAVE_INLINE)
	CHECK_C_SOURCE_COMPILES("__inline void foo(void) { } int main() {return 0;}" HAVE___INLINE)
	if(HAVE___INLINE)
		list(APPEND EXTRA_DEFS "-Dinline=__inline")
	else()
		CHECK_C_SOURCE_COMPILES("__inline__ void foo(void) { } int main() {return 0;}" HAVE___INLINE__)
		if(HAVE___INLINE__)
			list(APPEND EXTRA_DEFS "-Dinline=__inline__")
		else()
			message(FATAL_ERROR "No inline keyword found!")
		endif()
	endif(HAVE___INLINE)
endif(NOT HAVE_INLINE)

#include(CheckCCompilerFlag)
#CHECK_C_COMPILER_FLAG(-std=c99 HAVE_STD_C99)
#if(HAVE_STD_C99)
#    set(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")
#endif()

CHECK_C_SOURCE_COMPILES("int main() {
#ifdef __i386__
return 0;
#else
macroisundefinederror
#endif
}" HAVE___I386__)

CHECK_C_SOURCE_COMPILES("int main() {
#ifdef __x86_64__
return 0;
#else
macroisundefinederror
#endif
}" HAVE___X86_64__)

CHECK_C_SOURCE_COMPILES("int main() {
#if defined(__arm__) && defined(__APCS_32__)
return 0;
#else
macroisundefinederror
#endif
}" HAVE___ARM__APCS_32)

CHECK_C_SOURCE_COMPILES("int main() {
#if (defined(__powerpc__) || defined(__ppc__) || defined(__PPC__))
return 0;
#else
macroisundefinederror
#endif
}" HAVE_PPC)

CHECK_C_SOURCE_COMPILES("int main() {asm(\"vmovdqa %ymm0, %ymm1\");return 0;}" HAVE_AVX)
CHECK_C_SOURCE_COMPILES("int main() {asm(\"vprotd \$7, %xmm0, %xmm1\");return 0;}" HAVE_XOP)
CHECK_C_SOURCE_COMPILES("int main() {asm(\"vpaddd %ymm0, %ymm1, %ymm2\");return 0;}" HAVE_AVX2)

if(CMAKE_COMPILER_IS_GNUCC)
	if(HAVE___X86_64__)
		set(ASM_FILES ${X64_FILES})
	elseif(HAVE___I386__)
		set(ASM_FILES ${X86_FILES})
	elseif(HAVE___ARM__APCS_32)
		set(ASM_FILES ${ARM_FILES})
	elseif(HAVE_PPC)
		set(ASM_FILES ${PPC_FILES})
	endif()
	if(USE_ASM AND ASM_FILES)
		enable_language(ASM)
		list(APPEND EXTRA_DEFS "-DUSE_ASM")
		if(HAVE___X86_64__)
			if(HAVE_AVX AND USE_AVX)
				list(APPEND EXTRA_DEFS "-DUSE_AVX")
			endif()
			if(HAVE_XOP AND USE_XOP)
				list(APPEND EXTRA_DEFS "-DUSE_XOP")
			endif()
			if(HAVE_AVX2 AND USE_AVX2)
				list(APPEND EXTRA_DEFS "-DUSE_AVX2")
			endif()
		endif()
		set(ASM_SRC_FILES ${ASM_FILES})
	endif(USE_ASM AND ASM_FILES)

	list(APPEND EXTRA_CFLAGS "-O3")
	list(APPEND EXTRA_CFLAGS "-fno-strict-aliasing")
endif(CMAKE_COMPILER_IS_GNUCC)

if(WIN32)
	list(APPEND EXTRA_DEFS "-DWIN32_LEAN_AND_MEAN")
	#windows sockets library
	CHECK_LIBRARY_EXISTS(ws2_32 getch "" HAVE_LIBWS2_32)
	if(HAVE_LIBWS2_32)
		list(APPEND EXTRA_LIBS "ws2_32")
	endif()
endif()

if(MSVC)
	#basetsd.h is required for SSIZE_T to define ssize_t
	CHECK_INCLUDE_FILES(basetsd.h HAVE_BASETSD_H)
	CHECK_INCLUDE_FILES(malloc.h HAVE_MALLOC_H)
	CHECK_INCLUDE_FILES(winsock2.h HAVE_WINSOCK2_H)

	if(HAVE_MALLOC_H)
		CHECK_SYMBOL_EXISTS(_alloca malloc.h HAVE__ALLOCA)
	endif()
#	if(HAVE_WINSOCK2_H)
#		list(APPEND EXTRA_LIBS "ws2_32")
#	endif()
	if(HAVE_STRING_H)
		CHECK_SYMBOL_EXISTS(_strnicmp string.h HAVE__STRNICMP)
		CHECK_SYMBOL_EXISTS(_stricmp string.h HAVE__STRICMP)
	else()
		CHECK_FUNCTION_EXISTS(_strnicmp HAVE__STRNICMP)
		CHECK_FUNCTION_EXISTS(_stricmp HAVE__STRICMP)
	endif()
	if(NOT HAVE__STRNICMP)
		message(WARNING "No case-insensitive size-limitted string compare function found! strncasecmp()")
	endif()
	if(NOT HAVE__STRICMP)
		message(WARNING "No case-insensitive string compare function found! strcasecmp()")
	endif()

	list(APPEND EXTRA_DEFS "-D_CRT_SECURE_NO_WARNINGS")
	if(NOT HAVE_GETOPT AND NOT HAVE_GETOPT_LONG)
		set(HAVE_GETOPT_LONG TRUE)
		message(WARNING "Ignoring missing function getopt_long()! Manually add getopt.h")
	endif()

	if(USE_ASM AND ASM_FILES)
		enable_language(ASM_MASM)
		list(APPEND EXTRA_DEFS "-DUSE_ASM")
		set(ASM_SRC_FILES ${ASM_FILES})
	endif(USE_ASM AND ASM_FILES)
endif()

if(NOT JANSSON_FORCE)
	find_package(JANSSON)
	if(NOT JANSSON_FOUND)
		pkg_search_module(JANSSON QUIET jansson libjansson)
	endif()
endif()
if(JANSSON_FOUND)
	set(HAVE_JANSSON_H TRUE)
	include_directories(${JANSSON_INCLUDE_DIRS})
	list(APPEND EXTRA_LIBS ${JANSSON_LIBRARIES})
else(JANSSON_FOUND)
	if(EXISTS "${CMAKE_SOURCE_DIR}/compat/jansson/CMakeLists.txt")
		add_subdirectory("${CMAKE_SOURCE_DIR}/compat/jansson")
		set(JANSSON_COMPAT TRUE)
		include_directories("${CMAKE_BINARY_DIR}/compat/jansson/include")
		list(APPEND EXTRA_LIBS jansson)
	elseif(EXISTS "${CMAKE_SOURCE_DIR}/compat/jansson-2.9/CMakeLists.txt")
		add_subdirectory("${CMAKE_SOURCE_DIR}/compat/jansson-2.9")
		list(APPEND EXTRA_LIBS jansson)
		include_directories("${CMAKE_BINARY_DIR}/compat/jansson-2.9/include")
	else()
		message(WARNING "jansson library not found!")
	endif()

endif(JANSSON_FOUND)

if(NOT CURL_FORCE)
	find_package(CURL 7.15.2)
	if(NOT CURL_FOUND)
		pkg_search_module(CURL QUIET curl>=7.15.2 libcurl>=7.15.2)
	endif()
endif()
if(CURL_FOUND)
	set(HAVE_CURL_CURL_H TRUE)
	include_directories(${CURL_INCLUDE_DIRS})
	list(APPEND EXTRA_LIBS ${CURL_LIBRARIES})
endif()

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads)
if(CMAKE_USE_PTHREADS_INIT)
	set(HAVE_PTHREAD_H TRUE)
	list(APPEND EXTRA_LIBS ${CMAKE_THREAD_LIBS_INIT})
else()
	pkg_search_module(PTHREAD QUIET pthread libpthread)
	if(PTHREAD_FOUND)
		set(HAVE_PTHREAD_H TRUE)
		include_directories(${PTHREAD_INCLUDE_DIRS})
		list(APPEND EXTRA_LIBS ${PTHREAD_LIBRARIES})
	else()
		CHECK_INCLUDE_FILES(pthread.h HAVE_PTHREAD_H)
		if(MSVC AND NOT HAVE_PTHREAD_H)
			pkg_search_module(PTHREAD QUIET pthreads-win32 pthreads4w)
			if(PTHREAD_FOUND)
				set(HAVE_PTHREAD_H TRUE)
				include_directories(${PTHREAD_INCLUDE_DIRS})
				list(APPEND EXTRA_LIBS ${PTHREAD_LIBRARIES})
			endif()
		endif()
	endif(PTHREAD_FOUND)
endif(CMAKE_USE_PTHREADS_INIT)

#grouping sources for the project
source_group("Source Files" FILES ${SRC_FILES} ${ASM_SRC_FILES})
source_group("Header Files" FILES ${HEADER_FILES})

add_executable("${PACKAGE_TARGET}" ${SRC_FILES} ${HEADER_FILES} ${ASM_SRC_FILES})
target_compile_definitions("${PACKAGE_TARGET}" PRIVATE ${EXTRA_DEFS})
target_compile_options("${PACKAGE_TARGET}" PRIVATE ${EXTRA_CFLAGS})
target_link_libraries("${PACKAGE_TARGET}" ${EXTRA_LIBS} ${EXTRA_LDFLAGS})

if(MSVC)
	#set default MSVC project
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${PACKAGE_TARGET}")

	list(APPEND EXTRA_STATIC_DEFS "-DCURL_STATICLIB")
	#runtime library multi-threaded (/MT)
	list(APPEND EXTRA_STATIC_CFLAGS "/MT")

	add_executable("${PACKAGE_STATIC_TARGET}" ${SRC_FILES} ${HEADER_FILES} ${ASM_SRC_FILES})
	target_compile_definitions("${PACKAGE_STATIC_TARGET}" PRIVATE ${EXTRA_DEFS} ${EXTRA_STATIC_DEFS})
	target_compile_options("${PACKAGE_STATIC_TARGET}" PRIVATE ${EXTRA_CFLAGS} ${EXTRA_STATIC_CFLAGS})
	target_link_libraries("${PACKAGE_STATIC_TARGET}" ${EXTRA_LIBS} ${EXTRA_LDFLAGS} ${EXTRA_STATIC_LDFLAGS})

endif(MSVC)