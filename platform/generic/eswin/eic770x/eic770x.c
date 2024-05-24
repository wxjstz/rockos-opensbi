#include <platform_override.h>
#include <sbi_utils/fdt/fdt_helper.h>

/* Full tlb flush always */
#define EIC770X_TLB_RANGE_FLUSH_LIMIT	0

static u64 eic770x_get_tlbr_flush_limit(const struct fdt_match *match)
{
	return EIC770X_TLB_RANGE_FLUSH_LIMIT;
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
};