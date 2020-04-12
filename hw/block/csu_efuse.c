/*
 * QEMU model of the ZynqMP eFuse
 *
 * Copyright (c) 2015 Xilinx Inc.
 *
 * Written by Edgar E. Iglesias <edgari@xilinx.com>
 * Partially autogenerated by xregqemu.py 2015-01-02.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "qemu/osdep.h"
#include "hw/hw.h"
#include "hw/block/flash.h"
#include "qapi/error.h"
#include "qemu/timer.h"
#include "exec/address-spaces.h"
#include "qemu/host-utils.h"
#include "hw/sysbus.h"
#include "hw/register.h"
#include "hw/irq.h"
#include "sysemu/blockdev.h"
#include "qemu/bitops.h"
#include "qemu/log.h"
#include "qemu/main-loop.h"
#include "migration/vmstate.h"
#include "hw/qdev-properties.h"

#include "sysemu/block-backend.h"
#include "hw/zynqmp_aes_key.h"
#include "hw/block/xlnx-efuse.h"

#ifndef ZYNQMP_EFUSE_ERR_DEBUG
#define ZYNQMP_EFUSE_ERR_DEBUG 0
#endif

#define TYPE_ZYNQMP_EFUSE "xlnx.zynqmp-efuse"

#define ZYNQMP_EFUSE(obj) \
     OBJECT_CHECK(ZynqMPEFuse, (obj), TYPE_ZYNQMP_EFUSE)


REG32(WR_LOCK, 0x0)
    FIELD(WR_LOCK, LOCK, 0, 16)
REG32(CFG, 0x4)
    FIELD(CFG, SLVERR_ENABLE, 5, 1)
    FIELD(CFG, MARGIN_RD, 2, 2)
    FIELD(CFG, PGM_EN, 1, 1)
    FIELD(CFG, EFUSE_CLK_SEL, 0, 1)
REG32(STATUS, 0x8)
    FIELD(STATUS, AES_CRC_PASS, 7, 1)
    FIELD(STATUS, AES_CRC_DONE, 6, 1)
    FIELD(STATUS, CACHE_DONE, 5, 1)
    FIELD(STATUS, CACHE_LOAD, 4, 1)
    FIELD(STATUS, EFUSE_3_TBIT, 2, 1)
    FIELD(STATUS, EFUSE_2_TBIT, 1, 1)
    FIELD(STATUS, EFUSE_0_TBIT, 0, 1)
REG32(EFUSE_PGM_ADDR, 0xc)
    FIELD(EFUSE_PGM_ADDR, EFUSE, 11, 2)
    FIELD(EFUSE_PGM_ADDR, ROW, 5, 6)
    FIELD(EFUSE_PGM_ADDR, COLUMN, 0, 5)
REG32(EFUSE_RD_ADDR, 0x10)
    FIELD(EFUSE_RD_ADDR, EFUSE, 11, 2)
    FIELD(EFUSE_RD_ADDR, ROW, 5, 6)
REG32(EFUSE_RD_DATA, 0x14)
REG32(TPGM, 0x18)
    FIELD(TPGM, VALUE, 0, 16)
REG32(TRD, 0x1c)
    FIELD(TRD, VALUE, 0, 8)
REG32(TSU_H_PS, 0x20)
    FIELD(TSU_H_PS, VALUE, 0, 8)
REG32(TSU_H_PS_CS, 0x24)
    FIELD(TSU_H_PS_CS, VALUE, 0, 8)
REG32(TSU_H_CS, 0x2c)
    FIELD(TSU_H_CS, VALUE, 0, 4)
REG32(EFUSE_ISR, 0x30)
    FIELD(EFUSE_ISR, APB_SLVERR, 31, 1)
    FIELD(EFUSE_ISR, CACHE_ERROR, 4, 1)
    FIELD(EFUSE_ISR, RD_ERROR, 3, 1)
    FIELD(EFUSE_ISR, RD_DONE, 2, 1)
    FIELD(EFUSE_ISR, PGM_ERROR, 1, 1)
    FIELD(EFUSE_ISR, PGM_DONE, 0, 1)
REG32(EFUSE_IMR, 0x34)
    FIELD(EFUSE_IMR, APB_SLVERR, 31, 1)
    FIELD(EFUSE_IMR, CACHE_ERROR, 4, 1)
    FIELD(EFUSE_IMR, RD_ERROR, 3, 1)
    FIELD(EFUSE_IMR, RD_DONE, 2, 1)
    FIELD(EFUSE_IMR, PGM_ERROR, 1, 1)
    FIELD(EFUSE_IMR, PGM_DONE, 0, 1)
REG32(EFUSE_IER, 0x38)
    FIELD(EFUSE_IER, APB_SLVERR, 31, 1)
    FIELD(EFUSE_IER, CACHE_ERROR, 4, 1)
    FIELD(EFUSE_IER, RD_ERROR, 3, 1)
    FIELD(EFUSE_IER, RD_DONE, 2, 1)
    FIELD(EFUSE_IER, PGM_ERROR, 1, 1)
    FIELD(EFUSE_IER, PGM_DONE, 0, 1)
REG32(EFUSE_IDR, 0x3c)
    FIELD(EFUSE_IDR, APB_SLVERR, 31, 1)
    FIELD(EFUSE_IDR, CACHE_ERROR, 4, 1)
    FIELD(EFUSE_IDR, RD_ERROR, 3, 1)
    FIELD(EFUSE_IDR, RD_DONE, 2, 1)
    FIELD(EFUSE_IDR, PGM_ERROR, 1, 1)
    FIELD(EFUSE_IDR, PGM_DONE, 0, 1)
REG32(EFUSE_CACHE_LOAD, 0x40)
    FIELD(EFUSE_CACHE_LOAD, LOAD, 0, 1)
REG32(EFUSE_PGM_LOCK, 0x44)
    FIELD(EFUSE_PGM_LOCK, SPK_ID_LOCK, 0, 1)
REG32(EFUSE_AES_CRC, 0x48)
REG32(EFUSE_TBITS_PRGRMG_EN, 0x100)
    FIELD(EFUSE_TBITS_PRGRMG_EN, TBITS_PRGRMG_EN, 3, 1)
REG32(DNA_0, 0x100c)
REG32(DNA_1, 0x1010)
REG32(DNA_2, 0x1014)
REG32(IPDISABLE, 0x1018)
    FIELD(IPDISABLE, VCU_DIS, 8, 1)
    FIELD(IPDISABLE, GPU_DIS, 5, 1)
    FIELD(IPDISABLE, APU3_DIS, 3, 1)
    FIELD(IPDISABLE, APU2_DIS, 2, 1)
    FIELD(IPDISABLE, APU1_DIS, 1, 1)
    FIELD(IPDISABLE, APU0_DIS, 0, 1)
REG32(SYSOSC_CTRL, 0x101c)
    FIELD(SYSOSC_CTRL, SYSOSC_EN, 0, 1)
REG32(USER_0, 0x1020)
REG32(USER_1, 0x1024)
REG32(USER_2, 0x1028)
REG32(USER_3, 0x102c)
REG32(USER_4, 0x1030)
REG32(USER_5, 0x1034)
REG32(USER_6, 0x1038)
REG32(USER_7, 0x103c)
REG32(MISC_USER_CTRL, 0x1040)
    FIELD(MISC_USER_CTRL, FPD_SC_EN_0, 14, 1)
    FIELD(MISC_USER_CTRL, LPD_SC_EN_0, 11, 1)
    FIELD(MISC_USER_CTRL, LBIST_EN, 10, 1)
    FIELD(MISC_USER_CTRL, USR_WRLK_7, 7, 1)
    FIELD(MISC_USER_CTRL, USR_WRLK_6, 6, 1)
    FIELD(MISC_USER_CTRL, USR_WRLK_5, 5, 1)
    FIELD(MISC_USER_CTRL, USR_WRLK_4, 4, 1)
    FIELD(MISC_USER_CTRL, USR_WRLK_3, 3, 1)
    FIELD(MISC_USER_CTRL, USR_WRLK_2, 2, 1)
    FIELD(MISC_USER_CTRL, USR_WRLK_1, 1, 1)
    FIELD(MISC_USER_CTRL, USR_WRLK_0, 0, 1)
REG32(ROM_RSVD, 0x1044)
    FIELD(ROM_RSVD, PBR_BOOT_ERROR, 0, 3)
REG32(PUF_CHASH, 0x1050)
REG32(PUF_MISC, 0x1054)
    FIELD(PUF_MISC, REGISTER_DIS, 31, 1)
    FIELD(PUF_MISC, SYN_WRLK, 30, 1)
    FIELD(PUF_MISC, SYN_INVLD, 29, 1)
    FIELD(PUF_MISC, TEST2_DIS, 28, 1)
    FIELD(PUF_MISC, UNUSED27, 27, 1)
    FIELD(PUF_MISC, UNUSED26, 26, 1)
    FIELD(PUF_MISC, UNUSED25, 25, 1)
    FIELD(PUF_MISC, UNUSED24, 24, 1)
    FIELD(PUF_MISC, AUX, 0, 24)
REG32(SEC_CTRL, 0x1058)
    FIELD(SEC_CTRL, PPK1_INVLD, 30, 2)
    FIELD(SEC_CTRL, PPK1_WRLK, 29, 1)
    FIELD(SEC_CTRL, PPK0_INVLD, 27, 2)
    FIELD(SEC_CTRL, PPK0_WRLK, 26, 1)
    FIELD(SEC_CTRL, RSA_EN, 11, 15)
    FIELD(SEC_CTRL, SEC_LOCK, 10, 1)
    FIELD(SEC_CTRL, PROG_GATE_2, 9, 1)
    FIELD(SEC_CTRL, PROG_GATE_1, 8, 1)
    FIELD(SEC_CTRL, PROG_GATE_0, 7, 1)
    FIELD(SEC_CTRL, DFT_DIS, 6, 1)
    FIELD(SEC_CTRL, JTAG_DIS, 5, 1)
    FIELD(SEC_CTRL, ERROR_DIS, 4, 1)
    FIELD(SEC_CTRL, BBRAM_DIS, 3, 1)
    FIELD(SEC_CTRL, ENC_ONLY, 2, 1)
    FIELD(SEC_CTRL, AES_WRLK, 1, 1)
    FIELD(SEC_CTRL, AES_RDLK, 0, 1)
REG32(SPK_ID, 0x105c)
REG32(PPK0_0, 0x10a0)
REG32(PPK0_1, 0x10a4)
REG32(PPK0_2, 0x10a8)
REG32(PPK0_3, 0x10ac)
REG32(PPK0_4, 0x10b0)
REG32(PPK0_5, 0x10b4)
REG32(PPK0_6, 0x10b8)
REG32(PPK0_7, 0x10bc)
REG32(PPK0_8, 0x10c0)
REG32(PPK0_9, 0x10c4)
REG32(PPK0_10, 0x10c8)
REG32(PPK0_11, 0x10cc)
REG32(PPK1_0, 0x10d0)
REG32(PPK1_1, 0x10d4)
REG32(PPK1_2, 0x10d8)
REG32(PPK1_3, 0x10dc)
REG32(PPK1_4, 0x10e0)
REG32(PPK1_5, 0x10e4)
REG32(PPK1_6, 0x10e8)
REG32(PPK1_7, 0x10ec)
REG32(PPK1_8, 0x10f0)
REG32(PPK1_9, 0x10f4)
REG32(PPK1_10, 0x10f8)
REG32(PPK1_11, 0x10fc)

#define BIT_POS(ROW, COLUMN)    (ROW * 32 + COLUMN)
#define R_MAX (R_PPK1_11 + 1)

/* #define EFUSE_XOSC            26 */

