// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2024-2025, NVIDIA CORPORATION
 */

#include <compiler.h>
#include <console.h>
#include <ffa.h>
#include <drivers/ffa_console.h>
#include <drivers/serial.h>
#include <kernel/dt_driver.h>
#include <kernel/thread_arch.h>

static struct ffa_console_data ffa_console __nex_bss;

static void ffa_console_putc(struct serial_chip *chip __unused, int ch)
{
	struct ffa_console_data *pd =
		container_of(chip, struct ffa_console_data, chip);

	pd->buf[pd->pos++] = ch;

	if (pd->pos == FFA_CONSOLE_LOG_32_MAX_MSG_LEN) {
		ffa_console_flush(chip);
	}
}

static void ffa_console_flush(struct serial_chip *chip)
{
	struct ffa_console_data *pd =
		container_of(chip, struct ffa_console_data, chip);

	struct thread_smc_args args = { };
	args.a0 = FFA_CONSOLE_LOG_32;
	args.a1 = pd->pos;
	memcpy(&args.a2, pd->buf, pd->pos);
	thread_smccc(&args);

	pd->pos = 0;
}

static const struct serial_ops ffa_console_ops = {
	.putc = ffa_console_putc,
	.flush = ffa_console_flush,
};
DECLARE_KEEP_PAGER(ffa_console_ops);

void ffa_console_init(void)
{
	ffa_console.chip.ops = &ffa_console_ops;
	ffa_console.pos = 0;

	register_serial_console(&ffa_console);
}

#ifdef CFG_DT

static struct serial_chip *ffa_console_dev_alloc(void)
{
	return &ffa_console;
}

static int ffa_console_dev_init(struct serial_chip *chip __unused,
				const void *fdt __unused, int offs __unused,
				const char *params __unused)
{
	return 0;
}

static void ffa_console_dev_free(struct serial_chip *chip __unused)
{
}

static const struct serial_driver ffa_console_driver = {
	.dev_alloc = ffa_console_dev_alloc,
	.dev_init = ffa_console_dev_init,
	.dev_free = ffa_console_dev_free,
};

static const struct dt_device_match ffa_console_match_table[] = {
	{ .compatible = "arm,ffa-console" },
	{ }
};

DEFINE_DT_DRIVER(ffa_console_dt_driver) = {
	.name = "ffa-console",
	.type = DT_DRIVER_UART,
	.match_table = ffa_console_match_table,
	.driver = &ffa_console_driver,
};

#endif /* CFG_DT */
