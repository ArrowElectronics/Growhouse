#ifndef __REGS_H__
#define __REGS_H__                                           1

/* CM_LV block */
#define BLOCK_CM_LV_BASE                                     (0x40004000u)
#define BLOCK_CM_LV_END                                      (0x4000403Cu)
#define BLOCK_CM_LV_SIZE                                     (BLOCK_CM_LV_END - BLOCK_CM_LV_BASE + 1)

#define CPU_CLKSEL                                           *((volatile uint32_t *)0x40004020u)
#define CPU_CLKSEL_REG                                       *((volatile uint32_t *)0x40004020u)
#define CPU_CLKSEL_ADDR                                      (0x40004020u)
#define CPU_CLKSEL_RESET                                     (0x00000000u)

/* USBSUSP_CLKSEL_FIELD field */
        #define USBSUSP_CLKSEL_FIELD                         (0x00000002u)
        #define USBSUSP_CLKSEL_FIELD_MASK                    (0x00000002u)
        #define USBSUSP_CLKSEL_FIELD_BIT                     (1)
        #define USBSUSP_CLKSEL_FIELD_BITS                    (1)

/* CPU_CLKSEL_FIELD field */
        #define CPU_CLKSEL_FIELD                             (0x00000001u)
        #define CPU_CLKSEL_FIELD_MASK                        (0x00000001u)
        #define CPU_CLKSEL_FIELD_BIT                         (0)
        #define CPU_CLKSEL_FIELD_BITS                        (1)

#define PERIPHERAL_DISABLE                                   *((volatile uint32_t *)0x40004038u)
#define PERIPHERAL_DISABLE_REG                               *((volatile uint32_t *)0x40004038u)
#define PERIPHERAL_DISABLE_ADDR                              (0x40004038u)
#define PERIPHERAL_DISABLE_RESET                             (0x00000000u)

/* PERIDIS_USB field */
        #define PERIDIS_USB                                  (0x00000100u)
        #define PERIDIS_USB_MASK                             (0x00000100u)
        #define PERIDIS_USB_BIT                              (8)
        #define PERIDIS_USB_BITS                             (1)

/* PERIPHERAL_DISABLE_RSVD7 field */
        #define PERIPHERAL_DISABLE_RSVD7                     (0x00000080u)
        #define PERIPHERAL_DISABLE_RSVD7_MASK                (0x00000080u)
        #define PERIPHERAL_DISABLE_RSVD7_BIT                 (7)
        #define PERIPHERAL_DISABLE_RSVD7_BITS                (1)

/* PERIPHERAL_DISABLE_RSVD6 field */
        #define PERIPHERAL_DISABLE_RSVD6                     (0x00000040u)
        #define PERIPHERAL_DISABLE_RSVD6_MASK                (0x00000040u)
        #define PERIPHERAL_DISABLE_RSVD6_BIT                 (6)
        #define PERIPHERAL_DISABLE_RSVD6_BITS                (1)

/* PERIDIS_RSVD field */
        #define PERIDIS_RSVD                                 (0x00000020u)
        #define PERIDIS_RSVD_MASK                            (0x00000020u)
        #define PERIDIS_RSVD_BIT                             (5)
        #define PERIDIS_RSVD_BITS                            (1)

/* PERIDIS_ADC field */
        #define PERIDIS_ADC                                  (0x00000010u)
        #define PERIDIS_ADC_MASK                             (0x00000010u)
        #define PERIDIS_ADC_BIT                              (4)
        #define PERIDIS_ADC_BITS                             (1)

/* PERIDIS_TIM2 field */
        #define PERIDIS_TIM2                                 (0x00000008u)
        #define PERIDIS_TIM2_MASK                            (0x00000008u)
        #define PERIDIS_TIM2_BIT                             (3)
        #define PERIDIS_TIM2_BITS                            (1)

/* PERIDIS_TIM1 field */
        #define PERIDIS_TIM1                                 (0x00000004u)
        #define PERIDIS_TIM1_MASK                            (0x00000004u)
        #define PERIDIS_TIM1_BIT                             (2)
        #define PERIDIS_TIM1_BITS                            (1)

/* PERIDIS_SC1 field */
        #define PERIDIS_SC1                                  (0x00000002u)
        #define PERIDIS_SC1_MASK                             (0x00000002u)
        #define PERIDIS_SC1_BIT                              (1)
        #define PERIDIS_SC1_BITS                             (1)

/* PERIDIS_SC2 field */
        #define PERIDIS_SC2                                  (0x00000001u)
        #define PERIDIS_SC2_MASK                             (0x00000001u)
        #define PERIDIS_SC2_BIT                              (0)
        #define PERIDIS_SC2_BITS                             (1)

#define RAM_RETAIN                                           *((volatile uint32_t *)0x4000403Cu)
#define RAM_RETAIN_REG                                       *((volatile uint32_t *)0x4000403Cu)
#define RAM_RETAIN_ADDR                                      (0x4000403Cu)
#define RAM_RETAIN_RESET                                     (0x0000FFFFu)

/* RETAIN field */
        #define RETAIN                                       (0x0000FFFFu)
        #define RETAIN_MASK                                  (0x0000FFFFu)
        #define RETAIN_BIT                                   (0)
        #define RETAIN_BITS                                  (16)

/* INTERRUPTS block */
#define BLOCK_INTERRUPTS_BASE                                (0x4000A000u)
#define BLOCK_INTERRUPTS_END                                 (0x4000A88Cu)
#define BLOCK_INTERRUPTS_SIZE                                (BLOCK_INTERRUPTS_END - BLOCK_INTERRUPTS_BASE + 1)

#define INT_TIM1FLAG                                         *((volatile uint32_t *)0x4000A800u)
#define INT_TIM1FLAG_REG                                     *((volatile uint32_t *)0x4000A800u)
#define INT_TIM1FLAG_ADDR                                    (0x4000A800u)
#define INT_TIM1FLAG_RESET                                   (0x00000000u)

/* INT_TIMRSVD field */
        #define INT_TIMRSVD                                  (0x00001E00u)
        #define INT_TIMRSVD_MASK                             (0x00001E00u)
        #define INT_TIMRSVD_BIT                              (9)
        #define INT_TIMRSVD_BITS                             (4)

/* INT_TIMTIF field */
        #define INT_TIMTIF                                   (0x00000040u)
        #define INT_TIMTIF_MASK                              (0x00000040u)
        #define INT_TIMTIF_BIT                               (6)
        #define INT_TIMTIF_BITS                              (1)

/* INT_TIMCC4IF field */
        #define INT_TIMCC4IF                                 (0x00000010u)
        #define INT_TIMCC4IF_MASK                            (0x00000010u)
        #define INT_TIMCC4IF_BIT                             (4)
        #define INT_TIMCC4IF_BITS                            (1)

/* INT_TIMCC3IF field */
        #define INT_TIMCC3IF                                 (0x00000008u)
        #define INT_TIMCC3IF_MASK                            (0x00000008u)
        #define INT_TIMCC3IF_BIT                             (3)
        #define INT_TIMCC3IF_BITS                            (1)

/* INT_TIMCC2IF field */
        #define INT_TIMCC2IF                                 (0x00000004u)
        #define INT_TIMCC2IF_MASK                            (0x00000004u)
        #define INT_TIMCC2IF_BIT                             (2)
        #define INT_TIMCC2IF_BITS                            (1)

/* INT_TIMCC1IF field */
        #define INT_TIMCC1IF                                 (0x00000002u)
        #define INT_TIMCC1IF_MASK                            (0x00000002u)
        #define INT_TIMCC1IF_BIT                             (1)
        #define INT_TIMCC1IF_BITS                            (1)

/* INT_TIMUIF field */
        #define INT_TIMUIF                                   (0x00000001u)
        #define INT_TIMUIF_MASK                              (0x00000001u)
        #define INT_TIMUIF_BIT                               (0)
        #define INT_TIMUIF_BITS                              (1)

#define INT_TIM2FLAG                                         *((volatile uint32_t *)0x4000A804u)
#define INT_TIM2FLAG_REG                                     *((volatile uint32_t *)0x4000A804u)
#define INT_TIM2FLAG_ADDR                                    (0x4000A804u)
#define INT_TIM2FLAG_RESET                                   (0x00000000u)

/* INT_TIMRSVD field */
        #define INT_TIMRSVD                                  (0x00001E00u)
        #define INT_TIMRSVD_MASK                             (0x00001E00u)
        #define INT_TIMRSVD_BIT                              (9)
        #define INT_TIMRSVD_BITS                             (4)

/* INT_TIMTIF field */
        #define INT_TIMTIF                                   (0x00000040u)
        #define INT_TIMTIF_MASK                              (0x00000040u)
        #define INT_TIMTIF_BIT                               (6)
        #define INT_TIMTIF_BITS                              (1)

/* INT_TIMCC4IF field */
        #define INT_TIMCC4IF                                 (0x00000010u)
        #define INT_TIMCC4IF_MASK                            (0x00000010u)
        #define INT_TIMCC4IF_BIT                             (4)
        #define INT_TIMCC4IF_BITS                            (1)

/* INT_TIMCC3IF field */
        #define INT_TIMCC3IF                                 (0x00000008u)
        #define INT_TIMCC3IF_MASK                            (0x00000008u)
        #define INT_TIMCC3IF_BIT                             (3)
        #define INT_TIMCC3IF_BITS                            (1)

/* INT_TIMCC2IF field */
        #define INT_TIMCC2IF                                 (0x00000004u)
        #define INT_TIMCC2IF_MASK                            (0x00000004u)
        #define INT_TIMCC2IF_BIT                             (2)
        #define INT_TIMCC2IF_BITS                            (1)

/* INT_TIMCC1IF field */
        #define INT_TIMCC1IF                                 (0x00000002u)
        #define INT_TIMCC1IF_MASK                            (0x00000002u)
        #define INT_TIMCC1IF_BIT                             (1)
        #define INT_TIMCC1IF_BITS                            (1)

/* INT_TIMUIF field */
        #define INT_TIMUIF                                   (0x00000001u)
        #define INT_TIMUIF_MASK                              (0x00000001u)
        #define INT_TIMUIF_BIT                               (0)
        #define INT_TIMUIF_BITS                              (1)

#define INT_SC1FLAG                                          *((volatile uint32_t *)0x4000A808u)
#define INT_SC1FLAG_REG                                      *((volatile uint32_t *)0x4000A808u)
#define INT_SC1FLAG_ADDR                                     (0x4000A808u)
#define INT_SC1FLAG_RESET                                    (0x00000000u)

/* INT_SC1PARERR field */
        #define INT_SC1PARERR                                (0x00004000u)
        #define INT_SC1PARERR_MASK                           (0x00004000u)
        #define INT_SC1PARERR_BIT                            (14)
        #define INT_SC1PARERR_BITS                           (1)

/* INT_SC1FRMERR field */
        #define INT_SC1FRMERR                                (0x00002000u)
        #define INT_SC1FRMERR_MASK                           (0x00002000u)
        #define INT_SC1FRMERR_BIT                            (13)
        #define INT_SC1FRMERR_BITS                           (1)

/* INT_SCTXULDB field */
        #define INT_SCTXULDB                                 (0x00001000u)
        #define INT_SCTXULDB_MASK                            (0x00001000u)
        #define INT_SCTXULDB_BIT                             (12)
        #define INT_SCTXULDB_BITS                            (1)

/* INT_SCTXULDA field */
        #define INT_SCTXULDA                                 (0x00000800u)
        #define INT_SCTXULDA_MASK                            (0x00000800u)
        #define INT_SCTXULDA_BIT                             (11)
        #define INT_SCTXULDA_BITS                            (1)

/* INT_SCRXULDB field */
        #define INT_SCRXULDB                                 (0x00000400u)
        #define INT_SCRXULDB_MASK                            (0x00000400u)
        #define INT_SCRXULDB_BIT                             (10)
        #define INT_SCRXULDB_BITS                            (1)

/* INT_SCRXULDA field */
        #define INT_SCRXULDA                                 (0x00000200u)
        #define INT_SCRXULDA_MASK                            (0x00000200u)
        #define INT_SCRXULDA_BIT                             (9)
        #define INT_SCRXULDA_BITS                            (1)

/* INT_SCNAK field */
        #define INT_SCNAK                                    (0x00000100u)
        #define INT_SCNAK_MASK                               (0x00000100u)
        #define INT_SCNAK_BIT                                (8)
        #define INT_SCNAK_BITS                               (1)

/* INT_SCCMDFIN field */
        #define INT_SCCMDFIN                                 (0x00000080u)
        #define INT_SCCMDFIN_MASK                            (0x00000080u)
        #define INT_SCCMDFIN_BIT                             (7)
        #define INT_SCCMDFIN_BITS                            (1)

/* INT_SCTXFIN field */
        #define INT_SCTXFIN                                  (0x00000040u)
        #define INT_SCTXFIN_MASK                             (0x00000040u)
        #define INT_SCTXFIN_BIT                              (6)
        #define INT_SCTXFIN_BITS                             (1)

/* INT_SCRXFIN field */
        #define INT_SCRXFIN                                  (0x00000020u)
        #define INT_SCRXFIN_MASK                             (0x00000020u)
        #define INT_SCRXFIN_BIT                              (5)
        #define INT_SCRXFIN_BITS                             (1)

/* INT_SCTXUND field */
        #define INT_SCTXUND                                  (0x00000010u)
        #define INT_SCTXUND_MASK                             (0x00000010u)
        #define INT_SCTXUND_BIT                              (4)
        #define INT_SCTXUND_BITS                             (1)

/* INT_SCRXOVF field */
        #define INT_SCRXOVF                                  (0x00000008u)
        #define INT_SCRXOVF_MASK                             (0x00000008u)
        #define INT_SCRXOVF_BIT                              (3)
        #define INT_SCRXOVF_BITS                             (1)

/* INT_SCTXIDLE field */
        #define INT_SCTXIDLE                                 (0x00000004u)
        #define INT_SCTXIDLE_MASK                            (0x00000004u)
        #define INT_SCTXIDLE_BIT                             (2)
        #define INT_SCTXIDLE_BITS                            (1)

/* INT_SCTXFREE field */
        #define INT_SCTXFREE                                 (0x00000002u)
        #define INT_SCTXFREE_MASK                            (0x00000002u)
        #define INT_SCTXFREE_BIT                             (1)
        #define INT_SCTXFREE_BITS                            (1)

/* INT_SCRXVAL field */
        #define INT_SCRXVAL                                  (0x00000001u)
        #define INT_SCRXVAL_MASK                             (0x00000001u)
        #define INT_SCRXVAL_BIT                              (0)
        #define INT_SCRXVAL_BITS                             (1)

#define INT_SC2FLAG                                          *((volatile uint32_t *)0x4000A80Cu)
#define INT_SC2FLAG_REG                                      *((volatile uint32_t *)0x4000A80Cu)
#define INT_SC2FLAG_ADDR                                     (0x4000A80Cu)
#define INT_SC2FLAG_RESET                                    (0x00000000u)

/* INT_SCTXULDB field */
        #define INT_SCTXULDB                                 (0x00001000u)
        #define INT_SCTXULDB_MASK                            (0x00001000u)
        #define INT_SCTXULDB_BIT                             (12)
        #define INT_SCTXULDB_BITS                            (1)

/* INT_SCTXULDA field */
        #define INT_SCTXULDA                                 (0x00000800u)
        #define INT_SCTXULDA_MASK                            (0x00000800u)
        #define INT_SCTXULDA_BIT                             (11)
        #define INT_SCTXULDA_BITS                            (1)

/* INT_SCRXULDB field */
        #define INT_SCRXULDB                                 (0x00000400u)
        #define INT_SCRXULDB_MASK                            (0x00000400u)
        #define INT_SCRXULDB_BIT                             (10)
        #define INT_SCRXULDB_BITS                            (1)

/* INT_SCRXULDA field */
        #define INT_SCRXULDA                                 (0x00000200u)
        #define INT_SCRXULDA_MASK                            (0x00000200u)
        #define INT_SCRXULDA_BIT                             (9)
        #define INT_SCRXULDA_BITS                            (1)

/* INT_SCNAK field */
        #define INT_SCNAK                                    (0x00000100u)
        #define INT_SCNAK_MASK                               (0x00000100u)
        #define INT_SCNAK_BIT                                (8)
        #define INT_SCNAK_BITS                               (1)

/* INT_SCCMDFIN field */
        #define INT_SCCMDFIN                                 (0x00000080u)
        #define INT_SCCMDFIN_MASK                            (0x00000080u)
        #define INT_SCCMDFIN_BIT                             (7)
        #define INT_SCCMDFIN_BITS                            (1)

/* INT_SCTXFIN field */
        #define INT_SCTXFIN                                  (0x00000040u)
        #define INT_SCTXFIN_MASK                             (0x00000040u)
        #define INT_SCTXFIN_BIT                              (6)
        #define INT_SCTXFIN_BITS                             (1)

/* INT_SCRXFIN field */
        #define INT_SCRXFIN                                  (0x00000020u)
        #define INT_SCRXFIN_MASK                             (0x00000020u)
        #define INT_SCRXFIN_BIT                              (5)
        #define INT_SCRXFIN_BITS                             (1)

/* INT_SCTXUND field */
        #define INT_SCTXUND                                  (0x00000010u)
        #define INT_SCTXUND_MASK                             (0x00000010u)
        #define INT_SCTXUND_BIT                              (4)
        #define INT_SCTXUND_BITS                             (1)

/* INT_SCRXOVF field */
        #define INT_SCRXOVF                                  (0x00000008u)
        #define INT_SCRXOVF_MASK                             (0x00000008u)
        #define INT_SCRXOVF_BIT                              (3)
        #define INT_SCRXOVF_BITS                             (1)

/* INT_SCTXIDLE field */
        #define INT_SCTXIDLE                                 (0x00000004u)
        #define INT_SCTXIDLE_MASK                            (0x00000004u)
        #define INT_SCTXIDLE_BIT                             (2)
        #define INT_SCTXIDLE_BITS                            (1)

/* INT_SCTXFREE field */
        #define INT_SCTXFREE                                 (0x00000002u)
        #define INT_SCTXFREE_MASK                            (0x00000002u)
        #define INT_SCTXFREE_BIT                             (1)
        #define INT_SCTXFREE_BITS                            (1)

/* INT_SCRXVAL field */
        #define INT_SCRXVAL                                  (0x00000001u)
        #define INT_SCRXVAL_MASK                             (0x00000001u)
        #define INT_SCRXVAL_BIT                              (0)
        #define INT_SCRXVAL_BITS                             (1)

#define INT_ADCFLAG                                          *((volatile uint32_t *)0x4000A810u)
#define INT_ADCFLAG_REG                                      *((volatile uint32_t *)0x4000A810u)
#define INT_ADCFLAG_ADDR                                     (0x4000A810u)
#define INT_ADCFLAG_RESET                                    (0x00000000u)

/* INT_ADCOVF field */
        #define INT_ADCOVF                                   (0x00000010u)
        #define INT_ADCOVF_MASK                              (0x00000010u)
        #define INT_ADCOVF_BIT                               (4)
        #define INT_ADCOVF_BITS                              (1)

/* INT_ADCSAT field */
        #define INT_ADCSAT                                   (0x00000008u)
        #define INT_ADCSAT_MASK                              (0x00000008u)
        #define INT_ADCSAT_BIT                               (3)
        #define INT_ADCSAT_BITS                              (1)

/* INT_ADCULDFULL field */
        #define INT_ADCULDFULL                               (0x00000004u)
        #define INT_ADCULDFULL_MASK                          (0x00000004u)
        #define INT_ADCULDFULL_BIT                           (2)
        #define INT_ADCULDFULL_BITS                          (1)

/* INT_ADCULDHALF field */
        #define INT_ADCULDHALF                               (0x00000002u)
        #define INT_ADCULDHALF_MASK                          (0x00000002u)
        #define INT_ADCULDHALF_BIT                           (1)
        #define INT_ADCULDHALF_BITS                          (1)

/* INT_ADCDATA field */
        #define INT_ADCDATA                                  (0x00000001u)
        #define INT_ADCDATA_MASK                             (0x00000001u)
        #define INT_ADCDATA_BIT                              (0)
        #define INT_ADCDATA_BITS                             (1)

#define INT_GPIOFLAG                                         *((volatile uint32_t *)0x4000A814u)
#define INT_GPIOFLAG_REG                                     *((volatile uint32_t *)0x4000A814u)
#define INT_GPIOFLAG_ADDR                                    (0x4000A814u)
#define INT_GPIOFLAG_RESET                                   (0x00000000u)

/* INT_IRQDFLAG field */
        #define INT_IRQDFLAG                                 (0x00000008u)
        #define INT_IRQDFLAG_MASK                            (0x00000008u)
        #define INT_IRQDFLAG_BIT                             (3)
        #define INT_IRQDFLAG_BITS                            (1)

/* INT_IRQCFLAG field */
        #define INT_IRQCFLAG                                 (0x00000004u)
        #define INT_IRQCFLAG_MASK                            (0x00000004u)
        #define INT_IRQCFLAG_BIT                             (2)
        #define INT_IRQCFLAG_BITS                            (1)

/* INT_IRQBFLAG field */
        #define INT_IRQBFLAG                                 (0x00000002u)
        #define INT_IRQBFLAG_MASK                            (0x00000002u)
        #define INT_IRQBFLAG_BIT                             (1)
        #define INT_IRQBFLAG_BITS                            (1)

/* INT_IRQAFLAG field */
        #define INT_IRQAFLAG                                 (0x00000001u)
        #define INT_IRQAFLAG_MASK                            (0x00000001u)
        #define INT_IRQAFLAG_BIT                             (0)
        #define INT_IRQAFLAG_BITS                            (1)

#define INT_TIM1MISS                                         *((volatile uint32_t *)0x4000A818u)
#define INT_TIM1MISS_REG                                     *((volatile uint32_t *)0x4000A818u)
#define INT_TIM1MISS_ADDR                                    (0x4000A818u)
#define INT_TIM1MISS_RESET                                   (0x00000000u)

/* INT_TIMMISSCC4IF field */
        #define INT_TIMMISSCC4IF                             (0x00001000u)
        #define INT_TIMMISSCC4IF_MASK                        (0x00001000u)
        #define INT_TIMMISSCC4IF_BIT                         (12)
        #define INT_TIMMISSCC4IF_BITS                        (1)

/* INT_TIMMISSCC3IF field */
        #define INT_TIMMISSCC3IF                             (0x00000800u)
        #define INT_TIMMISSCC3IF_MASK                        (0x00000800u)
        #define INT_TIMMISSCC3IF_BIT                         (11)
        #define INT_TIMMISSCC3IF_BITS                        (1)

/* INT_TIMMISSCC2IF field */
        #define INT_TIMMISSCC2IF                             (0x00000400u)
        #define INT_TIMMISSCC2IF_MASK                        (0x00000400u)
        #define INT_TIMMISSCC2IF_BIT                         (10)
        #define INT_TIMMISSCC2IF_BITS                        (1)

/* INT_TIMMISSCC1IF field */
        #define INT_TIMMISSCC1IF                             (0x00000200u)
        #define INT_TIMMISSCC1IF_MASK                        (0x00000200u)
        #define INT_TIMMISSCC1IF_BIT                         (9)
        #define INT_TIMMISSCC1IF_BITS                        (1)

/* INT_TIMMISSRSVD field */
        #define INT_TIMMISSRSVD                              (0x0000007Fu)
        #define INT_TIMMISSRSVD_MASK                         (0x0000007Fu)
        #define INT_TIMMISSRSVD_BIT                          (0)
        #define INT_TIMMISSRSVD_BITS                         (7)

#define INT_TIM2MISS                                         *((volatile uint32_t *)0x4000A81Cu)
#define INT_TIM2MISS_REG                                     *((volatile uint32_t *)0x4000A81Cu)
#define INT_TIM2MISS_ADDR                                    (0x4000A81Cu)
#define INT_TIM2MISS_RESET                                   (0x00000000u)

/* INT_TIMMISSCC4IF field */
        #define INT_TIMMISSCC4IF                             (0x00001000u)
        #define INT_TIMMISSCC4IF_MASK                        (0x00001000u)
        #define INT_TIMMISSCC4IF_BIT                         (12)
        #define INT_TIMMISSCC4IF_BITS                        (1)

/* INT_TIMMISSCC3IF field */
        #define INT_TIMMISSCC3IF                             (0x00000800u)
        #define INT_TIMMISSCC3IF_MASK                        (0x00000800u)
        #define INT_TIMMISSCC3IF_BIT                         (11)
        #define INT_TIMMISSCC3IF_BITS                        (1)

/* INT_TIMMISSCC2IF field */
        #define INT_TIMMISSCC2IF                             (0x00000400u)
        #define INT_TIMMISSCC2IF_MASK                        (0x00000400u)
        #define INT_TIMMISSCC2IF_BIT                         (10)
        #define INT_TIMMISSCC2IF_BITS                        (1)

/* INT_TIMMISSCC1IF field */
        #define INT_TIMMISSCC1IF                             (0x00000200u)
        #define INT_TIMMISSCC1IF_MASK                        (0x00000200u)
        #define INT_TIMMISSCC1IF_BIT                         (9)
        #define INT_TIMMISSCC1IF_BITS                        (1)

/* INT_TIMMISSRSVD field */
        #define INT_TIMMISSRSVD                              (0x0000007Fu)
        #define INT_TIMMISSRSVD_MASK                         (0x0000007Fu)
        #define INT_TIMMISSRSVD_BIT                          (0)
        #define INT_TIMMISSRSVD_BITS                         (7)

#define INT_MISS                                             *((volatile uint32_t *)0x4000A820u)
#define INT_MISS_REG                                         *((volatile uint32_t *)0x4000A820u)
#define INT_MISS_ADDR                                        (0x4000A820u)
#define INT_MISS_RESET                                       (0x00000000u)

/* INT_MISSUSB field */
        #define INT_MISSUSB                                  (0x00080000u)
        #define INT_MISSUSB_MASK                             (0x00080000u)
        #define INT_MISSUSB_BIT                              (19)
        #define INT_MISSUSB_BITS                             (1)

/* INT_MISS_RSVD18 field */
        #define INT_MISS_RSVD18                              (0x00040000u)
        #define INT_MISS_RSVD18_MASK                         (0x00040000u)
        #define INT_MISS_RSVD18_BIT                          (18)
        #define INT_MISS_RSVD18_BITS                         (1)

/* INT_MISS_RSVD17 field */
        #define INT_MISS_RSVD17                              (0x00020000u)
        #define INT_MISS_RSVD17_MASK                         (0x00020000u)
        #define INT_MISS_RSVD17_BIT                          (17)
        #define INT_MISS_RSVD17_BITS                         (1)

/* INT_MISSIRQD field */
        #define INT_MISSIRQD                                 (0x00008000u)
        #define INT_MISSIRQD_MASK                            (0x00008000u)
        #define INT_MISSIRQD_BIT                             (15)
        #define INT_MISSIRQD_BITS                            (1)

/* INT_MISSIRQC field */
        #define INT_MISSIRQC                                 (0x00004000u)
        #define INT_MISSIRQC_MASK                            (0x00004000u)
        #define INT_MISSIRQC_BIT                             (14)
        #define INT_MISSIRQC_BITS                            (1)

/* INT_MISSIRQB field */
        #define INT_MISSIRQB                                 (0x00002000u)
        #define INT_MISSIRQB_MASK                            (0x00002000u)
        #define INT_MISSIRQB_BIT                             (13)
        #define INT_MISSIRQB_BITS                            (1)

/* INT_MISSIRQA field */
        #define INT_MISSIRQA                                 (0x00001000u)
        #define INT_MISSIRQA_MASK                            (0x00001000u)
        #define INT_MISSIRQA_BIT                             (12)
        #define INT_MISSIRQA_BITS                            (1)

/* INT_MISSADC field */
        #define INT_MISSADC                                  (0x00000800u)
        #define INT_MISSADC_MASK                             (0x00000800u)
        #define INT_MISSADC_BIT                              (11)
        #define INT_MISSADC_BITS                             (1)

/* INT_MISSMACRX field */
        #define INT_MISSMACRX                                (0x00000400u)
        #define INT_MISSMACRX_MASK                           (0x00000400u)
        #define INT_MISSMACRX_BIT                            (10)
        #define INT_MISSMACRX_BITS                           (1)

/* INT_MISSMACTX field */
        #define INT_MISSMACTX                                (0x00000200u)
        #define INT_MISSMACTX_MASK                           (0x00000200u)
        #define INT_MISSMACTX_BIT                            (9)
        #define INT_MISSMACTX_BITS                           (1)

/* INT_MISSMACTMR field */
        #define INT_MISSMACTMR                               (0x00000100u)
        #define INT_MISSMACTMR_MASK                          (0x00000100u)
        #define INT_MISSMACTMR_BIT                           (8)
        #define INT_MISSMACTMR_BITS                          (1)

/* INT_MISSSEC field */
        #define INT_MISSSEC                                  (0x00000080u)
        #define INT_MISSSEC_MASK                             (0x00000080u)
        #define INT_MISSSEC_BIT                              (7)
        #define INT_MISSSEC_BITS                             (1)

/* INT_MISSSC2 field */
        #define INT_MISSSC2                                  (0x00000040u)
        #define INT_MISSSC2_MASK                             (0x00000040u)
        #define INT_MISSSC2_BIT                              (6)
        #define INT_MISSSC2_BITS                             (1)

/* INT_MISSSC1 field */
        #define INT_MISSSC1                                  (0x00000020u)
        #define INT_MISSSC1_MASK                             (0x00000020u)
        #define INT_MISSSC1_BIT                              (5)
        #define INT_MISSSC1_BITS                             (1)

/* INT_MISSSLEEP field */
        #define INT_MISSSLEEP                                (0x00000010u)
        #define INT_MISSSLEEP_MASK                           (0x00000010u)
        #define INT_MISSSLEEP_BIT                            (4)
        #define INT_MISSSLEEP_BITS                           (1)

/* INT_MISSBB field */
        #define INT_MISSBB                                   (0x00000008u)
        #define INT_MISSBB_MASK                              (0x00000008u)
        #define INT_MISSBB_BIT                               (3)
        #define INT_MISSBB_BITS                              (1)

/* INT_MISSMGMT field */
        #define INT_MISSMGMT                                 (0x00000004u)
        #define INT_MISSMGMT_MASK                            (0x00000004u)
        #define INT_MISSMGMT_BIT                             (2)
        #define INT_MISSMGMT_BITS                            (1)

#define INT_TIM1CFG                                          *((volatile uint32_t *)0x4000A840u)
#define INT_TIM1CFG_REG                                      *((volatile uint32_t *)0x4000A840u)
#define INT_TIM1CFG_ADDR                                     (0x4000A840u)
#define INT_TIM1CFG_RESET                                    (0x00000000u)

/* INT_TIMTIF field */
        #define INT_TIMTIF                                   (0x00000040u)
        #define INT_TIMTIF_MASK                              (0x00000040u)
        #define INT_TIMTIF_BIT                               (6)
        #define INT_TIMTIF_BITS                              (1)

/* INT_TIMCC4IF field */
        #define INT_TIMCC4IF                                 (0x00000010u)
        #define INT_TIMCC4IF_MASK                            (0x00000010u)
        #define INT_TIMCC4IF_BIT                             (4)
        #define INT_TIMCC4IF_BITS                            (1)

/* INT_TIMCC3IF field */
        #define INT_TIMCC3IF                                 (0x00000008u)
        #define INT_TIMCC3IF_MASK                            (0x00000008u)
        #define INT_TIMCC3IF_BIT                             (3)
        #define INT_TIMCC3IF_BITS                            (1)

/* INT_TIMCC2IF field */
        #define INT_TIMCC2IF                                 (0x00000004u)
        #define INT_TIMCC2IF_MASK                            (0x00000004u)
        #define INT_TIMCC2IF_BIT                             (2)
        #define INT_TIMCC2IF_BITS                            (1)

/* INT_TIMCC1IF field */
        #define INT_TIMCC1IF                                 (0x00000002u)
        #define INT_TIMCC1IF_MASK                            (0x00000002u)
        #define INT_TIMCC1IF_BIT                             (1)
        #define INT_TIMCC1IF_BITS                            (1)

/* INT_TIMUIF field */
        #define INT_TIMUIF                                   (0x00000001u)
        #define INT_TIMUIF_MASK                              (0x00000001u)
        #define INT_TIMUIF_BIT                               (0)
        #define INT_TIMUIF_BITS                              (1)

#define INT_TIM2CFG                                          *((volatile uint32_t *)0x4000A844u)
#define INT_TIM2CFG_REG                                      *((volatile uint32_t *)0x4000A844u)
#define INT_TIM2CFG_ADDR                                     (0x4000A844u)
#define INT_TIM2CFG_RESET                                    (0x00000000u)

/* INT_TIMTIF field */
        #define INT_TIMTIF                                   (0x00000040u)
        #define INT_TIMTIF_MASK                              (0x00000040u)
        #define INT_TIMTIF_BIT                               (6)
        #define INT_TIMTIF_BITS                              (1)

/* INT_TIMCC4IF field */
        #define INT_TIMCC4IF                                 (0x00000010u)
        #define INT_TIMCC4IF_MASK                            (0x00000010u)
        #define INT_TIMCC4IF_BIT                             (4)
        #define INT_TIMCC4IF_BITS                            (1)

/* INT_TIMCC3IF field */
        #define INT_TIMCC3IF                                 (0x00000008u)
        #define INT_TIMCC3IF_MASK                            (0x00000008u)
        #define INT_TIMCC3IF_BIT                             (3)
        #define INT_TIMCC3IF_BITS                            (1)

/* INT_TIMCC2IF field */
        #define INT_TIMCC2IF                                 (0x00000004u)
        #define INT_TIMCC2IF_MASK                            (0x00000004u)
        #define INT_TIMCC2IF_BIT                             (2)
        #define INT_TIMCC2IF_BITS                            (1)

/* INT_TIMCC1IF field */
        #define INT_TIMCC1IF                                 (0x00000002u)
        #define INT_TIMCC1IF_MASK                            (0x00000002u)
        #define INT_TIMCC1IF_BIT                             (1)
        #define INT_TIMCC1IF_BITS                            (1)

/* INT_TIMUIF field */
        #define INT_TIMUIF                                   (0x00000001u)
        #define INT_TIMUIF_MASK                              (0x00000001u)
        #define INT_TIMUIF_BIT                               (0)
        #define INT_TIMUIF_BITS                              (1)

#define INT_SC1CFG                                           *((volatile uint32_t *)0x4000A848u)
#define INT_SC1CFG_REG                                       *((volatile uint32_t *)0x4000A848u)
#define INT_SC1CFG_ADDR                                      (0x4000A848u)
#define INT_SC1CFG_RESET                                     (0x00000000u)

/* INT_SC1PARERR field */
        #define INT_SC1PARERR                                (0x00004000u)
        #define INT_SC1PARERR_MASK                           (0x00004000u)
        #define INT_SC1PARERR_BIT                            (14)
        #define INT_SC1PARERR_BITS                           (1)

/* INT_SC1FRMERR field */
        #define INT_SC1FRMERR                                (0x00002000u)
        #define INT_SC1FRMERR_MASK                           (0x00002000u)
        #define INT_SC1FRMERR_BIT                            (13)
        #define INT_SC1FRMERR_BITS                           (1)

/* INT_SCTXULDB field */
        #define INT_SCTXULDB                                 (0x00001000u)
        #define INT_SCTXULDB_MASK                            (0x00001000u)
        #define INT_SCTXULDB_BIT                             (12)
        #define INT_SCTXULDB_BITS                            (1)

/* INT_SCTXULDA field */
        #define INT_SCTXULDA                                 (0x00000800u)
        #define INT_SCTXULDA_MASK                            (0x00000800u)
        #define INT_SCTXULDA_BIT                             (11)
        #define INT_SCTXULDA_BITS                            (1)

/* INT_SCRXULDB field */
        #define INT_SCRXULDB                                 (0x00000400u)
        #define INT_SCRXULDB_MASK                            (0x00000400u)
        #define INT_SCRXULDB_BIT                             (10)
        #define INT_SCRXULDB_BITS                            (1)

/* INT_SCRXULDA field */
        #define INT_SCRXULDA                                 (0x00000200u)
        #define INT_SCRXULDA_MASK                            (0x00000200u)
        #define INT_SCRXULDA_BIT                             (9)
        #define INT_SCRXULDA_BITS                            (1)

/* INT_SCNAK field */
        #define INT_SCNAK                                    (0x00000100u)
        #define INT_SCNAK_MASK                               (0x00000100u)
        #define INT_SCNAK_BIT                                (8)
        #define INT_SCNAK_BITS                               (1)

/* INT_SCCMDFIN field */
        #define INT_SCCMDFIN                                 (0x00000080u)
        #define INT_SCCMDFIN_MASK                            (0x00000080u)
        #define INT_SCCMDFIN_BIT                             (7)
        #define INT_SCCMDFIN_BITS                            (1)

/* INT_SCTXFIN field */
        #define INT_SCTXFIN                                  (0x00000040u)
        #define INT_SCTXFIN_MASK                             (0x00000040u)
        #define INT_SCTXFIN_BIT                              (6)
        #define INT_SCTXFIN_BITS                             (1)

/* INT_SCRXFIN field */
        #define INT_SCRXFIN                                  (0x00000020u)
        #define INT_SCRXFIN_MASK                             (0x00000020u)
        #define INT_SCRXFIN_BIT                              (5)
        #define INT_SCRXFIN_BITS                             (1)

/* INT_SCTXUND field */
        #define INT_SCTXUND                                  (0x00000010u)
        #define INT_SCTXUND_MASK                             (0x00000010u)
        #define INT_SCTXUND_BIT                              (4)
        #define INT_SCTXUND_BITS                             (1)

/* INT_SCRXOVF field */
        #define INT_SCRXOVF                                  (0x00000008u)
        #define INT_SCRXOVF_MASK                             (0x00000008u)
        #define INT_SCRXOVF_BIT                              (3)
        #define INT_SCRXOVF_BITS                             (1)

/* INT_SCTXIDLE field */
        #define INT_SCTXIDLE                                 (0x00000004u)
        #define INT_SCTXIDLE_MASK                            (0x00000004u)
        #define INT_SCTXIDLE_BIT                             (2)
        #define INT_SCTXIDLE_BITS                            (1)