/*
 * eFUSE layout references:
 *   ZynqMP: UG1085 (v2.1) August 21, 2019, p.277, Table 12-13
 */
#define EFUSE_DFT             BIT_POS(22, 6)
#define EFUSE_AES_RDLK        BIT_POS(22, 0)
#define EFUSE_AES_WRLK        BIT_POS(22, 1)
#define EFUSE_ENC_ONLY        BIT_POS(22, 2)
#define EFUSE_BBRAM_DIS       BIT_POS(22, 4)
#define EFUSE_RSA_EN          BIT_POS(22, 24)
#define EFUSE_PPK0_WRLK       BIT_POS(22, 26)
#define EFUSE_PPK0_VALID0     BIT_POS(22, 27)
#define EFUSE_PPK0_VALID1     BIT_POS(22, 28)
#define EFUSE_PPK1_WRLK       BIT_POS(22, 29)
#define EFUSE_PPK1_VALID0     BIT_POS(22, 30)
#define EFUSE_PPK1_VALID1     BIT_POS(22, 31)

/* Areas.  */
#define EFUSE_TRIM_START      BIT_POS(1, 0)
#define EFUSE_TRIM_END        BIT_POS(1, 30)
#define EFUSE_DNA_START       BIT_POS(3, 0)
#define EFUSE_DNA_END         BIT_POS(5, 31)
#define EFUSE_AES_START       BIT_POS(24, 0)
#define EFUSE_AES_END         BIT_POS(31, 31)
#define EFUSE_ROM_START       BIT_POS(17, 0)
#define EFUSE_ROM_END         BIT_POS(17, 31)
#define EFUSE_IPDIS_START     BIT_POS(6, 0)
#define EFUSE_IPDIS_END       BIT_POS(6, 31)
#define EFUSE_USER_START      BIT_POS(8, 0)
#define EFUSE_USER_END        BIT_POS(15, 31)
#define EFUSE_BISR_START      BIT_POS(32, 0)
#define EFUSE_BISR_END        BIT_POS(39, 31)

