/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_atomic.h>
#include <sbi/riscv_io.h>
#include <sbi/sbi_domain.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hartmask.h>
#include <sbi/sbi_ipi.h>
#include <sbi/sbi_timer.h>
#include <sbi_utils/ipi/aclint_mswi.h>
#include <sbi/sbi_console.h>

static struct aclint_mswi_data *mswi_hartid2data[SBI_HARTMASK_MAX_BITS];

static void mswi_ipi_send(u32 target_hart)
{
	u32 *msip;
	struct aclint_mswi_data *mswi;

	if (SBI_HARTMASK_MAX_BITS <= target_hart)
		return;
	mswi = mswi_hartid2data[target_hart];
	if (!mswi)
		return;

	/* Set ACLINT IPI */
#if defined(BR2_CLUSTER_1_CORE) && defined(BR2_CHIPLET_1_DIE1_AVAILABLE) && defined(BR2_CHIPLET_1)
	if(target_hart >= 1  )
	{
		msip = (void *)mswi->addr + 0x20000000;
		writel(1, &msip[(target_hart-1) - mswi->first_hartid]);
	// sbi_printf("mswi_ipi_send:msip[%d -%d] %lx\n",target_hart, mswi->first_hartid,(unsigned long)&msip[(target_hart-4) - mswi->first_hartid]);
	}
#else
	if(target_hart >= 4  )
	{
		msip = (void *)mswi->addr + 0x20000000;
		writel(1, &msip[(target_hart-4) - mswi->first_hartid]);
	// sbi_printf("mswi_ipi_send:msip[%d -%d] %lx\n",target_hart, mswi->first_hartid,(unsigned long)&msip[(target_hart-4) - mswi->first_hartid]);
	}
#endif
	
	else 
	{
		msip = (void *)mswi->addr;
		writel(1, &msip[target_hart - mswi->first_hartid]);
		// sbi_printf("mswi_ipi_send:msip[%d -%d] %lx\n",target_hart, mswi->first_hartid,(unsigned long)&msip[target_hart - mswi->first_hartid]);
	}
	// sbi_printf("exit mswi_ipi_send\n");
}

static void mswi_ipi_clear(u32 target_hart)
{
	u32 *msip;
	struct aclint_mswi_data *mswi;

	if (SBI_HARTMASK_MAX_BITS <= target_hart)
		return;
	mswi = mswi_hartid2data[target_hart];
	if (!mswi)
		return;

	/* Clear ACLINT IPI */
#if defined(BR2_CLUSTER_1_CORE) && defined(BR2_CHIPLET_1_DIE1_AVAILABLE) && defined(BR2_CHIPLET_1)
	if(target_hart >= 1  )
	{
		msip = (void *)mswi->addr + 0x20000000;
		writel(0, &msip[(target_hart-1) - mswi->first_hartid]);
		// sbi_printf("mswi_ipi_clear:msip[%d -%d] %lx\n",target_hart, mswi->first_hartid,(unsigned long)&msip[(target_hart-4) - mswi->first_hartid]);
	}
#else
	if(target_hart >= 4  )
	{
		msip = (void *)mswi->addr + 0x20000000;
		writel(0, &msip[(target_hart-4) - mswi->first_hartid]);
		// sbi_printf("mswi_ipi_clear:msip[%d -%d] %lx\n",target_hart, mswi->first_hartid,(unsigned long)&msip[(target_hart-4) - mswi->first_hartid]);
	}
#endif
	
	else 
	{
		msip = (void *)mswi->addr;
		writel(0, &msip[target_hart - mswi->first_hartid]);
		// sbi_printf("mswi_ipi_clear:msip[%d -%d] %lx\n",target_hart, mswi->first_hartid,(unsigned long)&msip[target_hart - mswi->first_hartid]);
	}
}

static struct sbi_ipi_device aclint_mswi = {
	.name = "aclint-mswi",
	.ipi_send = mswi_ipi_send,
	.ipi_clear = mswi_ipi_clear
};

int aclint_mswi_warm_init(void)
{
	/* Clear IPI for current HART */
	mswi_ipi_clear(current_hartid());

	return 0;
}

int aclint_mswi_cold_init(struct aclint_mswi_data *mswi)
{
	u32 i;
#ifndef HOLE_REGION
	int rc;
	unsigned long pos, region_size;
	struct sbi_domain_memregion reg;
#endif
	/* Sanity checks */
	if (!mswi || (mswi->addr & (ACLINT_MSWI_ALIGN - 1)) ||
	    (mswi->size < ACLINT_MSWI_SIZE) ||
	    (mswi->first_hartid >= SBI_HARTMASK_MAX_BITS) ||
	    (mswi->hart_count > ACLINT_MSWI_MAX_HARTS))
		return SBI_EINVAL;

	/* Update MSWI hartid table */
	for (i = 0; i < mswi->hart_count; i++)
		mswi_hartid2data[mswi->first_hartid + i] = mswi;

#ifndef HOLE_REGION
	/* Add MSWI regions to the root domain */
	for (pos = 0; pos < mswi->size; pos += ACLINT_MSWI_ALIGN) {
		region_size = ((mswi->size - pos) < ACLINT_MSWI_ALIGN) ?
			      (mswi->size - pos) : ACLINT_MSWI_ALIGN;
		sbi_domain_memregion_init(mswi->addr + pos, region_size,
					  SBI_DOMAIN_MEMREGION_MMIO, &reg,0);
		rc = sbi_domain_root_add_memregion(&reg);
		if (rc)
			return rc;
	}
#endif
	sbi_ipi_set_device(&aclint_mswi);

	return 0;
}