/* INT_SCTXFREE field */
        #define INT_SCTXFREE                                 (0x00000002u)
        #define INT_SCTXFREE_MASK                            (0x00000002u)
        #define INT_SCTXFREE_BIT                             (1)
        #define INT_SCTXFREE_BITS                            (1)

/* INT_SCRXVAL field */
        #define INT_SCRXVAL                                  (0x00000001u)
        #define INT_SCRXVAL_MASK                             (0x00000001u)
        #define INT_SCRXVAL_BIT                              (0)
        #define INT_SCRXVAL_BITS                             (1)

#define INT_SC2CFG                                           *((volatile uint32_t *)0x4000A84Cu)
#define INT_SC2CFG_REG                                       *((volatile uint32_t *)0x4000A84Cu)
#define INT_SC2CFG_ADDR                                      (0x4000A84Cu)
#define INT_SC2CFG_RESET                                     (0x00000000u)

/* INT_SCTXULDB field */
        #define INT_SCTXULDB                                 (0x00001000u)
        #define INT_SCTXULDB_MASK                            (0x00001000u)
        #define INT_SCTXULDB_BIT                             (12)
        #define INT_SCTXULDB_BITS                            (1)

/* INT_SCTXULDA field */
        #define INT_SCTXULDA                                 (0x00000800u)
        #define INT_SCTXULDA_MASK                            (0x00000800u)
        #define INT_SCTXULDA_BIT                             (11)
        #define INT_SCTXULDA_BITS                            (1)

/* INT_SCRXULDB field */
        #define INT_SCRXULDB                                 (0x00000400u)
        #define INT_SCRXULDB_MASK                            (0x00000400u)
        #define INT_SCRXULDB_BIT                             (10)
        #define INT_SCRXULDB_BITS                            (1)

/* INT_SCRXULDA field */
        #define INT_SCRXULDA                                 (0x00000200u)
        #define INT_SCRXULDA_MASK                            (0x00000200u)
        #define INT_SCRXULDA_BIT                             (9)
        #define INT_SCRXULDA_BITS                            (1)

/* INT_SCNAK field */
        #define INT_SCNAK                                    (0x00000100u)
        #define INT_SCNAK_MASK                               (0x00000100u)
        #define INT_SCNAK_BIT                                (8)
        #define INT_SCNAK_BITS                               (1)

/* INT_SCCMDFIN field */
        #define INT_SCCMDFIN                                 (0x00000080u)
        #define INT_SCCMDFIN_MASK                            (0x00000080u)
        #define INT_SCCMDFIN_BIT                             (7)
        #define INT_SCCMDFIN_BITS                            (1)

/* INT_SCTXFIN field */
        #define INT_SCTXFIN                                  (0x00000040u)
        #define INT_SCTXFIN_MASK                             (0x00000040u)
        #define INT_SCTXFIN_BIT                              (6)
        #define INT_SCTXFIN_BITS                             (1)

/* INT_SCRXFIN field */
        #define INT_SCRXFIN                                  (0x00000020u)
        #define INT_SCRXFIN_MASK                             (0x00000020u)
        #define INT_SCRXFIN_BIT                              (5)
        #define INT_SCRXFIN_BITS                             (1)

/* INT_SCTXUND field */
        #define INT_SCTXUND                                  (0x00000010u)
        #define INT_SCTXUND_MASK                             (0x00000010u)
        #define INT_SCTXUND_BIT                              (4)
        #define INT_SCTXUND_BITS                             (1)

/* INT_SCRXOVF field */
        #define INT_SCRXOVF                                  (0x00000008u)
        #define INT_SCRXOVF_MASK                             (0x00000008u)
        #define INT_SCRXOVF_BIT                              (3)
        #define INT_SCRXOVF_BITS                             (1)

/* INT_SCTXIDLE field */
        #define INT_SCTXIDLE                                 (0x00000004u)
        #define INT_SCTXIDLE_MASK                            (0x00000004u)
        #define INT_SCTXIDLE_BIT                             (2)
        #define INT_SCTXIDLE_BITS                            (1)

/* INT_SCTXFREE field */
        #define INT_SCTXFREE                                 (0x00000002u)
        #define INT_SCTXFREE_MASK                            (0x00000002u)
        #define INT_SCTXFREE_BIT                             (1)
        #define INT_SCTXFREE_BITS                            (1)

/* INT_SCRXVAL field */
        #define INT_SCRXVAL                                  (0x00000001u)
        #define INT_SCRXVAL_MASK                             (0x00000001u)
        #define INT_SCRXVAL_BIT                              (0)
        #define INT_SCRXVAL_BITS                             (1)

#define INT_ADCCFG                                           *((volatile uint32_t *)0x4000A850u)
#define INT_ADCCFG_REG                                       *((volatile uint32_t *)0x4000A850u)
#define INT_ADCCFG_ADDR                                      (0x4000A850u)
#define INT_ADCCFG_RESET                                     (0x00000000u)

/* INT_ADCOVF field */
        #define INT_ADCOVF                                   (0x00000010u)
        #define INT_ADCOVF_MASK                              (0x00000010u)
        #define INT_ADCOVF_BIT                               (4)
        #define INT_ADCOVF_BITS                              (1)

/* INT_ADCSAT field */
        #define INT_ADCSAT                                   (0x00000008u)
        #define INT_ADCSAT_MASK                              (0x00000008u)
        #define INT_ADCSAT_BIT                               (3)
        #define INT_ADCSAT_BITS                              (1)

/* INT_ADCULDFULL field */
        #define INT_ADCULDFULL                               (0x00000004u)
        #define INT_ADCULDFULL_MASK                          (0x00000004u)
        #define INT_ADCULDFULL_BIT                           (2)
        #define INT_ADCULDFULL_BITS                          (1)

/* INT_ADCULDHALF field */
        #define INT_ADCULDHALF                               (0x00000002u)
        #define INT_ADCULDHALF_MASK                          (0x00000002u)
        #define INT_ADCULDHALF_BIT                           (1)
        #define INT_ADCULDHALF_BITS                          (1)

/* INT_ADCDATA field */
        #define INT_ADCDATA                                  (0x00000001u)
        #define INT_ADCDATA_MASK                             (0x00000001u)
        #define INT_ADCDATA_BIT                              (0)
        #define INT_ADCDATA_BITS                             (1)

#define SC1_INTMODE                                          *((volatile uint32_t *)0x4000A854u)
#define SC1_INTMODE_REG                                      *((volatile uint32_t *)0x4000A854u)
#define SC1_INTMODE_ADDR                                     (0x4000A854u)
#define SC1_INTMODE_RESET                                    (0x00000000u)

/* SC_TXIDLELEVEL field */
        #define SC_TXIDLELEVEL                               (0x00000004u)
        #define SC_TXIDLELEVEL_MASK                          (0x00000004u)
        #define SC_TXIDLELEVEL_BIT                           (2)
        #define SC_TXIDLELEVEL_BITS                          (1)

/* SC_TXFREELEVEL field */
        #define SC_TXFREELEVEL                               (0x00000002u)
        #define SC_TXFREELEVEL_MASK                          (0x00000002u)
        #define SC_TXFREELEVEL_BIT                           (1)
        #define SC_TXFREELEVEL_BITS                          (1)

/* SC_RXVALLEVEL field */
        #define SC_RXVALLEVEL                                (0x00000001u)
        #define SC_RXVALLEVEL_MASK                           (0x00000001u)
        #define SC_RXVALLEVEL_BIT                            (0)
        #define SC_RXVALLEVEL_BITS                           (1)

#define SC2_INTMODE                                          *((volatile uint32_t *)0x4000A858u)
#define SC2_INTMODE_REG                                      *((volatile uint32_t *)0x4000A858u)
#define SC2_INTMODE_ADDR                                     (0x4000A858u)
#define SC2_INTMODE_RESET                                    (0x00000000u)

/* SC_TXIDLELEVEL field */
        #define SC_TXIDLELEVEL                               (0x00000004u)
        #define SC_TXIDLELEVEL_MASK                          (0x00000004u)
        #define SC_TXIDLELEVEL_BIT                           (2)
        #define SC_TXIDLELEVEL_BITS                          (1)

/* SC_TXFREELEVEL field */
        #define SC_TXFREELEVEL                               (0x00000002u)
        #define SC_TXFREELEVEL_MASK                          (0x00000002u)
        #define SC_TXFREELEVEL_BIT                           (1)
        #define SC_TXFREELEVEL_BITS                          (1)

/* SC_RXVALLEVEL field */
        #define SC_RXVALLEVEL                                (0x00000001u)
        #define SC_RXVALLEVEL_MASK                           (0x00000001u)
        #define SC_RXVALLEVEL_BIT                            (0)
        #define SC_RXVALLEVEL_BITS                           (1)

#define GPIO_INTCFGA                                         *((volatile uint32_t *)0x4000A860u)
#define GPIO_INTCFGA_REG                                     *((volatile uint32_t *)0x4000A860u)
#define GPIO_INTCFGA_ADDR                                    (0x4000A860u)
#define GPIO_INTCFGA_RESET                                   (0x00000000u)

/* GPIO_INTFILT field */
        #define GPIO_INTFILT                                 (0x00000100u)
        #define GPIO_INTFILT_MASK                            (0x00000100u)
        #define GPIO_INTFILT_BIT                             (8)
        #define GPIO_INTFILT_BITS                            (1)

/* GPIO_INTMOD field */
        #define GPIO_INTMOD                                  (0x000000E0u)
        #define GPIO_INTMOD_MASK                             (0x000000E0u)
        #define GPIO_INTMOD_BIT                              (5)
        #define GPIO_INTMOD_BITS                             (3)
                #define GPIOINTMOD_DISABLED                  (0)
                #define GPIOINTMOD_RISING_EDGE               (1)
                #define GPIOINTMOD_FALLING_EDGE              (2)
                #define GPIOINTMOD_BOTH_EDGES                (3)
                #define GPIOINTMOD_HIGH_LEVEL                (4)
                #define GPIOINTMOD_LOW_LEVEL                 (5)

#define GPIO_INTCFGB                                         *((volatile uint32_t *)0x4000A864u)
#define GPIO_INTCFGB_REG                                     *((volatile uint32_t *)0x4000A864u)
#define GPIO_INTCFGB_ADDR                                    (0x4000A864u)
#define GPIO_INTCFGB_RESET                                   (0x00000000u)

/* GPIO_INTFILT field */
        #define GPIO_INTFILT                                 (0x00000100u)
        #define GPIO_INTFILT_MASK                            (0x00000100u)
        #define GPIO_INTFILT_BIT                             (8)
        #define GPIO_INTFILT_BITS                            (1)

/* GPIO_INTMOD field */
        #define GPIO_INTMOD                                  (0x000000E0u)
        #define GPIO_INTMOD_MASK                             (0x000000E0u)
        #define GPIO_INTMOD_BIT                              (5)
        #define GPIO_INTMOD_BITS                             (3)

#define GPIO_INTCFGC                                         *((volatile uint32_t *)0x4000A868u)
#define GPIO_INTCFGC_REG                                     *((volatile uint32_t *)0x4000A868u)
#define GPIO_INTCFGC_ADDR                                    (0x4000A868u)
#define GPIO_INTCFGC_RESET                                   (0x00000000u)

/* GPIO_INTFILT field */
        #define GPIO_INTFILT                                 (0x00000100u)
        #define GPIO_INTFILT_MASK                            (0x00000100u)
        #define GPIO_INTFILT_BIT                             (8)
        #define GPIO_INTFILT_BITS                            (1)

/* GPIO_INTMOD field */
        #define GPIO_INTMOD                                  (0x000000E0u)
        #define GPIO_INTMOD_MASK                             (0x000000E0u)
        #define GPIO_INTMOD_BIT                              (5)
        #define GPIO_INTMOD_BITS                             (3)

#define GPIO_INTCFGD                                         *((volatile uint32_t *)0x4000A86Cu)
#define GPIO_INTCFGD_REG                                     *((volatile uint32_t *)0x4000A86Cu)
#define GPIO_INTCFGD_ADDR                                    (0x4000A86Cu)
#define GPIO_INTCFGD_RESET                                   (0x00000000u)

/* GPIO_INTFILT field */
        #define GPIO_INTFILT                                 (0x00000100u)
        #define GPIO_INTFILT_MASK                            (0x00000100u)
        #define GPIO_INTFILT_BIT                             (8)
        #define GPIO_INTFILT_BITS                            (1)

/* GPIO_INTMOD field */
        #define GPIO_INTMOD                                  (0x000000E0u)
        #define GPIO_INTMOD_MASK                             (0x000000E0u)
        #define GPIO_INTMOD_BIT                              (5)
        #define GPIO_INTMOD_BITS                             (3)

#define INT_USBFLAG                                          *((volatile uint32_t *)0x4000A888u)
#define INT_USBFLAG_REG                                      *((volatile uint32_t *)0x4000A888u)
#define INT_USBFLAG_ADDR                                     (0x4000A888u)
#define INT_USBFLAG_RESET                                    (0x00000000u)

/* INT_USBWAKEUP field */
        #define INT_USBWAKEUP                                (0x00800000u)
        #define INT_USBWAKEUP_MASK                           (0x00800000u)
        #define INT_USBWAKEUP_BIT                            (23)
        #define INT_USBWAKEUP_BITS                           (1)

/* INT_USBRESUME field */
        #define INT_USBRESUME                                (0x00400000u)
        #define INT_USBRESUME_MASK                           (0x00400000u)
        #define INT_USBRESUME_BIT                            (22)
        #define INT_USBRESUME_BITS                           (1)

/* INT_USBSUSPEND field */
        #define INT_USBSUSPEND                               (0x00200000u)
        #define INT_USBSUSPEND_MASK                          (0x00200000u)
        #define INT_USBSUSPEND_BIT                           (21)
        #define INT_USBSUSPEND_BITS                          (1)

/* INT_USBRESET field */
        #define INT_USBRESET                                 (0x00100000u)
        #define INT_USBRESET_MASK                            (0x00100000u)
        #define INT_USBRESET_BIT                             (20)
        #define INT_USBRESET_BITS                            (1)

/* INT_USBSOF field */
        #define INT_USBSOF                                   (0x00080000u)
        #define INT_USBSOF_MASK                              (0x00080000u)
        #define INT_USBSOF_BIT                               (19)
        #define INT_USBSOF_BITS                              (1)

/* INT_USBNAK field */
        #define INT_USBNAK                                   (0x00040000u)
        #define INT_USBNAK_MASK                              (0x00040000u)
        #define INT_USBNAK_BIT                               (18)
        #define INT_USBNAK_BITS                              (1)

/* INT_USBPIPERXOVF field */
        #define INT_USBPIPERXOVF                             (0x00020000u)
        #define INT_USBPIPERXOVF_MASK                        (0x00020000u)
        #define INT_USBPIPERXOVF_BIT                         (17)
        #define INT_USBPIPERXOVF_BITS                        (1)

/* INT_USBPIPETXUND field */
        #define INT_USBPIPETXUND                             (0x00010000u)
        #define INT_USBPIPETXUND_MASK                        (0x00010000u)
        #define INT_USBPIPETXUND_BIT                         (16)
        #define INT_USBPIPETXUND_BITS                        (1)

/* INT_USBBUFRXOVF field */
        #define INT_USBBUFRXOVF                              (0x00008000u)
        #define INT_USBBUFRXOVF_MASK                         (0x00008000u)
        #define INT_USBBUFRXOVF_BIT                          (15)
        #define INT_USBBUFRXOVF_BITS                         (1)

/* INT_USBBUFTXUND field */
        #define INT_USBBUFTXUND                              (0x00004000u)
        #define INT_USBBUFTXUND_MASK                         (0x00004000u)
        #define INT_USBBUFTXUND_BIT                          (14)
        #define INT_USBBUFTXUND_BITS                         (1)

/* INT_USBRXVALIDEP6 field */
        #define INT_USBRXVALIDEP6                            (0x00002000u)
        #define INT_USBRXVALIDEP6_MASK                       (0x00002000u)
        #define INT_USBRXVALIDEP6_BIT                        (13)
        #define INT_USBRXVALIDEP6_BITS                       (1)

/* INT_USBRXVALIDEP5 field */
        #define INT_USBRXVALIDEP5                            (0x00001000u)
        #define INT_USBRXVALIDEP5_MASK                       (0x00001000u)
        #define INT_USBRXVALIDEP5_BIT                        (12)
        #define INT_USBRXVALIDEP5_BITS                       (1)

/* INT_USBRXVALIDEP4 field */
        #define INT_USBRXVALIDEP4                            (0x00000800u)
        #define INT_USBRXVALIDEP4_MASK                       (0x00000800u)
        #define INT_USBRXVALIDEP4_BIT                        (11)
        #define INT_USBRXVALIDEP4_BITS                       (1)

/* INT_USBRXVALIDEP3 field */
        #define INT_USBRXVALIDEP3                            (0x00000400u)
        #define INT_USBRXVALIDEP3_MASK                       (0x00000400u)
        #define INT_USBRXVALIDEP3_BIT                        (10)
        #define INT_USBRXVALIDEP3_BITS                       (1)

/* INT_USBRXVALIDEP2 field */
        #define INT_USBRXVALIDEP2                            (0x00000200u)
        #define INT_USBRXVALIDEP2_MASK                       (0x00000200u)
        #define INT_USBRXVALIDEP2_BIT                        (9)
        #define INT_USBRXVALIDEP2_BITS                       (1)

/* INT_USBRXVALIDEP1 field */
        #define INT_USBRXVALIDEP1                            (0x00000100u)
        #define INT_USBRXVALIDEP1_MASK                       (0x00000100u)
        #define INT_USBRXVALIDEP1_BIT                        (8)
        #define INT_USBRXVALIDEP1_BITS                       (1)

/* INT_USBRXVALIDEP0 field */
        #define INT_USBRXVALIDEP0                            (0x00000080u)
        #define INT_USBRXVALIDEP0_MASK                       (0x00000080u)
        #define INT_USBRXVALIDEP0_BIT                        (7)
        #define INT_USBRXVALIDEP0_BITS                       (1)

/* INT_USBTXACTIVEEP6 field */
        #define INT_USBTXACTIVEEP6                           (0x00000040u)
        #define INT_USBTXACTIVEEP6_MASK                      (0x00000040u)
        #define INT_USBTXACTIVEEP6_BIT                       (6)
        #define INT_USBTXACTIVEEP6_BITS                      (1)

/* INT_USBTXACTIVEEP5 field */
        #define INT_USBTXACTIVEEP5                           (0x00000020u)
        #define INT_USBTXACTIVEEP5_MASK                      (0x00000020u)
        #define INT_USBTXACTIVEEP5_BIT                       (5)
        #define INT_USBTXACTIVEEP5_BITS                      (1)

/* INT_USBTXACTIVEEP4 field */
        #define INT_USBTXACTIVEEP4                           (0x00000010u)
        #define INT_USBTXACTIVEEP4_MASK                      (0x00000010u)
        #define INT_USBTXACTIVEEP4_BIT                       (4)
        #define INT_USBTXACTIVEEP4_BITS                      (1)

/* INT_USBTXACTIVEEP3 field */
        #define INT_USBTXACTIVEEP3                           (0x00000008u)
        #define INT_USBTXACTIVEEP3_MASK                      (0x00000008u)
        #define INT_USBTXACTIVEEP3_BIT                       (3)
        #define INT_USBTXACTIVEEP3_BITS                      (1)

/* INT_USBTXACTIVEEP2 field */
        #define INT_USBTXACTIVEEP2                           (0x00000004u)
        #define INT_USBTXACTIVEEP2_MASK                      (0x00000004u)
        #define INT_USBTXACTIVEEP2_BIT                       (2)
        #define INT_USBTXACTIVEEP2_BITS                      (1)

/* INT_USBTXACTIVEEP1 field */
        #define INT_USBTXACTIVEEP1                           (0x00000002u)
        #define INT_USBTXACTIVEEP1_MASK                      (0x00000002u)
        #define INT_USBTXACTIVEEP1_BIT                       (1)
        #define INT_USBTXACTIVEEP1_BITS                      (1)

/* INT_USBTXACTIVEEP0 field */
        #define INT_USBTXACTIVEEP0                           (0x00000001u)
        #define INT_USBTXACTIVEEP0_MASK                      (0x00000001u)
        #define INT_USBTXACTIVEEP0_BIT                       (0)
        #define INT_USBTXACTIVEEP0_BITS                      (1)

#define INT_USBCFG                                           *((volatile uint32_t *)0x4000A88Cu)
#define INT_USBCFG_REG                                       *((volatile uint32_t *)0x4000A88Cu)
#define INT_USBCFG_ADDR                                      (0x4000A88Cu)
#define INT_USBCFG_RESET                                     (0x00000000u)

/* INT_USBWAKEUP field */
        #define INT_USBWAKEUP                                (0x00800000u)
        #define INT_USBWAKEUP_MASK                           (0x00800000u)
        #define INT_USBWAKEUP_BIT                            (23)
        #define INT_USBWAKEUP_BITS                           (1)

/* INT_USBRESUME field */
        #define INT_USBRESUME                                (0x00400000u)
        #define INT_USBRESUME_MASK                           (0x00400000u)
        #define INT_USBRESUME_BIT                            (22)
        #define INT_USBRESUME_BITS                           (1)

/* INT_USBSUSPEND field */
        #define INT_USBSUSPEND                               (0x00200000u)
        #define INT_USBSUSPEND_MASK                          (0x00200000u)
        #define INT_USBSUSPEND_BIT                           (21)
        #define INT_USBSUSPEND_BITS                          (1)

/* INT_USBRESET field */
        #define INT_USBRESET                                 (0x00100000u)
        #define INT_USBRESET_MASK                            (0x00100000u)
        #define INT_USBRESET_BIT                             (20)
        #define INT_USBRESET_BITS                            (1)

/* INT_USBSOF field */
        #define INT_USBSOF                                   (0x00080000u)
        #define INT_USBSOF_MASK                              (0x00080000u)
        #define INT_USBSOF_BIT                               (19)
        #define INT_USBSOF_BITS                              (1)

/* INT_USBNAK field */
        #define INT_USBNAK                                   (0x00040000u)
        #define INT_USBNAK_MASK                              (0x00040000u)
        #define INT_USBNAK_BIT                               (18)
        #define INT_USBNAK_BITS                              (1)

/* INT_USBPIPERXOVF field */
        #define INT_USBPIPERXOVF                             (0x00020000u)
        #define INT_USBPIPERXOVF_MASK                        (0x00020000u)
        #define INT_USBPIPERXOVF_BIT                         (17)
        #define INT_USBPIPERXOVF_BITS                        (1)

/* INT_USBPIPETXUND field */
        #define INT_USBPIPETXUND                             (0x00010000u)
        #define INT_USBPIPETXUND_MASK                        (0x00010000u)
        #define INT_USBPIPETXUND_BIT                         (16)
        #define INT_USBPIPETXUND_BITS                        (1)

/* INT_USBBUFRXOVF field */
        #define INT_USBBUFRXOVF                              (0x00008000u)
        #define INT_USBBUFRXOVF_MASK                         (0x00008000u)
        #define INT_USBBUFRXOVF_BIT                          (15)
        #define INT_USBBUFRXOVF_BITS                         (1)

/* INT_USBBUFTXUND field */
        #define INT_USBBUFTXUND                              (0x00004000u)
        #define INT_USBBUFTXUND_MASK                         (0x00004000u)
        #define INT_USBBUFTXUND_BIT                          (14)
        #define INT_USBBUFTXUND_BITS                         (1)

/* INT_USBRXVALIDEP6 field */
        #define INT_USBRXVALIDEP6                            (0x00002000u)
        #define INT_USBRXVALIDEP6_MASK                       (0x00002000u)
        #define INT_USBRXVALIDEP6_BIT                        (13)
        #define INT_USBRXVALIDEP6_BITS                       (1)

/* INT_USBRXVALIDEP5 field */
        #define INT_USBRXVALIDEP5                            (0x00001000u)
        #define INT_USBRXVALIDEP5_MASK                       (0x00001000u)
        #define INT_USBRXVALIDEP5_BIT                        (12)
        #define INT_USBRXVALIDEP5_BITS                       (1)

/* INT_USBRXVALIDEP4 field */
        #define INT_USBRXVALIDEP4                            (0x00000800u)
        #define INT_USBRXVALIDEP4_MASK                       (0x00000800u)
        #define INT_USBRXVALIDEP4_BIT                        (11)
        #define INT_USBRXVALIDEP4_BITS                       (1)

/* INT_USBRXVALIDEP3 field */
        #define INT_USBRXVALIDEP3                            (0x00000400u)
        #define INT_USBRXVALIDEP3_MASK                       (0x00000400u)
        #define INT_USBRXVALIDEP3_BIT                        (10)
        #define INT_USBRXVALIDEP3_BITS                       (1)

/* INT_USBRXVALIDEP2 field */
        #define INT_USBRXVALIDEP2                            (0x00000200u)
        #define INT_USBRXVALIDEP2_MASK                       (0x00000200u)
        #define INT_USBRXVALIDEP2_BIT                        (9)
        #define INT_USBRXVALIDEP2_BITS                       (1)

/* INT_USBRXVALIDEP1 field */
        #define INT_USBRXVALIDEP1                            (0x00000100u)
        #define INT_USBRXVALIDEP1_MASK                       (0x00000100u)
        #define INT_USBRXVALIDEP1_BIT                        (8)
        #define INT_USBRXVALIDEP1_BITS                       (1)

/* INT_USBRXVALIDEP0 field */
        #define INT_USBRXVALIDEP0                            (0x00000080u)
        #define INT_USBRXVALIDEP0_MASK                       (0x00000080u)
        #define INT_USBRXVALIDEP0_BIT                        (7)
        #define INT_USBRXVALIDEP0_BITS                       (1)

/* INT_USBTXACTIVEEP6 field */
        #define INT_USBTXACTIVEEP6                           (0x00000040u)
        #define INT_USBTXACTIVEEP6_MASK                      (0x00000040u)
        #define INT_USBTXACTIVEEP6_BIT                       (6)
        #define INT_USBTXACTIVEEP6_BITS                      (1)

/* INT_USBTXACTIVEEP5 field */
        #define INT_USBTXACTIVEEP5                           (0x00000020u)
        #define INT_USBTXACTIVEEP5_MASK                      (0x00000020u)
        #define INT_USBTXACTIVEEP5_BIT                       (5)
        #define INT_USBTXACTIVEEP5_BITS                      (1)

/* INT_USBTXACTIVEEP4 field */
        #define INT_USBTXACTIVEEP4                           (0x00000010u)
        #define INT_USBTXACTIVEEP4_MASK                      (0x00000010u)
        #define INT_USBTXACTIVEEP4_BIT                       (4)
        #define INT_USBTXACTIVEEP4_BITS                      (1)

/* INT_USBTXACTIVEEP3 field */
        #define INT_USBTXACTIVEEP3                           (0x00000008u)
        #define INT_USBTXACTIVEEP3_MASK                      (0x00000008u)
        #define INT_USBTXACTIVEEP3_BIT                       (3)
        #define INT_USBTXACTIVEEP3_BITS                      (1)

/* INT_USBTXACTIVEEP2 field */
        #define INT_USBTXACTIVEEP2                           (0x00000004u)
        #define INT_USBTXACTIVEEP2_MASK                      (0x00000004u)
        #define INT_USBTXACTIVEEP2_BIT                       (2)
        #define INT_USBTXACTIVEEP2_BITS                      (1)

/* INT_USBTXACTIVEEP1 field */
        #define INT_USBTXACTIVEEP1                           (0x00000002u)
        #define INT_USBTXACTIVEEP1_MASK                      (0x00000002u)
        #define INT_USBTXACTIVEEP1_BIT                       (1)
        #define INT_USBTXACTIVEEP1_BITS                      (1)

/* INT_USBTXACTIVEEP0 field */
        #define INT_USBTXACTIVEEP0                           (0x00000001u)
        #define INT_USBTXACTIVEEP0_MASK                      (0x00000001u)
        #define INT_USBTXACTIVEEP0_BIT                       (0)
        #define INT_USBTXACTIVEEP0_BITS                      (1)

/* GPIO block */
#define BLOCK_GPIO_BASE                                      (0x4000B000u)
#define BLOCK_GPIO_END                                       (0x4000BC2Cu)
#define BLOCK_GPIO_SIZE                                      (BLOCK_GPIO_END - BLOCK_GPIO_BASE + 1)

#define GPIO_PACFGL                                          *((volatile uint32_t *)0x4000B000u)
#define GPIO_PACFGL_REG                                      *((volatile uint32_t *)0x4000B000u)
#define GPIO_PACFGL_ADDR                                     (0x4000B000u)
#define GPIO_PACFGL_RESET                                    (0x00004444u)

/* PA3_CFG field */
        #define PA3_CFG                                      (0x0000F000u)
        #define PA3_CFG_MASK                                 (0x0000F000u)
        #define PA3_CFG_BIT                                  (12)
        #define PA3_CFG_BITS                                 (4)

/* PA2_CFG field */
        #define PA2_CFG                                      (0x00000F00u)
        #define PA2_CFG_MASK                                 (0x00000F00u)
        #define PA2_CFG_BIT                                  (8)
        #define PA2_CFG_BITS                                 (4)

/* PA1_CFG field */
        #define PA1_CFG                                      (0x000000F0u)
        #define PA1_CFG_MASK                                 (0x000000F0u)
        #define PA1_CFG_BIT                                  (4)
        #define PA1_CFG_BITS                                 (4)

/* PA0_CFG field */
        #define PA0_CFG                                      (0x0000000Fu)
        #define PA0_CFG_MASK                                 (0x0000000Fu)
        #define PA0_CFG_BIT                                  (0)
        #define PA0_CFG_BITS                                 (4)

/* GPIO_PxCFGx Bit Field Values */
                #define GPIOCFG_OUT                          (0x1u)
                #define GPIOCFG_OUT_OD                       (0x5u)
                #define GPIOCFG_OUT_ALT                      (0x9u)
                #define GPIOCFG_OUT_ALT_OD                   (0xDu)
                #define GPIOCFG_OUT_ALT_SPI_SLAVE_MISO       (0xBu)
                #define GPIOCFG_ANALOG                       (0x0u)
                #define GPIOCFG_IN                           (0x4u)
                #define GPIOCFG_SWDIO                        (0x6u)
                #define GPIOCFG_IN_PUD                       (0x8u)

#define GPIO_PACFGH                                          *((volatile uint32_t *)0x4000B004u)
#define GPIO_PACFGH_REG                                      *((volatile uint32_t *)0x4000B004u)
#define GPIO_PACFGH_ADDR                                     (0x4000B004u)
#define GPIO_PACFGH_RESET                                    (0x00004444u)

/* PA7_CFG field */
        #define PA7_CFG                                      (0x0000F000u)
        #define PA7_CFG_MASK                                 (0x0000F000u)
        #define PA7_CFG_BIT                                  (12)
        #define PA7_CFG_BITS                                 (4)

/* PA6_CFG field */
        #define PA6_CFG                                      (0x00000F00u)
        #define PA6_CFG_MASK                                 (0x00000F00u)
        #define PA6_CFG_BIT                                  (8)
        #define PA6_CFG_BITS                                 (4)

/* PA5_CFG field */
        #define PA5_CFG                                      (0x000000F0u)
        #define PA5_CFG_MASK                                 (0x000000F0u)
        #define PA5_CFG_BIT                                  (4)
        #define PA5_CFG_BITS                                 (4)

/* PA4_CFG field */
        #define PA4_CFG                                      (0x0000000Fu)
        #define PA4_CFG_MASK                                 (0x0000000Fu)
        #define PA4_CFG_BIT                                  (0)
        #define PA4_CFG_BITS                                 (4)

#define GPIO_PAIN                                            *((volatile uint32_t *)0x4000B008u)
#define GPIO_PAIN_REG                                        *((volatile uint32_t *)0x4000B008u)
#define GPIO_PAIN_ADDR                                       (0x4000B008u)
#define GPIO_PAIN_RESET                                      (0x00000000u)

/* PA7 field */
        #define PA7                                          (0x00000080u)
        #define PA7_MASK                                     (0x00000080u)
        #define PA7_BIT                                      (7)
        #define PA7_BITS                                     (1)

/* PA6 field */
        #define PA6                                          (0x00000040u)
        #define PA6_MASK                                     (0x00000040u)
        #define PA6_BIT                                      (6)
        #define PA6_BITS                                     (1)

/* PA5 field */
        #define PA5                                          (0x00000020u)
        #define PA5_MASK                                     (0x00000020u)
        #define PA5_BIT                                      (5)
        #define PA5_BITS                                     (1)

/* PA4 field */
        #define PA4                                          (0x00000010u)
        #define PA4_MASK                                     (0x00000010u)
        #define PA4_BIT                                      (4)
        #define PA4_BITS                                     (1)

/* PA3 field */
        #define PA3                                          (0x00000008u)
        #define PA3_MASK                                     (0x00000008u)
        #define PA3_BIT                                      (3)
        #define PA3_BITS                                     (1)

/* PA2 field */
        #define PA2                                          (0x00000004u)
        #define PA2_MASK                                     (0x00000004u)
        #define PA2_BIT                                      (2)
        #define PA2_BITS                                     (1)

/* PA1 field */
        #define PA1                                          (0x00000002u)
        #define PA1_MASK                                     (0x00000002u)
        #define PA1_BIT                                      (1)
        #define PA1_BITS                                     (1)

/* PA0 field */
        #define PA0                                          (0x00000001u)
        #define PA0_MASK                                     (0x00000001u)
        #define PA0_BIT                                      (0)
        #define PA0_BITS                                     (1)

#define GPIO_PAOUT                                           *((volatile uint32_t *)0x4000B00Cu)
#define GPIO_PAOUT_REG                                       *((volatile uint32_t *)0x4000B00Cu)
#define GPIO_PAOUT_ADDR                                      (0x4000B00Cu)
#define GPIO_PAOUT_RESET                                     (0x00000000u)

/* PA7 field */
        #define PA7                                          (0x00000080u)
        #define PA7_MASK                                     (0x00000080u)
        #define PA7_BIT                                      (7)
        #define PA7_BITS                                     (1)

/* PA6 field */
        #define PA6                                          (0x00000040u)
        #define PA6_MASK                                     (0x00000040u)
        #define PA6_BIT                                      (6)
        #define PA6_BITS                                     (1)

/* PA5 field */
        #define PA5                                          (0x00000020u)
        #define PA5_MASK                                     (0x00000020u)
        #define PA5_BIT                                      (5)
        #define PA5_BITS                                     (1)

/* PA4 field */
        #define PA4                                          (0x00000010u)
        #define PA4_MASK                                     (0x00000010u)
        #define PA4_BIT                                      (4)
        #define PA4_BITS                                     (1)

/* PA3 field */
        #define PA3                                          (0x00000008u)
        #define PA3_MASK                                     (0x00000008u)
        #define PA3_BIT                                      (3)
        #define PA3_BITS                                     (1)

/* PA2 field */
        #define PA2                                          (0x00000004u)
        #define PA2_MASK                                     (0x00000004u)
        #define PA2_BIT                                      (2)
        #define PA2_BITS                                     (1)

/* PA1 field */
        #define PA1                                          (0x00000002u)
        #define PA1_MASK                                     (0x00000002u)
        #define PA1_BIT                                      (1)
        #define PA1_BITS                                     (1)

/* PA0 field */
        #define PA0                                          (0x00000001u)
        #define PA0_MASK                                     (0x00000001u)
        #define PA0_BIT                                      (0)
        #define PA0_BITS                                     (1)

/* GPIO_PxOUT Bit Field Values */
                #define GPIOOUT_PULLUP                       (0x1u)
                #define GPIOOUT_PULLDOWN                     (0x0u)

#define GPIO_PASET                                           *((volatile uint32_t *)0x4000B010u)
#define GPIO_PASET_REG                                       *((volatile uint32_t *)0x4000B010u)
#define GPIO_PASET_ADDR                                      (0x4000B010u)
#define GPIO_PASET_RESET                                     (0x00000000u)

/* GPIO_PXSETRSVD field */
        #define GPIO_PXSETRSVD                               (0x0000FF00u)
        #define GPIO_PXSETRSVD_MASK                          (0x0000FF00u)
        #define GPIO_PXSETRSVD_BIT                           (8)
        #define GPIO_PXSETRSVD_BITS                          (8)

/* PA7 field */
        #define PA7                                          (0x00000080u)
        #define PA7_MASK                                     (0x00000080u)
        #define PA7_BIT                                      (7)
        #define PA7_BITS                                     (1)

/* PA6 field */
        #define PA6                                          (0x00000040u)
        #define PA6_MASK                                     (0x00000040u)
        #define PA6_BIT                                      (6)
        #define PA6_BITS                                     (1)

/* PA5 field */
        #define PA5                                          (0x00000020u)
        #define PA5_MASK                                     (0x00000020u)
        #define PA5_BIT                                      (5)
        #define PA5_BITS                                     (1)

/* PA4 field */
        #define PA4                                          (0x00000010u)
        #define PA4_MASK                                     (0x00000010u)
        #define PA4_BIT                                      (4)
        #define PA4_BITS                                     (1)

/* PA3 field */
        #define PA3                                          (0x00000008u)
        #define PA3_MASK                                     (0x00000008u)
        #define PA3_BIT                                      (3)
        #define PA3_BITS                                     (1)

/* PA2 field */
        #define PA2                                          (0x00000004u)
        #define PA2_MASK                                     (0x00000004u)
        #define PA2_BIT                                      (2)
        #define PA2_BITS                                     (1)

/* PA1 field */
        #define PA1                                          (0x00000002u)
        #define PA1_MASK                                     (0x00000002u)
        #define PA1_BIT                                      (1)
        #define PA1_BITS                                     (1)

/* PA0 field */
        #define PA0                                          (0x00000001u)
        #define PA0_MASK                                     (0x00000001u)
        #define PA0_BIT                                      (0)
        #define PA0_BITS                                     (1)

#define GPIO_PACLR                                           *((volatile uint32_t *)0x4000B014u)
#define GPIO_PACLR_REG                                       *((volatile uint32_t *)0x4000B014u)
#define GPIO_PACLR_ADDR                                      (0x4000B014u)
#define GPIO_PACLR_RESET                                     (0x00000000u)

/* PA7 field */
        #define PA7                                          (0x00000080u)
        #define PA7_MASK                                     (0x00000080u)
        #define PA7_BIT                                      (7)
        #define PA7_BITS                                     (1)

/* PA6 field */
        #define PA6                                          (0x00000040u)
        #define PA6_MASK                                     (0x00000040u)
        #define PA6_BIT                                      (6)
        #define PA6_BITS                                     (1)