#define EFUSE_PUF_CHASH_START BIT_POS(20, 0)
#define EFUSE_PUF_CHASH_END   BIT_POS(20, 31)
#define EFUSE_PUF_MISC_START  BIT_POS(21, 0)
#define EFUSE_PUF_MISC_END    BIT_POS(21, 31)
#define EFUSE_PUF_SYN_WRLK    BIT_POS(21, 30)

#define EFUSE_SPK_START       BIT_POS(23, 0)
#define EFUSE_SPK_END         BIT_POS(23, 31)

#define EFUSE_PPK0_START      BIT_POS(40, 0)
#define EFUSE_PPK0_END        BIT_POS(51, 31)
#define EFUSE_PPK1_START      BIT_POS(52, 0)
#define EFUSE_PPK1_END        BIT_POS(63, 31)

typedef struct ZynqMPEFuse {
    SysBusDevice parent_obj;
    MemoryRegion iomem;
    qemu_irq irq;

    ZynqMPAESKeySink *key_sink;

    XLNXEFuse *efuse;
    void (*refresh_cache)(struct ZynqMPEFuse *, unsigned int);
    uint32_t regs[R_MAX];
    RegisterInfo regs_info[R_MAX];
} ZynqMPEFuse;

#define EFUSE_CACHE_BIT(s, reg, field) \
    ARRAY_FIELD_DP32((s)->regs, reg, field, efuse_get_bit((s->efuse), \
                EFUSE_ ## field))

static void update_tbit_status(ZynqMPEFuse *s)
{
    unsigned int check = efuse_tbits_check(s->efuse);
    uint32_t val = s->regs[R_STATUS];

    val = FIELD_DP32(val, STATUS, EFUSE_0_TBIT, !!(check & (1 << 0)));
    val = FIELD_DP32(val, STATUS, EFUSE_2_TBIT, !!(check & (1 << 1)));
    val = FIELD_DP32(val, STATUS, EFUSE_3_TBIT, !!(check & (1 << 2)));

    s->regs[R_STATUS] = val;
}

/*
 * Keep the syncs in bit order so we can bail out for the
 * slower ones.
 */
static void zynqmp_efuse_sync_cache(ZynqMPEFuse *s, unsigned int bit)
{
    unsigned int i;

    EFUSE_CACHE_BIT(s, SEC_CTRL, AES_RDLK);
    EFUSE_CACHE_BIT(s, SEC_CTRL, AES_WRLK);
    EFUSE_CACHE_BIT(s, SEC_CTRL, ENC_ONLY);
    EFUSE_CACHE_BIT(s, SEC_CTRL, BBRAM_DIS);

    EFUSE_CACHE_BIT(s, SEC_CTRL, RSA_EN);
    EFUSE_CACHE_BIT(s, SEC_CTRL, PPK0_WRLK);
    EFUSE_CACHE_BIT(s, SEC_CTRL, PPK1_WRLK);

    i = efuse_get_bit(s->efuse, EFUSE_PPK0_VALID0);
    i += efuse_get_bit(s->efuse, EFUSE_PPK0_VALID1) * 2;
    ARRAY_FIELD_DP32(s->regs, SEC_CTRL, PPK0_INVLD, i);
    i = efuse_get_bit(s->efuse, EFUSE_PPK1_VALID0);
    i += efuse_get_bit(s->efuse, EFUSE_PPK1_VALID1) * 2;
    ARRAY_FIELD_DP32(s->regs, SEC_CTRL, PPK1_INVLD, i);

    /* Update the tbits.  */
    update_tbit_status(s);

    /* Sync the various areas.  */
    s->regs[R_PUF_CHASH] = efuse_get_row(s->efuse, EFUSE_PUF_CHASH_START);
    s->regs[R_PUF_MISC]  = efuse_get_row(s->efuse, EFUSE_PUF_MISC_START);

    efuse_sync_u32(s->efuse, &s->regs[R_DNA_0], EFUSE_DNA_START,
                   EFUSE_DNA_END, bit);

    if (bit < EFUSE_AES_START) {
        return;
    }

    /* Sync the AES Key.  */
    efuse_k256_sync(s->efuse, s->key_sink, EFUSE_AES_START);

    efuse_sync_u32(s->efuse, &s->regs[R_ROM_RSVD],
                   EFUSE_ROM_START, EFUSE_ROM_END, bit);
    efuse_sync_u32(s->efuse, &s->regs[R_IPDISABLE],
                   EFUSE_IPDIS_START, EFUSE_IPDIS_END, bit);
    efuse_sync_u32(s->efuse, &s->regs[R_USER_0],
                   EFUSE_USER_START, EFUSE_USER_END, bit);
    efuse_sync_u32(s->efuse, &s->regs[R_SPK_ID],
                   EFUSE_SPK_START, EFUSE_SPK_END, bit);
    efuse_sync_u32(s->efuse, &s->regs[R_PPK0_0],
                   EFUSE_PPK0_START, EFUSE_PPK0_END, bit);
    efuse_sync_u32(s->efuse, &s->regs[R_PPK1_0],
                   EFUSE_PPK1_START, EFUSE_PPK1_END, bit);
}

static void zynqmp_efuse_update_irq(ZynqMPEFuse *s)
{
    bool pending = s->regs[R_EFUSE_ISR] & s->regs[R_EFUSE_IMR];
    qemu_set_irq(s->irq, pending);
}

static void zynqmp_efuse_isr_postw(RegisterInfo *reg, uint64_t val64)
{
    ZynqMPEFuse *s = ZYNQMP_EFUSE(reg->opaque);
    zynqmp_efuse_update_irq(s);
}

static uint64_t zynqmp_efuse_ier_prew(RegisterInfo *reg, uint64_t val64)
{
    ZynqMPEFuse *s = ZYNQMP_EFUSE(reg->opaque);
    uint32_t val = val64;

    s->regs[R_EFUSE_IMR] |= val;
    zynqmp_efuse_update_irq(s);
    return 0;
}

static uint64_t zynqmp_efuse_idr_prew(RegisterInfo *reg, uint64_t val64)
{
    ZynqMPEFuse *s = ZYNQMP_EFUSE(reg->opaque);
    uint32_t val = val64;

    s->regs[R_EFUSE_IMR] &= ~val;
    zynqmp_efuse_update_irq(s);
    return 0;
}

static bool zynqmp_efuse_idx_is_aes_key(unsigned int fuse)
{
    if (fuse >= EFUSE_AES_START && fuse <= EFUSE_AES_END) {
        return true;
    }
    return false;
}

static void zynqmp_efuse_pgm_addr_postw(RegisterInfo *reg, uint64_t val64)
{
    ZynqMPEFuse *s = ZYNQMP_EFUSE(reg->opaque);
    bool puf_prot = false;
    bool wr_lock, ps, prot;
    unsigned int efuse_idx;

    efuse_idx = s->regs[R_EFUSE_PGM_ADDR];

    /* Allow only valid array, and adjust for skipped array 1 */
    switch (ARRAY_FIELD_EX32(s->regs, EFUSE_PGM_ADDR, EFUSE)) {
    case 0:
        break;
    case 2:
        puf_prot = efuse_get_bit(s->efuse, EFUSE_PUF_SYN_WRLK);
        val64 = FIELD_DP32(efuse_idx, EFUSE_PGM_ADDR, EFUSE, 1);
        break;
    case 3:
        puf_prot = efuse_get_bit(s->efuse, EFUSE_PUF_SYN_WRLK);
        val64 = FIELD_DP32(efuse_idx, EFUSE_PGM_ADDR, EFUSE, 2);
        break;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Denied write to invalid eFuse array: addr=%#x\n",
                      object_get_canonical_path(OBJECT(s)),
                      efuse_idx);
        return;
    }

    /* TODO: Sanity check the timing setup.  */

    wr_lock = ARRAY_FIELD_EX32(s->regs, WR_LOCK, LOCK);
    if (wr_lock) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Write to eFuse %u while write protected\n",
                      object_get_canonical_path(OBJECT(s)),
                      efuse_idx);
        return;
    }

    ps = ARRAY_FIELD_EX32(s->regs, CFG, PGM_EN);
    if (!ps) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Write to eFuse %u while not powered\n",
                      object_get_canonical_path(OBJECT(s)),
                      efuse_idx);
        return;
    }

    if (puf_prot) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Write to eFuse PUF_HD page while write-locked\n",
                      object_get_canonical_path(OBJECT(s)));
        return;
    }

    /* I'm not sure if this is HW enforced or not.  */
    prot = ARRAY_FIELD_EX32(s->regs, SEC_CTRL, AES_WRLK);
    if (prot && zynqmp_efuse_idx_is_aes_key(efuse_idx)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Write to AES key eFuse %u while not wr protected\n",
                      object_get_canonical_path(OBJECT(s)),
                      efuse_idx);
        return;
    }

    /* Set up to detect done-transition */
    ARRAY_FIELD_DP32(s->regs, EFUSE_ISR, PGM_ERROR, 0);
    ARRAY_FIELD_DP32(s->regs, EFUSE_ISR, PGM_DONE, 0);

    efuse_pgm_start(s->efuse, 13, val64);
    /* To Vefify after programming, Cache reload should should happen or
     * Margin Read is done
     * FIXME: Implement Margin Read
     */
}

