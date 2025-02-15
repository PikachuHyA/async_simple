.PHONY:clean test build

STD_MODULE_PATH=../../third_party_module/stdmodules
OPT_FLAGS = -O2 -g -DNDEBUG -fprebuilt-module-path=$(STD_MODULE_PATH)
OBJECT_CFLAGS = -std=c++20 $(OPT_FLAGS) -Wall -Werror
INTERFACE_CFLAGS ?= $(OBJECT_CFLAGS) --precompile
EXECUTABLE_CFLAGS ?= $(OBJECT_CFLAGS)

ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
TEST_DEPENDENCY_LIB = -lasync_simple -laio -lgtest -lgmock -lpthread -lstd
TEST_DEPENDENCY_DIR = -L../ -L/usr/local/lib64 -L/usr/lib64 -L/usr/lib -L$(STD_MODULE_PATH)
TEST_DEPENDENCY = $(TEST_DEPENDENCY_DIR) $(TEST_DEPENDENCY_LIB) -Wl,-rpath=$(ROOT_DIR)/..

test: build
	./async_simple_test.out
	./util/async_simple_test.out
	./coro/async_simple_test.out
	./uthread/async_simple_test.out

build: async_simple_test.out util/async_simple_test.out coro/async_simple_test.out uthread/async_simple_test.out

clean:
	find . -name "*.o" -o -name "*.out" -o -name "*.pcm" | xargs rm -f

dotest.o: dotest.cpp
	clang++ $(OBJECT_CFLAGS) dotest.cpp -c -o dotest.o

#Try and Future
async_simple_test.out: dotest.o TryTest.o FutureStateTest.o FutureTest.o ../async_simple.pcm
	clang++ $(EXECUTABLE_CFLAGS) dotest.o TryTest.o FutureStateTest.o FutureTest.o  \
			$(TEST_DEPENDENCY) -fprebuilt-module-path=../ -o async_simple_test.out
TryTest.o: TryTest.cpp ../async_simple.pcm
	clang++ $(OBJECT_CFLAGS) TryTest.cpp -fprebuilt-module-path=../ -fprebuilt-module-path=. -c   
FutureStateTest.o: FutureStateTest.cpp ../async_simple.pcm
	clang++ $(OBJECT_CFLAGS) FutureStateTest.cpp  -fprebuilt-module-path=../ -fprebuilt-module-path=. -c 
FutureTest.o: FutureTest.cpp ../async_simple.pcm
	clang++ $(OBJECT_CFLAGS) FutureTest.cpp  -fprebuilt-module-path=../ -fprebuilt-module-path=. -c 
# util
util/async_simple_test.out: dotest.o util/ThreadPoolTest.o ../async_simple.pcm
	clang++ $(EXECUTABLE_CFLAGS) dotest.o util/ThreadPoolTest.o  \
			$(TEST_DEPENDENCY) -fprebuilt-module-path=../ -o util/async_simple_test.out
util/ThreadPoolTest.o: util/ThreadPoolTest.cpp ../async_simple.pcm
	clang++ $(OBJECT_CFLAGS) util/ThreadPoolTest.cpp  -fprebuilt-module-path=../ -fprebuilt-module-path=. -c -o util/ThreadPoolTest.o
# coro
coro/async_simple_test.out: dotest.o coro/LazyTest.o coro/SleepTest.o coro/ViaCoroutineTest.o ../async_simple.pcm
	clang++ $(EXECUTABLE_CFLAGS) dotest.o coro/LazyTest.o coro/SleepTest.o coro/ViaCoroutineTest.o \
			$(TEST_DEPENDENCY) -fprebuilt-module-path=../ -o coro/async_simple_test.out
coro/LazyTest.o: coro/LazyTest.cpp ../async_simple.pcm
	clang++ $(OBJECT_CFLAGS) coro/LazyTest.cpp  -fprebuilt-module-path=../ -fprebuilt-module-path=../experimental -fprebuilt-module-path=. -fprebuilt-module-path=./coro -c -o coro/LazyTest.o
coro/SleepTest.o: coro/SleepTest.cpp ../async_simple.pcm
	clang++ $(OBJECT_CFLAGS) coro/SleepTest.cpp  -fprebuilt-module-path=../ -fprebuilt-module-path=../experimental -fprebuilt-module-path=. -c -o coro/SleepTest.o
coro/ViaCoroutineTest.o: coro/ViaCoroutineTest.cpp ../async_simple.pcm
	clang++ $(OBJECT_CFLAGS) coro/ViaCoroutineTest.cpp  -fprebuilt-module-path=../ -fprebuilt-module-path=../experimental  -fprebuilt-module-path=. -c -o coro/ViaCoroutineTest.o
# uthread
uthread/async_simple_test.out: dotest.o uthread/UthreadTest.o ../async_simple.pcm
	clang++ $(EXECUTABLE_CFLAGS) dotest.o uthread/UthreadTest.o  \
			$(TEST_DEPENDENCY) -fprebuilt-module-path=../ -o uthread/async_simple_test.out
uthread/UthreadTest.o: uthread/UthreadTest.cpp ../async_simple.pcm
	clang++ $(OBJECT_CFLAGS) uthread/UthreadTest.cpp  -fprebuilt-module-path=../ -fprebuilt-module-path=../experimental -fprebuilt-module-path=. -c -o uthread/UthreadTest.o