/* PA5 field */
        #define PA5                                          (0x00000020u)
        #define PA5_MASK                                     (0x00000020u)
        #define PA5_BIT                                      (5)
        #define PA5_BITS                                     (1)

/* PA4 field */
        #define PA4                                          (0x00000010u)
        #define PA4_MASK                                     (0x00000010u)
        #define PA4_BIT                                      (4)
        #define PA4_BITS                                     (1)

/* PA3 field */
        #define PA3                                          (0x00000008u)
        #define PA3_MASK                                     (0x00000008u)
        #define PA3_BIT                                      (3)
        #define PA3_BITS                                     (1)

/* PA2 field */
        #define PA2                                          (0x00000004u)
        #define PA2_MASK                                     (0x00000004u)
        #define PA2_BIT                                      (2)
        #define PA2_BITS                                     (1)

/* PA1 field */
        #define PA1                                          (0x00000002u)
        #define PA1_MASK                                     (0x00000002u)
        #define PA1_BIT                                      (1)
        #define PA1_BITS                                     (1)

/* PA0 field */
        #define PA0                                          (0x00000001u)
        #define PA0_MASK                                     (0x00000001u)
        #define PA0_BIT                                      (0)
        #define PA0_BITS                                     (1)

#define GPIO_PBCFGL                                          *((volatile uint32_t *)0x4000B200u)
#define GPIO_PBCFGL_REG                                      *((volatile uint32_t *)0x4000B200u)
#define GPIO_PBCFGL_ADDR                                     (0x4000B200u)
#define GPIO_PBCFGL_RESET                                    (0x00004444u)

/* PB3_CFG field */
        #define PB3_CFG                                      (0x0000F000u)
        #define PB3_CFG_MASK                                 (0x0000F000u)
        #define PB3_CFG_BIT                                  (12)
        #define PB3_CFG_BITS                                 (4)

/* PB2_CFG field */
        #define PB2_CFG                                      (0x00000F00u)
        #define PB2_CFG_MASK                                 (0x00000F00u)
        #define PB2_CFG_BIT                                  (8)
        #define PB2_CFG_BITS                                 (4)

/* PB1_CFG field */
        #define PB1_CFG                                      (0x000000F0u)
        #define PB1_CFG_MASK                                 (0x000000F0u)
        #define PB1_CFG_BIT                                  (4)
        #define PB1_CFG_BITS                                 (4)

/* PB0_CFG field */
        #define PB0_CFG                                      (0x0000000Fu)
        #define PB0_CFG_MASK                                 (0x0000000Fu)
        #define PB0_CFG_BIT                                  (0)
        #define PB0_CFG_BITS                                 (4)

#define GPIO_PBCFGH                                          *((volatile uint32_t *)0x4000B204u)
#define GPIO_PBCFGH_REG                                      *((volatile uint32_t *)0x4000B204u)
#define GPIO_PBCFGH_ADDR                                     (0x4000B204u)
#define GPIO_PBCFGH_RESET                                    (0x00004444u)

/* PB7_CFG field */
        #define PB7_CFG                                      (0x0000F000u)
        #define PB7_CFG_MASK                                 (0x0000F000u)
        #define PB7_CFG_BIT                                  (12)
        #define PB7_CFG_BITS                                 (4)

/* PB6_CFG field */
        #define PB6_CFG                                      (0x00000F00u)
        #define PB6_CFG_MASK                                 (0x00000F00u)
        #define PB6_CFG_BIT                                  (8)
        #define PB6_CFG_BITS                                 (4)

/* PB5_CFG field */
        #define PB5_CFG                                      (0x000000F0u)
        #define PB5_CFG_MASK                                 (0x000000F0u)
        #define PB5_CFG_BIT                                  (4)
        #define PB5_CFG_BITS                                 (4)

/* PB4_CFG field */
        #define PB4_CFG                                      (0x0000000Fu)
        #define PB4_CFG_MASK                                 (0x0000000Fu)
        #define PB4_CFG_BIT                                  (0)
        #define PB4_CFG_BITS                                 (4)

#define GPIO_PBIN                                            *((volatile uint32_t *)0x4000B208u)
#define GPIO_PBIN_REG                                        *((volatile uint32_t *)0x4000B208u)
#define GPIO_PBIN_ADDR                                       (0x4000B208u)
#define GPIO_PBIN_RESET                                      (0x00000000u)

/* PB7 field */
        #define PB7                                          (0x00000080u)
        #define PB7_MASK                                     (0x00000080u)
        #define PB7_BIT                                      (7)
        #define PB7_BITS                                     (1)

/* PB6 field */
        #define PB6                                          (0x00000040u)
        #define PB6_MASK                                     (0x00000040u)
        #define PB6_BIT                                      (6)
        #define PB6_BITS                                     (1)

/* PB5 field */
        #define PB5                                          (0x00000020u)
        #define PB5_MASK                                     (0x00000020u)
        #define PB5_BIT                                      (5)
        #define PB5_BITS                                     (1)

/* PB4 field */
        #define PB4                                          (0x00000010u)
        #define PB4_MASK                                     (0x00000010u)
        #define PB4_BIT                                      (4)
        #define PB4_BITS                                     (1)

/* PB3 field */
        #define PB3                                          (0x00000008u)
        #define PB3_MASK                                     (0x00000008u)
        #define PB3_BIT                                      (3)
        #define PB3_BITS                                     (1)

/* PB2 field */
        #define PB2                                          (0x00000004u)
        #define PB2_MASK                                     (0x00000004u)
        #define PB2_BIT                                      (2)
        #define PB2_BITS                                     (1)

/* PB1 field */
        #define PB1                                          (0x00000002u)
        #define PB1_MASK                                     (0x00000002u)
        #define PB1_BIT                                      (1)
        #define PB1_BITS                                     (1)

/* PB0 field */
        #define PB0                                          (0x00000001u)
        #define PB0_MASK                                     (0x00000001u)
        #define PB0_BIT                                      (0)
        #define PB0_BITS                                     (1)

#define GPIO_PBOUT                                           *((volatile uint32_t *)0x4000B20Cu)
#define GPIO_PBOUT_REG                                       *((volatile uint32_t *)0x4000B20Cu)
#define GPIO_PBOUT_ADDR                                      (0x4000B20Cu)
#define GPIO_PBOUT_RESET                                     (0x00000000u)

/* PB7 field */
        #define PB7                                          (0x00000080u)
        #define PB7_MASK                                     (0x00000080u)
        #define PB7_BIT                                      (7)
        #define PB7_BITS                                     (1)

/* PB6 field */
        #define PB6                                          (0x00000040u)
        #define PB6_MASK                                     (0x00000040u)
        #define PB6_BIT                                      (6)
        #define PB6_BITS                                     (1)

/* PB5 field */
        #define PB5                                          (0x00000020u)
        #define PB5_MASK                                     (0x00000020u)
        #define PB5_BIT                                      (5)
        #define PB5_BITS                                     (1)

/* PB4 field */
        #define PB4                                          (0x00000010u)
        #define PB4_MASK                                     (0x00000010u)
        #define PB4_BIT                                      (4)
        #define PB4_BITS                                     (1)

/* PB3 field */
        #define PB3                                          (0x00000008u)
        #define PB3_MASK                                     (0x00000008u)
        #define PB3_BIT                                      (3)
        #define PB3_BITS                                     (1)

/* PB2 field */
        #define PB2                                          (0x00000004u)
        #define PB2_MASK                                     (0x00000004u)
        #define PB2_BIT                                      (2)
        #define PB2_BITS                                     (1)

/* PB1 field */
        #define PB1                                          (0x00000002u)
        #define PB1_MASK                                     (0x00000002u)
        #define PB1_BIT                                      (1)
        #define PB1_BITS                                     (1)

/* PB0 field */
        #define PB0                                          (0x00000001u)
        #define PB0_MASK                                     (0x00000001u)
        #define PB0_BIT                                      (0)
        #define PB0_BITS                                     (1)

#define GPIO_PBSET                                           *((volatile uint32_t *)0x4000B210u)
#define GPIO_PBSET_REG                                       *((volatile uint32_t *)0x4000B210u)
#define GPIO_PBSET_ADDR                                      (0x4000B210u)
#define GPIO_PBSET_RESET                                     (0x00000000u)

/* GPIO_PXSETRSVD field */
        #define GPIO_PXSETRSVD                               (0x0000FF00u)
        #define GPIO_PXSETRSVD_MASK                          (0x0000FF00u)
        #define GPIO_PXSETRSVD_BIT                           (8)
        #define GPIO_PXSETRSVD_BITS                          (8)

/* PB7 field */
        #define PB7                                          (0x00000080u)
        #define PB7_MASK                                     (0x00000080u)
        #define PB7_BIT                                      (7)
        #define PB7_BITS                                     (1)

/* PB6 field */
        #define PB6                                          (0x00000040u)
        #define PB6_MASK                                     (0x00000040u)
        #define PB6_BIT                                      (6)
        #define PB6_BITS                                     (1)

/* PB5 field */
        #define PB5                                          (0x00000020u)
        #define PB5_MASK                                     (0x00000020u)
        #define PB5_BIT                                      (5)
        #define PB5_BITS                                     (1)

/* PB4 field */
        #define PB4                                          (0x00000010u)
        #define PB4_MASK                                     (0x00000010u)
        #define PB4_BIT                                      (4)
        #define PB4_BITS                                     (1)

/* PB3 field */
        #define PB3                                          (0x00000008u)
        #define PB3_MASK                                     (0x00000008u)
        #define PB3_BIT                                      (3)
        #define PB3_BITS                                     (1)

/* PB2 field */
        #define PB2                                          (0x00000004u)
        #define PB2_MASK                                     (0x00000004u)
        #define PB2_BIT                                      (2)
        #define PB2_BITS                                     (1)

/* PB1 field */
        #define PB1                                          (0x00000002u)
        #define PB1_MASK                                     (0x00000002u)
        #define PB1_BIT                                      (1)
        #define PB1_BITS                                     (1)

/* PB0 field */
        #define PB0                                          (0x00000001u)
        #define PB0_MASK                                     (0x00000001u)
        #define PB0_BIT                                      (0)
        #define PB0_BITS                                     (1)

#define GPIO_PBCLR                                           *((volatile uint32_t *)0x4000B214u)
#define GPIO_PBCLR_REG                                       *((volatile uint32_t *)0x4000B214u)
#define GPIO_PBCLR_ADDR                                      (0x4000B214u)
#define GPIO_PBCLR_RESET                                     (0x00000000u)

/* PB7 field */
        #define PB7                                          (0x00000080u)
        #define PB7_MASK                                     (0x00000080u)
        #define PB7_BIT                                      (7)
        #define PB7_BITS                                     (1)

/* PB6 field */
        #define PB6                                          (0x00000040u)
        #define PB6_MASK                                     (0x00000040u)
        #define PB6_BIT                                      (6)
        #define PB6_BITS                                     (1)

/* PB5 field */
        #define PB5                                          (0x00000020u)
        #define PB5_MASK                                     (0x00000020u)
        #define PB5_BIT                                      (5)
        #define PB5_BITS                                     (1)

/* PB4 field */
        #define PB4                                          (0x00000010u)
        #define PB4_MASK                                     (0x00000010u)
        #define PB4_BIT                                      (4)
        #define PB4_BITS                                     (1)

/* PB3 field */
        #define PB3                                          (0x00000008u)
        #define PB3_MASK                                     (0x00000008u)
        #define PB3_BIT                                      (3)
        #define PB3_BITS                                     (1)

/* PB2 field */
        #define PB2                                          (0x00000004u)
        #define PB2_MASK                                     (0x00000004u)
        #define PB2_BIT                                      (2)
        #define PB2_BITS                                     (1)

/* PB1 field */
        #define PB1                                          (0x00000002u)
        #define PB1_MASK                                     (0x00000002u)
        #define PB1_BIT                                      (1)
        #define PB1_BITS                                     (1)

/* PB0 field */
        #define PB0                                          (0x00000001u)
        #define PB0_MASK                                     (0x00000001u)
        #define PB0_BIT                                      (0)
        #define PB0_BITS                                     (1)

#define GPIO_PCCFGL                                          *((volatile uint32_t *)0x4000B400u)
#define GPIO_PCCFGL_REG                                      *((volatile uint32_t *)0x4000B400u)
#define GPIO_PCCFGL_ADDR                                     (0x4000B400u)
#define GPIO_PCCFGL_RESET                                    (0x00004444u)

/* PC3_CFG field */
        #define PC3_CFG                                      (0x0000F000u)
        #define PC3_CFG_MASK                                 (0x0000F000u)
        #define PC3_CFG_BIT                                  (12)
        #define PC3_CFG_BITS                                 (4)

/* PC2_CFG field */
        #define PC2_CFG                                      (0x00000F00u)
        #define PC2_CFG_MASK                                 (0x00000F00u)
        #define PC2_CFG_BIT                                  (8)
        #define PC2_CFG_BITS                                 (4)

/* PC1_CFG field */
        #define PC1_CFG                                      (0x000000F0u)
        #define PC1_CFG_MASK                                 (0x000000F0u)
        #define PC1_CFG_BIT                                  (4)
        #define PC1_CFG_BITS                                 (4)

/* PC0_CFG field */
        #define PC0_CFG                                      (0x0000000Fu)
        #define PC0_CFG_MASK                                 (0x0000000Fu)
        #define PC0_CFG_BIT                                  (0)
        #define PC0_CFG_BITS                                 (4)

#define GPIO_PCCFGH                                          *((volatile uint32_t *)0x4000B404u)
#define GPIO_PCCFGH_REG                                      *((volatile uint32_t *)0x4000B404u)
#define GPIO_PCCFGH_ADDR                                     (0x4000B404u)
#define GPIO_PCCFGH_RESET                                    (0x00004444u)

/* PC7_CFG field */
        #define PC7_CFG                                      (0x0000F000u)
        #define PC7_CFG_MASK                                 (0x0000F000u)
        #define PC7_CFG_BIT                                  (12)
        #define PC7_CFG_BITS                                 (4)

/* PC6_CFG field */
        #define PC6_CFG                                      (0x00000F00u)
        #define PC6_CFG_MASK                                 (0x00000F00u)
        #define PC6_CFG_BIT                                  (8)
        #define PC6_CFG_BITS                                 (4)

/* PC5_CFG field */
        #define PC5_CFG                                      (0x000000F0u)
        #define PC5_CFG_MASK                                 (0x000000F0u)
        #define PC5_CFG_BIT                                  (4)
        #define PC5_CFG_BITS                                 (4)

/* PC4_CFG field */
        #define PC4_CFG                                      (0x0000000Fu)
        #define PC4_CFG_MASK                                 (0x0000000Fu)
        #define PC4_CFG_BIT                                  (0)
        #define PC4_CFG_BITS                                 (4)

#define GPIO_PCIN                                            *((volatile uint32_t *)0x4000B408u)
#define GPIO_PCIN_REG                                        *((volatile uint32_t *)0x4000B408u)
#define GPIO_PCIN_ADDR                                       (0x4000B408u)
#define GPIO_PCIN_RESET                                      (0x00000000u)

/* PC7 field */
        #define PC7                                          (0x00000080u)
        #define PC7_MASK                                     (0x00000080u)
        #define PC7_BIT                                      (7)
        #define PC7_BITS                                     (1)

/* PC6 field */
        #define PC6                                          (0x00000040u)
        #define PC6_MASK                                     (0x00000040u)
        #define PC6_BIT                                      (6)
        #define PC6_BITS                                     (1)

/* PC5 field */
        #define PC5                                          (0x00000020u)
        #define PC5_MASK                                     (0x00000020u)
        #define PC5_BIT                                      (5)
        #define PC5_BITS                                     (1)

/* PC4 field */
        #define PC4                                          (0x00000010u)
        #define PC4_MASK                                     (0x00000010u)
        #define PC4_BIT                                      (4)
        #define PC4_BITS                                     (1)

/* PC3 field */
        #define PC3                                          (0x00000008u)
        #define PC3_MASK                                     (0x00000008u)
        #define PC3_BIT                                      (3)
        #define PC3_BITS                                     (1)

/* PC2 field */
        #define PC2                                          (0x00000004u)
        #define PC2_MASK                                     (0x00000004u)
        #define PC2_BIT                                      (2)
        #define PC2_BITS                                     (1)

/* PC1 field */
        #define PC1                                          (0x00000002u)
        #define PC1_MASK                                     (0x00000002u)
        #define PC1_BIT                                      (1)
        #define PC1_BITS                                     (1)

/* PC0 field */
        #define PC0                                          (0x00000001u)
        #define PC0_MASK                                     (0x00000001u)
        #define PC0_BIT                                      (0)
        #define PC0_BITS                                     (1)

#define GPIO_PCOUT                                           *((volatile uint32_t *)0x4000B40Cu)
#define GPIO_PCOUT_REG                                       *((volatile uint32_t *)0x4000B40Cu)
#define GPIO_PCOUT_ADDR                                      (0x4000B40Cu)
#define GPIO_PCOUT_RESET                                     (0x00000000u)

/* PC7 field */
        #define PC7                                          (0x00000080u)
        #define PC7_MASK                                     (0x00000080u)
        #define PC7_BIT                                      (7)
        #define PC7_BITS                                     (1)

/* PC6 field */
        #define PC6                                          (0x00000040u)
        #define PC6_MASK                                     (0x00000040u)
        #define PC6_BIT                                      (6)
        #define PC6_BITS                                     (1)

/* PC5 field */
        #define PC5                                          (0x00000020u)
        #define PC5_MASK                                     (0x00000020u)
        #define PC5_BIT                                      (5)
        #define PC5_BITS                                     (1)

/* PC4 field */
        #define PC4                                          (0x00000010u)
        #define PC4_MASK                                     (0x00000010u)
        #define PC4_BIT                                      (4)
        #define PC4_BITS                                     (1)

/* PC3 field */
        #define PC3                                          (0x00000008u)
        #define PC3_MASK                                     (0x00000008u)
        #define PC3_BIT                                      (3)
        #define PC3_BITS                                     (1)

/* PC2 field */
        #define PC2                                          (0x00000004u)
        #define PC2_MASK                                     (0x00000004u)
        #define PC2_BIT                                      (2)
        #define PC2_BITS                                     (1)

/* PC1 field */
        #define PC1                                          (0x00000002u)
        #define PC1_MASK                                     (0x00000002u)
        #define PC1_BIT                                      (1)
        #define PC1_BITS                                     (1)

/* PC0 field */
        #define PC0                                          (0x00000001u)
        #define PC0_MASK                                     (0x00000001u)
        #define PC0_BIT                                      (0)
        #define PC0_BITS                                     (1)

#define GPIO_PCSET                                           *((volatile uint32_t *)0x4000B410u)
#define GPIO_PCSET_REG                                       *((volatile uint32_t *)0x4000B410u)
#define GPIO_PCSET_ADDR                                      (0x4000B410u)
#define GPIO_PCSET_RESET                                     (0x00000000u)

/* GPIO_PXSETRSVD field */
        #define GPIO_PXSETRSVD                               (0x0000FF00u)
        #define GPIO_PXSETRSVD_MASK                          (0x0000FF00u)
        #define GPIO_PXSETRSVD_BIT                           (8)
        #define GPIO_PXSETRSVD_BITS                          (8)

/* PC7 field */
        #define PC7                                          (0x00000080u)
        #define PC7_MASK                                     (0x00000080u)
        #define PC7_BIT                                      (7)
        #define PC7_BITS                                     (1)

/* PC6 field */
        #define PC6                                          (0x00000040u)
        #define PC6_MASK                                     (0x00000040u)
        #define PC6_BIT                                      (6)
        #define PC6_BITS                                     (1)

/* PC5 field */
        #define PC5                                          (0x00000020u)
        #define PC5_MASK                                     (0x00000020u)
        #define PC5_BIT                                      (5)
        #define PC5_BITS                                     (1)

/* PC4 field */
        #define PC4                                          (0x00000010u)
        #define PC4_MASK                                     (0x00000010u)
        #define PC4_BIT                                      (4)
        #define PC4_BITS                                     (1)

/* PC3 field */
        #define PC3                                          (0x00000008u)
        #define PC3_MASK                                     (0x00000008u)
        #define PC3_BIT                                      (3)
        #define PC3_BITS                                     (1)

/* PC2 field */
        #define PC2                                          (0x00000004u)
        #define PC2_MASK                                     (0x00000004u)
        #define PC2_BIT                                      (2)
        #define PC2_BITS                                     (1)

/* PC1 field */
        #define PC1                                          (0x00000002u)
        #define PC1_MASK                                     (0x00000002u)
        #define PC1_BIT                                      (1)
        #define PC1_BITS                                     (1)

/* PC0 field */
        #define PC0                                          (0x00000001u)
        #define PC0_MASK                                     (0x00000001u)
        #define PC0_BIT                                      (0)
        #define PC0_BITS                                     (1)

#define GPIO_PCCLR                                           *((volatile uint32_t *)0x4000B414u)
#define GPIO_PCCLR_REG                                       *((volatile uint32_t *)0x4000B414u)
#define GPIO_PCCLR_ADDR                                      (0x4000B414u)
#define GPIO_PCCLR_RESET                                     (0x00000000u)

/* PC7 field */
        #define PC7                                          (0x00000080u)
        #define PC7_MASK                                     (0x00000080u)
        #define PC7_BIT                                      (7)
        #define PC7_BITS                                     (1)

/* PC6 field */
        #define PC6                                          (0x00000040u)
        #define PC6_MASK                                     (0x00000040u)
        #define PC6_BIT                                      (6)
        #define PC6_BITS                                     (1)

/* PC5 field */
        #define PC5                                          (0x00000020u)
        #define PC5_MASK                                     (0x00000020u)
        #define PC5_BIT                                      (5)
        #define PC5_BITS                                     (1)

/* PC4 field */
        #define PC4                                          (0x00000010u)
        #define PC4_MASK                                     (0x00000010u)
        #define PC4_BIT                                      (4)
        #define PC4_BITS                                     (1)

/* PC3 field */
        #define PC3                                          (0x00000008u)
        #define PC3_MASK                                     (0x00000008u)
        #define PC3_BIT                                      (3)
        #define PC3_BITS                                     (1)

/* PC2 field */
        #define PC2                                          (0x00000004u)
        #define PC2_MASK                                     (0x00000004u)
        #define PC2_BIT                                      (2)
        #define PC2_BITS                                     (1)

/* PC1 field */
        #define PC1                                          (0x00000002u)
        #define PC1_MASK                                     (0x00000002u)
        #define PC1_BIT                                      (1)
        #define PC1_BITS                                     (1)

/* PC0 field */
        #define PC0                                          (0x00000001u)
        #define PC0_MASK                                     (0x00000001u)
        #define PC0_BIT                                      (0)
        #define PC0_BITS                                     (1)

#define GPIO_DBGCFG                                          *((volatile uint32_t *)0x4000BC00u)
#define GPIO_DBGCFG_REG                                      *((volatile uint32_t *)0x4000BC00u)
#define GPIO_DBGCFG_ADDR                                     (0x4000BC00u)
#define GPIO_DBGCFG_RESET                                    (0x00000010u)

/* GPIO_DEBUGDIS field */
        #define GPIO_DEBUGDIS                                (0x00000020u)
        #define GPIO_DEBUGDIS_MASK                           (0x00000020u)
        #define GPIO_DEBUGDIS_BIT                            (5)
        #define GPIO_DEBUGDIS_BITS                           (1)

/* GPIO_EXTREGEN field */
        #define GPIO_EXTREGEN                                (0x00000010u)
        #define GPIO_EXTREGEN_MASK                           (0x00000010u)
        #define GPIO_EXTREGEN_BIT                            (4)
        #define GPIO_EXTREGEN_BITS                           (1)

/* GPIO_DBGCFGRSVD field */
        #define GPIO_DBGCFGRSVD                              (0x00000008u)
        #define GPIO_DBGCFGRSVD_MASK                         (0x00000008u)
        #define GPIO_DBGCFGRSVD_BIT                          (3)
        #define GPIO_DBGCFGRSVD_BITS                         (1)

#define GPIO_DBGSTAT                                         *((volatile uint32_t *)0x4000BC04u)
#define GPIO_DBGSTAT_REG                                     *((volatile uint32_t *)0x4000BC04u)
#define GPIO_DBGSTAT_ADDR                                    (0x4000BC04u)
#define GPIO_DBGSTAT_RESET                                   (0x00000000u)

/* GPIO_BOOTMODE field */
        #define GPIO_BOOTMODE                                (0x00000008u)
        #define GPIO_BOOTMODE_MASK                           (0x00000008u)
        #define GPIO_BOOTMODE_BIT                            (3)
        #define GPIO_BOOTMODE_BITS                           (1)

/* GPIO_FORCEDBG field */
        #define GPIO_FORCEDBG                                (0x00000002u)
        #define GPIO_FORCEDBG_MASK                           (0x00000002u)
        #define GPIO_FORCEDBG_BIT                            (1)
        #define GPIO_FORCEDBG_BITS                           (1)

/* GPIO_SWEN field */
        #define GPIO_SWEN                                    (0x00000001u)
        #define GPIO_SWEN_MASK                               (0x00000001u)
        #define GPIO_SWEN_BIT                                (0)
        #define GPIO_SWEN_BITS                               (1)

#define GPIO_PAWAKE                                          *((volatile uint32_t *)0x4000BC08u)
#define GPIO_PAWAKE_REG                                      *((volatile uint32_t *)0x4000BC08u)
#define GPIO_PAWAKE_ADDR                                     (0x4000BC08u)
#define GPIO_PAWAKE_RESET                                    (0x00000000u)

/* PA7 field */
        #define PA7                                          (0x00000080u)
        #define PA7_MASK                                     (0x00000080u)
        #define PA7_BIT                                      (7)
        #define PA7_BITS                                     (1)

/* PA6 field */
        #define PA6                                          (0x00000040u)
        #define PA6_MASK                                     (0x00000040u)
        #define PA6_BIT                                      (6)
        #define PA6_BITS                                     (1)

/* PA5 field */
        #define PA5                                          (0x00000020u)
        #define PA5_MASK                                     (0x00000020u)
        #define PA5_BIT                                      (5)
        #define PA5_BITS                                     (1)

/* PA4 field */
        #define PA4                                          (0x00000010u)
        #define PA4_MASK                                     (0x00000010u)
        #define PA4_BIT                                      (4)
        #define PA4_BITS                                     (1)

/* PA3 field */
        #define PA3                                          (0x00000008u)
        #define PA3_MASK                                     (0x00000008u)
        #define PA3_BIT                                      (3)
        #define PA3_BITS                                     (1)

/* PA2 field */
        #define PA2                                          (0x00000004u)
        #define PA2_MASK                                     (0x00000004u)
        #define PA2_BIT                                      (2)
        #define PA2_BITS                                     (1)

/* PA1 field */
        #define PA1                                          (0x00000002u)
        #define PA1_MASK                                     (0x00000002u)
        #define PA1_BIT                                      (1)
        #define PA1_BITS                                     (1)

/* PA0 field */
        #define PA0                                          (0x00000001u)
        #define PA0_MASK                                     (0x00000001u)
        #define PA0_BIT                                      (0)
        #define PA0_BITS                                     (1)

#define GPIO_PBWAKE                                          *((volatile uint32_t *)0x4000BC0Cu)
#define GPIO_PBWAKE_REG                                      *((volatile uint32_t *)0x4000BC0Cu)
#define GPIO_PBWAKE_ADDR                                     (0x4000BC0Cu)
#define GPIO_PBWAKE_RESET                                    (0x00000000u)

/* PB7 field */
        #define PB7                                          (0x00000080u)
        #define PB7_MASK                                     (0x00000080u)
        #define PB7_BIT                                      (7)
        #define PB7_BITS                                     (1)

/* PB6 field */
        #define PB6                                          (0x00000040u)
        #define PB6_MASK                                     (0x00000040u)
        #define PB6_BIT                                      (6)
        #define PB6_BITS                                     (1)

/* PB5 field */
        #define PB5                                          (0x00000020u)
        #define PB5_MASK                                     (0x00000020u)
        #define PB5_BIT                                      (5)
        #define PB5_BITS                                     (1)

/* PB4 field */
        #define PB4                                          (0x00000010u)
        #define PB4_MASK                                     (0x00000010u)
        #define PB4_BIT                                      (4)
        #define PB4_BITS                                     (1)

/* PB3 field */
        #define PB3                                          (0x00000008u)
        #define PB3_MASK                                     (0x00000008u)
        #define PB3_BIT                                      (3)
        #define PB3_BITS                                     (1)

/* PB2 field */
        #define PB2                                          (0x00000004u)
        #define PB2_MASK                                     (0x00000004u)
        #define PB2_BIT                                      (2)
        #define PB2_BITS                                     (1)

/* PB1 field */
        #define PB1                                          (0x00000002u)
        #define PB1_MASK                                     (0x00000002u)
        #define PB1_BIT                                      (1)
        #define PB1_BITS                                     (1)

/* PB0 field */
        #define PB0                                          (0x00000001u)
        #define PB0_MASK                                     (0x00000001u)
        #define PB0_BIT                                      (0)
        #define PB0_BITS                                     (1)

#define GPIO_PCWAKE                                          *((volatile uint32_t *)0x4000BC10u)
#define GPIO_PCWAKE_REG                                      *((volatile uint32_t *)0x4000BC10u)
#define GPIO_PCWAKE_ADDR                                     (0x4000BC10u)
#define GPIO_PCWAKE_RESET                                    (0x00000000u)

/* PC7 field */
        #define PC7                                          (0x00000080u)
        #define PC7_MASK                                     (0x00000080u)
        #define PC7_BIT                                      (7)
        #define PC7_BITS                                     (1)

/* PC6 field */
        #define PC6                                          (0x00000040u)
        #define PC6_MASK                                     (0x00000040u)
        #define PC6_BIT                                      (6)
        #define PC6_BITS                                     (1)

/* PC5 field */
        #define PC5                                          (0x00000020u)
        #define PC5_MASK                                     (0x00000020u)
        #define PC5_BIT                                      (5)
        #define PC5_BITS                                     (1)

/* PC4 field */
        #define PC4                                          (0x00000010u)
        #define PC4_MASK                                     (0x00000010u)
        #define PC4_BIT                                      (4)
        #define PC4_BITS                                     (1)

/* PC3 field */
        #define PC3                                          (0x00000008u)
        #define PC3_MASK                                     (0x00000008u)
        #define PC3_BIT                                      (3)
        #define PC3_BITS                                     (1)

/* PC2 field */
        #define PC2                                          (0x00000004u)
        #define PC2_MASK                                     (0x00000004u)
        #define PC2_BIT                                      (2)
        #define PC2_BITS                                     (1)

/* PC1 field */
        #define PC1                                          (0x00000002u)
        #define PC1_MASK                                     (0x00000002u)
        #define PC1_BIT                                      (1)
        #define PC1_BITS                                     (1)

/* PC0 field */
        #define PC0                                          (0x00000001u)
        #define PC0_MASK                                     (0x00000001u)
        #define PC0_BIT                                      (0)
        #define PC0_BITS                                     (1)

#define GPIO_IRQCSEL                                         *((volatile uint32_t *)0x4000BC20u)
#define GPIO_IRQCSEL_REG                                     *((volatile uint32_t *)0x4000BC20u)
#define GPIO_IRQCSEL_ADDR                                    (0x4000BC20u)
#define GPIO_IRQCSEL_RESET                                   (0x0000000Fu)

/* SEL_GPIO field */
        #define SEL_GPIO                                     (0x0000003Fu)
        #define SEL_GPIO_MASK                                (0x0000003Fu)
        #define SEL_GPIO_BIT                                 (0)
        #define SEL_GPIO_BITS                                (6)

#define GPIO_IRQDSEL                                         *((volatile uint32_t *)0x4000BC24u)
#define GPIO_IRQDSEL_REG                                     *((volatile uint32_t *)0x4000BC24u)
#define GPIO_IRQDSEL_ADDR                                    (0x4000BC24u)
#define GPIO_IRQDSEL_RESET                                   (0x00000010u)

/* SEL_GPIO field */
        #define SEL_GPIO                                     (0x0000003Fu)
        #define SEL_GPIO_MASK                                (0x0000003Fu)
        #define SEL_GPIO_BIT                                 (0)
        #define SEL_GPIO_BITS                                (6)

#define GPIO_WAKEFILT                                        *((volatile uint32_t *)0x4000BC28u)
#define GPIO_WAKEFILT_REG                                    *((volatile uint32_t *)0x4000BC28u)
#define GPIO_WAKEFILT_ADDR                                   (0x4000BC28u)
#define GPIO_WAKEFILT_RESET                                  (0x00000000u)

/* IRQD_WAKE_FILTER field */
        #define IRQD_WAKE_FILTER                             (0x00000008u)
        #define IRQD_WAKE_FILTER_MASK                        (0x00000008u)
        #define IRQD_WAKE_FILTER_BIT                         (3)
        #define IRQD_WAKE_FILTER_BITS                        (1)

/* SC2_WAKE_FILTER field */
        #define SC2_WAKE_FILTER                              (0x00000004u)
        #define SC2_WAKE_FILTER_MASK                         (0x00000004u)
        #define SC2_WAKE_FILTER_BIT                          (2)
        #define SC2_WAKE_FILTER_BITS                         (1)

/* SC1_WAKE_FILTER field */
        #define SC1_WAKE_FILTER                              (0x00000002u)
        #define SC1_WAKE_FILTER_MASK                         (0x00000002u)
        #define SC1_WAKE_FILTER_BIT                          (1)
        #define SC1_WAKE_FILTER_BITS                         (1)

/* GPIO_WAKE_FILTER field */
        #define GPIO_WAKE_FILTER                             (0x00000001u)
        #define GPIO_WAKE_FILTER_MASK                        (0x00000001u)
        #define GPIO_WAKE_FILTER_BIT                         (0)
        #define GPIO_WAKE_FILTER_BITS                        (1)

/* SERIAL block */
#define BLOCK_SERIAL_BASE                                    (0x4000C000u)
#define BLOCK_SERIAL_END                                     (0x4000C870u)
#define BLOCK_SERIAL_SIZE                                    (BLOCK_SERIAL_END - BLOCK_SERIAL_BASE + 1)

#define SC2_RXBEGA                                           *((volatile uint32_t *)0x4000C000u)
#define SC2_RXBEGA_REG                                       *((volatile uint32_t *)0x4000C000u)
#define SC2_RXBEGA_ADDR                                      (0x4000C000u)
#define SC2_RXBEGA_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC2_RXBEGA_FIXED                             (0xFFFF0000u)
        #define SC2_RXBEGA_FIXED_MASK                        (0xFFFF0000u)
        #define SC2_RXBEGA_FIXED_BIT                         (16)
        #define SC2_RXBEGA_FIXED_BITS                        (16)

/* SC_RXBEGA field */
        #define SC_RXBEGA                                    (0x0000FFFFu)
        #define SC_RXBEGA_MASK                               (0x0000FFFFu)
        #define SC_RXBEGA_BIT                                (0)
        #define SC_RXBEGA_BITS                               (16)

#define SC2_RXENDA                                           *((volatile uint32_t *)0x4000C004u)
#define SC2_RXENDA_REG                                       *((volatile uint32_t *)0x4000C004u)
#define SC2_RXENDA_ADDR                                      (0x4000C004u)
#define SC2_RXENDA_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC2_RXENDA_FIXED                             (0xFFFF0000u)
        #define SC2_RXENDA_FIXED_MASK                        (0xFFFF0000u)
        #define SC2_RXENDA_FIXED_BIT                         (16)
        #define SC2_RXENDA_FIXED_BITS                        (16)

/* SC_RXENDA field */
        #define SC_RXENDA                                    (0x0000FFFFu)
        #define SC_RXENDA_MASK                               (0x0000FFFFu)
        #define SC_RXENDA_BIT                                (0)
        #define SC_RXENDA_BITS                               (16)

#define SC2_RXBEGB                                           *((volatile uint32_t *)0x4000C008u)
#define SC2_RXBEGB_REG                                       *((volatile uint32_t *)0x4000C008u)
#define SC2_RXBEGB_ADDR                                      (0x4000C008u)
#define SC2_RXBEGB_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC2_RXBEGB_FIXED                             (0xFFFF0000u)
        #define SC2_RXBEGB_FIXED_MASK                        (0xFFFF0000u)
        #define SC2_RXBEGB_FIXED_BIT                         (16)
        #define SC2_RXBEGB_FIXED_BITS                        (16)

/* SC_RXBEGB field */
        #define SC_RXBEGB                                    (0x0000FFFFu)
        #define SC_RXBEGB_MASK                               (0x0000FFFFu)
        #define SC_RXBEGB_BIT                                (0)
        #define SC_RXBEGB_BITS                               (16)

#define SC2_RXENDB                                           *((volatile uint32_t *)0x4000C00Cu)
#define SC2_RXENDB_REG                                       *((volatile uint32_t *)0x4000C00Cu)
#define SC2_RXENDB_ADDR                                      (0x4000C00Cu)
#define SC2_RXENDB_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC2_RXENDB_FIXED                             (0xFFFF0000u)
        #define SC2_RXENDB_FIXED_MASK                        (0xFFFF0000u)
        #define SC2_RXENDB_FIXED_BIT                         (16)
        #define SC2_RXENDB_FIXED_BITS                        (16)

/* SC_RXENDB field */
        #define SC_RXENDB                                    (0x0000FFFFu)
        #define SC_RXENDB_MASK                               (0x0000FFFFu)
        #define SC_RXENDB_BIT                                (0)
        #define SC_RXENDB_BITS                               (16)

#define SC2_TXBEGA                                           *((volatile uint32_t *)0x4000C010u)
#define SC2_TXBEGA_REG                                       *((volatile uint32_t *)0x4000C010u)
#define SC2_TXBEGA_ADDR                                      (0x4000C010u)
#define SC2_TXBEGA_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC2_TXBEGA_FIXED                             (0xFFFF0000u)
        #define SC2_TXBEGA_FIXED_MASK                        (0xFFFF0000u)
        #define SC2_TXBEGA_FIXED_BIT                         (16)
        #define SC2_TXBEGA_FIXED_BITS                        (16)

/* SC_TXBEGA field */
        #define SC_TXBEGA                                    (0x0000FFFFu)
        #define SC_TXBEGA_MASK                               (0x0000FFFFu)
        #define SC_TXBEGA_BIT                                (0)
        #define SC_TXBEGA_BITS                               (16)

