/*
 * drivers/staging/sec-libs/sec_getlog.c
 *
 * Copyright (C) 2014-2015 Samsung Electronics Co, Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <linux/module.h>
#include <linux/errno.h>
#include <asm/setup.h>

static struct {
	u32 special_mark_1;
	u32 special_mark_2;
	u32 special_mark_3;
	u32 special_mark_4;
	void *p_fb;		/* it must be physical address */
	u32 xres;
	u32 yres;
	u32 bpp;		/* color depth : 16 or 24 */
	u32 frames;		/* frame buffer count : 2 */
} frame_buf_mark = {
	.special_mark_1 = (('*' << 24) | ('^' << 16) | ('^' << 8) | ('*' << 0)),
	.special_mark_2 = (('I' << 24) | ('n' << 16) | ('f' << 8) | ('o' << 0)),
	.special_mark_3 = (('H' << 24) | ('e' << 16) | ('r' << 8) | ('e' << 0)),
	.special_mark_4 = (('f' << 24) | ('b' << 16) | ('u' << 8) | ('f' << 0)),
};

void sec_getlog_supply_fbinfo(void *p_fb, u32 xres, u32 yres, u32 bpp,
			      u32 frames)
{
	if (p_fb) {
		pr_info("%s: 0x%p %d %d %d %d\n", __func__, p_fb, xres, yres,
			bpp, frames);
		if(strcmp(CONFIG_DEFAULT_HOSTNAME,"vlx") == 0)
		frame_buf_mark.p_fb = (p_fb - 0x012c2000);
		else
                frame_buf_mark.p_fb = p_fb;

		frame_buf_mark.xres = xres;
		frame_buf_mark.yres = yres;
		frame_buf_mark.bpp = bpp;
		frame_buf_mark.frames = frames;
	}
}
EXPORT_SYMBOL(sec_getlog_supply_fbinfo);

static struct {
	u32 special_mark_1;
	u32 special_mark_2;
	u32 special_mark_3;
	u32 special_mark_4;
	u32 log_mark_version;
	u32 framebuffer_mark_version;
	void *this;		/* this is used for addressing
				   log buffer in 2 dump files */
	struct {
		u32 size;	/* memory block's size */
		u32 addr;	/* memory block'sPhysical address */
	} mem[2];
} marks_ver_mark = {
	.special_mark_1 = (('*' << 24) | ('^' << 16) | ('^' << 8) | ('*' << 0)),
	.special_mark_2 = (('I' << 24) | ('n' << 16) | ('f' << 8) | ('o' << 0)),
	.special_mark_3 = (('H' << 24) | ('e' << 16) | ('r' << 8) | ('e' << 0)),
	.special_mark_4 = (('v' << 24) | ('e' << 16) | ('r' << 8) | ('s' << 0)),
	.log_mark_version = 1,
	.framebuffer_mark_version = 1,
	.this = &marks_ver_mark,
};

/* mark for GetLog extraction */
static struct {
	u32 special_mark_1;
	u32 special_mark_2;
	u32 special_mark_3;
	u32 special_mark_4;
	void *p_main;
	void *p_radio;
	void *p_events;
	void *p_system;
} plat_log_mark = {
	.special_mark_1 = (('*' << 24) | ('^' << 16) | ('^' << 8) | ('*' << 0)),
	.special_mark_2 = (('I' << 24) | ('n' << 16) | ('f' << 8) | ('o' << 0)),
	.special_mark_3 = (('H' << 24) | ('e' << 16) | ('r' << 8) | ('e' << 0)),
	.special_mark_4 = (('p' << 24) | ('l' << 16) | ('o' << 8) | ('g' << 0)),
};

void sec_getlog_supply_loggerinfo(void *p_main,
				  void *p_radio, void *p_events, void *p_system)
{
	pr_info("%s: 0x%p 0x%p 0x%p 0x%p\n", __func__, p_main, p_radio,
		p_events, p_system);
	if(strcmp(CONFIG_DEFAULT_HOSTNAME,"vlx") == 0)
	{
		plat_log_mark.p_main = p_main-0x012c2000;  // 0x2500000 means CONFIG_TEXT_OFFSET
		plat_log_mark.p_radio = p_radio-0x012c2000;
		plat_log_mark.p_events = p_events-0x012c2000;
		plat_log_mark.p_system = p_system-0x012c2000;
	}
	else
	{
		plat_log_mark.p_main = p_main;
		plat_log_mark.p_radio = p_radio;
		plat_log_mark.p_events = p_events;
		plat_log_mark.p_system = p_system;
	}

}
EXPORT_SYMBOL(sec_getlog_supply_loggerinfo);

static struct {
	u32 special_mark_1;
	u32 special_mark_2;
	u32 special_mark_3;
	u32 special_mark_4;
	void *klog_buf;
} kernel_log_mark = {
	.special_mark_1 = (('*' << 24) | ('^' << 16) | ('^' << 8) | ('*' << 0)),
	.special_mark_2 = (('I' << 24) | ('n' << 16) | ('f' << 8) | ('o' << 0)),
	.special_mark_3 = (('H' << 24) | ('e' << 16) | ('r' << 8) | ('e' << 0)),
	.special_mark_4 = (('k' << 24) | ('l' << 16) | ('o' << 8) | ('g' << 0)),
};

void sec_getlog_supply_kloginfo(void *klog_buf)
{
	pr_info("%s: 0x%p\n", __func__, klog_buf);
	if(strcmp(CONFIG_DEFAULT_HOSTNAME,"vlx") == 0)
	kernel_log_mark.klog_buf = klog_buf-0x012c2000; // for SPRD
	else
#ifdef CONFIG_SEC_LOG_BUF_NOCACHE
	//This is ugly code to support getlog tool(getlog tool doesnt support various vitual addr)
	kernel_log_mark.klog_buf = 0xC6B00010;
#else
	kernel_log_mark.klog_buf = klog_buf;
#endif
}
EXPORT_SYMBOL(sec_getlog_supply_kloginfo);

static int __init sec_getlog_init(void)
{
	marks_ver_mark.mem[0].size =
		meminfo.bank[0].size;
	marks_ver_mark.mem[0].addr = meminfo.bank[0].start;
	marks_ver_mark.mem[1].size =
		meminfo.bank[1].size;
	marks_ver_mark.mem[1].addr = meminfo.bank[1].start;

	return 0;
}

core_initcall(sec_getlog_init);

