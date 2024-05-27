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
};