#define SC2_TXENDA                                           *((volatile uint32_t *)0x4000C014u)
#define SC2_TXENDA_REG                                       *((volatile uint32_t *)0x4000C014u)
#define SC2_TXENDA_ADDR                                      (0x4000C014u)
#define SC2_TXENDA_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC2_TXENDA_FIXED                             (0xFFFF0000u)
        #define SC2_TXENDA_FIXED_MASK                        (0xFFFF0000u)
        #define SC2_TXENDA_FIXED_BIT                         (16)
        #define SC2_TXENDA_FIXED_BITS                        (16)

/* SC_TXENDA field */
        #define SC_TXENDA                                    (0x0000FFFFu)
        #define SC_TXENDA_MASK                               (0x0000FFFFu)
        #define SC_TXENDA_BIT                                (0)
        #define SC_TXENDA_BITS                               (16)

#define SC2_TXBEGB                                           *((volatile uint32_t *)0x4000C018u)
#define SC2_TXBEGB_REG                                       *((volatile uint32_t *)0x4000C018u)
#define SC2_TXBEGB_ADDR                                      (0x4000C018u)
#define SC2_TXBEGB_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC2_TXBEGB_FIXED                             (0xFFFF0000u)
        #define SC2_TXBEGB_FIXED_MASK                        (0xFFFF0000u)
        #define SC2_TXBEGB_FIXED_BIT                         (16)
        #define SC2_TXBEGB_FIXED_BITS                        (16)

/* SC_TXBEGB field */
        #define SC_TXBEGB                                    (0x0000FFFFu)
        #define SC_TXBEGB_MASK                               (0x0000FFFFu)
        #define SC_TXBEGB_BIT                                (0)
        #define SC_TXBEGB_BITS                               (16)

#define SC2_TXENDB                                           *((volatile uint32_t *)0x4000C01Cu)
#define SC2_TXENDB_REG                                       *((volatile uint32_t *)0x4000C01Cu)
#define SC2_TXENDB_ADDR                                      (0x4000C01Cu)
#define SC2_TXENDB_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC2_TXENDB_FIXED                             (0xFFFF0000u)
        #define SC2_TXENDB_FIXED_MASK                        (0xFFFF0000u)
        #define SC2_TXENDB_FIXED_BIT                         (16)
        #define SC2_TXENDB_FIXED_BITS                        (16)

/* SC_TXENDB field */
        #define SC_TXENDB                                    (0x0000FFFFu)
        #define SC_TXENDB_MASK                               (0x0000FFFFu)
        #define SC_TXENDB_BIT                                (0)
        #define SC_TXENDB_BITS                               (16)

#define SC2_RXCNTA                                           *((volatile uint32_t *)0x4000C020u)
#define SC2_RXCNTA_REG                                       *((volatile uint32_t *)0x4000C020u)
#define SC2_RXCNTA_ADDR                                      (0x4000C020u)
#define SC2_RXCNTA_RESET                                     (0x00000000u)

/* SC_RXCNTA field */
        #define SC_RXCNTA                                    (0x0000FFFFu)
        #define SC_RXCNTA_MASK                               (0x0000FFFFu)
        #define SC_RXCNTA_BIT                                (0)
        #define SC_RXCNTA_BITS                               (16)

#define SC2_RXCNTB                                           *((volatile uint32_t *)0x4000C024u)
#define SC2_RXCNTB_REG                                       *((volatile uint32_t *)0x4000C024u)
#define SC2_RXCNTB_ADDR                                      (0x4000C024u)
#define SC2_RXCNTB_RESET                                     (0x00000000u)

/* SC_RXCNTB field */
        #define SC_RXCNTB                                    (0x0000FFFFu)
        #define SC_RXCNTB_MASK                               (0x0000FFFFu)
        #define SC_RXCNTB_BIT                                (0)
        #define SC_RXCNTB_BITS                               (16)

#define SC2_TXCNT                                            *((volatile uint32_t *)0x4000C028u)
#define SC2_TXCNT_REG                                        *((volatile uint32_t *)0x4000C028u)
#define SC2_TXCNT_ADDR                                       (0x4000C028u)
#define SC2_TXCNT_RESET                                      (0x00000000u)

/* SC_TXCNT field */
        #define SC_TXCNT                                     (0x0000FFFFu)
        #define SC_TXCNT_MASK                                (0x0000FFFFu)
        #define SC_TXCNT_BIT                                 (0)
        #define SC_TXCNT_BITS                                (16)

#define SC2_DMASTAT                                          *((volatile uint32_t *)0x4000C02Cu)
#define SC2_DMASTAT_REG                                      *((volatile uint32_t *)0x4000C02Cu)
#define SC2_DMASTAT_ADDR                                     (0x4000C02Cu)
#define SC2_DMASTAT_RESET                                    (0x00000000u)

/* SC_RXSSEL field */
        #define SC_RXSSEL                                    (0x00001C00u)
        #define SC_RXSSEL_MASK                               (0x00001C00u)
        #define SC_RXSSEL_BIT                                (10)
        #define SC_RXSSEL_BITS                               (3)

/* SC_RXOVFB field */
        #define SC_RXOVFB                                    (0x00000020u)
        #define SC_RXOVFB_MASK                               (0x00000020u)
        #define SC_RXOVFB_BIT                                (5)
        #define SC_RXOVFB_BITS                               (1)

/* SC_RXOVFA field */
        #define SC_RXOVFA                                    (0x00000010u)
        #define SC_RXOVFA_MASK                               (0x00000010u)
        #define SC_RXOVFA_BIT                                (4)
        #define SC_RXOVFA_BITS                               (1)

/* SC_TXACTB field */
        #define SC_TXACTB                                    (0x00000008u)
        #define SC_TXACTB_MASK                               (0x00000008u)
        #define SC_TXACTB_BIT                                (3)
        #define SC_TXACTB_BITS                               (1)

/* SC_TXACTA field */
        #define SC_TXACTA                                    (0x00000004u)
        #define SC_TXACTA_MASK                               (0x00000004u)
        #define SC_TXACTA_BIT                                (2)
        #define SC_TXACTA_BITS                               (1)

/* SC_RXACTB field */
        #define SC_RXACTB                                    (0x00000002u)
        #define SC_RXACTB_MASK                               (0x00000002u)
        #define SC_RXACTB_BIT                                (1)
        #define SC_RXACTB_BITS                               (1)

/* SC_RXACTA field */
        #define SC_RXACTA                                    (0x00000001u)
        #define SC_RXACTA_MASK                               (0x00000001u)
        #define SC_RXACTA_BIT                                (0)
        #define SC_RXACTA_BITS                               (1)

#define SC2_DMACTRL                                          *((volatile uint32_t *)0x4000C030u)
#define SC2_DMACTRL_REG                                      *((volatile uint32_t *)0x4000C030u)
#define SC2_DMACTRL_ADDR                                     (0x4000C030u)
#define SC2_DMACTRL_RESET                                    (0x00000000u)

/* SC_TXDMARST field */
        #define SC_TXDMARST                                  (0x00000020u)
        #define SC_TXDMARST_MASK                             (0x00000020u)
        #define SC_TXDMARST_BIT                              (5)
        #define SC_TXDMARST_BITS                             (1)

/* SC_RXDMARST field */
        #define SC_RXDMARST                                  (0x00000010u)
        #define SC_RXDMARST_MASK                             (0x00000010u)
        #define SC_RXDMARST_BIT                              (4)
        #define SC_RXDMARST_BITS                             (1)

/* SC_TXLODB field */
        #define SC_TXLODB                                    (0x00000008u)
        #define SC_TXLODB_MASK                               (0x00000008u)
        #define SC_TXLODB_BIT                                (3)
        #define SC_TXLODB_BITS                               (1)

/* SC_TXLODA field */
        #define SC_TXLODA                                    (0x00000004u)
        #define SC_TXLODA_MASK                               (0x00000004u)
        #define SC_TXLODA_BIT                                (2)
        #define SC_TXLODA_BITS                               (1)

/* SC_RXLODB field */
        #define SC_RXLODB                                    (0x00000002u)
        #define SC_RXLODB_MASK                               (0x00000002u)
        #define SC_RXLODB_BIT                                (1)
        #define SC_RXLODB_BITS                               (1)

/* SC_RXLODA field */
        #define SC_RXLODA                                    (0x00000001u)
        #define SC_RXLODA_MASK                               (0x00000001u)
        #define SC_RXLODA_BIT                                (0)
        #define SC_RXLODA_BITS                               (1)

#define SC2_RXERRA                                           *((volatile uint32_t *)0x4000C034u)
#define SC2_RXERRA_REG                                       *((volatile uint32_t *)0x4000C034u)
#define SC2_RXERRA_ADDR                                      (0x4000C034u)
#define SC2_RXERRA_RESET                                     (0x00000000u)

/* SC_RXERRA field */
        #define SC_RXERRA                                    (0x0000FFFFu)
        #define SC_RXERRA_MASK                               (0x0000FFFFu)
        #define SC_RXERRA_BIT                                (0)
        #define SC_RXERRA_BITS                               (16)

#define SC2_RXERRB                                           *((volatile uint32_t *)0x4000C038u)
#define SC2_RXERRB_REG                                       *((volatile uint32_t *)0x4000C038u)
#define SC2_RXERRB_ADDR                                      (0x4000C038u)
#define SC2_RXERRB_RESET                                     (0x00000000u)

/* SC_RXERRB field */
        #define SC_RXERRB                                    (0x0000FFFFu)
        #define SC_RXERRB_MASK                               (0x0000FFFFu)
        #define SC_RXERRB_BIT                                (0)
        #define SC_RXERRB_BITS                               (16)

#define SC2_DATA                                             *((volatile uint32_t *)0x4000C03Cu)
#define SC2_DATA_REG                                         *((volatile uint32_t *)0x4000C03Cu)
#define SC2_DATA_ADDR                                        (0x4000C03Cu)
#define SC2_DATA_RESET                                       (0x00000000u)

/* SC_DATA field */
        #define SC_DATA                                      (0x000000FFu)
        #define SC_DATA_MASK                                 (0x000000FFu)
        #define SC_DATA_BIT                                  (0)
        #define SC_DATA_BITS                                 (8)

#define SC2_SPISTAT                                          *((volatile uint32_t *)0x4000C040u)
#define SC2_SPISTAT_REG                                      *((volatile uint32_t *)0x4000C040u)
#define SC2_SPISTAT_ADDR                                     (0x4000C040u)
#define SC2_SPISTAT_RESET                                    (0x00000000u)

/* SC_SPITXIDLE field */
        #define SC_SPITXIDLE                                 (0x00000008u)
        #define SC_SPITXIDLE_MASK                            (0x00000008u)
        #define SC_SPITXIDLE_BIT                             (3)
        #define SC_SPITXIDLE_BITS                            (1)

/* SC_SPITXFREE field */
        #define SC_SPITXFREE                                 (0x00000004u)
        #define SC_SPITXFREE_MASK                            (0x00000004u)
        #define SC_SPITXFREE_BIT                             (2)
        #define SC_SPITXFREE_BITS                            (1)

/* SC_SPIRXVAL field */
        #define SC_SPIRXVAL                                  (0x00000002u)
        #define SC_SPIRXVAL_MASK                             (0x00000002u)
        #define SC_SPIRXVAL_BIT                              (1)
        #define SC_SPIRXVAL_BITS                             (1)

/* SC_SPIRXOVF field */
        #define SC_SPIRXOVF                                  (0x00000001u)
        #define SC_SPIRXOVF_MASK                             (0x00000001u)
        #define SC_SPIRXOVF_BIT                              (0)
        #define SC_SPIRXOVF_BITS                             (1)

#define SC2_TWISTAT                                          *((volatile uint32_t *)0x4000C044u)
#define SC2_TWISTAT_REG                                      *((volatile uint32_t *)0x4000C044u)
#define SC2_TWISTAT_ADDR                                     (0x4000C044u)
#define SC2_TWISTAT_RESET                                    (0x00000000u)

/* SC_TWICMDFIN field */
        #define SC_TWICMDFIN                                 (0x00000008u)
        #define SC_TWICMDFIN_MASK                            (0x00000008u)
        #define SC_TWICMDFIN_BIT                             (3)
        #define SC_TWICMDFIN_BITS                            (1)

/* SC_TWIRXFIN field */
        #define SC_TWIRXFIN                                  (0x00000004u)
        #define SC_TWIRXFIN_MASK                             (0x00000004u)
        #define SC_TWIRXFIN_BIT                              (2)
        #define SC_TWIRXFIN_BITS                             (1)

/* SC_TWITXFIN field */
        #define SC_TWITXFIN                                  (0x00000002u)
        #define SC_TWITXFIN_MASK                             (0x00000002u)
        #define SC_TWITXFIN_BIT                              (1)
        #define SC_TWITXFIN_BITS                             (1)

/* SC_TWIRXNAK field */
        #define SC_TWIRXNAK                                  (0x00000001u)
        #define SC_TWIRXNAK_MASK                             (0x00000001u)
        #define SC_TWIRXNAK_BIT                              (0)
        #define SC_TWIRXNAK_BITS                             (1)

#define SC2_TWICTRL1                                         *((volatile uint32_t *)0x4000C04Cu)
#define SC2_TWICTRL1_REG                                     *((volatile uint32_t *)0x4000C04Cu)
#define SC2_TWICTRL1_ADDR                                    (0x4000C04Cu)
#define SC2_TWICTRL1_RESET                                   (0x00000000u)

/* SC_TWISTOP field */
        #define SC_TWISTOP                                   (0x00000008u)
        #define SC_TWISTOP_MASK                              (0x00000008u)
        #define SC_TWISTOP_BIT                               (3)
        #define SC_TWISTOP_BITS                              (1)

/* SC_TWISTART field */
        #define SC_TWISTART                                  (0x00000004u)
        #define SC_TWISTART_MASK                             (0x00000004u)
        #define SC_TWISTART_BIT                              (2)
        #define SC_TWISTART_BITS                             (1)

/* SC_TWISEND field */
        #define SC_TWISEND                                   (0x00000002u)
        #define SC_TWISEND_MASK                              (0x00000002u)
        #define SC_TWISEND_BIT                               (1)
        #define SC_TWISEND_BITS                              (1)

/* SC_TWIRECV field */
        #define SC_TWIRECV                                   (0x00000001u)
        #define SC_TWIRECV_MASK                              (0x00000001u)
        #define SC_TWIRECV_BIT                               (0)
        #define SC_TWIRECV_BITS                              (1)

#define SC2_TWICTRL2                                         *((volatile uint32_t *)0x4000C050u)
#define SC2_TWICTRL2_REG                                     *((volatile uint32_t *)0x4000C050u)
#define SC2_TWICTRL2_ADDR                                    (0x4000C050u)
#define SC2_TWICTRL2_RESET                                   (0x00000000u)

/* SC_TWIACK field */
        #define SC_TWIACK                                    (0x00000001u)
        #define SC_TWIACK_MASK                               (0x00000001u)
        #define SC_TWIACK_BIT                                (0)
        #define SC_TWIACK_BITS                               (1)

#define SC2_MODE                                             *((volatile uint32_t *)0x4000C054u)
#define SC2_MODE_REG                                         *((volatile uint32_t *)0x4000C054u)
#define SC2_MODE_ADDR                                        (0x4000C054u)
#define SC2_MODE_RESET                                       (0x00000000u)

/* SC_MODE field */
        #define SC_MODE                                      (0x00000003u)
        #define SC_MODE_MASK                                 (0x00000003u)
        #define SC_MODE_BIT                                  (0)
        #define SC_MODE_BITS                                 (2)

/* SC_MODE Bit Field Values */
                #define SC2_MODE_DISABLED                    (0)
                #define SC2_MODE_SPI                         (2)
                #define SC2_MODE_I2C                         (3)

#define SC2_SPICFG                                           *((volatile uint32_t *)0x4000C058u)
#define SC2_SPICFG_REG                                       *((volatile uint32_t *)0x4000C058u)
#define SC2_SPICFG_ADDR                                      (0x4000C058u)
#define SC2_SPICFG_RESET                                     (0x00000000u)

/* SC_SPIRXDRV field */
        #define SC_SPIRXDRV                                  (0x00000020u)
        #define SC_SPIRXDRV_MASK                             (0x00000020u)
        #define SC_SPIRXDRV_BIT                              (5)
        #define SC_SPIRXDRV_BITS                             (1)

/* SC_SPIMST field */
        #define SC_SPIMST                                    (0x00000010u)
        #define SC_SPIMST_MASK                               (0x00000010u)
        #define SC_SPIMST_BIT                                (4)
        #define SC_SPIMST_BITS                               (1)

/* SC_SPIRPT field */
        #define SC_SPIRPT                                    (0x00000008u)
        #define SC_SPIRPT_MASK                               (0x00000008u)
        #define SC_SPIRPT_BIT                                (3)
        #define SC_SPIRPT_BITS                               (1)

/* SC_SPIORD field */
        #define SC_SPIORD                                    (0x00000004u)
        #define SC_SPIORD_MASK                               (0x00000004u)
        #define SC_SPIORD_BIT                                (2)
        #define SC_SPIORD_BITS                               (1)

/* SC_SPIPHA field */
        #define SC_SPIPHA                                    (0x00000002u)
        #define SC_SPIPHA_MASK                               (0x00000002u)
        #define SC_SPIPHA_BIT                                (1)
        #define SC_SPIPHA_BITS                               (1)

/* SC_SPIPOL field */
        #define SC_SPIPOL                                    (0x00000001u)
        #define SC_SPIPOL_MASK                               (0x00000001u)
        #define SC_SPIPOL_BIT                                (0)
        #define SC_SPIPOL_BITS                               (1)

#define SC2_RATELIN                                          *((volatile uint32_t *)0x4000C060u)
#define SC2_RATELIN_REG                                      *((volatile uint32_t *)0x4000C060u)
#define SC2_RATELIN_ADDR                                     (0x4000C060u)
#define SC2_RATELIN_RESET                                    (0x00000000u)

/* SC_RATELIN field */
        #define SC_RATELIN                                   (0x0000000Fu)
        #define SC_RATELIN_MASK                              (0x0000000Fu)
        #define SC_RATELIN_BIT                               (0)
        #define SC_RATELIN_BITS                              (4)

#define SC2_RATEEXP                                          *((volatile uint32_t *)0x4000C064u)
#define SC2_RATEEXP_REG                                      *((volatile uint32_t *)0x4000C064u)
#define SC2_RATEEXP_ADDR                                     (0x4000C064u)
#define SC2_RATEEXP_RESET                                    (0x00000000u)

/* SC_RATEEXP field */
        #define SC_RATEEXP                                   (0x0000000Fu)
        #define SC_RATEEXP_MASK                              (0x0000000Fu)
        #define SC_RATEEXP_BIT                               (0)
        #define SC_RATEEXP_BITS                              (4)

#define SC2_RXCNTSAVED                                       *((volatile uint32_t *)0x4000C070u)
#define SC2_RXCNTSAVED_REG                                   *((volatile uint32_t *)0x4000C070u)
#define SC2_RXCNTSAVED_ADDR                                  (0x4000C070u)
#define SC2_RXCNTSAVED_RESET                                 (0x00000000u)

/* SC_RXCNTSAVED field */
        #define SC_RXCNTSAVED                                (0x0000FFFFu)
        #define SC_RXCNTSAVED_MASK                           (0x0000FFFFu)
        #define SC_RXCNTSAVED_BIT                            (0)
        #define SC_RXCNTSAVED_BITS                           (16)

#define SC1_RXBEGA                                           *((volatile uint32_t *)0x4000C800u)
#define SC1_RXBEGA_REG                                       *((volatile uint32_t *)0x4000C800u)
#define SC1_RXBEGA_ADDR                                      (0x4000C800u)
#define SC1_RXBEGA_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC1_RXBEGA_FIXED                             (0xFFFF0000u)
        #define SC1_RXBEGA_FIXED_MASK                        (0xFFFF0000u)
        #define SC1_RXBEGA_FIXED_BIT                         (16)
        #define SC1_RXBEGA_FIXED_BITS                        (16)

/* SC_RXBEGA field */
        #define SC_RXBEGA                                    (0x0000FFFFu)
        #define SC_RXBEGA_MASK                               (0x0000FFFFu)
        #define SC_RXBEGA_BIT                                (0)
        #define SC_RXBEGA_BITS                               (16)

#define SC1_RXENDA                                           *((volatile uint32_t *)0x4000C804u)
#define SC1_RXENDA_REG                                       *((volatile uint32_t *)0x4000C804u)
#define SC1_RXENDA_ADDR                                      (0x4000C804u)
#define SC1_RXENDA_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC1_RXENDA_FIXED                             (0xFFFF0000u)
        #define SC1_RXENDA_FIXED_MASK                        (0xFFFF0000u)
        #define SC1_RXENDA_FIXED_BIT                         (16)
        #define SC1_RXENDA_FIXED_BITS                        (16)

/* SC_RXENDA field */
        #define SC_RXENDA                                    (0x0000FFFFu)
        #define SC_RXENDA_MASK                               (0x0000FFFFu)
        #define SC_RXENDA_BIT                                (0)
        #define SC_RXENDA_BITS                               (16)

#define SC1_RXBEGB                                           *((volatile uint32_t *)0x4000C808u)
#define SC1_RXBEGB_REG                                       *((volatile uint32_t *)0x4000C808u)
#define SC1_RXBEGB_ADDR                                      (0x4000C808u)
#define SC1_RXBEGB_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC1_RXBEGB_FIXED                             (0xFFFF0000u)
        #define SC1_RXBEGB_FIXED_MASK                        (0xFFFF0000u)
        #define SC1_RXBEGB_FIXED_BIT                         (16)
        #define SC1_RXBEGB_FIXED_BITS                        (16)

/* SC_RXBEGB field */
        #define SC_RXBEGB                                    (0x0000FFFFu)
        #define SC_RXBEGB_MASK                               (0x0000FFFFu)
        #define SC_RXBEGB_BIT                                (0)
        #define SC_RXBEGB_BITS                               (16)

#define SC1_RXENDB                                           *((volatile uint32_t *)0x4000C80Cu)
#define SC1_RXENDB_REG                                       *((volatile uint32_t *)0x4000C80Cu)
#define SC1_RXENDB_ADDR                                      (0x4000C80Cu)
#define SC1_RXENDB_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC1_RXENDB_FIXED                             (0xFFFF0000u)
        #define SC1_RXENDB_FIXED_MASK                        (0xFFFF0000u)
        #define SC1_RXENDB_FIXED_BIT                         (16)
        #define SC1_RXENDB_FIXED_BITS                        (16)

/* SC_RXENDB field */
        #define SC_RXENDB                                    (0x0000FFFFu)
        #define SC_RXENDB_MASK                               (0x0000FFFFu)
        #define SC_RXENDB_BIT                                (0)
        #define SC_RXENDB_BITS                               (16)

#define SC1_TXBEGA                                           *((volatile uint32_t *)0x4000C810u)
#define SC1_TXBEGA_REG                                       *((volatile uint32_t *)0x4000C810u)
#define SC1_TXBEGA_ADDR                                      (0x4000C810u)
#define SC1_TXBEGA_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC1_TXBEGA_FIXED                             (0xFFFF0000u)
        #define SC1_TXBEGA_FIXED_MASK                        (0xFFFF0000u)
        #define SC1_TXBEGA_FIXED_BIT                         (16)
        #define SC1_TXBEGA_FIXED_BITS                        (16)

/* SC_TXBEGA field */
        #define SC_TXBEGA                                    (0x0000FFFFu)
        #define SC_TXBEGA_MASK                               (0x0000FFFFu)
        #define SC_TXBEGA_BIT                                (0)
        #define SC_TXBEGA_BITS                               (16)

#define SC1_TXENDA                                           *((volatile uint32_t *)0x4000C814u)
#define SC1_TXENDA_REG                                       *((volatile uint32_t *)0x4000C814u)
#define SC1_TXENDA_ADDR                                      (0x4000C814u)
#define SC1_TXENDA_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC1_TXENDA_FIXED                             (0xFFFF0000u)
        #define SC1_TXENDA_FIXED_MASK                        (0xFFFF0000u)
        #define SC1_TXENDA_FIXED_BIT                         (16)
        #define SC1_TXENDA_FIXED_BITS                        (16)

/* SC_TXENDA field */
        #define SC_TXENDA                                    (0x0000FFFFu)
        #define SC_TXENDA_MASK                               (0x0000FFFFu)
        #define SC_TXENDA_BIT                                (0)
        #define SC_TXENDA_BITS                               (16)

#define SC1_TXBEGB                                           *((volatile uint32_t *)0x4000C818u)
#define SC1_TXBEGB_REG                                       *((volatile uint32_t *)0x4000C818u)
#define SC1_TXBEGB_ADDR                                      (0x4000C818u)
#define SC1_TXBEGB_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC1_TXBEGB_FIXED                             (0xFFFF0000u)
        #define SC1_TXBEGB_FIXED_MASK                        (0xFFFF0000u)
        #define SC1_TXBEGB_FIXED_BIT                         (16)
        #define SC1_TXBEGB_FIXED_BITS                        (16)

/* SC_TXBEGB field */
        #define SC_TXBEGB                                    (0x0000FFFFu)
        #define SC_TXBEGB_MASK                               (0x0000FFFFu)
        #define SC_TXBEGB_BIT                                (0)
        #define SC_TXBEGB_BITS                               (16)

#define SC1_TXENDB                                           *((volatile uint32_t *)0x4000C81Cu)
#define SC1_TXENDB_REG                                       *((volatile uint32_t *)0x4000C81Cu)
#define SC1_TXENDB_ADDR                                      (0x4000C81Cu)
#define SC1_TXENDB_RESET                                     (0x20000000u)

/* FIXED field */
        #define SC1_TXENDB_FIXED                             (0xFFFF0000u)
        #define SC1_TXENDB_FIXED_MASK                        (0xFFFF0000u)
        #define SC1_TXENDB_FIXED_BIT                         (16)
        #define SC1_TXENDB_FIXED_BITS                        (16)

/* SC_TXENDB field */
        #define SC_TXENDB                                    (0x0000FFFFu)
        #define SC_TXENDB_MASK                               (0x0000FFFFu)
        #define SC_TXENDB_BIT                                (0)
        #define SC_TXENDB_BITS                               (16)

#define SC1_RXCNTA                                           *((volatile uint32_t *)0x4000C820u)
#define SC1_RXCNTA_REG                                       *((volatile uint32_t *)0x4000C820u)
#define SC1_RXCNTA_ADDR                                      (0x4000C820u)
#define SC1_RXCNTA_RESET                                     (0x00000000u)

/* SC_RXCNTA field */
        #define SC_RXCNTA                                    (0x0000FFFFu)
        #define SC_RXCNTA_MASK                               (0x0000FFFFu)
        #define SC_RXCNTA_BIT                                (0)
        #define SC_RXCNTA_BITS                               (16)

#define SC1_RXCNTB                                           *((volatile uint32_t *)0x4000C824u)
#define SC1_RXCNTB_REG                                       *((volatile uint32_t *)0x4000C824u)
#define SC1_RXCNTB_ADDR                                      (0x4000C824u)
#define SC1_RXCNTB_RESET                                     (0x00000000u)

/* SC_RXCNTB field */
        #define SC_RXCNTB                                    (0x0000FFFFu)
        #define SC_RXCNTB_MASK                               (0x0000FFFFu)
        #define SC_RXCNTB_BIT                                (0)
        #define SC_RXCNTB_BITS                               (16)

#define SC1_TXCNT                                            *((volatile uint32_t *)0x4000C828u)
#define SC1_TXCNT_REG                                        *((volatile uint32_t *)0x4000C828u)
#define SC1_TXCNT_ADDR                                       (0x4000C828u)
#define SC1_TXCNT_RESET                                      (0x00000000u)

/* SC_TXCNT field */
        #define SC_TXCNT                                     (0x0000FFFFu)
        #define SC_TXCNT_MASK                                (0x0000FFFFu)
        #define SC_TXCNT_BIT                                 (0)
        #define SC_TXCNT_BITS                                (16)

#define SC1_DMASTAT                                          *((volatile uint32_t *)0x4000C82Cu)
#define SC1_DMASTAT_REG                                      *((volatile uint32_t *)0x4000C82Cu)
#define SC1_DMASTAT_ADDR                                     (0x4000C82Cu)
#define SC1_DMASTAT_RESET                                    (0x00000000u)

/* SC_RXSSEL field */
        #define SC_RXSSEL                                    (0x00001C00u)
        #define SC_RXSSEL_MASK                               (0x00001C00u)
        #define SC_RXSSEL_BIT                                (10)
        #define SC_RXSSEL_BITS                               (3)

/* SC_RXFRMB field */
        #define SC_RXFRMB                                    (0x00000200u)
        #define SC_RXFRMB_MASK                               (0x00000200u)
        #define SC_RXFRMB_BIT                                (9)
        #define SC_RXFRMB_BITS                               (1)

/* SC_RXFRMA field */
        #define SC_RXFRMA                                    (0x00000100u)
        #define SC_RXFRMA_MASK                               (0x00000100u)
        #define SC_RXFRMA_BIT                                (8)
        #define SC_RXFRMA_BITS                               (1)

/* SC_RXPARB field */
        #define SC_RXPARB                                    (0x00000080u)
        #define SC_RXPARB_MASK                               (0x00000080u)
        #define SC_RXPARB_BIT                                (7)
        #define SC_RXPARB_BITS                               (1)

/* SC_RXPARA field */
        #define SC_RXPARA                                    (0x00000040u)
        #define SC_RXPARA_MASK                               (0x00000040u)
        #define SC_RXPARA_BIT                                (6)
        #define SC_RXPARA_BITS                               (1)

/* SC_RXOVFB field */
        #define SC_RXOVFB                                    (0x00000020u)
        #define SC_RXOVFB_MASK                               (0x00000020u)
        #define SC_RXOVFB_BIT                                (5)
        #define SC_RXOVFB_BITS                               (1)

/* SC_RXOVFA field */
        #define SC_RXOVFA                                    (0x00000010u)
        #define SC_RXOVFA_MASK                               (0x00000010u)
        #define SC_RXOVFA_BIT                                (4)
        #define SC_RXOVFA_BITS                               (1)

/* SC_TXACTB field */
        #define SC_TXACTB                                    (0x00000008u)
        #define SC_TXACTB_MASK                               (0x00000008u)
        #define SC_TXACTB_BIT                                (3)
        #define SC_TXACTB_BITS                               (1)

/* SC_TXACTA field */
        #define SC_TXACTA                                    (0x00000004u)
        #define SC_TXACTA_MASK                               (0x00000004u)
        #define SC_TXACTA_BIT                                (2)
        #define SC_TXACTA_BITS                               (1)

/* SC_RXACTB field */
        #define SC_RXACTB                                    (0x00000002u)
        #define SC_RXACTB_MASK                               (0x00000002u)
        #define SC_RXACTB_BIT                                (1)
        #define SC_RXACTB_BITS                               (1)

/* SC_RXACTA field */
        #define SC_RXACTA                                    (0x00000001u)
        #define SC_RXACTA_MASK                               (0x00000001u)
        #define SC_RXACTA_BIT                                (0)
        #define SC_RXACTA_BITS                               (1)

#define SC1_DMACTRL                                          *((volatile uint32_t *)0x4000C830u)
#define SC1_DMACTRL_REG                                      *((volatile uint32_t *)0x4000C830u)
#define SC1_DMACTRL_ADDR                                     (0x4000C830u)
#define SC1_DMACTRL_RESET                                    (0x00000000u)

/* SC_TXDMARST field */
        #define SC_TXDMARST                                  (0x00000020u)
        #define SC_TXDMARST_MASK                             (0x00000020u)
        #define SC_TXDMARST_BIT                              (5)
        #define SC_TXDMARST_BITS                             (1)

/* SC_RXDMARST field */
        #define SC_RXDMARST                                  (0x00000010u)
        #define SC_RXDMARST_MASK                             (0x00000010u)
        #define SC_RXDMARST_BIT                              (4)
        #define SC_RXDMARST_BITS                             (1)

/* SC_TXLODB field */
        #define SC_TXLODB                                    (0x00000008u)
        #define SC_TXLODB_MASK                               (0x00000008u)
        #define SC_TXLODB_BIT                                (3)
        #define SC_TXLODB_BITS                               (1)

/* SC_TXLODA field */
        #define SC_TXLODA                                    (0x00000004u)
        #define SC_TXLODA_MASK                               (0x00000004u)
        #define SC_TXLODA_BIT                                (2)
        #define SC_TXLODA_BITS                               (1)

/* SC_RXLODB field */
        #define SC_RXLODB                                    (0x00000002u)
        #define SC_RXLODB_MASK                               (0x00000002u)
        #define SC_RXLODB_BIT                                (1)
        #define SC_RXLODB_BITS                               (1)

/* SC_RXLODA field */
        #define SC_RXLODA                                    (0x00000001u)
        #define SC_RXLODA_MASK                               (0x00000001u)
        #define SC_RXLODA_BIT                                (0)
        #define SC_RXLODA_BITS                               (1)

#define SC1_RXERRA                                           *((volatile uint32_t *)0x4000C834u)
#define SC1_RXERRA_REG                                       *((volatile uint32_t *)0x4000C834u)
#define SC1_RXERRA_ADDR                                      (0x4000C834u)
#define SC1_RXERRA_RESET                                     (0x00000000u)

/* SC_RXERRA field */
        #define SC_RXERRA                                    (0x0000FFFFu)
        #define SC_RXERRA_MASK                               (0x0000FFFFu)
        #define SC_RXERRA_BIT                                (0)
        #define SC_RXERRA_BITS                               (16)

#define SC1_RXERRB                                           *((volatile uint32_t *)0x4000C838u)
#define SC1_RXERRB_REG                                       *((volatile uint32_t *)0x4000C838u)
#define SC1_RXERRB_ADDR                                      (0x4000C838u)
#define SC1_RXERRB_RESET                                     (0x00000000u)

/* SC_RXERRB field */
        #define SC_RXERRB                                    (0x0000FFFFu)
        #define SC_RXERRB_MASK                               (0x0000FFFFu)
        #define SC_RXERRB_BIT                                (0)
        #define SC_RXERRB_BITS                               (16)

#define SC1_DATA                                             *((volatile uint32_t *)0x4000C83Cu)
#define SC1_DATA_REG                                         *((volatile uint32_t *)0x4000C83Cu)
#define SC1_DATA_ADDR                                        (0x4000C83Cu)
#define SC1_DATA_RESET                                       (0x00000000u)

/* SC_DATA field */
        #define SC_DATA                                      (0x000000FFu)
        #define SC_DATA_MASK                                 (0x000000FFu)
        #define SC_DATA_BIT                                  (0)
        #define SC_DATA_BITS                                 (8)

#define SC1_SPISTAT                                          *((volatile uint32_t *)0x4000C840u)
#define SC1_SPISTAT_REG                                      *((volatile uint32_t *)0x4000C840u)
#define SC1_SPISTAT_ADDR                                     (0x4000C840u)
#define SC1_SPISTAT_RESET                                    (0x00000000u)

/* SC_SPITXIDLE field */
        #define SC_SPITXIDLE                                 (0x00000008u)
        #define SC_SPITXIDLE_MASK                            (0x00000008u)
        #define SC_SPITXIDLE_BIT                             (3)
        #define SC_SPITXIDLE_BITS                            (1)

/* SC_SPITXFREE field */
        #define SC_SPITXFREE                                 (0x00000004u)
        #define SC_SPITXFREE_MASK                            (0x00000004u)
        #define SC_SPITXFREE_BIT                             (2)
        #define SC_SPITXFREE_BITS                            (1)

/* SC_SPIRXVAL field */
        #define SC_SPIRXVAL                                  (0x00000002u)
        #define SC_SPIRXVAL_MASK                             (0x00000002u)
        #define SC_SPIRXVAL_BIT                              (1)
        #define SC_SPIRXVAL_BITS                             (1)

/* SC_SPIRXOVF field */
        #define SC_SPIRXOVF                                  (0x00000001u)
        #define SC_SPIRXOVF_MASK                             (0x00000001u)
        #define SC_SPIRXOVF_BIT                              (0)
        #define SC_SPIRXOVF_BITS                             (1)

#define SC1_TWISTAT                                          *((volatile uint32_t *)0x4000C844u)
#define SC1_TWISTAT_REG                                      *((volatile uint32_t *)0x4000C844u)
#define SC1_TWISTAT_ADDR                                     (0x4000C844u)
#define SC1_TWISTAT_RESET                                    (0x00000000u)

/* SC_TWICMDFIN field */
        #define SC_TWICMDFIN                                 (0x00000008u)
        #define SC_TWICMDFIN_MASK                            (0x00000008u)
        #define SC_TWICMDFIN_BIT                             (3)
        #define SC_TWICMDFIN_BITS                            (1)

/* SC_TWIRXFIN field */
        #define SC_TWIRXFIN                                  (0x00000004u)
        #define SC_TWIRXFIN_MASK                             (0x00000004u)
        #define SC_TWIRXFIN_BIT                              (2)
        #define SC_TWIRXFIN_BITS                             (1)

/* SC_TWITXFIN field */
        #define SC_TWITXFIN                                  (0x00000002u)
        #define SC_TWITXFIN_MASK                             (0x00000002u)
        #define SC_TWITXFIN_BIT                              (1)
        #define SC_TWITXFIN_BITS                             (1)

/* SC_TWIRXNAK field */
        #define SC_TWIRXNAK                                  (0x00000001u)
        #define SC_TWIRXNAK_MASK                             (0x00000001u)
        #define SC_TWIRXNAK_BIT                              (0)
        #define SC_TWIRXNAK_BITS                             (1)

#define SC1_UARTSTAT                                         *((volatile uint32_t *)0x4000C848u)
#define SC1_UARTSTAT_REG                                     *((volatile uint32_t *)0x4000C848u)
#define SC1_UARTSTAT_ADDR                                    (0x4000C848u)
#define SC1_UARTSTAT_RESET                                   (0x00000040u)

/* SC_UARTTXIDLE field */
        #define SC_UARTTXIDLE                                (0x00000040u)
        #define SC_UARTTXIDLE_MASK                           (0x00000040u)
        #define SC_UARTTXIDLE_BIT                            (6)
        #define SC_UARTTXIDLE_BITS                           (1)

