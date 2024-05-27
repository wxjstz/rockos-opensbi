#include <platform_override.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi/riscv_io.h>

/* Full tlb flush always */
#define EIC770X_TLB_RANGE_FLUSH_LIMIT	0

static u64 eic770x_get_tlbr_flush_limit(const struct fdt_match *match)
{
	return EIC770X_TLB_RANGE_FLUSH_LIMIT;
}

static void eic770x_fw_init(void *fdt, const struct fdt_match *match)
{
/*
 * CONFIG_BR2_CHIPLE_2 can currently be defined in platform/generic/eswin/eic770x/Kconfig
 * and can be obtained through fdt in the future
 */
#if BR2_CHIPLE_2
	unsigned long fw_text_start_addr;

	// set die1 u84 boot addr
	fw_text_start_addr = FW_TEXT_START;
	writel(fw_text_start_addr>>32, (void *)(0x71828000UL + 0x31c));
	writel(fw_text_start_addr&0xfffffffful, (void *)(0x71828000UL + 0x320));
	writel(fw_text_start_addr>>32, (void *)(0x71828000UL + 0x324));
	writel(fw_text_start_addr&0xfffffffful, (void *)(0x71828000UL + 0x328));
	writel(fw_text_start_addr>>32, (void *)(0x71828000UL + 0x32c));
	writel(fw_text_start_addr&0xfffffffful, (void *)(0x71828000UL + 0x330));
	writel(fw_text_start_addr>>32, (void *)(0x71828000UL + 0x334));
	writel(fw_text_start_addr&0xfffffffful, (void *)(0x71828000UL + 0x338));
	writel(0xfffffffful, (void *)(0x71828000UL + 0x44c));  //release die1 u84
#endif
}

static int eic770x_nascent_init(void)
{
	unsigned long hwpf;
	hwpf = 0x104095C1BE241UL;
	__asm__ volatile("csrw 0x7c3 , %0" : : "r"(hwpf));
	hwpf = 0x38c84eUL;
	__asm__ volatile("csrw 0x7c4 , %0" : : "r"(hwpf));
	return 0;
}


static void init_fcsr(void)
{
	unsigned long hwpf;	// Hardware Prefetcher 0 : 0x104095C1BE241 | Hardware Prefetcher 1 : 0x38c84e

	hwpf = 0x104095C1BE241UL;
	__asm__ volatile("csrw 0x7c3 , %0" : : "r"(hwpf));
	hwpf = 0x38c84eUL;
	__asm__ volatile("csrw 0x7c4 , %0" : : "r"(hwpf));

	/* enable speculative icache refill */
	__asm__ volatile("csrw 0x7c1 , x0" : :);
	__asm__ volatile("csrw 0x7c2 , x0" : :);

}

#ifndef BR2_CHIPLET_2
static void init_bus_blocker(void)
{
#if (defined BR2_CHIPLET_1) && (defined BR2_CHIPLET_1_DIE0_AVAILABLE)
	#define BLOCKER_TL64D2D_OUT	(void *)0x200000
	#define BLOCKER_TL256D2D_OUT	(void *)0x202000
	#define BLOCKER_TL256D2D_IN	(void *)0x204000
	writel(1,BLOCKER_TL64D2D_OUT);
	writel(1,BLOCKER_TL256D2D_OUT);
	writel(1,BLOCKER_TL256D2D_IN);
#elif (defined BR2_CHIPLET_1) && (defined BR2_CHIPLET_1_DIE1_AVAILABLE)
	#define BLOCKER_TL64D2D_OUT	(void *)(0x200000+0x20000000)
	#define BLOCKER_TL256D2D_OUT	(void *)(0x202000+0x20000000)
	#define BLOCKER_TL256D2D_IN	(void *)(0x204000+0x20000000)
	writel(1,BLOCKER_TL64D2D_OUT);
	writel(1,BLOCKER_TL256D2D_OUT);
	writel(1,BLOCKER_TL256D2D_IN);
#endif
}
#endif

static void sbi_hart_blocker_fscr_configure(struct sbi_scratch *scratch)
{
	struct sbi_domain *dom = sbi_domain_thishart_ptr();

	if (dom->boot_hartid == current_hartid()) {
		#ifndef BR2_CHIPLET_2
		/* if only one die, need config blocker to
		generate fake response when access remote target */
		init_bus_blocker();
		#endif
	}

	init_fcsr();
}

static int eic770x_final_init(int clod_boot, const struct fdt_match *match)
{
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	sbi_hart_blocker_fscr_configure(scratch);
	return 0;
}


static int eic770x_resume_finish(struct sbi_scratch *scratch)
{
	sbi_hart_blocker_fscr_configure(scratch);
	return 0;
}

static const struct fdt_match eic770x_match[] = {
	{ .compatible = "SiFive,FU800-dev" },
	{ .compatible = "fu800-dev" },
	{ .compatible = "sifive-dev" },
	{ },
};

const struct platform_override eic770x = {
	.match_table		= eic770x_match,
	.tlbr_flush_limit	= eic770x_get_tlbr_flush_limit,
	.fw_init		= eic770x_fw_init,
	.nascent_init		= eic770x_nascent_init,
	.final_init		= eic770x_final_init,
	.resume_finish		= eic770x_resume_finish,
};