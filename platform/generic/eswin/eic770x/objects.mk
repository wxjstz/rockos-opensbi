#
# SPDX-License-Identifier: BSD-2-Clause
#
# Copyright (C) 2023 Inochi Amaoto <inochiama@outlook.com>
# Copyright (C) 2023 Alibaba Group Holding Limited.
#

carray-platform_override_modules-$(CONFIG_PLATFORM_ESWIN_EIC770X) += eic770x
platform-objs-$(CONFIG_PLATFORM_ESWIN_EIC770X) += eswin/eic770x/eic770x.o
