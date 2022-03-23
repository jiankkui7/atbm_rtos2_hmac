#ifndef ATBM_OS_TEST_H
#define ATBM_OS_TEST_H

struct atbm_test_item{
	char *name;
	int (*test_func)(void);
};

#endif