static void zynqmp_efuse_pgm_done(DeviceState *dev, bool failed)
{
    ZynqMPEFuse *s = ZYNQMP_EFUSE(dev);

    ARRAY_FIELD_DP32(s->regs, EFUSE_ISR, PGM_ERROR, (failed ? 1 : 0));
    ARRAY_FIELD_DP32(s->regs, EFUSE_ISR, PGM_DONE, 1);
    zynqmp_efuse_update_irq(s);
}

static void zynqmp_efuse_rd_addr_postw(RegisterInfo *reg, uint64_t val64)
{
    ZynqMPEFuse *s = ZYNQMP_EFUSE(reg->opaque);

    /*
     * zynqmp_efuse_mem_read() has already applied in-programming guard.
     *
     * Still need to grant reads only to allowed bits; reference sources:
     * 1/ XilSKey - XilSKey_ZynqMp_EfusePs_ReadRow()
     * 2/ UG1085, v2.0, table 12-13
     * (note: enumerates the masks as <first, last> per described in
     *  references to avoid mental translation).
     */
#define COL_MASK(L_, H_) \
    ((uint32_t)MAKE_64BIT_MASK((L_), (1 + (H_) - (L_))))

    static const uint32_t ary0_col_mask[] = {
        /* XilSKey - XSK_ZYNQMP_EFUSEPS_TBITS_ROW */
        [0]  = COL_MASK(28, 31),

        /* XilSKey - XSK_ZYNQMP_EFUSEPS_USR{0:7}_FUSE_ROW */
        [8]  = COL_MASK(0, 31), [9]  = COL_MASK(0, 31),
        [10] = COL_MASK(0, 31), [11] = COL_MASK(0, 31),
        [12] = COL_MASK(0, 31), [13] = COL_MASK(0, 31),
        [14] = COL_MASK(0, 31), [15] = COL_MASK(0, 31),

        /* XilSKey - XSK_ZYNQMP_EFUSEPS_MISC_USR_CTRL_ROW */
        [16] = COL_MASK(0, 0) | COL_MASK(10, 16),

        /* XilSKey - XSK_ZYNQMP_EFUSEPS_PBR_BOOT_ERR_ROW */
        [17] = COL_MASK(0, 2),

        /* XilSKey - XSK_ZYNQMP_EFUSEPS_PUF_CHASH_ROW */
        [20] = COL_MASK(0, 31),

        /* XilSKey - XSK_ZYNQMP_EFUSEPS_PUF_AUX_ROW */
        [21] = COL_MASK(0, 23) | COL_MASK(29, 31),

        /* XilSKey - XSK_ZYNQMP_EFUSEPS_SEC_CTRL_ROW */
        [22] = COL_MASK(0, 31),

        /* XilSKey - XSK_ZYNQMP_EFUSEPS_SPK_ID_ROW */
        [23] = COL_MASK(0, 31),

        /* XilSKey - XSK_ZYNQMP_EFUSEPS_PPK0_START_ROW */
        [40] = COL_MASK(0, 31), [41] = COL_MASK(0, 31),
        [42] = COL_MASK(0, 31), [43] = COL_MASK(0, 31),
        [44] = COL_MASK(0, 31), [45] = COL_MASK(0, 31),
        [46] = COL_MASK(0, 31), [47] = COL_MASK(0, 31),
        [48] = COL_MASK(0, 31), [49] = COL_MASK(0, 31),
        [50] = COL_MASK(0, 31), [51] = COL_MASK(0, 31),

        /* XilSKey - XSK_ZYNQMP_EFUSEPS_PPK1_START_ROW */
        [52] = COL_MASK(0, 31), [53] = COL_MASK(0, 31),
        [54] = COL_MASK(0, 31), [55] = COL_MASK(0, 31),
        [56] = COL_MASK(0, 31), [57] = COL_MASK(0, 31),
        [58] = COL_MASK(0, 31), [59] = COL_MASK(0, 31),
        [60] = COL_MASK(0, 31), [61] = COL_MASK(0, 31),
        [62] = COL_MASK(0, 31), [63] = COL_MASK(0, 31),
    };

    uint32_t col_mask = COL_MASK(0, 31);
#undef COL_MASK

    uint32_t efuse_idx = s->regs[R_EFUSE_RD_ADDR];
    uint32_t efuse_ary = FIELD_EX32(efuse_idx, EFUSE_RD_ADDR, EFUSE);
    uint32_t efuse_row = FIELD_EX32(efuse_idx, EFUSE_RD_ADDR, ROW);

    switch (efuse_ary) {
    case 0:     /* Various */
        if (efuse_row >= ARRAY_SIZE(ary0_col_mask)) {
            goto denied;
        }

        col_mask = ary0_col_mask[efuse_row];
        if (!col_mask) {
            goto denied;
        }
        break;
    case 2:     /* PUF helper data, adjust for skipped array 1 */
    case 3:
        val64 = FIELD_DP32(efuse_idx, EFUSE_RD_ADDR, EFUSE, efuse_ary - 1);
        break;
    default:
        goto denied;
    }

    s->regs[R_EFUSE_RD_DATA] = efuse_get_row(s->efuse, val64) & col_mask;

    ARRAY_FIELD_DP32(s->regs, EFUSE_ISR, RD_ERROR, 0);
    ARRAY_FIELD_DP32(s->regs, EFUSE_ISR, RD_DONE, 1);
    zynqmp_efuse_update_irq(s);
    return;

 denied:
    qemu_log_mask(LOG_GUEST_ERROR,
                  "%s: Denied efuse read from array %u, row %u\n",
                  object_get_canonical_path(OBJECT(s)),
                  efuse_ary, efuse_row);

    s->regs[R_EFUSE_RD_DATA] = 0;

    ARRAY_FIELD_DP32(s->regs, EFUSE_ISR, RD_ERROR, 1);
    ARRAY_FIELD_DP32(s->regs, EFUSE_ISR, RD_DONE, 0);
    zynqmp_efuse_update_irq(s);
}

