INCLUDE_DIR=include
SRC_DIR=src
TEST_DIR=test
BUILD_DIR=build
BUILD_SRC=${BUILD_DIR}/${SRC_DIR}
BUILD_TEST=${BUILD_DIR}/${TEST_DIR}

STANDARDS=rfc_3986 rfc_3966
HELPERS=rbtree
INCLUDES=${patsubst %,${INCLUDE_DIR}/%.h,${STANDARDS}}
HELPER_INCLUDES=${patsubst %,${SRC_DIR}/%.h,${HELPERS}}
SRC=${patsubst %,${SRC_DIR}/%.c,${STANDARDS}}
TEST_SRC=${patsubst %,${TEST_DIR}/%.c,${STANDARDS}} \
         ${patsubst %,${TEST_DIR}/%.c,${HELPERS}}
TARGETS=${patsubst %,${BUILD_SRC}/%.o,${STANDARDS}}
TEST_TARGETS=${patsubst %,${BUILD_TEST}/%.o,${STANDARDS}} \
             ${patsubst %,${BUILD_TEST}/%.o,${HELPERS}}
TESTS=${patsubst %,${BUILD_DIR}/test_%,${STANDARDS}} \
      ${patsubst %,${BUILD_DIR}/test_%,${HELPERS}}

STATIC_LIB=${BUILD_DIR}/libURIPathFinder.a

CC=gcc
CFLAGS=-Wall -Wextra -Wno-comment -Wno-logical-op-parentheses $\
	   -Wno-unused-function -std=c89 -O3 -I${INCLUDE_DIR}

.PHONY: lib
lib: ${STATIC_LIB}

${BUILD_DIR}/%.o: %.c ${INCLUDES} ${HELPER_INCLUDES}
	mkdir -p ${dir $@}
	${CC} -o $@ $< -c ${CFLAGS}

${STATIC_LIB}: ${TARGETS}
	ar cru $@ $^
	ranlib $@

${TESTS}: ${BUILD_DIR}/test_% : ${BUILD_TEST}/%.o ${STATIC_LIB}
	${CC} -I ${CFLAGS} -o $@ $^

.PHONY: test
test: ${TESTS}
	${foreach exe,$^,./${exe};}

.PHONY: clean
clean:
	rm -rf ${BUILD_DIR}
