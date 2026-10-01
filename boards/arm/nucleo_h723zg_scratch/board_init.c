/* SPDX-License-Identifier: Apache-2.0 */

#include <zephyr/init.h>
#include <zephyr/sys/printk.h>

static int scratch_board_init(void)
{
	printk("Board Initialized\n");
	return 0;
}

SYS_INIT(scratch_board_init, PRE_KERNEL_2, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