static void zynqmp_efuse_aes_crc_postw(RegisterInfo *reg, uint64_t val64)
{
    ZynqMPEFuse *s = ZYNQMP_EFUSE(reg->opaque);
    bool ok;

    ok = efuse_k256_check(s->efuse, (uint32_t)val64, EFUSE_AES_START);

    ARRAY_FIELD_DP32(s->regs, STATUS, AES_CRC_PASS, (ok ? 1 : 0));
    ARRAY_FIELD_DP32(s->regs, STATUS, AES_CRC_DONE, 1);
}

static void zynqmp_efuse_cfg_postw(RegisterInfo *reg, uint64_t val64)
{
    ZynqMPEFuse *s = ZYNQMP_EFUSE(reg->opaque);
    unsigned int ps;
    uint32_t val32 = val64;

    ps = FIELD_EX32(val32, CFG, PGM_EN);
    efuse_stop_timer_ps(s->efuse);
    if (ps) {
        efuse_set_timer_ps(s->efuse, 500);
    }
}

static uint64_t zynqmp_efuse_cache_load_prew(RegisterInfo *reg,
                                             uint64_t valu64)
{
    ZynqMPEFuse *s = ZYNQMP_EFUSE(reg->opaque);

    if (valu64 & R_EFUSE_CACHE_LOAD_LOAD_MASK) {
        s->refresh_cache(s, FBIT_UNKNOWN);
        ARRAY_FIELD_DP32(s->regs, STATUS, CACHE_DONE, 1);
        zynqmp_efuse_update_irq(s);
    }

    return 0;
}

