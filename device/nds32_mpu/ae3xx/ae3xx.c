/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include <nds32_intrinsic.h>
#include "nds32_defs.h"
#include "ae3xx.h"
#include "cache.h"


#define CACHE_NONE              0
#define CACHE_WRITEBACK         2
#define CACHE_WRITETHROUGH      3

#ifdef CFG_CACHE_ENABLE
/* Cacheable */
#ifdef CFG_CACHE_WRITETHROUGH
#define CACHE_MODE              CACHE_WRITETHROUGH
#else
#define CACHE_MODE              CACHE_WRITEBACK
#endif
#else
/* Uncacheable */
#define CACHE_MODE              CACHE_NONE
#endif


#define MMU_CTL_MSK                                     \
        (MMU_CTL_mskD                                   \
         | MMU_CTL_mskNTC0                              \
         | MMU_CTL_mskNTC1                              \
         | MMU_CTL_mskNTC2                              \
         | MMU_CTL_mskNTC3                              \
         | MMU_CTL_mskTBALCK                            \
         | MMU_CTL_mskMPZIU                             \
         | MMU_CTL_mskNTM0                              \
         | MMU_CTL_mskNTM1                              \
         | MMU_CTL_mskNTM2                              \
         | MMU_CTL_mskNTM3)
/*
 * NTC0: CACHE_MODE, NTC1~NTC3: Non-cacheable
 * NTM0~NTM3 are mapped to partition 0/1/2/3
 */
#define MMU_CTL_INIT                                    \
        (0x0UL << MMU_CTL_offD                          \
         | (CACHE_MODE) << MMU_CTL_offNTC0              \
         | 0x0UL << MMU_CTL_offNTC1                     \
         | 0x0UL << MMU_CTL_offNTC2                     \
         | 0x0UL << MMU_CTL_offNTC3                     \
         | 0x0UL << MMU_CTL_offTBALCK                   \
         | 0x0UL << MMU_CTL_offMPZIU                    \
         | 0x0UL << MMU_CTL_offNTM0                     \
         | 0x1UL << MMU_CTL_offNTM1                     \
         | 0x2UL << MMU_CTL_offNTM2                     \
         | 0x3UL << MMU_CTL_offNTM3)

#define CACHE_CTL_MSK                                   \
        (CACHE_CTL_mskIC_EN                             \
         | CACHE_CTL_mskDC_EN                           \
         | CACHE_CTL_mskICALCK                          \
         | CACHE_CTL_mskDCALCK                          \
         | CACHE_CTL_mskDCCWF                           \
         | CACHE_CTL_mskDCPMW)

/* ICache/DCache enable */
#define CACHE_CTL_CACHE_ON                              \
        (0x1UL << CACHE_CTL_offIC_EN                    \
         | 0x1UL << CACHE_CTL_offDC_EN                  \
         | 0x0UL << CACHE_CTL_offICALCK                 \
         | 0x0UL << CACHE_CTL_offDCALCK                 \
         | 0x1UL << CACHE_CTL_offDCCWF                  \
         | 0x1UL << CACHE_CTL_offDCPMW)

/*
 * Interrupt priority
 * Default: lowest priority
 */
#define PRI1_DEFAULT            0xFFFFFFFF
#define PRI2_DEFAULT            0xFFFFFFFF


/* This must be a leaf function, no child function */
void _nds32_init_mem(void) __attribute__((naked));
void _nds32_init_mem(void)
{
}

/*
 * Initialize MMU configure and cache ability.
 */
static void mmu_init(void)
{
	extern void vPortSetupMPU( unsigned int io_entry );

	/* Peripherial region : 0xE0000000 ~ 0xFFFFFFFF */
	vPortSetupMPU(7);
}

/*
 * Platform specific initialization
 */
static void plf_init(void)
{
	/* Set default Hardware interrupts priority */
	__nds32__mtsr(PRI1_DEFAULT, NDS32_SR_INT_PRI);
	__nds32__mtsr(PRI2_DEFAULT, NDS32_SR_INT_PRI2);
}

/*
 * All AE3XX system initialization
 */
void system_init(void)
{
	mmu_init();                     /* mmu/cache */
	plf_init();                     /* Perform any platform specific initializations */

#ifdef CFG_CACHE_ENABLE
	unsigned int reg;

	/* Invalid ICache */
	nds32_icache_flush();

	/* Invalid DCache */
	nds32_dcache_invalidate();

	/* Enable I/Dcache */
	reg = (__nds32__mfsr(NDS32_SR_CACHE_CTL) & ~CACHE_CTL_MSK) | CACHE_CTL_CACHE_ON;
	__nds32__mtsr(reg, NDS32_SR_CACHE_CTL);
#endif
}
