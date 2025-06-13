/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2024-2025, NVIDIA CORPORATION
 */

#ifndef FFA_CONSOLE_H
#define FFA_CONSOLE_H

#ifdef CFG_FFA_CONSOLE

struct ffa_console_data {
	struct serial_chip chip;
	char buf[FFA_CONSOLE_LOG_32_MAX_MSG_LEN];
	size_t pos;
};

/*
 * Initialize console which uses FFA_CONSOLE_LOG of hafnium.
 */
void ffa_console_init(void);
#else
static inline void ffa_console_init(void)
{
}
#endif

#endif /* FFA_CONSOLE_H */