static uint64_t zynqmp_wr_lock_prew(RegisterInfo *reg, uint64_t val)
{
    return val == 0xDF0D ? 0 : 1;
}

static RegisterAccessInfo zynqmp_efuse_regs_info[] = {
    {   .name = "WR_LOCK",  .addr = A_WR_LOCK,
        .reset = 0x1,
        .pre_write = zynqmp_wr_lock_prew,
    },{ .name = "CFG",  .addr = A_CFG,
        .post_write = zynqmp_efuse_cfg_postw
    },{ .name = "STATUS",  .addr = A_STATUS,
        .rsvd = 0x8,
        .ro = 0xff,
    },{ .name = "EFUSE_PGM_ADDR",  .addr = A_EFUSE_PGM_ADDR,
         .post_write = zynqmp_efuse_pgm_addr_postw
    },{ .name = "EFUSE_RD_ADDR",  .addr = A_EFUSE_RD_ADDR,
        .rsvd = 0x1f,
        .post_write = zynqmp_efuse_rd_addr_postw,
    },{ .name = "EFUSE_RD_DATA",  .addr = A_EFUSE_RD_DATA,
        .ro = 0xffffffff,
    },{ .name = "TPGM",  .addr = A_TPGM,
    },{ .name = "TRD",  .addr = A_TRD,
        .reset = 0x1b,
    },{ .name = "TSU_H_PS",  .addr = A_TSU_H_PS,
        .reset = 0xff,
    },{ .name = "TSU_H_PS_CS",  .addr = A_TSU_H_PS_CS,
        .reset = 0xb,
    },{ .name = "TSU_H_CS",  .addr = A_TSU_H_CS,
        .reset = 0x7,
    },{ .name = "EFUSE_ISR",  .addr = A_EFUSE_ISR,
        .rsvd = 0x7fffffe0,
        .w1c = 0x8000001f,
        .post_write = zynqmp_efuse_isr_postw,
    },{ .name = "EFUSE_IMR",  .addr = A_EFUSE_IMR,
        .reset = 0x8000001f,
        .rsvd = 0x7fffffe0,
        .ro = 0xffffffff,
    },{ .name = "EFUSE_IER",  .addr = A_EFUSE_IER,
        .rsvd = 0x7fffffe0,
        .pre_write = zynqmp_efuse_ier_prew,
    },{ .name = "EFUSE_IDR",  .addr = A_EFUSE_IDR,
        .rsvd = 0x7fffffe0,
        .pre_write = zynqmp_efuse_idr_prew,
    },{ .name = "EFUSE_CACHE_LOAD",  .addr = A_EFUSE_CACHE_LOAD,
        .pre_write = zynqmp_efuse_cache_load_prew,
    },{ .name = "EFUSE_PGM_LOCK",  .addr = A_EFUSE_PGM_LOCK,
    },{ .name = "EFUSE_AES_CRC",  .addr = A_EFUSE_AES_CRC,
        .post_write = zynqmp_efuse_aes_crc_postw,
    },{ .name = "EFUSE_TBITS_PRGRMG_EN",  .addr = A_EFUSE_TBITS_PRGRMG_EN,
        .reset = R_EFUSE_TBITS_PRGRMG_EN_TBITS_PRGRMG_EN_MASK,
    },{ .name = "DNA_0",  .addr = A_DNA_0,
        .ro = 0xffffffff,
    },{ .name = "DNA_1",  .addr = A_DNA_1,
        .ro = 0xffffffff,
    },{ .name = "DNA_2",  .addr = A_DNA_2,
        .ro = 0xffffffff,
    },{ .name = "IPDISABLE",  .addr = A_IPDISABLE,
        .ro = 0xffffffff,
    },{ .name = "SYSOSC_CTRL",  .addr = A_SYSOSC_CTRL,
        .ro = 0xffffffff,
    },{ .name = "USER_0",  .addr = A_USER_0,
        .ro = 0xffffffff,
    },{ .name = "USER_1",  .addr = A_USER_1,
        .ro = 0xffffffff,
    },{ .name = "USER_2",  .addr = A_USER_2,
        .ro = 0xffffffff,
    },{ .name = "USER_3",  .addr = A_USER_3,
        .ro = 0xffffffff,
    },{ .name = "USER_4",  .addr = A_USER_4,
        .ro = 0xffffffff,
    },{ .name = "USER_5",  .addr = A_USER_5,
        .ro = 0xffffffff,
    },{ .name = "USER_6",  .addr = A_USER_6,
        .ro = 0xffffffff,
    },{ .name = "USER_7",  .addr = A_USER_7,
        .ro = 0xffffffff,
    },{ .name = "MISC_USER_CTRL",  .addr = A_MISC_USER_CTRL,
        .ro = 0xffffffff,
    },{ .name = "ROM_RSVD",  .addr = A_ROM_RSVD,
        .ro = 0xffffffff,
    },{ .name = "PUF_CHASH", .addr = A_PUF_CHASH,
        .ro = 0xffffffff,
    },{ .name = "PUF_MISC",  .addr = A_PUF_MISC,
        .ro = 0xffffffff,
    },{ .name = "SEC_CTRL",  .addr = A_SEC_CTRL,
        .ro = 0xffffffff,
    },{ .name = "SPK_ID",  .addr = A_SPK_ID,
        .ro = 0xffffffff,
    },{ .name = "PPK0_0",  .addr = A_PPK0_0,
        .ro = 0xffffffff,
    },{ .name = "PPK0_1",  .addr = A_PPK0_1,
        .ro = 0xffffffff,
    },{ .name = "PPK0_2",  .addr = A_PPK0_2,
        .ro = 0xffffffff,
    },{ .name = "PPK0_3",  .addr = A_PPK0_3,
        .ro = 0xffffffff,
    },{ .name = "PPK0_4",  .addr = A_PPK0_4,
        .ro = 0xffffffff,
    },{ .name = "PPK0_5",  .addr = A_PPK0_5,
        .ro = 0xffffffff,
    },{ .name = "PPK0_6",  .addr = A_PPK0_6,
        .ro = 0xffffffff,
    },{ .name = "PPK0_7",  .addr = A_PPK0_7,
        .ro = 0xffffffff,
    },{ .name = "PPK0_8",  .addr = A_PPK0_8,
        .ro = 0xffffffff,
    },{ .name = "PPK0_9",  .addr = A_PPK0_9,
        .ro = 0xffffffff,
    },{ .name = "PPK0_10",  .addr = A_PPK0_10,
        .ro = 0xffffffff,
    },{ .name = "PPK0_11",  .addr = A_PPK0_11,
        .ro = 0xffffffff,
    },{ .name = "PPK1_0",  .addr = A_PPK1_0,
        .ro = 0xffffffff,
    },{ .name = "PPK1_1",  .addr = A_PPK1_1,
        .ro = 0xffffffff,
    },{ .name = "PPK1_2",  .addr = A_PPK1_2,
        .ro = 0xffffffff,
    },{ .name = "PPK1_3",  .addr = A_PPK1_3,
        .ro = 0xffffffff,
    },{ .name = "PPK1_4",  .addr = A_PPK1_4,
        .ro = 0xffffffff,
    },{ .name = "PPK1_5",  .addr = A_PPK1_5,
        .ro = 0xffffffff,
    },{ .name = "PPK1_6",  .addr = A_PPK1_6,
        .ro = 0xffffffff,
    },{ .name = "PPK1_7",  .addr = A_PPK1_7,
        .ro = 0xffffffff,
    },{ .name = "PPK1_8",  .addr = A_PPK1_8,
        .ro = 0xffffffff,
    },{ .name = "PPK1_9",  .addr = A_PPK1_9,
        .ro = 0xffffffff,
    },{ .name = "PPK1_10",  .addr = A_PPK1_10,
        .ro = 0xffffffff,
    },{ .name = "PPK1_11",  .addr = A_PPK1_11,
        .ro = 0xffffffff,
    }
};

