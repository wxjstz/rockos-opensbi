#include <platform_override.h>
#include <sbi_utils/fdt/fdt_helper.h>

static const struct fdt_match eic770x_match[] = {
	{ .compatible = "SiFive,FU800-dev" },
	{ .compatible = "fu800-dev" },
	{ .compatible = "sifive-dev" },
	{ },
};

const struct platform_override eic770x = {
	.match_table		= eic770x_match,
};