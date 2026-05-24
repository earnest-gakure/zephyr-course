#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>

static int board_init(void)
{
	printk("Board Initialized\n");
	return 0;
}

SYS_INIT(board_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

int main(void)
{
	printf("Hello World! %s\n", CONFIG_BOARD_TARGET);
	return 0;
}