/* SC_UARTPARERR field */
        #define SC_UARTPARERR                                (0x00000020u)
        #define SC_UARTPARERR_MASK                           (0x00000020u)
        #define SC_UARTPARERR_BIT                            (5)
        #define SC_UARTPARERR_BITS                           (1)

/* SC_UARTFRMERR field */
        #define SC_UARTFRMERR                                (0x00000010u)
        #define SC_UARTFRMERR_MASK                           (0x00000010u)
        #define SC_UARTFRMERR_BIT                            (4)
        #define SC_UARTFRMERR_BITS                           (1)

/* SC_UARTRXOVF field */
        #define SC_UARTRXOVF                                 (0x00000008u)
        #define SC_UARTRXOVF_MASK                            (0x00000008u)
        #define SC_UARTRXOVF_BIT                             (3)
        #define SC_UARTRXOVF_BITS                            (1)

/* SC_UARTTXFREE field */
        #define SC_UARTTXFREE                                (0x00000004u)
        #define SC_UARTTXFREE_MASK                           (0x00000004u)
        #define SC_UARTTXFREE_BIT                            (2)
        #define SC_UARTTXFREE_BITS                           (1)

/* SC_UARTRXVAL field */
        #define SC_UARTRXVAL                                 (0x00000002u)
        #define SC_UARTRXVAL_MASK                            (0x00000002u)
        #define SC_UARTRXVAL_BIT                             (1)
        #define SC_UARTRXVAL_BITS                            (1)

/* SC_UARTCTS field */
        #define SC_UARTCTS                                   (0x00000001u)
        #define SC_UARTCTS_MASK                              (0x00000001u)
        #define SC_UARTCTS_BIT                               (0)
        #define SC_UARTCTS_BITS                              (1)

#define SC1_TWICTRL1                                         *((volatile uint32_t *)0x4000C84Cu)
#define SC1_TWICTRL1_REG                                     *((volatile uint32_t *)0x4000C84Cu)
#define SC1_TWICTRL1_ADDR                                    (0x4000C84Cu)
#define SC1_TWICTRL1_RESET                                   (0x00000000u)

/* SC_TWISTOP field */
        #define SC_TWISTOP                                   (0x00000008u)
        #define SC_TWISTOP_MASK                              (0x00000008u)
        #define SC_TWISTOP_BIT                               (3)
        #define SC_TWISTOP_BITS                              (1)

/* SC_TWISTART field */
        #define SC_TWISTART                                  (0x00000004u)
        #define SC_TWISTART_MASK                             (0x00000004u)
        #define SC_TWISTART_BIT                              (2)
        #define SC_TWISTART_BITS                             (1)

/* SC_TWISEND field */
        #define SC_TWISEND                                   (0x00000002u)
        #define SC_TWISEND_MASK                              (0x00000002u)
        #define SC_TWISEND_BIT                               (1)
        #define SC_TWISEND_BITS                              (1)

/* SC_TWIRECV field */
        #define SC_TWIRECV                                   (0x00000001u)
        #define SC_TWIRECV_MASK                              (0x00000001u)
        #define SC_TWIRECV_BIT                               (0)
        #define SC_TWIRECV_BITS                              (1)

#define SC1_TWICTRL2                                         *((volatile uint32_t *)0x4000C850u)
#define SC1_TWICTRL2_REG                                     *((volatile uint32_t *)0x4000C850u)
#define SC1_TWICTRL2_ADDR                                    (0x4000C850u)
#define SC1_TWICTRL2_RESET                                   (0x00000000u)

/* SC_TWIACK field */
        #define SC_TWIACK                                    (0x00000001u)
        #define SC_TWIACK_MASK                               (0x00000001u)
        #define SC_TWIACK_BIT                                (0)
        #define SC_TWIACK_BITS                               (1)

#define SC1_MODE                                             *((volatile uint32_t *)0x4000C854u)
#define SC1_MODE_REG                                         *((volatile uint32_t *)0x4000C854u)
#define SC1_MODE_ADDR                                        (0x4000C854u)
#define SC1_MODE_RESET                                       (0x00000000u)

/* SC_MODE field */
        #define SC_MODE                                      (0x00000003u)
        #define SC_MODE_MASK                                 (0x00000003u)
        #define SC_MODE_BIT                                  (0)
        #define SC_MODE_BITS                                 (2)

/* SC_MODE Bit Field Values */
                #define SC1_MODE_DISABLED                    (0)
                #define SC1_MODE_UART                        (1)
                #define SC1_MODE_SPI                         (2)
                #define SC1_MODE_I2C                         (3)

#define SC1_SPICFG                                           *((volatile uint32_t *)0x4000C858u)
#define SC1_SPICFG_REG                                       *((volatile uint32_t *)0x4000C858u)
#define SC1_SPICFG_ADDR                                      (0x4000C858u)
#define SC1_SPICFG_RESET                                     (0x00000000u)

/* SC_SPIRXDRV field */
        #define SC_SPIRXDRV                                  (0x00000020u)
        #define SC_SPIRXDRV_MASK                             (0x00000020u)
        #define SC_SPIRXDRV_BIT                              (5)
        #define SC_SPIRXDRV_BITS                             (1)

/* SC_SPIMST field */
        #define SC_SPIMST                                    (0x00000010u)
        #define SC_SPIMST_MASK                               (0x00000010u)
        #define SC_SPIMST_BIT                                (4)
        #define SC_SPIMST_BITS                               (1)

/* SC_SPIRPT field */
        #define SC_SPIRPT                                    (0x00000008u)
        #define SC_SPIRPT_MASK                               (0x00000008u)
        #define SC_SPIRPT_BIT                                (3)
        #define SC_SPIRPT_BITS                               (1)

/* SC_SPIORD field */
        #define SC_SPIORD                                    (0x00000004u)
        #define SC_SPIORD_MASK                               (0x00000004u)
        #define SC_SPIORD_BIT                                (2)
        #define SC_SPIORD_BITS                               (1)

/* SC_SPIPHA field */
        #define SC_SPIPHA                                    (0x00000002u)
        #define SC_SPIPHA_MASK                               (0x00000002u)
        #define SC_SPIPHA_BIT                                (1)
        #define SC_SPIPHA_BITS                               (1)

/* SC_SPIPOL field */
        #define SC_SPIPOL                                    (0x00000001u)
        #define SC_SPIPOL_MASK                               (0x00000001u)
        #define SC_SPIPOL_BIT                                (0)
        #define SC_SPIPOL_BITS                               (1)

#define SC1_UARTCFG                                          *((volatile uint32_t *)0x4000C85Cu)
#define SC1_UARTCFG_REG                                      *((volatile uint32_t *)0x4000C85Cu)
#define SC1_UARTCFG_ADDR                                     (0x4000C85Cu)
#define SC1_UARTCFG_RESET                                    (0x00000000u)

/* SC_UARTAUTO field */
        #define SC_UARTAUTO                                  (0x00000040u)
        #define SC_UARTAUTO_MASK                             (0x00000040u)
        #define SC_UARTAUTO_BIT                              (6)
        #define SC_UARTAUTO_BITS                             (1)

/* SC_UARTFLOW field */
        #define SC_UARTFLOW                                  (0x00000020u)
        #define SC_UARTFLOW_MASK                             (0x00000020u)
        #define SC_UARTFLOW_BIT                              (5)
        #define SC_UARTFLOW_BITS                             (1)

/* SC_UARTODD field */
        #define SC_UARTODD                                   (0x00000010u)
        #define SC_UARTODD_MASK                              (0x00000010u)
        #define SC_UARTODD_BIT                               (4)
        #define SC_UARTODD_BITS                              (1)

/* SC_UARTPAR field */
        #define SC_UARTPAR                                   (0x00000008u)
        #define SC_UARTPAR_MASK                              (0x00000008u)
        #define SC_UARTPAR_BIT                               (3)
        #define SC_UARTPAR_BITS                              (1)

/* SC_UART2STP field */
        #define SC_UART2STP                                  (0x00000004u)
        #define SC_UART2STP_MASK                             (0x00000004u)
        #define SC_UART2STP_BIT                              (2)
        #define SC_UART2STP_BITS                             (1)

/* SC_UART8BIT field */
        #define SC_UART8BIT                                  (0x00000002u)
        #define SC_UART8BIT_MASK                             (0x00000002u)
        #define SC_UART8BIT_BIT                              (1)
        #define SC_UART8BIT_BITS                             (1)

/* SC_UARTRTS field */
        #define SC_UARTRTS                                   (0x00000001u)
        #define SC_UARTRTS_MASK                              (0x00000001u)
        #define SC_UARTRTS_BIT                               (0)
        #define SC_UARTRTS_BITS                              (1)

#define SC1_RATELIN                                          *((volatile uint32_t *)0x4000C860u)
#define SC1_RATELIN_REG                                      *((volatile uint32_t *)0x4000C860u)
#define SC1_RATELIN_ADDR                                     (0x4000C860u)
#define SC1_RATELIN_RESET                                    (0x00000000u)

/* SC_RATELIN field */
        #define SC_RATELIN                                   (0x0000000Fu)
        #define SC_RATELIN_MASK                              (0x0000000Fu)
        #define SC_RATELIN_BIT                               (0)
        #define SC_RATELIN_BITS                              (4)

#define SC1_RATEEXP                                          *((volatile uint32_t *)0x4000C864u)
#define SC1_RATEEXP_REG                                      *((volatile uint32_t *)0x4000C864u)
#define SC1_RATEEXP_ADDR                                     (0x4000C864u)
#define SC1_RATEEXP_RESET                                    (0x00000000u)

/* SC_RATEEXP field */
        #define SC_RATEEXP                                   (0x0000000Fu)
        #define SC_RATEEXP_MASK                              (0x0000000Fu)
        #define SC_RATEEXP_BIT                               (0)
        #define SC_RATEEXP_BITS                              (4)

#define SC1_UARTPER                                          *((volatile uint32_t *)0x4000C868u)
#define SC1_UARTPER_REG                                      *((volatile uint32_t *)0x4000C868u)
#define SC1_UARTPER_ADDR                                     (0x4000C868u)
#define SC1_UARTPER_RESET                                    (0x00000000u)

/* SC_UARTPER field */
        #define SC_UARTPER                                   (0x0000FFFFu)
        #define SC_UARTPER_MASK                              (0x0000FFFFu)
        #define SC_UARTPER_BIT                               (0)
        #define SC_UARTPER_BITS                              (16)

#define SC1_UARTFRAC                                         *((volatile uint32_t *)0x4000C86Cu)
#define SC1_UARTFRAC_REG                                     *((volatile uint32_t *)0x4000C86Cu)
#define SC1_UARTFRAC_ADDR                                    (0x4000C86Cu)
#define SC1_UARTFRAC_RESET                                   (0x00000000u)

/* SC_UARTFRAC field */
        #define SC_UARTFRAC                                  (0x00000001u)
        #define SC_UARTFRAC_MASK                             (0x00000001u)
        #define SC_UARTFRAC_BIT                              (0)
        #define SC_UARTFRAC_BITS                             (1)

#define SC1_RXCNTSAVED                                       *((volatile uint32_t *)0x4000C870u)
#define SC1_RXCNTSAVED_REG                                   *((volatile uint32_t *)0x4000C870u)
#define SC1_RXCNTSAVED_ADDR                                  (0x4000C870u)
#define SC1_RXCNTSAVED_RESET                                 (0x00000000u)

/* SC_RXCNTSAVED field */
        #define SC_RXCNTSAVED                                (0x0000FFFFu)
        #define SC_RXCNTSAVED_MASK                           (0x0000FFFFu)
        #define SC_RXCNTSAVED_BIT                            (0)
        #define SC_RXCNTSAVED_BITS                           (16)

/* ADC block */
#define BLOCK_ADC_BASE                                       (0x4000E000u)
#define BLOCK_ADC_END                                        (0x4000E024u)
#define BLOCK_ADC_SIZE                                       (BLOCK_ADC_END - BLOCK_ADC_BASE + 1)

#define ADC_DATA                                             *((volatile uint32_t *)0x4000E000u)
#define ADC_DATA_REG                                         *((volatile uint32_t *)0x4000E000u)
#define ADC_DATA_ADDR                                        (0x4000E000u)
#define ADC_DATA_RESET                                       (0x00000000u)

/* ADC_DATA_FIELD field */
        #define ADC_DATA_FIELD                               (0x0000FFFFu)
        #define ADC_DATA_FIELD_MASK                          (0x0000FFFFu)
        #define ADC_DATA_FIELD_BIT                           (0)
        #define ADC_DATA_FIELD_BITS                          (16)

#define ADC_CFG                                              *((volatile uint32_t *)0x4000E004u)
#define ADC_CFG_REG                                          *((volatile uint32_t *)0x4000E004u)
#define ADC_CFG_ADDR                                         (0x4000E004u)
#define ADC_CFG_RESET                                        (0x00001800u)

/* ADC_PERIOD field */
        #define ADC_PERIOD                                   (0x0000E000u)
        #define ADC_PERIOD_MASK                              (0x0000E000u)
        #define ADC_PERIOD_BIT                               (13)
        #define ADC_PERIOD_BITS                              (3)

/* ADC_CFGRSVD12 field */
        #define ADC_CFGRSVD12                                (0x00001000u)
        #define ADC_CFGRSVD12_MASK                           (0x00001000u)
        #define ADC_CFGRSVD12_BIT                            (12)
        #define ADC_CFGRSVD12_BITS                           (1)

/* ADC_CFGRSVD11 field */
        #define ADC_CFGRSVD11                                (0x00000800u)
        #define ADC_CFGRSVD11_MASK                           (0x00000800u)
        #define ADC_CFGRSVD11_BIT                            (11)
        #define ADC_CFGRSVD11_BITS                           (1)

/* ADC_MUXP field */
        #define ADC_MUXP                                     (0x00000780u)
        #define ADC_MUXP_MASK                                (0x00000780u)
        #define ADC_MUXP_BIT                                 (7)
        #define ADC_MUXP_BITS                                (4)

/* ADC_MUXN field */
        #define ADC_MUXN                                     (0x00000078u)
        #define ADC_MUXN_MASK                                (0x00000078u)
        #define ADC_MUXN_BIT                                 (3)
        #define ADC_MUXN_BITS                                (4)

/* ADC_1MHZCLK field */
        #define ADC_1MHZCLK                                  (0x00000004u)
        #define ADC_1MHZCLK_MASK                             (0x00000004u)
        #define ADC_1MHZCLK_BIT                              (2)
        #define ADC_1MHZCLK_BITS                             (1)

/* ADC_CFGRSVD field */
        #define ADC_CFGRSVD                                  (0x00000002u)
        #define ADC_CFGRSVD_MASK                             (0x00000002u)
        #define ADC_CFGRSVD_BIT                              (1)
        #define ADC_CFGRSVD_BITS                             (1)

/* ADC_ENABLE field */
        #define ADC_ENABLE                                   (0x00000001u)
        #define ADC_ENABLE_MASK                              (0x00000001u)
        #define ADC_ENABLE_BIT                               (0)
        #define ADC_ENABLE_BITS                              (1)

#define ADC_OFFSET                                           *((volatile uint32_t *)0x4000E008u)
#define ADC_OFFSET_REG                                       *((volatile uint32_t *)0x4000E008u)
#define ADC_OFFSET_ADDR                                      (0x4000E008u)
#define ADC_OFFSET_RESET                                     (0x00000000u)

/* ADC_OFFSET_FIELD field */
        #define ADC_OFFSET_FIELD                             (0x0000FFFFu)
        #define ADC_OFFSET_FIELD_MASK                        (0x0000FFFFu)
        #define ADC_OFFSET_FIELD_BIT                         (0)
        #define ADC_OFFSET_FIELD_BITS                        (16)

#define ADC_GAIN                                             *((volatile uint32_t *)0x4000E00Cu)
#define ADC_GAIN_REG                                         *((volatile uint32_t *)0x4000E00Cu)
#define ADC_GAIN_ADDR                                        (0x4000E00Cu)
#define ADC_GAIN_RESET                                       (0x00008000u)

/* ADC_GAIN_FIELD field */
        #define ADC_GAIN_FIELD                               (0x0000FFFFu)
        #define ADC_GAIN_FIELD_MASK                          (0x0000FFFFu)
        #define ADC_GAIN_FIELD_BIT                           (0)
        #define ADC_GAIN_FIELD_BITS                          (16)

#define ADC_DMACFG                                           *((volatile uint32_t *)0x4000E010u)
#define ADC_DMACFG_REG                                       *((volatile uint32_t *)0x4000E010u)
#define ADC_DMACFG_ADDR                                      (0x4000E010u)
#define ADC_DMACFG_RESET                                     (0x00000000u)

/* ADC_DMARST field */
        #define ADC_DMARST                                   (0x00000010u)
        #define ADC_DMARST_MASK                              (0x00000010u)
        #define ADC_DMARST_BIT                               (4)
        #define ADC_DMARST_BITS                              (1)

/* ADC_DMAAUTOWRAP field */
        #define ADC_DMAAUTOWRAP                              (0x00000002u)
        #define ADC_DMAAUTOWRAP_MASK                         (0x00000002u)
        #define ADC_DMAAUTOWRAP_BIT                          (1)
        #define ADC_DMAAUTOWRAP_BITS                         (1)

/* ADC_DMALOAD field */
        #define ADC_DMALOAD                                  (0x00000001u)
        #define ADC_DMALOAD_MASK                             (0x00000001u)
        #define ADC_DMALOAD_BIT                              (0)
        #define ADC_DMALOAD_BITS                             (1)

#define ADC_DMASTAT                                          *((volatile uint32_t *)0x4000E014u)
#define ADC_DMASTAT_REG                                      *((volatile uint32_t *)0x4000E014u)
#define ADC_DMASTAT_ADDR                                     (0x4000E014u)
#define ADC_DMASTAT_RESET                                    (0x00000000u)

/* ADC_DMAOVF field */
        #define ADC_DMAOVF                                   (0x00000002u)
        #define ADC_DMAOVF_MASK                              (0x00000002u)
        #define ADC_DMAOVF_BIT                               (1)
        #define ADC_DMAOVF_BITS                              (1)

/* ADC_DMAACT field */
        #define ADC_DMAACT                                   (0x00000001u)
        #define ADC_DMAACT_MASK                              (0x00000001u)
        #define ADC_DMAACT_BIT                               (0)
        #define ADC_DMAACT_BITS                              (1)

#define ADC_DMABEG                                           *((volatile uint32_t *)0x4000E018u)
#define ADC_DMABEG_REG                                       *((volatile uint32_t *)0x4000E018u)
#define ADC_DMABEG_ADDR                                      (0x4000E018u)
#define ADC_DMABEG_RESET                                     (0x20000000u)

/* ADC_DMABEG_FIXED field */
        #define ADC_DMABEG_FIXED                             (0xFFFFC000u)
        #define ADC_DMABEG_FIXED_MASK                        (0xFFFFC000u)
        #define ADC_DMABEG_FIXED_BIT                         (14)
        #define ADC_DMABEG_FIXED_BITS                        (18)

/* ADC_DMABEG_FIELD field */
        #define ADC_DMABEG_FIELD                             (0x00003FFFu)
        #define ADC_DMABEG_FIELD_MASK                        (0x00003FFFu)
        #define ADC_DMABEG_FIELD_BIT                         (0)
        #define ADC_DMABEG_FIELD_BITS                        (14)

#define ADC_DMASIZE                                          *((volatile uint32_t *)0x4000E01Cu)
#define ADC_DMASIZE_REG                                      *((volatile uint32_t *)0x4000E01Cu)
#define ADC_DMASIZE_ADDR                                     (0x4000E01Cu)
#define ADC_DMASIZE_RESET                                    (0x00000000u)

/* ADC_DMASIZE_FIELD field */
        #define ADC_DMASIZE_FIELD                            (0x00007FFFu)
        #define ADC_DMASIZE_FIELD_MASK                       (0x00007FFFu)
        #define ADC_DMASIZE_FIELD_BIT                        (0)
        #define ADC_DMASIZE_FIELD_BITS                       (15)

#define ADC_DMACUR                                           *((volatile uint32_t *)0x4000E020u)
#define ADC_DMACUR_REG                                       *((volatile uint32_t *)0x4000E020u)
#define ADC_DMACUR_ADDR                                      (0x4000E020u)
#define ADC_DMACUR_RESET                                     (0x20000000u)

/* ADC_DMACUR_FIXED field */
        #define ADC_DMACUR_FIXED                             (0xFFFF0000u)
        #define ADC_DMACUR_FIXED_MASK                        (0xFFFF0000u)
        #define ADC_DMACUR_FIXED_BIT                         (16)
        #define ADC_DMACUR_FIXED_BITS                        (16)

/* ADC_DMACUR_FIELD field */
        #define ADC_DMACUR_FIELD                             (0x0000FFFFu)
        #define ADC_DMACUR_FIELD_MASK                        (0x0000FFFFu)
        #define ADC_DMACUR_FIELD_BIT                         (0)
        #define ADC_DMACUR_FIELD_BITS                        (16)

#define ADC_DMACNT                                           *((volatile uint32_t *)0x4000E024u)
#define ADC_DMACNT_REG                                       *((volatile uint32_t *)0x4000E024u)
#define ADC_DMACNT_ADDR                                      (0x4000E024u)
#define ADC_DMACNT_RESET                                     (0x00000000u)

/* ADC_DMACNT_FIELD field */
        #define ADC_DMACNT_FIELD                             (0x00007FFFu)
        #define ADC_DMACNT_FIELD_MASK                        (0x00007FFFu)
        #define ADC_DMACNT_FIELD_BIT                         (0)
        #define ADC_DMACNT_FIELD_BITS                        (15)

/* TIM1 block */
#define BLOCK_TIM1_BASE                                      (0x4000F000u)
#define BLOCK_TIM1_END                                       (0x4000F050u)
#define BLOCK_TIM1_SIZE                                      (BLOCK_TIM1_END - BLOCK_TIM1_BASE + 1)

#define TIM1_CR1                                             *((volatile uint32_t *)0x4000F000u)
#define TIM1_CR1_REG                                         *((volatile uint32_t *)0x4000F000u)
#define TIM1_CR1_ADDR                                        (0x4000F000u)
#define TIM1_CR1_RESET                                       (0x00000000u)

/* TIM_ARBE field */
        #define TIM_ARBE                                     (0x00000080u)
        #define TIM_ARBE_MASK                                (0x00000080u)
        #define TIM_ARBE_BIT                                 (7)
        #define TIM_ARBE_BITS                                (1)

/* TIM_CMS field */
        #define TIM_CMS                                      (0x00000060u)
        #define TIM_CMS_MASK                                 (0x00000060u)
        #define TIM_CMS_BIT                                  (5)
        #define TIM_CMS_BITS                                 (2)

/* TIM_DIR field */
        #define TIM_DIR                                      (0x00000010u)
        #define TIM_DIR_MASK                                 (0x00000010u)
        #define TIM_DIR_BIT                                  (4)
        #define TIM_DIR_BITS                                 (1)

/* TIM_OPM field */
        #define TIM_OPM                                      (0x00000008u)
        #define TIM_OPM_MASK                                 (0x00000008u)
        #define TIM_OPM_BIT                                  (3)
        #define TIM_OPM_BITS                                 (1)

/* TIM_URS field */
        #define TIM_URS                                      (0x00000004u)
        #define TIM_URS_MASK                                 (0x00000004u)
        #define TIM_URS_BIT                                  (2)
        #define TIM_URS_BITS                                 (1)

/* TIM_UDIS field */
        #define TIM_UDIS                                     (0x00000002u)
        #define TIM_UDIS_MASK                                (0x00000002u)
        #define TIM_UDIS_BIT                                 (1)
        #define TIM_UDIS_BITS                                (1)

/* TIM_CEN field */
        #define TIM_CEN                                      (0x00000001u)
        #define TIM_CEN_MASK                                 (0x00000001u)
        #define TIM_CEN_BIT                                  (0)
        #define TIM_CEN_BITS                                 (1)

#define TIM1_CR2                                             *((volatile uint32_t *)0x4000F004u)
#define TIM1_CR2_REG                                         *((volatile uint32_t *)0x4000F004u)
#define TIM1_CR2_ADDR                                        (0x4000F004u)
#define TIM1_CR2_RESET                                       (0x00000000u)

/* TIM_TI1S field */
        #define TIM_TI1S                                     (0x00000080u)
        #define TIM_TI1S_MASK                                (0x00000080u)
        #define TIM_TI1S_BIT                                 (7)
        #define TIM_TI1S_BITS                                (1)

/* TIM_MMS field */
        #define TIM_MMS                                      (0x00000070u)
        #define TIM_MMS_MASK                                 (0x00000070u)
        #define TIM_MMS_BIT                                  (4)
        #define TIM_MMS_BITS                                 (3)

#define TIM1_SMCR                                            *((volatile uint32_t *)0x4000F008u)
#define TIM1_SMCR_REG                                        *((volatile uint32_t *)0x4000F008u)
#define TIM1_SMCR_ADDR                                       (0x4000F008u)
#define TIM1_SMCR_RESET                                      (0x00000000u)

/* TIM_ETP field */
        #define TIM_ETP                                      (0x00008000u)
        #define TIM_ETP_MASK                                 (0x00008000u)
        #define TIM_ETP_BIT                                  (15)
        #define TIM_ETP_BITS                                 (1)

/* TIM_ECE field */
        #define TIM_ECE                                      (0x00004000u)
        #define TIM_ECE_MASK                                 (0x00004000u)
        #define TIM_ECE_BIT                                  (14)
        #define TIM_ECE_BITS                                 (1)

/* TIM_ETPS field */
        #define TIM_ETPS                                     (0x00003000u)
        #define TIM_ETPS_MASK                                (0x00003000u)
        #define TIM_ETPS_BIT                                 (12)
        #define TIM_ETPS_BITS                                (2)

/* TIM_ETF field */
        #define TIM_ETF                                      (0x00000F00u)
        #define TIM_ETF_MASK                                 (0x00000F00u)
        #define TIM_ETF_BIT                                  (8)
        #define TIM_ETF_BITS                                 (4)

/* TIM_MSM field */
        #define TIM_MSM                                      (0x00000080u)
        #define TIM_MSM_MASK                                 (0x00000080u)
        #define TIM_MSM_BIT                                  (7)
        #define TIM_MSM_BITS                                 (1)

/* TIM_TS field */
        #define TIM_TS                                       (0x00000070u)
        #define TIM_TS_MASK                                  (0x00000070u)
        #define TIM_TS_BIT                                   (4)
        #define TIM_TS_BITS                                  (3)

/* TIM_SMS field */
        #define TIM_SMS                                      (0x00000007u)
        #define TIM_SMS_MASK                                 (0x00000007u)
        #define TIM_SMS_BIT                                  (0)
        #define TIM_SMS_BITS                                 (3)

#define TIM1_EGR                                             *((volatile uint32_t *)0x4000F014u)
#define TIM1_EGR_REG                                         *((volatile uint32_t *)0x4000F014u)
#define TIM1_EGR_ADDR                                        (0x4000F014u)
#define TIM1_EGR_RESET                                       (0x00000000u)

/* TIM_TG field */
        #define TIM_TG                                       (0x00000040u)
        #define TIM_TG_MASK                                  (0x00000040u)
        #define TIM_TG_BIT                                   (6)
        #define TIM_TG_BITS                                  (1)

/* TIM_CC4G field */
        #define TIM_CC4G                                     (0x00000010u)
        #define TIM_CC4G_MASK                                (0x00000010u)
        #define TIM_CC4G_BIT                                 (4)
        #define TIM_CC4G_BITS                                (1)

/* TIM_CC3G field */
        #define TIM_CC3G                                     (0x00000008u)
        #define TIM_CC3G_MASK                                (0x00000008u)
        #define TIM_CC3G_BIT                                 (3)
        #define TIM_CC3G_BITS                                (1)

/* TIM_CC2G field */
        #define TIM_CC2G                                     (0x00000004u)
        #define TIM_CC2G_MASK                                (0x00000004u)
        #define TIM_CC2G_BIT                                 (2)
        #define TIM_CC2G_BITS                                (1)

/* TIM_CC1G field */
        #define TIM_CC1G                                     (0x00000002u)
        #define TIM_CC1G_MASK                                (0x00000002u)
        #define TIM_CC1G_BIT                                 (1)
        #define TIM_CC1G_BITS                                (1)

/* TIM_UG field */
        #define TIM_UG                                       (0x00000001u)
        #define TIM_UG_MASK                                  (0x00000001u)
        #define TIM_UG_BIT                                   (0)
        #define TIM_UG_BITS                                  (1)

#define TIM1_CCMR1                                           *((volatile uint32_t *)0x4000F018u)
#define TIM1_CCMR1_REG                                       *((volatile uint32_t *)0x4000F018u)
#define TIM1_CCMR1_ADDR                                      (0x4000F018u)
#define TIM1_CCMR1_RESET                                     (0x00000000u)

/* TIM_IC2F field */
        #define TIM_IC2F                                     (0x0000F000u)
        #define TIM_IC2F_MASK                                (0x0000F000u)
        #define TIM_IC2F_BIT                                 (12)
        #define TIM_IC2F_BITS                                (4)

/* TIM_IC2PSC field */
        #define TIM_IC2PSC                                   (0x00000C00u)
        #define TIM_IC2PSC_MASK                              (0x00000C00u)
        #define TIM_IC2PSC_BIT                               (10)
        #define TIM_IC2PSC_BITS                              (2)

/* TIM_IC1F field */
        #define TIM_IC1F                                     (0x000000F0u)
        #define TIM_IC1F_MASK                                (0x000000F0u)
        #define TIM_IC1F_BIT                                 (4)
        #define TIM_IC1F_BITS                                (4)

/* TIM_IC1PSC field */
        #define TIM_IC1PSC                                   (0x0000000Cu)
        #define TIM_IC1PSC_MASK                              (0x0000000Cu)
        #define TIM_IC1PSC_BIT                               (2)
        #define TIM_IC1PSC_BITS                              (2)

/* TIM_OC2CE field */
        #define TIM_OC2CE                                    (0x00008000u)
        #define TIM_OC2CE_MASK                               (0x00008000u)
        #define TIM_OC2CE_BIT                                (15)
        #define TIM_OC2CE_BITS                               (1)

/* TIM_OC2M field */
        #define TIM_OC2M                                     (0x00007000u)
        #define TIM_OC2M_MASK                                (0x00007000u)
        #define TIM_OC2M_BIT                                 (12)
        #define TIM_OC2M_BITS                                (3)

/* TIM_OC2BE field */
        #define TIM_OC2BE                                    (0x00000800u)
        #define TIM_OC2BE_MASK                               (0x00000800u)
        #define TIM_OC2BE_BIT                                (11)
        #define TIM_OC2BE_BITS                               (1)

/* TIM_OC2FE field */
        #define TIM_OC2FE                                    (0x00000400u)
        #define TIM_OC2FE_MASK                               (0x00000400u)
        #define TIM_OC2FE_BIT                                (10)
        #define TIM_OC2FE_BITS                               (1)

/* TIM_CC2S field */
        #define TIM_CC2S                                     (0x00000300u)
        #define TIM_CC2S_MASK                                (0x00000300u)
        #define TIM_CC2S_BIT                                 (8)
        #define TIM_CC2S_BITS                                (2)

/* TIM_OC1CE field */
        #define TIM_OC1CE                                    (0x00000080u)
        #define TIM_OC1CE_MASK                               (0x00000080u)
        #define TIM_OC1CE_BIT                                (7)
        #define TIM_OC1CE_BITS                               (1)

/* TIM_OC1M field */
        #define TIM_OC1M                                     (0x00000070u)
        #define TIM_OC1M_MASK                                (0x00000070u)
        #define TIM_OC1M_BIT                                 (4)
        #define TIM_OC1M_BITS                                (3)

/* TIM_OC1PE field */
        #define TIM_OC1PE                                    (0x00000008u)
        #define TIM_OC1PE_MASK                               (0x00000008u)
        #define TIM_OC1PE_BIT                                (3)
        #define TIM_OC1PE_BITS                               (1)

/* TIM_OC1FE field */
        #define TIM_OC1FE                                    (0x00000004u)
        #define TIM_OC1FE_MASK                               (0x00000004u)
        #define TIM_OC1FE_BIT                                (2)
        #define TIM_OC1FE_BITS                               (1)

/* TIM_CC1S field */
        #define TIM_CC1S                                     (0x00000003u)
        #define TIM_CC1S_MASK                                (0x00000003u)
        #define TIM_CC1S_BIT                                 (0)
        #define TIM_CC1S_BITS                                (2)

#define TIM1_CCMR2                                           *((volatile uint32_t *)0x4000F01Cu)
#define TIM1_CCMR2_REG                                       *((volatile uint32_t *)0x4000F01Cu)
#define TIM1_CCMR2_ADDR                                      (0x4000F01Cu)
#define TIM1_CCMR2_RESET                                     (0x00000000u)

/* TIM_IC4F field */
        #define TIM_IC4F                                     (0x0000F000u)
        #define TIM_IC4F_MASK                                (0x0000F000u)
        #define TIM_IC4F_BIT                                 (12)
        #define TIM_IC4F_BITS                                (4)

/* TIM_IC4PSC field */
        #define TIM_IC4PSC                                   (0x00000C00u)
        #define TIM_IC4PSC_MASK                              (0x00000C00u)
        #define TIM_IC4PSC_BIT                               (10)
        #define TIM_IC4PSC_BITS                              (2)

/* TIM_IC3F field */
        #define TIM_IC3F                                     (0x000000F0u)
        #define TIM_IC3F_MASK                                (0x000000F0u)
        #define TIM_IC3F_BIT                                 (4)
        #define TIM_IC3F_BITS                                (4)

/* TIM_IC3PSC field */
        #define TIM_IC3PSC                                   (0x0000000Cu)
        #define TIM_IC3PSC_MASK                              (0x0000000Cu)
        #define TIM_IC3PSC_BIT                               (2)
        #define TIM_IC3PSC_BITS                              (2)

/* TIM_OC4CE field */
        #define TIM_OC4CE                                    (0x00008000u)
        #define TIM_OC4CE_MASK                               (0x00008000u)
        #define TIM_OC4CE_BIT                                (15)
        #define TIM_OC4CE_BITS                               (1)

/* TIM_OC4M field */
        #define TIM_OC4M                                     (0x00007000u)
        #define TIM_OC4M_MASK                                (0x00007000u)
        #define TIM_OC4M_BIT                                 (12)
        #define TIM_OC4M_BITS                                (3)

/* TIM_OC4BE field */
        #define TIM_OC4BE                                    (0x00000800u)
        #define TIM_OC4BE_MASK                               (0x00000800u)
        #define TIM_OC4BE_BIT                                (11)
        #define TIM_OC4BE_BITS                               (1)

/* TIM_OC4FE field */
        #define TIM_OC4FE                                    (0x00000400u)
        #define TIM_OC4FE_MASK                               (0x00000400u)
        #define TIM_OC4FE_BIT                                (10)
        #define TIM_OC4FE_BITS                               (1)

/* TIM_CC4S field */
        #define TIM_CC4S                                     (0x00000300u)
        #define TIM_CC4S_MASK                                (0x00000300u)
        #define TIM_CC4S_BIT                                 (8)
        #define TIM_CC4S_BITS                                (2)

/* TIM_OC3CE field */
        #define TIM_OC3CE                                    (0x00000080u)
        #define TIM_OC3CE_MASK                               (0x00000080u)
        #define TIM_OC3CE_BIT                                (7)
        #define TIM_OC3CE_BITS                               (1)

/* TIM_OC3M field */
        #define TIM_OC3M                                     (0x00000070u)
        #define TIM_OC3M_MASK                                (0x00000070u)
        #define TIM_OC3M_BIT                                 (4)
        #define TIM_OC3M_BITS                                (3)

/* TIM_OC3BE field */
        #define TIM_OC3BE                                    (0x00000008u)
        #define TIM_OC3BE_MASK                               (0x00000008u)
        #define TIM_OC3BE_BIT                                (3)
        #define TIM_OC3BE_BITS                               (1)

/* TIM_OC3FE field */
        #define TIM_OC3FE                                    (0x00000004u)
        #define TIM_OC3FE_MASK                               (0x00000004u)
        #define TIM_OC3FE_BIT                                (2)
        #define TIM_OC3FE_BITS                               (1)

/* TIM_CC3S field */
        #define TIM_CC3S                                     (0x00000003u)
        #define TIM_CC3S_MASK                                (0x00000003u)
        #define TIM_CC3S_BIT                                 (0)
        #define TIM_CC3S_BITS                                (2)

#define TIM1_CCER                                            *((volatile uint32_t *)0x4000F020u)
#define TIM1_CCER_REG                                        *((volatile uint32_t *)0x4000F020u)
#define TIM1_CCER_ADDR                                       (0x4000F020u)
#define TIM1_CCER_RESET                                      (0x00000000u)

/* TIM_CC4P field */
        #define TIM_CC4P                                     (0x00002000u)
        #define TIM_CC4P_MASK                                (0x00002000u)
        #define TIM_CC4P_BIT                                 (13)
        #define TIM_CC4P_BITS                                (1)

/* TIM_CC4E field */
        #define TIM_CC4E                                     (0x00001000u)
        #define TIM_CC4E_MASK                                (0x00001000u)
        #define TIM_CC4E_BIT                                 (12)
        #define TIM_CC4E_BITS                                (1)

/* TIM_CC3P field */
        #define TIM_CC3P                                     (0x00000200u)
        #define TIM_CC3P_MASK                                (0x00000200u)
        #define TIM_CC3P_BIT                                 (9)
        #define TIM_CC3P_BITS                                (1)

/* TIM_CC3E field */
        #define TIM_CC3E                                     (0x00000100u)
        #define TIM_CC3E_MASK                                (0x00000100u)
        #define TIM_CC3E_BIT                                 (8)
        #define TIM_CC3E_BITS                                (1)

/* TIM_CC2P field */
        #define TIM_CC2P                                     (0x00000020u)
        #define TIM_CC2P_MASK                                (0x00000020u)
        #define TIM_CC2P_BIT                                 (5)
        #define TIM_CC2P_BITS                                (1)

/* TIM_CC2E field */
        #define TIM_CC2E                                     (0x00000010u)
        #define TIM_CC2E_MASK                                (0x00000010u)
        #define TIM_CC2E_BIT                                 (4)
        #define TIM_CC2E_BITS                                (1)

/* TIM_CC1P field */
        #define TIM_CC1P                                     (0x00000002u)
        #define TIM_CC1P_MASK                                (0x00000002u)
        #define TIM_CC1P_BIT                                 (1)
        #define TIM_CC1P_BITS                                (1)