/* Return dev-obj from reg-region created by register_init_block32 */
static ZynqMPEFuse *zynqmp_efuse_of_mr(void *mr_accessor)
{
    RegisterInfoArray *reg_array = mr_accessor;
    Object *dev;

    assert(reg_array != NULL);

    dev = reg_array->mem.owner;
    assert(dev);

    return ZYNQMP_EFUSE(dev);
}

static uint64_t zynqmp_efuse_mem_read(void *opaque, hwaddr addr, unsigned size)
{
    ZynqMPEFuse *s = zynqmp_efuse_of_mr(opaque);

    /* Process programming completion if late.  */
    efuse_pgm_complete(s->efuse);

    if (efuse_is_pgm(s->efuse)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Accessing device while programming\n",
                      object_get_canonical_path(OBJECT(s)));
    }

    return register_read_memory(opaque, addr, size);
}

static MemTxResult zynqmp_efuse_mem_write_with_attrs(void *opaque, hwaddr addr,
                            uint64_t v64, unsigned size, MemTxAttrs attrs)
{
    ZynqMPEFuse *s = zynqmp_efuse_of_mr(opaque);

    /* Process programming completion if late.  */
    efuse_pgm_complete(s->efuse);

    if (efuse_is_pgm(s->efuse)) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Accessing device while programming\n",
                      object_get_canonical_path(OBJECT(s)));
    }

    return register_write_memory_with_attrs(opaque, addr, v64, size, attrs);
}

