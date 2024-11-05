INCLUDE_DIR=include
SRC_DIR=src
TEST_DIR=test
BUILD_DIR=build
BUILD_SRC=${BUILD_DIR}/${SRC_DIR}
BUILD_TEST=${BUILD_DIR}/${TEST_DIR}

STANDARDS=rfc_3986
INCLUDES=${patsubst %,${INCLUDE_DIR}/%.h,${STANDARDS}}
SRC=${patsubst %,${SRC_DIR}/%.c,${STANDARDS}}
TEST_SRC=${patsubst %,${TEST_DIR}/%.c,${STANDARDS}}
TARGETS=${patsubst %,${BUILD_SRC}/%.o,${STANDARDS}}
TEST_TARGETS=${patsubst %,${BUILD_TEST}/%.o,${STANDARDS}}
TESTS=${patsubst %,${BUILD_DIR}/test_%,${STANDARDS}}

STATIC_LIB=${BUILD_DIR}/libURIPathFinder.a

CC=gcc
CFLAGS=-Wall -Wextra -Wno-comment -Wno-logical-op-parentheses $\
	   -Wno-unused-function -std=c89 -O3 -I${INCLUDE_DIR}

.PHONY: lib
lib: ${STATIC_LIB}

${BUILD_DIR}/%.o: %.c ${INCLUDES}
	mkdir -p ${dir $@}
	${CC} -o $@ $< -c ${CFLAGS}

${STATIC_LIB}: ${TARGETS}
	ar cru $@ $^
	ranlib $@

${TESTS}: ${TEST_TARGETS} ${STATIC_LIB}
	${CC} -I ${CFLAGS} -o $@ $^

.PHONY: test
test: ${TESTS}
	${foreach exe,$^,./${exe};}

.PHONY: clean
clean:
	rm -rf ${BUILD_DIR}