/* TIM_CC1E field */
        #define TIM_CC1E                                     (0x00000001u)
        #define TIM_CC1E_MASK                                (0x00000001u)
        #define TIM_CC1E_BIT                                 (0)
        #define TIM_CC1E_BITS                                (1)

#define TIM1_CNT                                             *((volatile uint32_t *)0x4000F024u)
#define TIM1_CNT_REG                                         *((volatile uint32_t *)0x4000F024u)
#define TIM1_CNT_ADDR                                        (0x4000F024u)
#define TIM1_CNT_RESET                                       (0x00000000u)

/* TIM_CNT field */
        #define TIM_CNT                                      (0x0000FFFFu)
        #define TIM_CNT_MASK                                 (0x0000FFFFu)
        #define TIM_CNT_BIT                                  (0)
        #define TIM_CNT_BITS                                 (16)

#define TIM1_PSC                                             *((volatile uint32_t *)0x4000F028u)
#define TIM1_PSC_REG                                         *((volatile uint32_t *)0x4000F028u)
#define TIM1_PSC_ADDR                                        (0x4000F028u)
#define TIM1_PSC_RESET                                       (0x00000000u)

/* TIM_PSC field */
        #define TIM_PSC                                      (0x0000000Fu)
        #define TIM_PSC_MASK                                 (0x0000000Fu)
        #define TIM_PSC_BIT                                  (0)
        #define TIM_PSC_BITS                                 (4)

#define TIM1_ARR                                             *((volatile uint32_t *)0x4000F02Cu)
#define TIM1_ARR_REG                                         *((volatile uint32_t *)0x4000F02Cu)
#define TIM1_ARR_ADDR                                        (0x4000F02Cu)
#define TIM1_ARR_RESET                                       (0x0000FFFFu)

/* TIM_ARR field */
        #define TIM_ARR                                      (0x0000FFFFu)
        #define TIM_ARR_MASK                                 (0x0000FFFFu)
        #define TIM_ARR_BIT                                  (0)
        #define TIM_ARR_BITS                                 (16)

#define TIM1_CCR1                                            *((volatile uint32_t *)0x4000F034u)
#define TIM1_CCR1_REG                                        *((volatile uint32_t *)0x4000F034u)
#define TIM1_CCR1_ADDR                                       (0x4000F034u)
#define TIM1_CCR1_RESET                                      (0x00000000u)

/* TIM_CCR field */
        #define TIM_CCR                                      (0x0000FFFFu)
        #define TIM_CCR_MASK                                 (0x0000FFFFu)
        #define TIM_CCR_BIT                                  (0)
        #define TIM_CCR_BITS                                 (16)

#define TIM1_CCR2                                            *((volatile uint32_t *)0x4000F038u)
#define TIM1_CCR2_REG                                        *((volatile uint32_t *)0x4000F038u)
#define TIM1_CCR2_ADDR                                       (0x4000F038u)
#define TIM1_CCR2_RESET                                      (0x00000000u)

/* TIM_CCR field */
        #define TIM_CCR                                      (0x0000FFFFu)
        #define TIM_CCR_MASK                                 (0x0000FFFFu)
        #define TIM_CCR_BIT                                  (0)
        #define TIM_CCR_BITS                                 (16)

#define TIM1_CCR3                                            *((volatile uint32_t *)0x4000F03Cu)
#define TIM1_CCR3_REG                                        *((volatile uint32_t *)0x4000F03Cu)
#define TIM1_CCR3_ADDR                                       (0x4000F03Cu)
#define TIM1_CCR3_RESET                                      (0x00000000u)

/* TIM_CCR field */
        #define TIM_CCR                                      (0x0000FFFFu)
        #define TIM_CCR_MASK                                 (0x0000FFFFu)
        #define TIM_CCR_BIT                                  (0)
        #define TIM_CCR_BITS                                 (16)

#define TIM1_CCR4                                            *((volatile uint32_t *)0x4000F040u)
#define TIM1_CCR4_REG                                        *((volatile uint32_t *)0x4000F040u)
#define TIM1_CCR4_ADDR                                       (0x4000F040u)
#define TIM1_CCR4_RESET                                      (0x00000000u)

/* TIM_CCR field */
        #define TIM_CCR                                      (0x0000FFFFu)
        #define TIM_CCR_MASK                                 (0x0000FFFFu)
        #define TIM_CCR_BIT                                  (0)
        #define TIM_CCR_BITS                                 (16)

#define TIM1_OR                                              *((volatile uint32_t *)0x4000F050u)
#define TIM1_OR_REG                                          *((volatile uint32_t *)0x4000F050u)
#define TIM1_OR_ADDR                                         (0x4000F050u)
#define TIM1_OR_RESET                                        (0x00000000u)

/* TIM_ORRSVD field */
        #define TIM_ORRSVD                                   (0x00000008u)
        #define TIM_ORRSVD_MASK                              (0x00000008u)
        #define TIM_ORRSVD_BIT                               (3)
        #define TIM_ORRSVD_BITS                              (1)

/* TIM_CLKMSKEN field */
        #define TIM_CLKMSKEN                                 (0x00000004u)
        #define TIM_CLKMSKEN_MASK                            (0x00000004u)
        #define TIM_CLKMSKEN_BIT                             (2)
        #define TIM_CLKMSKEN_BITS                            (1)

/* TIM_EXTRIGSEL field */
        #define TIM_EXTRIGSEL                                (0x00000003u)
        #define TIM_EXTRIGSEL_MASK                           (0x00000003u)
        #define TIM_EXTRIGSEL_BIT                            (0)
        #define TIM_EXTRIGSEL_BITS                           (2)

/* TIM1_EXTRIGSEL defines for backwards compatibility */
                #define TIM1_EXTRIGSEL                       TIM_EXTRIGSEL
                #define TIM1_EXTRIGSEL_MASK                  TIM_EXTRIGSEL_MASK
                #define TIM1_EXTRIGSEL_BIT                   TIM_EXTRIGSEL_BIT
                #define TIM1_EXTRIGSEL_BITS                  TIM_EXTRIGSEL_BITS

/* TIM2 block */
#define BLOCK_TIM2_BASE                                      (0x40010000u)
#define BLOCK_TIM2_END                                       (0x40010050u)
#define BLOCK_TIM2_SIZE                                      (BLOCK_TIM2_END - BLOCK_TIM2_BASE + 1)

#define TIM2_CR1                                             *((volatile uint32_t *)0x40010000u)
#define TIM2_CR1_REG                                         *((volatile uint32_t *)0x40010000u)
#define TIM2_CR1_ADDR                                        (0x40010000u)
#define TIM2_CR1_RESET                                       (0x00000000u)

/* TIM_ARBE field */
        #define TIM_ARBE                                     (0x00000080u)
        #define TIM_ARBE_MASK                                (0x00000080u)
        #define TIM_ARBE_BIT                                 (7)
        #define TIM_ARBE_BITS                                (1)

/* TIM_CMS field */
        #define TIM_CMS                                      (0x00000060u)
        #define TIM_CMS_MASK                                 (0x00000060u)
        #define TIM_CMS_BIT                                  (5)
        #define TIM_CMS_BITS                                 (2)

/* TIM_DIR field */
        #define TIM_DIR                                      (0x00000010u)
        #define TIM_DIR_MASK                                 (0x00000010u)
        #define TIM_DIR_BIT                                  (4)
        #define TIM_DIR_BITS                                 (1)

/* TIM_OPM field */
        #define TIM_OPM                                      (0x00000008u)
        #define TIM_OPM_MASK                                 (0x00000008u)
        #define TIM_OPM_BIT                                  (3)
        #define TIM_OPM_BITS                                 (1)

/* TIM_URS field */
        #define TIM_URS                                      (0x00000004u)
        #define TIM_URS_MASK                                 (0x00000004u)
        #define TIM_URS_BIT                                  (2)
        #define TIM_URS_BITS                                 (1)

/* TIM_UDIS field */
        #define TIM_UDIS                                     (0x00000002u)
        #define TIM_UDIS_MASK                                (0x00000002u)
        #define TIM_UDIS_BIT                                 (1)
        #define TIM_UDIS_BITS                                (1)

/* TIM_CEN field */
        #define TIM_CEN                                      (0x00000001u)
        #define TIM_CEN_MASK                                 (0x00000001u)
        #define TIM_CEN_BIT                                  (0)
        #define TIM_CEN_BITS                                 (1)

#define TIM2_CR2                                             *((volatile uint32_t *)0x40010004u)
#define TIM2_CR2_REG                                         *((volatile uint32_t *)0x40010004u)
#define TIM2_CR2_ADDR                                        (0x40010004u)
#define TIM2_CR2_RESET                                       (0x00000000u)

/* TIM_TI1S field */
        #define TIM_TI1S                                     (0x00000080u)
        #define TIM_TI1S_MASK                                (0x00000080u)
        #define TIM_TI1S_BIT                                 (7)
        #define TIM_TI1S_BITS                                (1)

/* TIM_MMS field */
        #define TIM_MMS                                      (0x00000070u)
        #define TIM_MMS_MASK                                 (0x00000070u)
        #define TIM_MMS_BIT                                  (4)
        #define TIM_MMS_BITS                                 (3)

#define TIM2_SMCR                                            *((volatile uint32_t *)0x40010008u)
#define TIM2_SMCR_REG                                        *((volatile uint32_t *)0x40010008u)
#define TIM2_SMCR_ADDR                                       (0x40010008u)
#define TIM2_SMCR_RESET                                      (0x00000000u)

/* TIM_ETP field */
        #define TIM_ETP                                      (0x00008000u)
        #define TIM_ETP_MASK                                 (0x00008000u)
        #define TIM_ETP_BIT                                  (15)
        #define TIM_ETP_BITS                                 (1)

/* TIM_ECE field */
        #define TIM_ECE                                      (0x00004000u)
        #define TIM_ECE_MASK                                 (0x00004000u)
        #define TIM_ECE_BIT                                  (14)
        #define TIM_ECE_BITS                                 (1)

/* TIM_ETPS field */
        #define TIM_ETPS                                     (0x00003000u)
        #define TIM_ETPS_MASK                                (0x00003000u)
        #define TIM_ETPS_BIT                                 (12)
        #define TIM_ETPS_BITS                                (2)

/* TIM_ETF field */
        #define TIM_ETF                                      (0x00000F00u)
        #define TIM_ETF_MASK                                 (0x00000F00u)
        #define TIM_ETF_BIT                                  (8)
        #define TIM_ETF_BITS                                 (4)

/* TIM_MSM field */
        #define TIM_MSM                                      (0x00000080u)
        #define TIM_MSM_MASK                                 (0x00000080u)
        #define TIM_MSM_BIT                                  (7)
        #define TIM_MSM_BITS                                 (1)

/* TIM_TS field */
        #define TIM_TS                                       (0x00000070u)
        #define TIM_TS_MASK                                  (0x00000070u)
        #define TIM_TS_BIT                                   (4)
        #define TIM_TS_BITS                                  (3)

/* TIM_SMS field */
        #define TIM_SMS                                      (0x00000007u)
        #define TIM_SMS_MASK                                 (0x00000007u)
        #define TIM_SMS_BIT                                  (0)
        #define TIM_SMS_BITS                                 (3)

#define TIM2_EGR                                             *((volatile uint32_t *)0x40010014u)
#define TIM2_EGR_REG                                         *((volatile uint32_t *)0x40010014u)
#define TIM2_EGR_ADDR                                        (0x40010014u)
#define TIM2_EGR_RESET                                       (0x00000000u)

/* TIM_TG field */
        #define TIM_TG                                       (0x00000040u)
        #define TIM_TG_MASK                                  (0x00000040u)
        #define TIM_TG_BIT                                   (6)
        #define TIM_TG_BITS                                  (1)

/* TIM_CC4G field */
        #define TIM_CC4G                                     (0x00000010u)
        #define TIM_CC4G_MASK                                (0x00000010u)
        #define TIM_CC4G_BIT                                 (4)
        #define TIM_CC4G_BITS                                (1)

/* TIM_CC3G field */
        #define TIM_CC3G                                     (0x00000008u)
        #define TIM_CC3G_MASK                                (0x00000008u)
        #define TIM_CC3G_BIT                                 (3)
        #define TIM_CC3G_BITS                                (1)

/* TIM_CC2G field */
        #define TIM_CC2G                                     (0x00000004u)
        #define TIM_CC2G_MASK                                (0x00000004u)
        #define TIM_CC2G_BIT                                 (2)
        #define TIM_CC2G_BITS                                (1)

/* TIM_CC1G field */
        #define TIM_CC1G                                     (0x00000002u)
        #define TIM_CC1G_MASK                                (0x00000002u)
        #define TIM_CC1G_BIT                                 (1)
        #define TIM_CC1G_BITS                                (1)

/* TIM_UG field */
        #define TIM_UG                                       (0x00000001u)
        #define TIM_UG_MASK                                  (0x00000001u)
        #define TIM_UG_BIT                                   (0)
        #define TIM_UG_BITS                                  (1)

#define TIM2_CCMR1                                           *((volatile uint32_t *)0x40010018u)
#define TIM2_CCMR1_REG                                       *((volatile uint32_t *)0x40010018u)
#define TIM2_CCMR1_ADDR                                      (0x40010018u)
#define TIM2_CCMR1_RESET                                     (0x00000000u)

/* TIM_IC2F field */
        #define TIM_IC2F                                     (0x0000F000u)
        #define TIM_IC2F_MASK                                (0x0000F000u)
        #define TIM_IC2F_BIT                                 (12)
        #define TIM_IC2F_BITS                                (4)

/* TIM_IC2PSC field */
        #define TIM_IC2PSC                                   (0x00000C00u)
        #define TIM_IC2PSC_MASK                              (0x00000C00u)
        #define TIM_IC2PSC_BIT                               (10)
        #define TIM_IC2PSC_BITS                              (2)

/* TIM_IC1F field */
        #define TIM_IC1F                                     (0x000000F0u)
        #define TIM_IC1F_MASK                                (0x000000F0u)
        #define TIM_IC1F_BIT                                 (4)
        #define TIM_IC1F_BITS                                (4)

/* TIM_IC1PSC field */
        #define TIM_IC1PSC                                   (0x0000000Cu)
        #define TIM_IC1PSC_MASK                              (0x0000000Cu)
        #define TIM_IC1PSC_BIT                               (2)
        #define TIM_IC1PSC_BITS                              (2)

/* TIM_OC2CE field */
        #define TIM_OC2CE                                    (0x00008000u)
        #define TIM_OC2CE_MASK                               (0x00008000u)
        #define TIM_OC2CE_BIT                                (15)
        #define TIM_OC2CE_BITS                               (1)

/* TIM_OC2M field */
        #define TIM_OC2M                                     (0x00007000u)
        #define TIM_OC2M_MASK                                (0x00007000u)
        #define TIM_OC2M_BIT                                 (12)
        #define TIM_OC2M_BITS                                (3)

/* TIM_OC2BE field */
        #define TIM_OC2BE                                    (0x00000800u)
        #define TIM_OC2BE_MASK                               (0x00000800u)
        #define TIM_OC2BE_BIT                                (11)
        #define TIM_OC2BE_BITS                               (1)

/* TIM_OC2FE field */
        #define TIM_OC2FE                                    (0x00000400u)
        #define TIM_OC2FE_MASK                               (0x00000400u)
        #define TIM_OC2FE_BIT                                (10)
        #define TIM_OC2FE_BITS                               (1)

/* TIM_CC2S field */
        #define TIM_CC2S                                     (0x00000300u)
        #define TIM_CC2S_MASK                                (0x00000300u)
        #define TIM_CC2S_BIT                                 (8)
        #define TIM_CC2S_BITS                                (2)

/* TIM_OC1CE field */
        #define TIM_OC1CE                                    (0x00000080u)
        #define TIM_OC1CE_MASK                               (0x00000080u)
        #define TIM_OC1CE_BIT                                (7)
        #define TIM_OC1CE_BITS                               (1)

/* TIM_OC1M field */
        #define TIM_OC1M                                     (0x00000070u)
        #define TIM_OC1M_MASK                                (0x00000070u)
        #define TIM_OC1M_BIT                                 (4)
        #define TIM_OC1M_BITS                                (3)

/* TIM_OC1PE field */
        #define TIM_OC1PE                                    (0x00000008u)
        #define TIM_OC1PE_MASK                               (0x00000008u)
        #define TIM_OC1PE_BIT                                (3)
        #define TIM_OC1PE_BITS                               (1)

/* TIM_OC1FE field */
        #define TIM_OC1FE                                    (0x00000004u)
        #define TIM_OC1FE_MASK                               (0x00000004u)
        #define TIM_OC1FE_BIT                                (2)
        #define TIM_OC1FE_BITS                               (1)

/* TIM_CC1S field */
        #define TIM_CC1S                                     (0x00000003u)
        #define TIM_CC1S_MASK                                (0x00000003u)
        #define TIM_CC1S_BIT                                 (0)
        #define TIM_CC1S_BITS                                (2)

#define TIM2_CCMR2                                           *((volatile uint32_t *)0x4001001Cu)
#define TIM2_CCMR2_REG                                       *((volatile uint32_t *)0x4001001Cu)
#define TIM2_CCMR2_ADDR                                      (0x4001001Cu)
#define TIM2_CCMR2_RESET                                     (0x00000000u)

/* TIM_IC4F field */
        #define TIM_IC4F                                     (0x0000F000u)
        #define TIM_IC4F_MASK                                (0x0000F000u)
        #define TIM_IC4F_BIT                                 (12)
        #define TIM_IC4F_BITS                                (4)

/* TIM_IC4PSC field */
        #define TIM_IC4PSC                                   (0x00000C00u)
        #define TIM_IC4PSC_MASK                              (0x00000C00u)
        #define TIM_IC4PSC_BIT                               (10)
        #define TIM_IC4PSC_BITS                              (2)

/* TIM_IC3F field */
        #define TIM_IC3F                                     (0x000000F0u)
        #define TIM_IC3F_MASK                                (0x000000F0u)
        #define TIM_IC3F_BIT                                 (4)
        #define TIM_IC3F_BITS                                (4)

/* TIM_IC3PSC field */
        #define TIM_IC3PSC                                   (0x0000000Cu)
        #define TIM_IC3PSC_MASK                              (0x0000000Cu)
        #define TIM_IC3PSC_BIT                               (2)
        #define TIM_IC3PSC_BITS                              (2)

/* TIM_OC4CE field */
        #define TIM_OC4CE                                    (0x00008000u)
        #define TIM_OC4CE_MASK                               (0x00008000u)
        #define TIM_OC4CE_BIT                                (15)
        #define TIM_OC4CE_BITS                               (1)

/* TIM_OC4M field */
        #define TIM_OC4M                                     (0x00007000u)
        #define TIM_OC4M_MASK                                (0x00007000u)
        #define TIM_OC4M_BIT                                 (12)
        #define TIM_OC4M_BITS                                (3)

/* TIM_OC4BE field */
        #define TIM_OC4BE                                    (0x00000800u)
        #define TIM_OC4BE_MASK                               (0x00000800u)
        #define TIM_OC4BE_BIT                                (11)
        #define TIM_OC4BE_BITS                               (1)

/* TIM_OC4FE field */
        #define TIM_OC4FE                                    (0x00000400u)
        #define TIM_OC4FE_MASK                               (0x00000400u)
        #define TIM_OC4FE_BIT                                (10)
        #define TIM_OC4FE_BITS                               (1)

/* TIM_CC4S field */
        #define TIM_CC4S                                     (0x00000300u)
        #define TIM_CC4S_MASK                                (0x00000300u)
        #define TIM_CC4S_BIT                                 (8)
        #define TIM_CC4S_BITS                                (2)

/* TIM_OC3CE field */
        #define TIM_OC3CE                                    (0x00000080u)
        #define TIM_OC3CE_MASK                               (0x00000080u)
        #define TIM_OC3CE_BIT                                (7)
        #define TIM_OC3CE_BITS                               (1)

/* TIM_OC3M field */
        #define TIM_OC3M                                     (0x00000070u)
        #define TIM_OC3M_MASK                                (0x00000070u)
        #define TIM_OC3M_BIT                                 (4)
        #define TIM_OC3M_BITS                                (3)

/* TIM_OC3BE field */
        #define TIM_OC3BE                                    (0x00000008u)
        #define TIM_OC3BE_MASK                               (0x00000008u)
        #define TIM_OC3BE_BIT                                (3)
        #define TIM_OC3BE_BITS                               (1)

/* TIM_OC3FE field */
        #define TIM_OC3FE                                    (0x00000004u)
        #define TIM_OC3FE_MASK                               (0x00000004u)
        #define TIM_OC3FE_BIT                                (2)
        #define TIM_OC3FE_BITS                               (1)

/* TIM_CC3S field */
        #define TIM_CC3S                                     (0x00000003u)
        #define TIM_CC3S_MASK                                (0x00000003u)
        #define TIM_CC3S_BIT                                 (0)
        #define TIM_CC3S_BITS                                (2)

#define TIM2_CCER                                            *((volatile uint32_t *)0x40010020u)
#define TIM2_CCER_REG                                        *((volatile uint32_t *)0x40010020u)
#define TIM2_CCER_ADDR                                       (0x40010020u)
#define TIM2_CCER_RESET                                      (0x00000000u)

/* TIM_CC4P field */
        #define TIM_CC4P                                     (0x00002000u)
        #define TIM_CC4P_MASK                                (0x00002000u)
        #define TIM_CC4P_BIT                                 (13)
        #define TIM_CC4P_BITS                                (1)

/* TIM_CC4E field */
        #define TIM_CC4E                                     (0x00001000u)
        #define TIM_CC4E_MASK                                (0x00001000u)
        #define TIM_CC4E_BIT                                 (12)
        #define TIM_CC4E_BITS                                (1)

/* TIM_CC3P field */
        #define TIM_CC3P                                     (0x00000200u)
        #define TIM_CC3P_MASK                                (0x00000200u)
        #define TIM_CC3P_BIT                                 (9)
        #define TIM_CC3P_BITS                                (1)

/* TIM_CC3E field */
        #define TIM_CC3E                                     (0x00000100u)
        #define TIM_CC3E_MASK                                (0x00000100u)
        #define TIM_CC3E_BIT                                 (8)
        #define TIM_CC3E_BITS                                (1)

/* TIM_CC2P field */
        #define TIM_CC2P                                     (0x00000020u)
        #define TIM_CC2P_MASK                                (0x00000020u)
        #define TIM_CC2P_BIT                                 (5)
        #define TIM_CC2P_BITS                                (1)

/* TIM_CC2E field */
        #define TIM_CC2E                                     (0x00000010u)
        #define TIM_CC2E_MASK                                (0x00000010u)
        #define TIM_CC2E_BIT                                 (4)
        #define TIM_CC2E_BITS                                (1)

/* TIM_CC1P field */
        #define TIM_CC1P                                     (0x00000002u)
        #define TIM_CC1P_MASK                                (0x00000002u)
        #define TIM_CC1P_BIT                                 (1)
        #define TIM_CC1P_BITS                                (1)

/* TIM_CC1E field */
        #define TIM_CC1E                                     (0x00000001u)
        #define TIM_CC1E_MASK                                (0x00000001u)
        #define TIM_CC1E_BIT                                 (0)
        #define TIM_CC1E_BITS                                (1)

#define TIM2_CNT                                             *((volatile uint32_t *)0x40010024u)
#define TIM2_CNT_REG                                         *((volatile uint32_t *)0x40010024u)
#define TIM2_CNT_ADDR                                        (0x40010024u)
#define TIM2_CNT_RESET                                       (0x00000000u)

/* TIM_CNT field */
        #define TIM_CNT                                      (0x0000FFFFu)
        #define TIM_CNT_MASK                                 (0x0000FFFFu)
        #define TIM_CNT_BIT                                  (0)
        #define TIM_CNT_BITS                                 (16)

#define TIM2_PSC                                             *((volatile uint32_t *)0x40010028u)
#define TIM2_PSC_REG                                         *((volatile uint32_t *)0x40010028u)
#define TIM2_PSC_ADDR                                        (0x40010028u)
#define TIM2_PSC_RESET                                       (0x00000000u)

/* TIM_PSC field */
        #define TIM_PSC                                      (0x0000000Fu)
        #define TIM_PSC_MASK                                 (0x0000000Fu)
        #define TIM_PSC_BIT                                  (0)
        #define TIM_PSC_BITS                                 (4)

#define TIM2_ARR                                             *((volatile uint32_t *)0x4001002Cu)
#define TIM2_ARR_REG                                         *((volatile uint32_t *)0x4001002Cu)
#define TIM2_ARR_ADDR                                        (0x4001002Cu)
#define TIM2_ARR_RESET                                       (0x0000FFFFu)

/* TIM_ARR field */
        #define TIM_ARR                                      (0x0000FFFFu)
        #define TIM_ARR_MASK                                 (0x0000FFFFu)
        #define TIM_ARR_BIT                                  (0)
        #define TIM_ARR_BITS                                 (16)

#define TIM2_CCR1                                            *((volatile uint32_t *)0x40010034u)
#define TIM2_CCR1_REG                                        *((volatile uint32_t *)0x40010034u)
#define TIM2_CCR1_ADDR                                       (0x40010034u)
#define TIM2_CCR1_RESET                                      (0x00000000u)

/* TIM_CCR field */
        #define TIM_CCR                                      (0x0000FFFFu)
        #define TIM_CCR_MASK                                 (0x0000FFFFu)
        #define TIM_CCR_BIT                                  (0)
        #define TIM_CCR_BITS                                 (16)

#define TIM2_CCR2                                            *((volatile uint32_t *)0x40010038u)
#define TIM2_CCR2_REG                                        *((volatile uint32_t *)0x40010038u)
#define TIM2_CCR2_ADDR                                       (0x40010038u)
#define TIM2_CCR2_RESET                                      (0x00000000u)

/* TIM_CCR field */
        #define TIM_CCR                                      (0x0000FFFFu)
        #define TIM_CCR_MASK                                 (0x0000FFFFu)
        #define TIM_CCR_BIT                                  (0)
        #define TIM_CCR_BITS                                 (16)

#define TIM2_CCR3                                            *((volatile uint32_t *)0x4001003Cu)
#define TIM2_CCR3_REG                                        *((volatile uint32_t *)0x4001003Cu)
#define TIM2_CCR3_ADDR                                       (0x4001003Cu)
#define TIM2_CCR3_RESET                                      (0x00000000u)

/* TIM_CCR field */
        #define TIM_CCR                                      (0x0000FFFFu)
        #define TIM_CCR_MASK                                 (0x0000FFFFu)
        #define TIM_CCR_BIT                                  (0)
        #define TIM_CCR_BITS                                 (16)

#define TIM2_CCR4                                            *((volatile uint32_t *)0x40010040u)
#define TIM2_CCR4_REG                                        *((volatile uint32_t *)0x40010040u)
#define TIM2_CCR4_ADDR                                       (0x40010040u)
#define TIM2_CCR4_RESET                                      (0x00000000u)

/* TIM_CCR field */
        #define TIM_CCR                                      (0x0000FFFFu)
        #define TIM_CCR_MASK                                 (0x0000FFFFu)
        #define TIM_CCR_BIT                                  (0)
        #define TIM_CCR_BITS                                 (16)

#define TIM2_OR                                              *((volatile uint32_t *)0x40010050u)
#define TIM2_OR_REG                                          *((volatile uint32_t *)0x40010050u)
#define TIM2_OR_ADDR                                         (0x40010050u)
#define TIM2_OR_RESET                                        (0x00000000u)

/* TIM_REMAPC4 field */
        #define TIM_REMAPC4                                  (0x00000080u)
        #define TIM_REMAPC4_MASK                             (0x00000080u)
        #define TIM_REMAPC4_BIT                              (7)
        #define TIM_REMAPC4_BITS                             (1)

/* TIM_REMAPC3 field */
        #define TIM_REMAPC3                                  (0x00000040u)
        #define TIM_REMAPC3_MASK                             (0x00000040u)
        #define TIM_REMAPC3_BIT                              (6)
        #define TIM_REMAPC3_BITS                             (1)

/* TIM_REMAPC2 field */
        #define TIM_REMAPC2                                  (0x00000020u)
        #define TIM_REMAPC2_MASK                             (0x00000020u)
        #define TIM_REMAPC2_BIT                              (5)
        #define TIM_REMAPC2_BITS                             (1)

/* TIM_REMAPC1 field */
        #define TIM_REMAPC1                                  (0x00000010u)
        #define TIM_REMAPC1_MASK                             (0x00000010u)
        #define TIM_REMAPC1_BIT                              (4)
        #define TIM_REMAPC1_BITS                             (1)

/* TIM_ORRSVD field */
        #define TIM_ORRSVD                                   (0x00000008u)
        #define TIM_ORRSVD_MASK                              (0x00000008u)
        #define TIM_ORRSVD_BIT                               (3)
        #define TIM_ORRSVD_BITS                              (1)

/* TIM_CLKMSKEN field */
        #define TIM_CLKMSKEN                                 (0x00000004u)
        #define TIM_CLKMSKEN_MASK                            (0x00000004u)
        #define TIM_CLKMSKEN_BIT                             (2)
        #define TIM_CLKMSKEN_BITS                            (1)

/* TIM_EXTRIGSEL field */
        #define TIM_EXTRIGSEL                                (0x00000003u)
        #define TIM_EXTRIGSEL_MASK                           (0x00000003u)
        #define TIM_EXTRIGSEL_BIT                            (0)
        #define TIM_EXTRIGSEL_BITS                           (2)

/* USB block */
#define BLOCK_USB_BASE                                       (0x40011000u)
#define BLOCK_USB_END                                        (0x400110ACu)
#define BLOCK_USB_SIZE                                       (BLOCK_USB_END - BLOCK_USB_BASE + 1)

#define USB_BUFBASEA                                         *((volatile uint32_t *)0x40011000u)
#define USB_BUFBASEA_REG                                     *((volatile uint32_t *)0x40011000u)
#define USB_BUFBASEA_ADDR                                    (0x40011000u)
#define USB_BUFBASEA_RESET                                   (0x20000000u)

/* USB_BUFBASEA_FIELD field */
        #define USB_BUFBASEA_FIELD                           (0xFFFFFFFFu)
        #define USB_BUFBASEA_FIELD_MASK                      (0xFFFFFFFFu)
        #define USB_BUFBASEA_FIELD_BIT                       (0)
        #define USB_BUFBASEA_FIELD_BITS                      (32)

#define USB_BUFBASEB                                         *((volatile uint32_t *)0x40011004u)
#define USB_BUFBASEB_REG                                     *((volatile uint32_t *)0x40011004u)
#define USB_BUFBASEB_ADDR                                    (0x40011004u)
#define USB_BUFBASEB_RESET                                   (0x20000000u)

/* USB_BUFBASEB_FIELD field */
        #define USB_BUFBASEB_FIELD                           (0xFFFFFFFFu)
        #define USB_BUFBASEB_FIELD_MASK                      (0xFFFFFFFFu)
        #define USB_BUFBASEB_FIELD_BIT                       (0)
        #define USB_BUFBASEB_FIELD_BITS                      (32)

#define USB_TXLOAD                                           *((volatile uint32_t *)0x40011008u)
#define USB_TXLOAD_REG                                       *((volatile uint32_t *)0x40011008u)
#define USB_TXLOAD_ADDR                                      (0x40011008u)
#define USB_TXLOAD_RESET                                     (0x00000000u)

/* USB_TXLOADEP6B field */
        #define USB_TXLOADEP6B                               (0x00004000u)
        #define USB_TXLOADEP6B_MASK                          (0x00004000u)
        #define USB_TXLOADEP6B_BIT                           (14)
        #define USB_TXLOADEP6B_BITS                          (1)

/* USB_TXLOADEP5B field */
        #define USB_TXLOADEP5B                               (0x00002000u)
        #define USB_TXLOADEP5B_MASK                          (0x00002000u)
        #define USB_TXLOADEP5B_BIT                           (13)
        #define USB_TXLOADEP5B_BITS                          (1)

/* USB_TXLOADEP4B field */
        #define USB_TXLOADEP4B                               (0x00001000u)
        #define USB_TXLOADEP4B_MASK                          (0x00001000u)
        #define USB_TXLOADEP4B_BIT                           (12)
        #define USB_TXLOADEP4B_BITS                          (1)

/* USB_TXLOADEP3B field */
        #define USB_TXLOADEP3B                               (0x00000800u)
        #define USB_TXLOADEP3B_MASK                          (0x00000800u)
        #define USB_TXLOADEP3B_BIT                           (11)
        #define USB_TXLOADEP3B_BITS                          (1)

/* USB_TXLOADEP2B field */
        #define USB_TXLOADEP2B                               (0x00000400u)
        #define USB_TXLOADEP2B_MASK                          (0x00000400u)
        #define USB_TXLOADEP2B_BIT                           (10)
        #define USB_TXLOADEP2B_BITS                          (1)

/* USB_TXLOADEP1B field */
        #define USB_TXLOADEP1B                               (0x00000200u)
        #define USB_TXLOADEP1B_MASK                          (0x00000200u)
        #define USB_TXLOADEP1B_BIT                           (9)
        #define USB_TXLOADEP1B_BITS                          (1)

/* USB_TXLOADEP0B field */
        #define USB_TXLOADEP0B                               (0x00000100u)
        #define USB_TXLOADEP0B_MASK                          (0x00000100u)
        #define USB_TXLOADEP0B_BIT                           (8)
        #define USB_TXLOADEP0B_BITS                          (1)

/* USB_TXLOADEP6A field */
        #define USB_TXLOADEP6A                               (0x00000040u)
        #define USB_TXLOADEP6A_MASK                          (0x00000040u)
        #define USB_TXLOADEP6A_BIT                           (6)
        #define USB_TXLOADEP6A_BITS                          (1)

/* USB_TXLOADEP5A field */
        #define USB_TXLOADEP5A                               (0x00000020u)
        #define USB_TXLOADEP5A_MASK                          (0x00000020u)
        #define USB_TXLOADEP5A_BIT                           (5)
        #define USB_TXLOADEP5A_BITS                          (1)

/* USB_TXLOADEP4A field */
        #define USB_TXLOADEP4A                               (0x00000010u)
        #define USB_TXLOADEP4A_MASK                          (0x00000010u)
        #define USB_TXLOADEP4A_BIT                           (4)
        #define USB_TXLOADEP4A_BITS                          (1)

/* USB_TXLOADEP3A field */
        #define USB_TXLOADEP3A                               (0x00000008u)
        #define USB_TXLOADEP3A_MASK                          (0x00000008u)
        #define USB_TXLOADEP3A_BIT                           (3)
        #define USB_TXLOADEP3A_BITS                          (1)

/* USB_TXLOADEP2A field */
        #define USB_TXLOADEP2A                               (0x00000004u)
        #define USB_TXLOADEP2A_MASK                          (0x00000004u)
        #define USB_TXLOADEP2A_BIT                           (2)
        #define USB_TXLOADEP2A_BITS                          (1)

/* USB_TXLOADEP1A field */
        #define USB_TXLOADEP1A                               (0x00000002u)
        #define USB_TXLOADEP1A_MASK                          (0x00000002u)
        #define USB_TXLOADEP1A_BIT                           (1)
        #define USB_TXLOADEP1A_BITS                          (1)

/* USB_TXLOADEP0A field */
        #define USB_TXLOADEP0A                               (0x00000001u)
        #define USB_TXLOADEP0A_MASK                          (0x00000001u)
        #define USB_TXLOADEP0A_BIT                           (0)
        #define USB_TXLOADEP0A_BITS                          (1)

#define USB_TXACTIVE                                         *((volatile uint32_t *)0x4001100Cu)
#define USB_TXACTIVE_REG                                     *((volatile uint32_t *)0x4001100Cu)
#define USB_TXACTIVE_ADDR                                    (0x4001100Cu)
#define USB_TXACTIVE_RESET                                   (0x00000000u)

/* USB_TXACTIVEEP6B field */
        #define USB_TXACTIVEEP6B                             (0x00004000u)
        #define USB_TXACTIVEEP6B_MASK                        (0x00004000u)
        #define USB_TXACTIVEEP6B_BIT                         (14)
        #define USB_TXACTIVEEP6B_BITS                        (1)

/* USB_TXACTIVEEP5B field */
        #define USB_TXACTIVEEP5B                             (0x00002000u)
        #define USB_TXACTIVEEP5B_MASK                        (0x00002000u)
        #define USB_TXACTIVEEP5B_BIT                         (13)
        #define USB_TXACTIVEEP5B_BITS                        (1)

/* USB_TXACTIVEEP4B field */
        #define USB_TXACTIVEEP4B                             (0x00001000u)
        #define USB_TXACTIVEEP4B_MASK                        (0x00001000u)
        #define USB_TXACTIVEEP4B_BIT                         (12)
        #define USB_TXACTIVEEP4B_BITS                        (1)

/* USB_TXACTIVEEP3B field */
        #define USB_TXACTIVEEP3B                             (0x00000800u)
        #define USB_TXACTIVEEP3B_MASK                        (0x00000800u)
        #define USB_TXACTIVEEP3B_BIT                         (11)
        #define USB_TXACTIVEEP3B_BITS                        (1)

/* USB_TXACTIVEEP2B field */
        #define USB_TXACTIVEEP2B                             (0x00000400u)
        #define USB_TXACTIVEEP2B_MASK                        (0x00000400u)
        #define USB_TXACTIVEEP2B_BIT                         (10)
        #define USB_TXACTIVEEP2B_BITS                        (1)

/* USB_TXACTIVEEP1B field */
        #define USB_TXACTIVEEP1B                             (0x00000200u)
        #define USB_TXACTIVEEP1B_MASK                        (0x00000200u)
        #define USB_TXACTIVEEP1B_BIT                         (9)
        #define USB_TXACTIVEEP1B_BITS                        (1)

/* USB_TXACTIVEEP0B field */
        #define USB_TXACTIVEEP0B                             (0x00000100u)
        #define USB_TXACTIVEEP0B_MASK                        (0x00000100u)
        #define USB_TXACTIVEEP0B_BIT                         (8)
        #define USB_TXACTIVEEP0B_BITS                        (1)

/* USB_TXACTIVEEP6A field */
        #define USB_TXACTIVEEP6A                             (0x00000040u)
        #define USB_TXACTIVEEP6A_MASK                        (0x00000040u)
        #define USB_TXACTIVEEP6A_BIT                         (6)
        #define USB_TXACTIVEEP6A_BITS                        (1)