static const MemoryRegionOps zynqmp_efuse_ops = {
    .read = zynqmp_efuse_mem_read,
    .write_with_attrs = zynqmp_efuse_mem_write_with_attrs,
    .endianness = DEVICE_LITTLE_ENDIAN,
    .valid = {
        .min_access_size = 4,
        .max_access_size = 4,
    },
};

static void zynqmp_efuse_reset(DeviceState *dev)
{
    ZynqMPEFuse *s = ZYNQMP_EFUSE(dev);
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(s->regs_info); ++i) {
        register_reset(&s->regs_info[i]);
    }

    s->refresh_cache(s, FBIT_UNKNOWN);
    ARRAY_FIELD_DP32(s->regs, STATUS, CACHE_DONE, 1);
    zynqmp_efuse_update_irq(s);
}

static void zynqmp_efuse_realize(DeviceState *dev, Error **errp)
{
    ZynqMPEFuse *s = ZYNQMP_EFUSE(dev);
    const char *prefix = object_get_canonical_path(OBJECT(dev));

    if (!s->key_sink) {
        error_setg(&error_abort,
                   "%s: AES EFUSE key sink not connected\n", prefix);
    }

    if (!s->efuse) {
        error_setg(&error_abort, "%s: XLN-EFUSE not connected", prefix);
    }
    s->efuse->pgm_done = zynqmp_efuse_pgm_done;
    s->efuse->dev = dev;
}

static void zynqmp_efuse_init(Object *obj)
{
    ZynqMPEFuse *s = ZYNQMP_EFUSE(obj);
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);
    RegisterInfoArray *reg_array;

    memory_region_init(&s->iomem, obj, TYPE_ZYNQMP_EFUSE, R_MAX * 4);
    reg_array =
        register_init_block32(DEVICE(obj), zynqmp_efuse_regs_info,
                              ARRAY_SIZE(zynqmp_efuse_regs_info),
                              s->regs_info, s->regs,
                              &zynqmp_efuse_ops,
                              ZYNQMP_EFUSE_ERR_DEBUG,
                              R_MAX * 4);
    memory_region_add_subregion(&s->iomem, 0, &reg_array->mem);

    sysbus_init_mmio(sbd, &s->iomem);
    sysbus_init_irq(sbd, &s->irq);

    s->refresh_cache = zynqmp_efuse_sync_cache;
}

static const VMStateDescription vmstate_efuse = {
    .name = TYPE_ZYNQMP_EFUSE,
    .version_id = 1,
    .minimum_version_id = 1,
    .minimum_version_id_old = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32_ARRAY(regs, ZynqMPEFuse, R_MAX),
        VMSTATE_END_OF_LIST(),
    }
};

static Property zynqmp_efuse_props[] = {
    DEFINE_PROP_LINK("efuse",
                     ZynqMPEFuse, efuse,
                     TYPE_XLNX_EFUSE, XLNXEFuse *),

    DEFINE_PROP_LINK("zynqmp-aes-key-sink-efuses",
                     ZynqMPEFuse, key_sink,
                     TYPE_ZYNQMP_AES_KEY_SINK, ZynqMPAESKeySink *),

    DEFINE_PROP_END_OF_LIST(),
};

static void zynqmp_efuse_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = zynqmp_efuse_reset;
    dc->realize = zynqmp_efuse_realize;
    dc->vmsd = &vmstate_efuse;
    device_class_set_props(dc, zynqmp_efuse_props);
}


static const TypeInfo efuse_info = {
    .name          = TYPE_ZYNQMP_EFUSE,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(ZynqMPEFuse),
    .class_init    = zynqmp_efuse_class_init,
    .instance_init = zynqmp_efuse_init,
};

static void efuse_register_types(void)
{
    type_register_static(&efuse_info);
}

type_init(efuse_register_types)
