/*
 * Copyright (c) 2016 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <asm/asm.h>
#include <asm/apic.h>
#include <asm/processor.h>

#include <lego/kernel.h>
#include <lego/bitops.h>

#ifdef CONFIG_X86_X2APIC
void __init x2apic_enable(void)
{
	u64 msr;

	rdmsrl(MSR_IA32_APICBASE, msr);
	if (msr & X2APIC_ENABLE)
		return;
	wrmsrl(MSR_IA32_APICBASE, msr | X2APIC_ENABLE);
	printk(KERN_INFO "x2apic enabled\n");
}

void __init check_x2apic(void)
{
	if (x2apic_enabled())
		pr_info("x2apic: enabled by BIOS, switching to x2apic\n");
	else if (!x2apic_supported())
		pr_info("x2apic: not supported by your CPU\n");
	else
		pr_info("x2apic: disabled by your BIOS\n");
}
#endif

void __init setup_apic_driver(void)
{
	struct apic **drv;

	for (drv = __apicdrivers; drv < __apicdrivers_end; drv++) {
		if ((*drv)->probe && (*drv)->probe()) {
			if (apic != *drv) {
				apic = *drv;
				pr_info("Switched APIC routing to %s.\n",
					apic->name);
			}
			return;
		}
	}
	panic("APIC: no driver found");
}