/* USB_TXACTIVEEP5A field */
        #define USB_TXACTIVEEP5A                             (0x00000020u)
        #define USB_TXACTIVEEP5A_MASK                        (0x00000020u)
        #define USB_TXACTIVEEP5A_BIT                         (5)
        #define USB_TXACTIVEEP5A_BITS                        (1)

/* USB_TXACTIVEEP4A field */
        #define USB_TXACTIVEEP4A                             (0x00000010u)
        #define USB_TXACTIVEEP4A_MASK                        (0x00000010u)
        #define USB_TXACTIVEEP4A_BIT                         (4)
        #define USB_TXACTIVEEP4A_BITS                        (1)

/* USB_TXACTIVEEP3A field */
        #define USB_TXACTIVEEP3A                             (0x00000008u)
        #define USB_TXACTIVEEP3A_MASK                        (0x00000008u)
        #define USB_TXACTIVEEP3A_BIT                         (3)
        #define USB_TXACTIVEEP3A_BITS                        (1)

/* USB_TXACTIVEEP2A field */
        #define USB_TXACTIVEEP2A                             (0x00000004u)
        #define USB_TXACTIVEEP2A_MASK                        (0x00000004u)
        #define USB_TXACTIVEEP2A_BIT                         (2)
        #define USB_TXACTIVEEP2A_BITS                        (1)

/* USB_TXACTIVEEP1A field */
        #define USB_TXACTIVEEP1A                             (0x00000002u)
        #define USB_TXACTIVEEP1A_MASK                        (0x00000002u)
        #define USB_TXACTIVEEP1A_BIT                         (1)
        #define USB_TXACTIVEEP1A_BITS                        (1)

/* USB_TXACTIVEEP0A field */
        #define USB_TXACTIVEEP0A                             (0x00000001u)
        #define USB_TXACTIVEEP0A_MASK                        (0x00000001u)
        #define USB_TXACTIVEEP0A_BIT                         (0)
        #define USB_TXACTIVEEP0A_BITS                        (1)

#define USB_TXBUFSIZEEP0A                                    *((volatile uint32_t *)0x40011010u)
#define USB_TXBUFSIZEEP0A_REG                                *((volatile uint32_t *)0x40011010u)
#define USB_TXBUFSIZEEP0A_ADDR                               (0x40011010u)
#define USB_TXBUFSIZEEP0A_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP0A_FIELD field */
        #define USB_TXBUFSIZEEP0A_FIELD                      (0x0000000Fu)
        #define USB_TXBUFSIZEEP0A_FIELD_MASK                 (0x0000000Fu)
        #define USB_TXBUFSIZEEP0A_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP0A_FIELD_BITS                 (4)

#define USB_TXBUFSIZEEP1A                                    *((volatile uint32_t *)0x40011014u)
#define USB_TXBUFSIZEEP1A_REG                                *((volatile uint32_t *)0x40011014u)
#define USB_TXBUFSIZEEP1A_ADDR                               (0x40011014u)
#define USB_TXBUFSIZEEP1A_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP1A_FIELD field */
        #define USB_TXBUFSIZEEP1A_FIELD                      (0x0000000Fu)
        #define USB_TXBUFSIZEEP1A_FIELD_MASK                 (0x0000000Fu)
        #define USB_TXBUFSIZEEP1A_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP1A_FIELD_BITS                 (4)

#define USB_TXBUFSIZEEP2A                                    *((volatile uint32_t *)0x40011018u)
#define USB_TXBUFSIZEEP2A_REG                                *((volatile uint32_t *)0x40011018u)
#define USB_TXBUFSIZEEP2A_ADDR                               (0x40011018u)
#define USB_TXBUFSIZEEP2A_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP2A_FIELD field */
        #define USB_TXBUFSIZEEP2A_FIELD                      (0x0000000Fu)
        #define USB_TXBUFSIZEEP2A_FIELD_MASK                 (0x0000000Fu)
        #define USB_TXBUFSIZEEP2A_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP2A_FIELD_BITS                 (4)

#define USB_TXBUFSIZEEP3A                                    *((volatile uint32_t *)0x4001101Cu)
#define USB_TXBUFSIZEEP3A_REG                                *((volatile uint32_t *)0x4001101Cu)
#define USB_TXBUFSIZEEP3A_ADDR                               (0x4001101Cu)
#define USB_TXBUFSIZEEP3A_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP3A_FIELD field */
        #define USB_TXBUFSIZEEP3A_FIELD                      (0x0000007Fu)
        #define USB_TXBUFSIZEEP3A_FIELD_MASK                 (0x0000007Fu)
        #define USB_TXBUFSIZEEP3A_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP3A_FIELD_BITS                 (7)

#define USB_TXBUFSIZEEP4A                                    *((volatile uint32_t *)0x40011020u)
#define USB_TXBUFSIZEEP4A_REG                                *((volatile uint32_t *)0x40011020u)
#define USB_TXBUFSIZEEP4A_ADDR                               (0x40011020u)
#define USB_TXBUFSIZEEP4A_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP4A_FIELD field */
        #define USB_TXBUFSIZEEP4A_FIELD                      (0x0000003Fu)
        #define USB_TXBUFSIZEEP4A_FIELD_MASK                 (0x0000003Fu)
        #define USB_TXBUFSIZEEP4A_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP4A_FIELD_BITS                 (6)

#define USB_TXBUFSIZEEP5A                                    *((volatile uint32_t *)0x40011024u)
#define USB_TXBUFSIZEEP5A_REG                                *((volatile uint32_t *)0x40011024u)
#define USB_TXBUFSIZEEP5A_ADDR                               (0x40011024u)
#define USB_TXBUFSIZEEP5A_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP5A_FIELD field */
        #define USB_TXBUFSIZEEP5A_FIELD                      (0x0000007Fu)
        #define USB_TXBUFSIZEEP5A_FIELD_MASK                 (0x0000007Fu)
        #define USB_TXBUFSIZEEP5A_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP5A_FIELD_BITS                 (7)

#define USB_TXBUFSIZEEP6A                                    *((volatile uint32_t *)0x40011028u)
#define USB_TXBUFSIZEEP6A_REG                                *((volatile uint32_t *)0x40011028u)
#define USB_TXBUFSIZEEP6A_ADDR                               (0x40011028u)
#define USB_TXBUFSIZEEP6A_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP6A_FIELD field */
        #define USB_TXBUFSIZEEP6A_FIELD                      (0x000003FFu)
        #define USB_TXBUFSIZEEP6A_FIELD_MASK                 (0x000003FFu)
        #define USB_TXBUFSIZEEP6A_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP6A_FIELD_BITS                 (10)

#define USB_TXBUFSIZEEP0B                                    *((volatile uint32_t *)0x4001102Cu)
#define USB_TXBUFSIZEEP0B_REG                                *((volatile uint32_t *)0x4001102Cu)
#define USB_TXBUFSIZEEP0B_ADDR                               (0x4001102Cu)
#define USB_TXBUFSIZEEP0B_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP0B_FIELD field */
        #define USB_TXBUFSIZEEP0B_FIELD                      (0x0000000Fu)
        #define USB_TXBUFSIZEEP0B_FIELD_MASK                 (0x0000000Fu)
        #define USB_TXBUFSIZEEP0B_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP0B_FIELD_BITS                 (4)

#define USB_TXBUFSIZEEP1B                                    *((volatile uint32_t *)0x40011030u)
#define USB_TXBUFSIZEEP1B_REG                                *((volatile uint32_t *)0x40011030u)
#define USB_TXBUFSIZEEP1B_ADDR                               (0x40011030u)
#define USB_TXBUFSIZEEP1B_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP1B_FIELD field */
        #define USB_TXBUFSIZEEP1B_FIELD                      (0x0000000Fu)
        #define USB_TXBUFSIZEEP1B_FIELD_MASK                 (0x0000000Fu)
        #define USB_TXBUFSIZEEP1B_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP1B_FIELD_BITS                 (4)

#define USB_TXBUFSIZEEP2B                                    *((volatile uint32_t *)0x40011034u)
#define USB_TXBUFSIZEEP2B_REG                                *((volatile uint32_t *)0x40011034u)
#define USB_TXBUFSIZEEP2B_ADDR                               (0x40011034u)
#define USB_TXBUFSIZEEP2B_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP2B_FIELD field */
        #define USB_TXBUFSIZEEP2B_FIELD                      (0x0000000Fu)
        #define USB_TXBUFSIZEEP2B_FIELD_MASK                 (0x0000000Fu)
        #define USB_TXBUFSIZEEP2B_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP2B_FIELD_BITS                 (4)

#define USB_TXBUFSIZEEP3B                                    *((volatile uint32_t *)0x40011038u)
#define USB_TXBUFSIZEEP3B_REG                                *((volatile uint32_t *)0x40011038u)
#define USB_TXBUFSIZEEP3B_ADDR                               (0x40011038u)
#define USB_TXBUFSIZEEP3B_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP3B_FIELD field */
        #define USB_TXBUFSIZEEP3B_FIELD                      (0x0000007Fu)
        #define USB_TXBUFSIZEEP3B_FIELD_MASK                 (0x0000007Fu)
        #define USB_TXBUFSIZEEP3B_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP3B_FIELD_BITS                 (7)

#define USB_TXBUFSIZEEP4B                                    *((volatile uint32_t *)0x4001103Cu)
#define USB_TXBUFSIZEEP4B_REG                                *((volatile uint32_t *)0x4001103Cu)
#define USB_TXBUFSIZEEP4B_ADDR                               (0x4001103Cu)
#define USB_TXBUFSIZEEP4B_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP4B_FIELD field */
        #define USB_TXBUFSIZEEP4B_FIELD                      (0x0000003Fu)
        #define USB_TXBUFSIZEEP4B_FIELD_MASK                 (0x0000003Fu)
        #define USB_TXBUFSIZEEP4B_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP4B_FIELD_BITS                 (6)

#define USB_TXBUFSIZEEP5B                                    *((volatile uint32_t *)0x40011040u)
#define USB_TXBUFSIZEEP5B_REG                                *((volatile uint32_t *)0x40011040u)
#define USB_TXBUFSIZEEP5B_ADDR                               (0x40011040u)
#define USB_TXBUFSIZEEP5B_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP5B_FIELD field */
        #define USB_TXBUFSIZEEP5B_FIELD                      (0x0000007Fu)
        #define USB_TXBUFSIZEEP5B_FIELD_MASK                 (0x0000007Fu)
        #define USB_TXBUFSIZEEP5B_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP5B_FIELD_BITS                 (7)

#define USB_TXBUFSIZEEP6B                                    *((volatile uint32_t *)0x40011044u)
#define USB_TXBUFSIZEEP6B_REG                                *((volatile uint32_t *)0x40011044u)
#define USB_TXBUFSIZEEP6B_ADDR                               (0x40011044u)
#define USB_TXBUFSIZEEP6B_RESET                              (0x00000000u)

/* USB_TXBUFSIZEEP6B_FIELD field */
        #define USB_TXBUFSIZEEP6B_FIELD                      (0x000003FFu)
        #define USB_TXBUFSIZEEP6B_FIELD_MASK                 (0x000003FFu)
        #define USB_TXBUFSIZEEP6B_FIELD_BIT                  (0)
        #define USB_TXBUFSIZEEP6B_FIELD_BITS                 (10)

#define USB_RXVALID                                          *((volatile uint32_t *)0x40011048u)
#define USB_RXVALID_REG                                      *((volatile uint32_t *)0x40011048u)
#define USB_RXVALID_ADDR                                     (0x40011048u)
#define USB_RXVALID_RESET                                    (0x00000000u)

/* USB_RXVALIDEP6B field */
        #define USB_RXVALIDEP6B                              (0x00004000u)
        #define USB_RXVALIDEP6B_MASK                         (0x00004000u)
        #define USB_RXVALIDEP6B_BIT                          (14)
        #define USB_RXVALIDEP6B_BITS                         (1)

/* USB_RXVALIDEP5B field */
        #define USB_RXVALIDEP5B                              (0x00002000u)
        #define USB_RXVALIDEP5B_MASK                         (0x00002000u)
        #define USB_RXVALIDEP5B_BIT                          (13)
        #define USB_RXVALIDEP5B_BITS                         (1)

/* USB_RXVALIDEP4B field */
        #define USB_RXVALIDEP4B                              (0x00001000u)
        #define USB_RXVALIDEP4B_MASK                         (0x00001000u)
        #define USB_RXVALIDEP4B_BIT                          (12)
        #define USB_RXVALIDEP4B_BITS                         (1)

/* USB_RXVALIDEP3B field */
        #define USB_RXVALIDEP3B                              (0x00000800u)
        #define USB_RXVALIDEP3B_MASK                         (0x00000800u)
        #define USB_RXVALIDEP3B_BIT                          (11)
        #define USB_RXVALIDEP3B_BITS                         (1)

/* USB_RXVALIDEP2B field */
        #define USB_RXVALIDEP2B                              (0x00000400u)
        #define USB_RXVALIDEP2B_MASK                         (0x00000400u)
        #define USB_RXVALIDEP2B_BIT                          (10)
        #define USB_RXVALIDEP2B_BITS                         (1)

/* USB_RXVALIDEP1B field */
        #define USB_RXVALIDEP1B                              (0x00000200u)
        #define USB_RXVALIDEP1B_MASK                         (0x00000200u)
        #define USB_RXVALIDEP1B_BIT                          (9)
        #define USB_RXVALIDEP1B_BITS                         (1)

/* USB_RXVALIDEP0B field */
        #define USB_RXVALIDEP0B                              (0x00000100u)
        #define USB_RXVALIDEP0B_MASK                         (0x00000100u)
        #define USB_RXVALIDEP0B_BIT                          (8)
        #define USB_RXVALIDEP0B_BITS                         (1)

/* USB_RXVALIDEP6A field */
        #define USB_RXVALIDEP6A                              (0x00000040u)
        #define USB_RXVALIDEP6A_MASK                         (0x00000040u)
        #define USB_RXVALIDEP6A_BIT                          (6)
        #define USB_RXVALIDEP6A_BITS                         (1)

/* USB_RXVALIDEP5A field */
        #define USB_RXVALIDEP5A                              (0x00000020u)
        #define USB_RXVALIDEP5A_MASK                         (0x00000020u)
        #define USB_RXVALIDEP5A_BIT                          (5)
        #define USB_RXVALIDEP5A_BITS                         (1)

/* USB_RXVALIDEP4A field */
        #define USB_RXVALIDEP4A                              (0x00000010u)
        #define USB_RXVALIDEP4A_MASK                         (0x00000010u)
        #define USB_RXVALIDEP4A_BIT                          (4)
        #define USB_RXVALIDEP4A_BITS                         (1)

/* USB_RXVALIDEP3A field */
        #define USB_RXVALIDEP3A                              (0x00000008u)
        #define USB_RXVALIDEP3A_MASK                         (0x00000008u)
        #define USB_RXVALIDEP3A_BIT                          (3)
        #define USB_RXVALIDEP3A_BITS                         (1)

/* USB_RXVALIDEP2A field */
        #define USB_RXVALIDEP2A                              (0x00000004u)
        #define USB_RXVALIDEP2A_MASK                         (0x00000004u)
        #define USB_RXVALIDEP2A_BIT                          (2)
        #define USB_RXVALIDEP2A_BITS                         (1)

/* USB_RXVALIDEP1A field */
        #define USB_RXVALIDEP1A                              (0x00000002u)
        #define USB_RXVALIDEP1A_MASK                         (0x00000002u)
        #define USB_RXVALIDEP1A_BIT                          (1)
        #define USB_RXVALIDEP1A_BITS                         (1)

/* USB_RXVALIDEP0A field */
        #define USB_RXVALIDEP0A                              (0x00000001u)
        #define USB_RXVALIDEP0A_MASK                         (0x00000001u)
        #define USB_RXVALIDEP0A_BIT                          (0)
        #define USB_RXVALIDEP0A_BITS                         (1)

#define USB_ENABLEIN                                         *((volatile uint32_t *)0x4001104Cu)
#define USB_ENABLEIN_REG                                     *((volatile uint32_t *)0x4001104Cu)
#define USB_ENABLEIN_ADDR                                    (0x4001104Cu)
#define USB_ENABLEIN_RESET                                   (0x00000001u)

/* USB_ENABLEINEP6 field */
        #define USB_ENABLEINEP6                              (0x00000040u)
        #define USB_ENABLEINEP6_MASK                         (0x00000040u)
        #define USB_ENABLEINEP6_BIT                          (6)
        #define USB_ENABLEINEP6_BITS                         (1)

/* USB_ENABLEINEP5 field */
        #define USB_ENABLEINEP5                              (0x00000020u)
        #define USB_ENABLEINEP5_MASK                         (0x00000020u)
        #define USB_ENABLEINEP5_BIT                          (5)
        #define USB_ENABLEINEP5_BITS                         (1)

/* USB_ENABLEINEP4 field */
        #define USB_ENABLEINEP4                              (0x00000010u)
        #define USB_ENABLEINEP4_MASK                         (0x00000010u)
        #define USB_ENABLEINEP4_BIT                          (4)
        #define USB_ENABLEINEP4_BITS                         (1)

/* USB_ENABLEINEP3 field */
        #define USB_ENABLEINEP3                              (0x00000008u)
        #define USB_ENABLEINEP3_MASK                         (0x00000008u)
        #define USB_ENABLEINEP3_BIT                          (3)
        #define USB_ENABLEINEP3_BITS                         (1)

/* USB_ENABLEINEP2 field */
        #define USB_ENABLEINEP2                              (0x00000004u)
        #define USB_ENABLEINEP2_MASK                         (0x00000004u)
        #define USB_ENABLEINEP2_BIT                          (2)
        #define USB_ENABLEINEP2_BITS                         (1)

/* USB_ENABLEINEP1 field */
        #define USB_ENABLEINEP1                              (0x00000002u)
        #define USB_ENABLEINEP1_MASK                         (0x00000002u)
        #define USB_ENABLEINEP1_BIT                          (1)
        #define USB_ENABLEINEP1_BITS                         (1)

/* USB_ENABLEINEP0 field */
        #define USB_ENABLEINEP0                              (0x00000001u)
        #define USB_ENABLEINEP0_MASK                         (0x00000001u)
        #define USB_ENABLEINEP0_BIT                          (0)
        #define USB_ENABLEINEP0_BITS                         (1)

#define USB_ENABLEOUT                                        *((volatile uint32_t *)0x40011050u)
#define USB_ENABLEOUT_REG                                    *((volatile uint32_t *)0x40011050u)
#define USB_ENABLEOUT_ADDR                                   (0x40011050u)
#define USB_ENABLEOUT_RESET                                  (0x00000001u)

/* USB_ENABLEOUTEP6 field */
        #define USB_ENABLEOUTEP6                             (0x00000040u)
        #define USB_ENABLEOUTEP6_MASK                        (0x00000040u)
        #define USB_ENABLEOUTEP6_BIT                         (6)
        #define USB_ENABLEOUTEP6_BITS                        (1)

/* USB_ENABLEOUTEP5 field */
        #define USB_ENABLEOUTEP5                             (0x00000020u)
        #define USB_ENABLEOUTEP5_MASK                        (0x00000020u)
        #define USB_ENABLEOUTEP5_BIT                         (5)
        #define USB_ENABLEOUTEP5_BITS                        (1)

/* USB_ENABLEOUTEP4 field */
        #define USB_ENABLEOUTEP4                             (0x00000010u)
        #define USB_ENABLEOUTEP4_MASK                        (0x00000010u)
        #define USB_ENABLEOUTEP4_BIT                         (4)
        #define USB_ENABLEOUTEP4_BITS                        (1)

/* USB_ENABLEOUTEP3 field */
        #define USB_ENABLEOUTEP3                             (0x00000008u)
        #define USB_ENABLEOUTEP3_MASK                        (0x00000008u)
        #define USB_ENABLEOUTEP3_BIT                         (3)
        #define USB_ENABLEOUTEP3_BITS                        (1)

/* USB_ENABLEOUTEP2 field */
        #define USB_ENABLEOUTEP2                             (0x00000004u)
        #define USB_ENABLEOUTEP2_MASK                        (0x00000004u)
        #define USB_ENABLEOUTEP2_BIT                         (2)
        #define USB_ENABLEOUTEP2_BITS                        (1)

/* USB_ENABLEOUTEP1 field */
        #define USB_ENABLEOUTEP1                             (0x00000002u)
        #define USB_ENABLEOUTEP1_MASK                        (0x00000002u)
        #define USB_ENABLEOUTEP1_BIT                         (1)
        #define USB_ENABLEOUTEP1_BITS                        (1)

/* USB_ENABLEOUTEP0 field */
        #define USB_ENABLEOUTEP0                             (0x00000001u)
        #define USB_ENABLEOUTEP0_MASK                        (0x00000001u)
        #define USB_ENABLEOUTEP0_BIT                         (0)
        #define USB_ENABLEOUTEP0_BITS                        (1)

#define USB_STALLIN                                          *((volatile uint32_t *)0x40011054u)
#define USB_STALLIN_REG                                      *((volatile uint32_t *)0x40011054u)
#define USB_STALLIN_ADDR                                     (0x40011054u)
#define USB_STALLIN_RESET                                    (0x00000000u)

/* USB_STALLINEP6 field */
        #define USB_STALLINEP6                               (0x00000040u)
        #define USB_STALLINEP6_MASK                          (0x00000040u)
        #define USB_STALLINEP6_BIT                           (6)
        #define USB_STALLINEP6_BITS                          (1)

/* USB_STALLINEP5 field */
        #define USB_STALLINEP5                               (0x00000020u)
        #define USB_STALLINEP5_MASK                          (0x00000020u)
        #define USB_STALLINEP5_BIT                           (5)
        #define USB_STALLINEP5_BITS                          (1)

/* USB_STALLINEP4 field */
        #define USB_STALLINEP4                               (0x00000010u)
        #define USB_STALLINEP4_MASK                          (0x00000010u)
        #define USB_STALLINEP4_BIT                           (4)
        #define USB_STALLINEP4_BITS                          (1)

/* USB_STALLINEP3 field */
        #define USB_STALLINEP3                               (0x00000008u)
        #define USB_STALLINEP3_MASK                          (0x00000008u)
        #define USB_STALLINEP3_BIT                           (3)
        #define USB_STALLINEP3_BITS                          (1)

/* USB_STALLINEP2 field */
        #define USB_STALLINEP2                               (0x00000004u)
        #define USB_STALLINEP2_MASK                          (0x00000004u)
        #define USB_STALLINEP2_BIT                           (2)
        #define USB_STALLINEP2_BITS                          (1)

/* USB_STALLINEP1 field */
        #define USB_STALLINEP1                               (0x00000002u)
        #define USB_STALLINEP1_MASK                          (0x00000002u)
        #define USB_STALLINEP1_BIT                           (1)
        #define USB_STALLINEP1_BITS                          (1)

/* USB_STALLINEP0 field */
        #define USB_STALLINEP0                               (0x00000001u)
        #define USB_STALLINEP0_MASK                          (0x00000001u)
        #define USB_STALLINEP0_BIT                           (0)
        #define USB_STALLINEP0_BITS                          (1)

#define USB_STALLOUT                                         *((volatile uint32_t *)0x40011058u)
#define USB_STALLOUT_REG                                     *((volatile uint32_t *)0x40011058u)
#define USB_STALLOUT_ADDR                                    (0x40011058u)
#define USB_STALLOUT_RESET                                   (0x00000000u)

/* USB_STALLOUTEP6 field */
        #define USB_STALLOUTEP6                              (0x00000040u)
        #define USB_STALLOUTEP6_MASK                         (0x00000040u)
        #define USB_STALLOUTEP6_BIT                          (6)
        #define USB_STALLOUTEP6_BITS                         (1)

/* USB_STALLOUTEP5 field */
        #define USB_STALLOUTEP5                              (0x00000020u)
        #define USB_STALLOUTEP5_MASK                         (0x00000020u)
        #define USB_STALLOUTEP5_BIT                          (5)
        #define USB_STALLOUTEP5_BITS                         (1)

/* USB_STALLOUTEP4 field */
        #define USB_STALLOUTEP4                              (0x00000010u)
        #define USB_STALLOUTEP4_MASK                         (0x00000010u)
        #define USB_STALLOUTEP4_BIT                          (4)
        #define USB_STALLOUTEP4_BITS                         (1)

/* USB_STALLOUTEP3 field */
        #define USB_STALLOUTEP3                              (0x00000008u)
        #define USB_STALLOUTEP3_MASK                         (0x00000008u)
        #define USB_STALLOUTEP3_BIT                          (3)
        #define USB_STALLOUTEP3_BITS                         (1)

/* USB_STALLOUTEP2 field */
        #define USB_STALLOUTEP2                              (0x00000004u)
        #define USB_STALLOUTEP2_MASK                         (0x00000004u)
        #define USB_STALLOUTEP2_BIT                          (2)
        #define USB_STALLOUTEP2_BITS                         (1)

/* USB_STALLOUTEP1 field */
        #define USB_STALLOUTEP1                              (0x00000002u)
        #define USB_STALLOUTEP1_MASK                         (0x00000002u)
        #define USB_STALLOUTEP1_BIT                          (1)
        #define USB_STALLOUTEP1_BITS                         (1)

/* USB_STALLOUTEP0 field */
        #define USB_STALLOUTEP0                              (0x00000001u)
        #define USB_STALLOUTEP0_MASK                         (0x00000001u)
        #define USB_STALLOUTEP0_BIT                          (0)
        #define USB_STALLOUTEP0_BITS                         (1)

#define USB_CTRL                                             *((volatile uint32_t *)0x4001105Cu)
#define USB_CTRL_REG                                         *((volatile uint32_t *)0x4001105Cu)
#define USB_CTRL_ADDR                                        (0x4001105Cu)
#define USB_CTRL_RESET                                       (0x00000004u)

/* USB_RESETCTRL field */
        #define USB_RESETCTRL                                (0x00800000u)
        #define USB_RESETCTRL_MASK                           (0x00800000u)
        #define USB_RESETCTRL_BIT                            (23)
        #define USB_RESETCTRL_BITS                           (1)

/* USB_ENBUFOUTEP6B field */
        #define USB_ENBUFOUTEP6B                             (0x00400000u)
        #define USB_ENBUFOUTEP6B_MASK                        (0x00400000u)
        #define USB_ENBUFOUTEP6B_BIT                         (22)
        #define USB_ENBUFOUTEP6B_BITS                        (1)

/* USB_ENBUFOUTEP5B field */
        #define USB_ENBUFOUTEP5B                             (0x00200000u)
        #define USB_ENBUFOUTEP5B_MASK                        (0x00200000u)
        #define USB_ENBUFOUTEP5B_BIT                         (21)
        #define USB_ENBUFOUTEP5B_BITS                        (1)

/* USB_ENBUFOUTEP4B field */
        #define USB_ENBUFOUTEP4B                             (0x00100000u)
        #define USB_ENBUFOUTEP4B_MASK                        (0x00100000u)
        #define USB_ENBUFOUTEP4B_BIT                         (20)
        #define USB_ENBUFOUTEP4B_BITS                        (1)

/* USB_ENBUFOUTEP3B field */
        #define USB_ENBUFOUTEP3B                             (0x00080000u)
        #define USB_ENBUFOUTEP3B_MASK                        (0x00080000u)
        #define USB_ENBUFOUTEP3B_BIT                         (19)
        #define USB_ENBUFOUTEP3B_BITS                        (1)

/* USB_ENBUFOUTEP2B field */
        #define USB_ENBUFOUTEP2B                             (0x00040000u)
        #define USB_ENBUFOUTEP2B_MASK                        (0x00040000u)
        #define USB_ENBUFOUTEP2B_BIT                         (18)
        #define USB_ENBUFOUTEP2B_BITS                        (1)

/* USB_ENBUFOUTEP1B field */
        #define USB_ENBUFOUTEP1B                             (0x00020000u)
        #define USB_ENBUFOUTEP1B_MASK                        (0x00020000u)
        #define USB_ENBUFOUTEP1B_BIT                         (17)
        #define USB_ENBUFOUTEP1B_BITS                        (1)

/* USB_ENBUFOUTEP0B field */
        #define USB_ENBUFOUTEP0B                             (0x00010000u)
        #define USB_ENBUFOUTEP0B_MASK                        (0x00010000u)
        #define USB_ENBUFOUTEP0B_BIT                         (16)
        #define USB_ENBUFOUTEP0B_BITS                        (1)

/* USB_ENBUFINEP6B field */
        #define USB_ENBUFINEP6B                              (0x00004000u)
        #define USB_ENBUFINEP6B_MASK                         (0x00004000u)
        #define USB_ENBUFINEP6B_BIT                          (14)
        #define USB_ENBUFINEP6B_BITS                         (1)

/* USB_ENBUFINEP5B field */
        #define USB_ENBUFINEP5B                              (0x00002000u)
        #define USB_ENBUFINEP5B_MASK                         (0x00002000u)
        #define USB_ENBUFINEP5B_BIT                          (13)
        #define USB_ENBUFINEP5B_BITS                         (1)

/* USB_ENBUFINEP4B field */
        #define USB_ENBUFINEP4B                              (0x00001000u)
        #define USB_ENBUFINEP4B_MASK                         (0x00001000u)
        #define USB_ENBUFINEP4B_BIT                          (12)
        #define USB_ENBUFINEP4B_BITS                         (1)

/* USB_ENBUFINEP3B field */
        #define USB_ENBUFINEP3B                              (0x00000800u)
        #define USB_ENBUFINEP3B_MASK                         (0x00000800u)
        #define USB_ENBUFINEP3B_BIT                          (11)
        #define USB_ENBUFINEP3B_BITS                         (1)

/* USB_ENBUFINEP2B field */
        #define USB_ENBUFINEP2B                              (0x00000400u)
        #define USB_ENBUFINEP2B_MASK                         (0x00000400u)
        #define USB_ENBUFINEP2B_BIT                          (10)
        #define USB_ENBUFINEP2B_BITS                         (1)

/* USB_ENBUFINEP1B field */
        #define USB_ENBUFINEP1B                              (0x00000200u)
        #define USB_ENBUFINEP1B_MASK                         (0x00000200u)
        #define USB_ENBUFINEP1B_BIT                          (9)
        #define USB_ENBUFINEP1B_BITS                         (1)

/* USB_ENBUFINEP0B field */
        #define USB_ENBUFINEP0B                              (0x00000100u)
        #define USB_ENBUFINEP0B_MASK                         (0x00000100u)
        #define USB_ENBUFINEP0B_BIT                          (8)
        #define USB_ENBUFINEP0B_BITS                         (1)

/* USB_REMOTEWKUPEN field */
        #define USB_REMOTEWKUPEN                             (0x00000004u)
        #define USB_REMOTEWKUPEN_MASK                        (0x00000004u)
        #define USB_REMOTEWKUPEN_BIT                         (2)
        #define USB_REMOTEWKUPEN_BITS                        (1)

/* USB_CLRFEP0STALL field */
        #define USB_CLRFEP0STALL                             (0x00000002u)
        #define USB_CLRFEP0STALL_MASK                        (0x00000002u)
        #define USB_CLRFEP0STALL_BIT                         (1)
        #define USB_CLRFEP0STALL_BITS                        (1)

/* USB_SELFPWRD field */
        #define USB_SELFPWRD                                 (0x00000001u)
        #define USB_SELFPWRD_MASK                            (0x00000001u)
        #define USB_SELFPWRD_BIT                             (0)
        #define USB_SELFPWRD_BITS                            (1)

#define USB_PIPECLR                                          *((volatile uint32_t *)0x40011060u)
#define USB_PIPECLR_REG                                      *((volatile uint32_t *)0x40011060u)
#define USB_PIPECLR_ADDR                                     (0x40011060u)
#define USB_PIPECLR_RESET                                    (0x00000000u)

/* USB_RXPIPECLR field */
        #define USB_RXPIPECLR                                (0x00000002u)
        #define USB_RXPIPECLR_MASK                           (0x00000002u)
        #define USB_RXPIPECLR_BIT                            (1)
        #define USB_RXPIPECLR_BITS                           (1)

/* USB_TXPIPECLR field */
        #define USB_TXPIPECLR                                (0x00000001u)
        #define USB_TXPIPECLR_MASK                           (0x00000001u)
        #define USB_TXPIPECLR_BIT                            (0)
        #define USB_TXPIPECLR_BITS                           (1)

#define USB_BUFCLR                                           *((volatile uint32_t *)0x40011064u)
#define USB_BUFCLR_REG                                       *((volatile uint32_t *)0x40011064u)
#define USB_BUFCLR_ADDR                                      (0x40011064u)
#define USB_BUFCLR_RESET                                     (0x00000000u)

/* USB_BUFCLRINEP6 field */
        #define USB_BUFCLRINEP6                              (0x00000040u)
        #define USB_BUFCLRINEP6_MASK                         (0x00000040u)
        #define USB_BUFCLRINEP6_BIT                          (6)
        #define USB_BUFCLRINEP6_BITS                         (1)

/* USB_BUFCLRINEP5 field */
        #define USB_BUFCLRINEP5                              (0x00000020u)
        #define USB_BUFCLRINEP5_MASK                         (0x00000020u)
        #define USB_BUFCLRINEP5_BIT                          (5)
        #define USB_BUFCLRINEP5_BITS                         (1)

/* USB_BUFCLRINEP4 field */
        #define USB_BUFCLRINEP4                              (0x00000010u)
        #define USB_BUFCLRINEP4_MASK                         (0x00000010u)
        #define USB_BUFCLRINEP4_BIT                          (4)
        #define USB_BUFCLRINEP4_BITS                         (1)

/* USB_BUFCLRINEP3 field */
        #define USB_BUFCLRINEP3                              (0x00000008u)
        #define USB_BUFCLRINEP3_MASK                         (0x00000008u)
        #define USB_BUFCLRINEP3_BIT                          (3)
        #define USB_BUFCLRINEP3_BITS                         (1)

/* USB_BUFCLRINEP2 field */
        #define USB_BUFCLRINEP2                              (0x00000004u)
        #define USB_BUFCLRINEP2_MASK                         (0x00000004u)
        #define USB_BUFCLRINEP2_BIT                          (2)
        #define USB_BUFCLRINEP2_BITS                         (1)

/* USB_BUFCLRINEP1 field */
        #define USB_BUFCLRINEP1                              (0x00000002u)
        #define USB_BUFCLRINEP1_MASK                         (0x00000002u)
        #define USB_BUFCLRINEP1_BIT                          (1)
        #define USB_BUFCLRINEP1_BITS                         (1)

/* USB_BUFCLRINEP0 field */
        #define USB_BUFCLRINEP0                              (0x00000001u)
        #define USB_BUFCLRINEP0_MASK                         (0x00000001u)
        #define USB_BUFCLRINEP0_BIT                          (0)
        #define USB_BUFCLRINEP0_BITS                         (1)

#define USB_RESUME                                           *((volatile uint32_t *)0x40011068u)
#define USB_RESUME_REG                                       *((volatile uint32_t *)0x40011068u)
#define USB_RESUME_ADDR                                      (0x40011068u)
#define USB_RESUME_RESET                                     (0x00000000u)

/* USB_RESUME_FIELD field */
        #define USB_RESUME_FIELD                             (0x00000001u)
        #define USB_RESUME_FIELD_MASK                        (0x00000001u)
        #define USB_RESUME_FIELD_BIT                         (0)
        #define USB_RESUME_FIELD_BITS                        (1)

#define USB_STATUS                                           *((volatile uint32_t *)0x4001106Cu)
#define USB_STATUS_REG                                       *((volatile uint32_t *)0x4001106Cu)
#define USB_STATUS_ADDR                                      (0x4001106Cu)
#define USB_STATUS_RESET                                     (0x00000000u)

/* USB_RESETSTAT field */
        #define USB_RESETSTAT                                (0x00001000u)
        #define USB_RESETSTAT_MASK                           (0x00001000u)
        #define USB_RESETSTAT_BIT                            (12)
        #define USB_RESETSTAT_BITS                           (1)

/* USB_SUSPENDED field */
        #define USB_SUSPENDED                                (0x00000800u)
        #define USB_SUSPENDED_MASK                           (0x00000800u)
        #define USB_SUSPENDED_BIT                            (11)
        #define USB_SUSPENDED_BITS                           (1)

/* USB_TIMESTAMP field */
        #define USB_TIMESTAMP                                (0x000007FFu)
        #define USB_TIMESTAMP_MASK                           (0x000007FFu)
        #define USB_TIMESTAMP_BIT                            (0)
        #define USB_TIMESTAMP_BITS                           (11)

#define USB_INTF1SEL                                         *((volatile uint32_t *)0x40011074u)
#define USB_INTF1SEL_REG                                     *((volatile uint32_t *)0x40011074u)
#define USB_INTF1SEL_ADDR                                    (0x40011074u)
#define USB_INTF1SEL_RESET                                   (0x00000000u)

/* USB_INTF1SELEN field */
        #define USB_INTF1SELEN                               (0x80000000u)
        #define USB_INTF1SELEN_MASK                          (0x80000000u)
        #define USB_INTF1SELEN_BIT                           (31)
        #define USB_INTF1SELEN_BITS                          (1)

/* USB_INTF1SELEP6 field */
        #define USB_INTF1SELEP6                              (0x00000040u)
        #define USB_INTF1SELEP6_MASK                         (0x00000040u)
        #define USB_INTF1SELEP6_BIT                          (6)
        #define USB_INTF1SELEP6_BITS                         (1)

/* USB_INTF1SELEP5 field */
        #define USB_INTF1SELEP5                              (0x00000020u)
        #define USB_INTF1SELEP5_MASK                         (0x00000020u)
        #define USB_INTF1SELEP5_BIT                          (5)
        #define USB_INTF1SELEP5_BITS                         (1)

/* USB_INTF1SELEP4 field */
        #define USB_INTF1SELEP4                              (0x00000010u)
        #define USB_INTF1SELEP4_MASK                         (0x00000010u)
        #define USB_INTF1SELEP4_BIT                          (4)
        #define USB_INTF1SELEP4_BITS                         (1)

/* USB_INTF1SELEP3 field */
        #define USB_INTF1SELEP3                              (0x00000008u)
        #define USB_INTF1SELEP3_MASK                         (0x00000008u)
        #define USB_INTF1SELEP3_BIT                          (3)
        #define USB_INTF1SELEP3_BITS                         (1)

/* USB_INTF1SELEP2 field */
        #define USB_INTF1SELEP2                              (0x00000004u)
        #define USB_INTF1SELEP2_MASK                         (0x00000004u)
        #define USB_INTF1SELEP2_BIT                          (2)
        #define USB_INTF1SELEP2_BITS                         (1)

/* USB_INTF1SELEP1 field */
        #define USB_INTF1SELEP1                              (0x00000002u)
        #define USB_INTF1SELEP1_MASK                         (0x00000002u)
        #define USB_INTF1SELEP1_BIT                          (1)
        #define USB_INTF1SELEP1_BITS                         (1)

#define USB_RXBUFSIZEEP0A                                    *((volatile uint32_t *)0x40011078u)
#define USB_RXBUFSIZEEP0A_REG                                *((volatile uint32_t *)0x40011078u)
#define USB_RXBUFSIZEEP0A_ADDR                               (0x40011078u)
#define USB_RXBUFSIZEEP0A_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP0A_FIELD field */
        #define USB_RXBUFSIZEEP0A_FIELD                      (0x0000000Fu)
        #define USB_RXBUFSIZEEP0A_FIELD_MASK                 (0x0000000Fu)
        #define USB_RXBUFSIZEEP0A_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP0A_FIELD_BITS                 (4)

#define USB_RXBUFSIZEEP1A                                    *((volatile uint32_t *)0x4001107Cu)
#define USB_RXBUFSIZEEP1A_REG                                *((volatile uint32_t *)0x4001107Cu)
#define USB_RXBUFSIZEEP1A_ADDR                               (0x4001107Cu)
#define USB_RXBUFSIZEEP1A_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP1A_FIELD field */
        #define USB_RXBUFSIZEEP1A_FIELD                      (0x0000000Fu)
        #define USB_RXBUFSIZEEP1A_FIELD_MASK                 (0x0000000Fu)
        #define USB_RXBUFSIZEEP1A_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP1A_FIELD_BITS                 (4)

#define USB_RXBUFSIZEEP2A                                    *((volatile uint32_t *)0x40011080u)
#define USB_RXBUFSIZEEP2A_REG                                *((volatile uint32_t *)0x40011080u)
#define USB_RXBUFSIZEEP2A_ADDR                               (0x40011080u)
#define USB_RXBUFSIZEEP2A_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP2A_FIELD field */
        #define USB_RXBUFSIZEEP2A_FIELD                      (0x0000000Fu)
        #define USB_RXBUFSIZEEP2A_FIELD_MASK                 (0x0000000Fu)
        #define USB_RXBUFSIZEEP2A_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP2A_FIELD_BITS                 (4)

#define USB_RXBUFSIZEEP3A                                    *((volatile uint32_t *)0x40011084u)
#define USB_RXBUFSIZEEP3A_REG                                *((volatile uint32_t *)0x40011084u)
#define USB_RXBUFSIZEEP3A_ADDR                               (0x40011084u)
#define USB_RXBUFSIZEEP3A_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP3A_FIELD field */
        #define USB_RXBUFSIZEEP3A_FIELD                      (0x0000007Fu)
        #define USB_RXBUFSIZEEP3A_FIELD_MASK                 (0x0000007Fu)
        #define USB_RXBUFSIZEEP3A_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP3A_FIELD_BITS                 (7)

#define USB_RXBUFSIZEEP4A                                    *((volatile uint32_t *)0x40011088u)
#define USB_RXBUFSIZEEP4A_REG                                *((volatile uint32_t *)0x40011088u)
#define USB_RXBUFSIZEEP4A_ADDR                               (0x40011088u)
#define USB_RXBUFSIZEEP4A_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP4A_FIELD field */
        #define USB_RXBUFSIZEEP4A_FIELD                      (0x0000003Fu)
        #define USB_RXBUFSIZEEP4A_FIELD_MASK                 (0x0000003Fu)
        #define USB_RXBUFSIZEEP4A_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP4A_FIELD_BITS                 (6)

#define USB_RXBUFSIZEEP5A                                    *((volatile uint32_t *)0x4001108Cu)
#define USB_RXBUFSIZEEP5A_REG                                *((volatile uint32_t *)0x4001108Cu)
#define USB_RXBUFSIZEEP5A_ADDR                               (0x4001108Cu)
#define USB_RXBUFSIZEEP5A_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP5A_FIELD field */
        #define USB_RXBUFSIZEEP5A_FIELD                      (0x0000007Fu)
        #define USB_RXBUFSIZEEP5A_FIELD_MASK                 (0x0000007Fu)
        #define USB_RXBUFSIZEEP5A_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP5A_FIELD_BITS                 (7)

#define USB_RXBUFSIZEEP6A                                    *((volatile uint32_t *)0x40011090u)
#define USB_RXBUFSIZEEP6A_REG                                *((volatile uint32_t *)0x40011090u)
#define USB_RXBUFSIZEEP6A_ADDR                               (0x40011090u)
#define USB_RXBUFSIZEEP6A_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP6A_FIELD field */
        #define USB_RXBUFSIZEEP6A_FIELD                      (0x000003FFu)
        #define USB_RXBUFSIZEEP6A_FIELD_MASK                 (0x000003FFu)
        #define USB_RXBUFSIZEEP6A_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP6A_FIELD_BITS                 (10)

#define USB_RXBUFSIZEEP0B                                    *((volatile uint32_t *)0x40011094u)
#define USB_RXBUFSIZEEP0B_REG                                *((volatile uint32_t *)0x40011094u)
#define USB_RXBUFSIZEEP0B_ADDR                               (0x40011094u)
#define USB_RXBUFSIZEEP0B_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP0B_FIELD field */
        #define USB_RXBUFSIZEEP0B_FIELD                      (0x0000000Fu)
        #define USB_RXBUFSIZEEP0B_FIELD_MASK                 (0x0000000Fu)
        #define USB_RXBUFSIZEEP0B_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP0B_FIELD_BITS                 (4)

#define USB_RXBUFSIZEEP1B                                    *((volatile uint32_t *)0x40011098u)
#define USB_RXBUFSIZEEP1B_REG                                *((volatile uint32_t *)0x40011098u)
#define USB_RXBUFSIZEEP1B_ADDR                               (0x40011098u)
#define USB_RXBUFSIZEEP1B_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP1B_FIELD field */
        #define USB_RXBUFSIZEEP1B_FIELD                      (0x0000000Fu)
        #define USB_RXBUFSIZEEP1B_FIELD_MASK                 (0x0000000Fu)
        #define USB_RXBUFSIZEEP1B_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP1B_FIELD_BITS                 (4)

#define USB_RXBUFSIZEEP2B                                    *((volatile uint32_t *)0x4001109Cu)
#define USB_RXBUFSIZEEP2B_REG                                *((volatile uint32_t *)0x4001109Cu)
#define USB_RXBUFSIZEEP2B_ADDR                               (0x4001109Cu)
#define USB_RXBUFSIZEEP2B_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP2B_FIELD field */
        #define USB_RXBUFSIZEEP2B_FIELD                      (0x0000000Fu)
        #define USB_RXBUFSIZEEP2B_FIELD_MASK                 (0x0000000Fu)
        #define USB_RXBUFSIZEEP2B_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP2B_FIELD_BITS                 (4)

#define USB_RXBUFSIZEEP3B                                    *((volatile uint32_t *)0x400110A0u)
#define USB_RXBUFSIZEEP3B_REG                                *((volatile uint32_t *)0x400110A0u)
#define USB_RXBUFSIZEEP3B_ADDR                               (0x400110A0u)
#define USB_RXBUFSIZEEP3B_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP3B_FIELD field */
        #define USB_RXBUFSIZEEP3B_FIELD                      (0x0000007Fu)
        #define USB_RXBUFSIZEEP3B_FIELD_MASK                 (0x0000007Fu)
        #define USB_RXBUFSIZEEP3B_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP3B_FIELD_BITS                 (7)

#define USB_RXBUFSIZEEP4B                                    *((volatile uint32_t *)0x400110A4u)
#define USB_RXBUFSIZEEP4B_REG                                *((volatile uint32_t *)0x400110A4u)
#define USB_RXBUFSIZEEP4B_ADDR                               (0x400110A4u)
#define USB_RXBUFSIZEEP4B_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP4B_FIELD field */
        #define USB_RXBUFSIZEEP4B_FIELD                      (0x0000003Fu)
        #define USB_RXBUFSIZEEP4B_FIELD_MASK                 (0x0000003Fu)
        #define USB_RXBUFSIZEEP4B_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP4B_FIELD_BITS                 (6)

#define USB_RXBUFSIZEEP5B                                    *((volatile uint32_t *)0x400110A8u)
#define USB_RXBUFSIZEEP5B_REG                                *((volatile uint32_t *)0x400110A8u)
#define USB_RXBUFSIZEEP5B_ADDR                               (0x400110A8u)
#define USB_RXBUFSIZEEP5B_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP5B_FIELD field */
        #define USB_RXBUFSIZEEP5B_FIELD                      (0x0000007Fu)
        #define USB_RXBUFSIZEEP5B_FIELD_MASK                 (0x0000007Fu)
        #define USB_RXBUFSIZEEP5B_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP5B_FIELD_BITS                 (7)

#define USB_RXBUFSIZEEP6B                                    *((volatile uint32_t *)0x400110ACu)
#define USB_RXBUFSIZEEP6B_REG                                *((volatile uint32_t *)0x400110ACu)
#define USB_RXBUFSIZEEP6B_ADDR                               (0x400110ACu)
#define USB_RXBUFSIZEEP6B_RESET                              (0x00000000u)

/* USB_RXBUFSIZEEP6B_FIELD field */
        #define USB_RXBUFSIZEEP6B_FIELD                      (0x000003FFu)
        #define USB_RXBUFSIZEEP6B_FIELD_MASK                 (0x000003FFu)
        #define USB_RXBUFSIZEEP6B_FIELD_BIT                  (0)
        #define USB_RXBUFSIZEEP6B_FIELD_BITS                 (10)

/* NVIC block */
#define BLOCK_NVIC_BASE                                      (0xE000E000u)
#define BLOCK_NVIC_END                                       (0xE000EFFFu)
#define BLOCK_NVIC_SIZE                                      (BLOCK_NVIC_END - BLOCK_NVIC_BASE + 1)

#define INT_CFGSET                                           *((volatile uint32_t *)0xE000E100u)
#define INT_CFGSET_REG                                       *((volatile uint32_t *)0xE000E100u)
#define INT_CFGSET_ADDR                                      (0xE000E100u)
#define INT_CFGSET_RESET                                     (0x00000000u)

/* INT_USB field */
        #define INT_USB                                      (0x00080000u)
        #define INT_USB_MASK                                 (0x00080000u)
        #define INT_USB_BIT                                  (19)
        #define INT_USB_BITS                                 (1)

/* INT_CFGSET_RSVD18 field */
        #define INT_CFGSET_RSVD18                            (0x00040000u)
        #define INT_CFGSET_RSVD18_MASK                       (0x00040000u)
        #define INT_CFGSET_RSVD18_BIT                        (18)
        #define INT_CFGSET_RSVD18_BITS                       (1)

/* INT_CFGSET_RSVD17 field */
        #define INT_CFGSET_RSVD17                            (0x00020000u)
        #define INT_CFGSET_RSVD17_MASK                       (0x00020000u)
        #define INT_CFGSET_RSVD17_BIT                        (17)
        #define INT_CFGSET_RSVD17_BITS                       (1)

/* INT_DEBUG field */
        #define INT_DEBUG                                    (0x00010000u)
        #define INT_DEBUG_MASK                               (0x00010000u)
        #define INT_DEBUG_BIT                                (16)
        #define INT_DEBUG_BITS                               (1)

/* INT_IRQD field */
        #define INT_IRQD                                     (0x00008000u)
        #define INT_IRQD_MASK                                (0x00008000u)
        #define INT_IRQD_BIT                                 (15)
        #define INT_IRQD_BITS                                (1)

/* INT_IRQC field */
        #define INT_IRQC                                     (0x00004000u)
        #define INT_IRQC_MASK                                (0x00004000u)
        #define INT_IRQC_BIT                                 (14)
        #define INT_IRQC_BITS                                (1)

/* INT_IRQB field */
        #define INT_IRQB                                     (0x00002000u)
        #define INT_IRQB_MASK                                (0x00002000u)
        #define INT_IRQB_BIT                                 (13)
        #define INT_IRQB_BITS                                (1)

/* INT_IRQA field */
        #define INT_IRQA                                     (0x00001000u)
        #define INT_IRQA_MASK                                (0x00001000u)
        #define INT_IRQA_BIT                                 (12)
        #define INT_IRQA_BITS                                (1)

/* INT_ADC field */
        #define INT_ADC                                      (0x00000800u)
        #define INT_ADC_MASK                                 (0x00000800u)
        #define INT_ADC_BIT                                  (11)
        #define INT_ADC_BITS                                 (1)

/* INT_MACRX field */
        #define INT_MACRX                                    (0x00000400u)
        #define INT_MACRX_MASK                               (0x00000400u)
        #define INT_MACRX_BIT                                (10)
        #define INT_MACRX_BITS                               (1)

/* INT_MACTX field */
        #define INT_MACTX                                    (0x00000200u)
        #define INT_MACTX_MASK                               (0x00000200u)
        #define INT_MACTX_BIT                                (9)
        #define INT_MACTX_BITS                               (1)

/* INT_MACTMR field */
        #define INT_MACTMR                                   (0x00000100u)
        #define INT_MACTMR_MASK                              (0x00000100u)
        #define INT_MACTMR_BIT                               (8)
        #define INT_MACTMR_BITS                              (1)

/* INT_SEC field */
        #define INT_SEC                                      (0x00000080u)
        #define INT_SEC_MASK                                 (0x00000080u)
        #define INT_SEC_BIT                                  (7)
        #define INT_SEC_BITS                                 (1)

/* INT_SC2 field */
        #define INT_SC2                                      (0x00000040u)
        #define INT_SC2_MASK                                 (0x00000040u)
        #define INT_SC2_BIT                                  (6)
        #define INT_SC2_BITS                                 (1)

/* INT_SC1 field */
        #define INT_SC1                                      (0x00000020u)
        #define INT_SC1_MASK                                 (0x00000020u)
        #define INT_SC1_BIT                                  (5)
        #define INT_SC1_BITS                                 (1)

/* INT_SLEEPTMR field */
        #define INT_SLEEPTMR                                 (0x00000010u)
        #define INT_SLEEPTMR_MASK                            (0x00000010u)
        #define INT_SLEEPTMR_BIT                             (4)
        #define INT_SLEEPTMR_BITS                            (1)

/* INT_BB field */
        #define INT_BB                                       (0x00000008u)
        #define INT_BB_MASK                                  (0x00000008u)
        #define INT_BB_BIT                                   (3)
        #define INT_BB_BITS                                  (1)

/* INT_MGMT field */
        #define INT_MGMT                                     (0x00000004u)
        #define INT_MGMT_MASK                                (0x00000004u)
        #define INT_MGMT_BIT                                 (2)
        #define INT_MGMT_BITS                                (1)

/* INT_TIM2 field */
        #define INT_TIM2                                     (0x00000002u)
        #define INT_TIM2_MASK                                (0x00000002u)
        #define INT_TIM2_BIT                                 (1)
        #define INT_TIM2_BITS                                (1)

/* INT_TIM1 field */
        #define INT_TIM1                                     (0x00000001u)
        #define INT_TIM1_MASK                                (0x00000001u)
        #define INT_TIM1_BIT                                 (0)
        #define INT_TIM1_BITS                                (1)

#define INT_CFGCLR                                           *((volatile uint32_t *)0xE000E180u)
#define INT_CFGCLR_REG                                       *((volatile uint32_t *)0xE000E180u)
#define INT_CFGCLR_ADDR                                      (0xE000E180u)
#define INT_CFGCLR_RESET                                     (0x00000000u)

/* INT_USB field */
        #define INT_USB                                      (0x00080000u)
        #define INT_USB_MASK                                 (0x00080000u)
        #define INT_USB_BIT                                  (19)
        #define INT_USB_BITS                                 (1)

/* INT_CFGCLR_RSVD18 field */
        #define INT_CFGCLR_RSVD18                            (0x00040000u)
        #define INT_CFGCLR_RSVD18_MASK                       (0x00040000u)
        #define INT_CFGCLR_RSVD18_BIT                        (18)
        #define INT_CFGCLR_RSVD18_BITS                       (1)

/* INT_CFGCLR_RSVD17 field */
        #define INT_CFGCLR_RSVD17                            (0x00020000u)
        #define INT_CFGCLR_RSVD17_MASK                       (0x00020000u)
        #define INT_CFGCLR_RSVD17_BIT                        (17)
        #define INT_CFGCLR_RSVD17_BITS                       (1)

/* INT_DEBUG field */
        #define INT_DEBUG                                    (0x00010000u)
        #define INT_DEBUG_MASK                               (0x00010000u)
        #define INT_DEBUG_BIT                                (16)
        #define INT_DEBUG_BITS                               (1)

/* INT_IRQD field */
        #define INT_IRQD                                     (0x00008000u)
        #define INT_IRQD_MASK                                (0x00008000u)
        #define INT_IRQD_BIT                                 (15)
        #define INT_IRQD_BITS                                (1)

/* INT_IRQC field */
        #define INT_IRQC                                     (0x00004000u)
        #define INT_IRQC_MASK                                (0x00004000u)
        #define INT_IRQC_BIT                                 (14)
        #define INT_IRQC_BITS                                (1)

/* INT_IRQB field */
        #define INT_IRQB                                     (0x00002000u)
        #define INT_IRQB_MASK                                (0x00002000u)
        #define INT_IRQB_BIT                                 (13)
        #define INT_IRQB_BITS                                (1)

/* INT_IRQA field */
        #define INT_IRQA                                     (0x00001000u)
        #define INT_IRQA_MASK                                (0x00001000u)
        #define INT_IRQA_BIT                                 (12)
        #define INT_IRQA_BITS                                (1)

/* INT_ADC field */
        #define INT_ADC                                      (0x00000800u)
        #define INT_ADC_MASK                                 (0x00000800u)
        #define INT_ADC_BIT                                  (11)
        #define INT_ADC_BITS                                 (1)

/* INT_MACRX field */
        #define INT_MACRX                                    (0x00000400u)
        #define INT_MACRX_MASK                               (0x00000400u)
        #define INT_MACRX_BIT                                (10)
        #define INT_MACRX_BITS                               (1)

/* INT_MACTX field */
        #define INT_MACTX                                    (0x00000200u)
        #define INT_MACTX_MASK                               (0x00000200u)
        #define INT_MACTX_BIT                                (9)
        #define INT_MACTX_BITS                               (1)

/* INT_MACTMR field */
        #define INT_MACTMR                                   (0x00000100u)
        #define INT_MACTMR_MASK                              (0x00000100u)
        #define INT_MACTMR_BIT                               (8)
        #define INT_MACTMR_BITS                              (1)

/* INT_SEC field */
        #define INT_SEC                                      (0x00000080u)
        #define INT_SEC_MASK                                 (0x00000080u)
        #define INT_SEC_BIT                                  (7)
        #define INT_SEC_BITS                                 (1)

/* INT_SC2 field */
        #define INT_SC2                                      (0x00000040u)
        #define INT_SC2_MASK                                 (0x00000040u)
        #define INT_SC2_BIT                                  (6)
        #define INT_SC2_BITS                                 (1)

/* INT_SC1 field */
        #define INT_SC1                                      (0x00000020u)
        #define INT_SC1_MASK                                 (0x00000020u)
        #define INT_SC1_BIT                                  (5)
        #define INT_SC1_BITS                                 (1)

/* INT_SLEEPTMR field */
        #define INT_SLEEPTMR                                 (0x00000010u)
        #define INT_SLEEPTMR_MASK                            (0x00000010u)
        #define INT_SLEEPTMR_BIT                             (4)
        #define INT_SLEEPTMR_BITS                            (1)

/* INT_BB field */
        #define INT_BB                                       (0x00000008u)
        #define INT_BB_MASK                                  (0x00000008u)
        #define INT_BB_BIT                                   (3)
        #define INT_BB_BITS                                  (1)

/* INT_MGMT field */
        #define INT_MGMT                                     (0x00000004u)
        #define INT_MGMT_MASK                                (0x00000004u)
        #define INT_MGMT_BIT                                 (2)
        #define INT_MGMT_BITS                                (1)

/* INT_TIM2 field */
        #define INT_TIM2                                     (0x00000002u)
        #define INT_TIM2_MASK                                (0x00000002u)
        #define INT_TIM2_BIT                                 (1)
        #define INT_TIM2_BITS                                (1)

/* INT_TIM1 field */
        #define INT_TIM1                                     (0x00000001u)
        #define INT_TIM1_MASK                                (0x00000001u)
        #define INT_TIM1_BIT                                 (0)
        #define INT_TIM1_BITS                                (1)

#define INT_PENDSET                                          *((volatile uint32_t *)0xE000E200u)
#define INT_PENDSET_REG                                      *((volatile uint32_t *)0xE000E200u)
#define INT_PENDSET_ADDR                                     (0xE000E200u)
#define INT_PENDSET_RESET                                    (0x00000000u)

/* INT_USB field */
        #define INT_USB                                      (0x00080000u)
        #define INT_USB_MASK                                 (0x00080000u)
        #define INT_USB_BIT                                  (19)
        #define INT_USB_BITS                                 (1)

/* INT_PENDSET_RSVD18 field */
        #define INT_PENDSET_RSVD18                           (0x00040000u)
        #define INT_PENDSET_RSVD18_MASK                      (0x00040000u)
        #define INT_PENDSET_RSVD18_BIT                       (18)
        #define INT_PENDSET_RSVD18_BITS                      (1)

/* INT_PENDSET_RSVD17 field */
        #define INT_PENDSET_RSVD17                           (0x00020000u)
        #define INT_PENDSET_RSVD17_MASK                      (0x00020000u)
        #define INT_PENDSET_RSVD17_BIT                       (17)
        #define INT_PENDSET_RSVD17_BITS                      (1)

/* INT_DEBUG field */
        #define INT_DEBUG                                    (0x00010000u)
        #define INT_DEBUG_MASK                               (0x00010000u)
        #define INT_DEBUG_BIT                                (16)
        #define INT_DEBUG_BITS                               (1)

/* INT_IRQD field */
        #define INT_IRQD                                     (0x00008000u)
        #define INT_IRQD_MASK                                (0x00008000u)
        #define INT_IRQD_BIT                                 (15)
        #define INT_IRQD_BITS                                (1)

/* INT_IRQC field */
        #define INT_IRQC                                     (0x00004000u)
        #define INT_IRQC_MASK                                (0x00004000u)
        #define INT_IRQC_BIT                                 (14)
        #define INT_IRQC_BITS                                (1)

/* INT_IRQB field */
        #define INT_IRQB                                     (0x00002000u)
        #define INT_IRQB_MASK                                (0x00002000u)
        #define INT_IRQB_BIT                                 (13)
        #define INT_IRQB_BITS                                (1)

/* INT_IRQA field */
        #define INT_IRQA                                     (0x00001000u)
        #define INT_IRQA_MASK                                (0x00001000u)
        #define INT_IRQA_BIT                                 (12)
        #define INT_IRQA_BITS                                (1)

/* INT_ADC field */
        #define INT_ADC                                      (0x00000800u)
        #define INT_ADC_MASK                                 (0x00000800u)
        #define INT_ADC_BIT                                  (11)
        #define INT_ADC_BITS                                 (1)

/* INT_MACRX field */
        #define INT_MACRX                                    (0x00000400u)
        #define INT_MACRX_MASK                               (0x00000400u)
        #define INT_MACRX_BIT                                (10)
        #define INT_MACRX_BITS                               (1)

/* INT_MACTX field */
        #define INT_MACTX                                    (0x00000200u)
        #define INT_MACTX_MASK                               (0x00000200u)
        #define INT_MACTX_BIT                                (9)
        #define INT_MACTX_BITS                               (1)

/* INT_MACTMR field */
        #define INT_MACTMR                                   (0x00000100u)
        #define INT_MACTMR_MASK                              (0x00000100u)
        #define INT_MACTMR_BIT                               (8)
        #define INT_MACTMR_BITS                              (1)

/* INT_SEC field */
        #define INT_SEC                                      (0x00000080u)
        #define INT_SEC_MASK                                 (0x00000080u)
        #define INT_SEC_BIT                                  (7)
        #define INT_SEC_BITS                                 (1)

/* INT_SC2 field */
        #define INT_SC2                                      (0x00000040u)
        #define INT_SC2_MASK                                 (0x00000040u)
        #define INT_SC2_BIT                                  (6)
        #define INT_SC2_BITS                                 (1)

/* INT_SC1 field */
        #define INT_SC1                                      (0x00000020u)
        #define INT_SC1_MASK                                 (0x00000020u)
        #define INT_SC1_BIT                                  (5)
        #define INT_SC1_BITS                                 (1)

/* INT_SLEEPTMR field */
        #define INT_SLEEPTMR                                 (0x00000010u)
        #define INT_SLEEPTMR_MASK                            (0x00000010u)
        #define INT_SLEEPTMR_BIT                             (4)
        #define INT_SLEEPTMR_BITS                            (1)

/* INT_BB field */
        #define INT_BB                                       (0x00000008u)
        #define INT_BB_MASK                                  (0x00000008u)
        #define INT_BB_BIT                                   (3)
        #define INT_BB_BITS                                  (1)

/* INT_MGMT field */
        #define INT_MGMT                                     (0x00000004u)
        #define INT_MGMT_MASK                                (0x00000004u)
        #define INT_MGMT_BIT                                 (2)
        #define INT_MGMT_BITS                                (1)

/* INT_TIM2 field */
        #define INT_TIM2                                     (0x00000002u)
        #define INT_TIM2_MASK                                (0x00000002u)
        #define INT_TIM2_BIT                                 (1)
        #define INT_TIM2_BITS                                (1)

/* INT_TIM1 field */
        #define INT_TIM1                                     (0x00000001u)
        #define INT_TIM1_MASK                                (0x00000001u)
        #define INT_TIM1_BIT                                 (0)
        #define INT_TIM1_BITS                                (1)

#define INT_PENDCLR                                          *((volatile uint32_t *)0xE000E280u)
#define INT_PENDCLR_REG                                      *((volatile uint32_t *)0xE000E280u)
#define INT_PENDCLR_ADDR                                     (0xE000E280u)
#define INT_PENDCLR_RESET                                    (0x00000000u)

/* INT_USB field */
        #define INT_USB                                      (0x00080000u)
        #define INT_USB_MASK                                 (0x00080000u)
        #define INT_USB_BIT                                  (19)
        #define INT_USB_BITS                                 (1)

/* INT_PENDCLR_RSVD18 field */
        #define INT_PENDCLR_RSVD18                           (0x00040000u)
        #define INT_PENDCLR_RSVD18_MASK                      (0x00040000u)
        #define INT_PENDCLR_RSVD18_BIT                       (18)
        #define INT_PENDCLR_RSVD18_BITS                      (1)

/* INT_PENDCLR_RSVD17 field */
        #define INT_PENDCLR_RSVD17                           (0x00020000u)
        #define INT_PENDCLR_RSVD17_MASK                      (0x00020000u)
        #define INT_PENDCLR_RSVD17_BIT                       (17)
        #define INT_PENDCLR_RSVD17_BITS                      (1)

/* INT_DEBUG field */
        #define INT_DEBUG                                    (0x00010000u)
        #define INT_DEBUG_MASK                               (0x00010000u)
        #define INT_DEBUG_BIT                                (16)
        #define INT_DEBUG_BITS                               (1)

/* INT_IRQD field */
        #define INT_IRQD                                     (0x00008000u)
        #define INT_IRQD_MASK                                (0x00008000u)
        #define INT_IRQD_BIT                                 (15)
        #define INT_IRQD_BITS                                (1)

/* INT_IRQC field */
        #define INT_IRQC                                     (0x00004000u)
        #define INT_IRQC_MASK                                (0x00004000u)
        #define INT_IRQC_BIT                                 (14)
        #define INT_IRQC_BITS                                (1)

/* INT_IRQB field */
        #define INT_IRQB                                     (0x00002000u)
        #define INT_IRQB_MASK                                (0x00002000u)
        #define INT_IRQB_BIT                                 (13)
        #define INT_IRQB_BITS                                (1)

/* INT_IRQA field */
        #define INT_IRQA                                     (0x00001000u)
        #define INT_IRQA_MASK                                (0x00001000u)
        #define INT_IRQA_BIT                                 (12)
        #define INT_IRQA_BITS                                (1)

/* INT_ADC field */
        #define INT_ADC                                      (0x00000800u)
        #define INT_ADC_MASK                                 (0x00000800u)
        #define INT_ADC_BIT                                  (11)
        #define INT_ADC_BITS                                 (1)

/* INT_MACRX field */
        #define INT_MACRX                                    (0x00000400u)
        #define INT_MACRX_MASK                               (0x00000400u)
        #define INT_MACRX_BIT                                (10)
        #define INT_MACRX_BITS                               (1)

/* INT_MACTX field */
        #define INT_MACTX                                    (0x00000200u)
        #define INT_MACTX_MASK                               (0x00000200u)
        #define INT_MACTX_BIT                                (9)
        #define INT_MACTX_BITS                               (1)

/* INT_MACTMR field */
        #define INT_MACTMR                                   (0x00000100u)
        #define INT_MACTMR_MASK                              (0x00000100u)
        #define INT_MACTMR_BIT                               (8)
        #define INT_MACTMR_BITS                              (1)

/* INT_SEC field */
        #define INT_SEC                                      (0x00000080u)
        #define INT_SEC_MASK                                 (0x00000080u)
        #define INT_SEC_BIT                                  (7)
        #define INT_SEC_BITS                                 (1)

/* INT_SC2 field */
        #define INT_SC2                                      (0x00000040u)
        #define INT_SC2_MASK                                 (0x00000040u)
        #define INT_SC2_BIT                                  (6)
        #define INT_SC2_BITS                                 (1)

/* INT_SC1 field */
        #define INT_SC1                                      (0x00000020u)
        #define INT_SC1_MASK                                 (0x00000020u)
        #define INT_SC1_BIT                                  (5)
        #define INT_SC1_BITS                                 (1)

/* INT_SLEEPTMR field */
        #define INT_SLEEPTMR                                 (0x00000010u)
        #define INT_SLEEPTMR_MASK                            (0x00000010u)
        #define INT_SLEEPTMR_BIT                             (4)
        #define INT_SLEEPTMR_BITS                            (1)

/* INT_BB field */
        #define INT_BB                                       (0x00000008u)
        #define INT_BB_MASK                                  (0x00000008u)
        #define INT_BB_BIT                                   (3)
        #define INT_BB_BITS                                  (1)

/* INT_MGMT field */
        #define INT_MGMT                                     (0x00000004u)
        #define INT_MGMT_MASK                                (0x00000004u)
        #define INT_MGMT_BIT                                 (2)
        #define INT_MGMT_BITS                                (1)

/* INT_TIM2 field */
        #define INT_TIM2                                     (0x00000002u)
        #define INT_TIM2_MASK                                (0x00000002u)
        #define INT_TIM2_BIT                                 (1)
        #define INT_TIM2_BITS                                (1)

/* INT_TIM1 field */
        #define INT_TIM1                                     (0x00000001u)
        #define INT_TIM1_MASK                                (0x00000001u)
        #define INT_TIM1_BIT                                 (0)
        #define INT_TIM1_BITS                                (1)

#define INT_ACTIVE                                           *((volatile uint32_t *)0xE000E300u)
#define INT_ACTIVE_REG                                       *((volatile uint32_t *)0xE000E300u)
#define INT_ACTIVE_ADDR                                      (0xE000E300u)
#define INT_ACTIVE_RESET                                     (0x00000000u)

/* INT_USB field */
        #define INT_USB                                      (0x00080000u)
        #define INT_USB_MASK                                 (0x00080000u)
        #define INT_USB_BIT                                  (19)
        #define INT_USB_BITS                                 (1)

/* INT_ACTIVE_RSVD18 field */
        #define INT_ACTIVE_RSVD18                            (0x00040000u)
        #define INT_ACTIVE_RSVD18_MASK                       (0x00040000u)
        #define INT_ACTIVE_RSVD18_BIT                        (18)
        #define INT_ACTIVE_RSVD18_BITS                       (1)

/* INT_ACTIVE_RSVD17 field */
        #define INT_ACTIVE_RSVD17                            (0x00020000u)
        #define INT_ACTIVE_RSVD17_MASK                       (0x00020000u)
        #define INT_ACTIVE_RSVD17_BIT                        (17)
        #define INT_ACTIVE_RSVD17_BITS                       (1)

/* INT_DEBUG field */
        #define INT_DEBUG                                    (0x00010000u)
        #define INT_DEBUG_MASK                               (0x00010000u)
        #define INT_DEBUG_BIT                                (16)
        #define INT_DEBUG_BITS                               (1)

/* INT_IRQD field */
        #define INT_IRQD                                     (0x00008000u)
        #define INT_IRQD_MASK                                (0x00008000u)
        #define INT_IRQD_BIT                                 (15)
        #define INT_IRQD_BITS                                (1)

/* INT_IRQC field */
        #define INT_IRQC                                     (0x00004000u)
        #define INT_IRQC_MASK                                (0x00004000u)
        #define INT_IRQC_BIT                                 (14)
        #define INT_IRQC_BITS                                (1)

/* INT_IRQB field */
        #define INT_IRQB                                     (0x00002000u)
        #define INT_IRQB_MASK                                (0x00002000u)
        #define INT_IRQB_BIT                                 (13)
        #define INT_IRQB_BITS                                (1)

/* INT_IRQA field */
        #define INT_IRQA                                     (0x00001000u)
        #define INT_IRQA_MASK                                (0x00001000u)
        #define INT_IRQA_BIT                                 (12)
        #define INT_IRQA_BITS                                (1)

/* INT_ADC field */
        #define INT_ADC                                      (0x00000800u)
        #define INT_ADC_MASK                                 (0x00000800u)
        #define INT_ADC_BIT                                  (11)
        #define INT_ADC_BITS                                 (1)

/* INT_MACRX field */
        #define INT_MACRX                                    (0x00000400u)
        #define INT_MACRX_MASK                               (0x00000400u)
        #define INT_MACRX_BIT                                (10)
        #define INT_MACRX_BITS                               (1)

/* INT_MACTX field */
        #define INT_MACTX                                    (0x00000200u)
        #define INT_MACTX_MASK                               (0x00000200u)
        #define INT_MACTX_BIT                                (9)
        #define INT_MACTX_BITS                               (1)

/* INT_MACTMR field */
        #define INT_MACTMR                                   (0x00000100u)
        #define INT_MACTMR_MASK                              (0x00000100u)
        #define INT_MACTMR_BIT                               (8)
        #define INT_MACTMR_BITS                              (1)

/* INT_SEC field */
        #define INT_SEC                                      (0x00000080u)
        #define INT_SEC_MASK                                 (0x00000080u)
        #define INT_SEC_BIT                                  (7)
        #define INT_SEC_BITS                                 (1)

/* INT_SC2 field */
        #define INT_SC2                                      (0x00000040u)
        #define INT_SC2_MASK                                 (0x00000040u)
        #define INT_SC2_BIT                                  (6)
        #define INT_SC2_BITS                                 (1)

/* INT_SC1 field */
        #define INT_SC1                                      (0x00000020u)
        #define INT_SC1_MASK                                 (0x00000020u)
        #define INT_SC1_BIT                                  (5)
        #define INT_SC1_BITS                                 (1)

/* INT_SLEEPTMR field */
        #define INT_SLEEPTMR                                 (0x00000010u)
        #define INT_SLEEPTMR_MASK                            (0x00000010u)
        #define INT_SLEEPTMR_BIT                             (4)
        #define INT_SLEEPTMR_BITS                            (1)

/* INT_BB field */
        #define INT_BB                                       (0x00000008u)
        #define INT_BB_MASK                                  (0x00000008u)
        #define INT_BB_BIT                                   (3)
        #define INT_BB_BITS                                  (1)

/* INT_MGMT field */
        #define INT_MGMT                                     (0x00000004u)
        #define INT_MGMT_MASK                                (0x00000004u)
        #define INT_MGMT_BIT                                 (2)
        #define INT_MGMT_BITS                                (1)

/* INT_TIM2 field */
        #define INT_TIM2                                     (0x00000002u)
        #define INT_TIM2_MASK                                (0x00000002u)
        #define INT_TIM2_BIT                                 (1)
        #define INT_TIM2_BITS                                (1)

/* INT_TIM1 field */
        #define INT_TIM1                                     (0x00000001u)
        #define INT_TIM1_MASK                                (0x00000001u)
        #define INT_TIM1_BIT                                 (0)
        #define INT_TIM1_BITS                                (1)

#define SCS_AFSR                                             *((volatile uint32_t *)0xE000ED3Cu)
#define SCS_AFSR_REG                                         *((volatile uint32_t *)0xE000ED3Cu)
#define SCS_AFSR_ADDR                                        (0xE000ED3Cu)
#define SCS_AFSR_RESET                                       (0x00000000u)

/* WRONGSIZE field */
        #define SCS_AFSR_WRONGSIZE                           (0x00000008u)
        #define SCS_AFSR_WRONGSIZE_MASK                      (0x00000008u)
        #define SCS_AFSR_WRONGSIZE_BIT                       (3)
        #define SCS_AFSR_WRONGSIZE_BITS                      (1)

/* PROTECTED field */
        #define SCS_AFSR_PROTECTED                           (0x00000004u)
        #define SCS_AFSR_PROTECTED_MASK                      (0x00000004u)
        #define SCS_AFSR_PROTECTED_BIT                       (2)
        #define SCS_AFSR_PROTECTED_BITS                      (1)

/* RESERVED field */
        #define SCS_AFSR_RESERVED                            (0x00000002u)
        #define SCS_AFSR_RESERVED_MASK                       (0x00000002u)
        #define SCS_AFSR_RESERVED_BIT                        (1)
        #define SCS_AFSR_RESERVED_BITS                       (1)

/* MISSED field */
        #define SCS_AFSR_MISSED                              (0x00000001u)
        #define SCS_AFSR_MISSED_MASK                         (0x00000001u)
        #define SCS_AFSR_MISSED_BIT                          (0)
        #define SCS_AFSR_MISSED_BITS                         (1)

#include "regs-internal.h"

#endif /* __REGS_H__ */
