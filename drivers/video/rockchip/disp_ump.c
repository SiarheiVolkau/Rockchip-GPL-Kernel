/*
 * Copyright (C) 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "ump/ump_kernel_interface_ref_drv.h"
#include <linux/rk_fb.h>
#include <asm/uaccess.h>

static int _disp_get_ump_secure_id(struct fb_info *info, unsigned long arg, int nbuf)
{
	u32 __user *psecureid = (u32 __user *) arg;
	ump_secure_id secure_id;
	struct rk_lcdc_device_driver *dev_drv = (struct rk_lcdc_device_driver *)info->par;
	struct rk_fb_inf *g_fbi = dev_get_drvdata(info->device);
	int layer_id = dev_drv->fb_get_layer(dev_drv,info->fix.id);

	if (!g_fbi->ump_wrapped_buffer[info->node][nbuf]) {
		ump_dd_physical_block ump_memory_description;

		ump_memory_description.addr = info->fix.smem_start;
		ump_memory_description.size = info->fix.smem_len;

		printk("UMP: %s layerid %i - nbuf:%i, addr:%lu, size:%lu\n\n",
			dev_drv->name, layer_id, nbuf, ump_memory_description.addr, ump_memory_description.size);

		g_fbi->ump_wrapped_buffer[info->node][nbuf] =
			ump_dd_handle_create_from_phys_blocks
			(&ump_memory_description, 1);
	}
	secure_id = ump_dd_secure_id_get(g_fbi->
					 ump_wrapped_buffer[info->node][nbuf]);
	return put_user((unsigned int)secure_id, psecureid);
}

/*
static int _disp_get_ump_secure_id(struct fb_info *info, unsigned long arg, int nbuf)
{
    u32 __user *psecureid = (u32 __user *) arg;
    void __user *argp = (void __user *)arg;
    int buf_len = FB_MAXPGSIZE;
//    int bpp = (info->var.bits_per_pixel >> 3); //bytes per pixel
//    int buf_len = info->var.xres * info->var.yres * bpp;
//    u32 uoffs = 0;
    ump_secure_id secure_id;
    struct rk_fb_inf *g_fbi = dev_get_drvdata(info->device);
//Galland substituted for 2 lines below: int layer_id = get_fb_layer_id(&info->fix);
    struct rk_lcdc_device_driver *dev_drv = (struct rk_lcdc_device_driver *)info->par;
    int layer_id = dev_drv->fb_get_layer(dev_drv,info->fix.id);

//printk("\nUMP: ENTER   num_fb:%d  num_buf:%d  buf_adr:%x buf_len:%x",layer_id,nbuf,info->fix.smem_start,buf_len);
//	if (!(info->var.yres * 2 <= info->var.yres_virtual))//IAM
//	    printk("\nUMP: Double-buffering not enabled");
//	else

    if (!g_fbi->ump_wrapped_buffer[layer_id][nbuf]) {
		ump_dd_physical_block ump_memory_description;

		ump_memory_description.addr = info->fix.smem_start;
		ump_memory_description.size = buf_len;
		if (nbuf > 0)
	    	ump_memory_description.addr += (buf_len * nbuf);

		printk("UMP: %s layerid %d - nbuf:%d, addr:%X, size:%X\n\n",
			dev_drv->name, layer_id,
			nbuf, ump_memory_description.addr, ump_memory_description.size);
			
		g_fbi->ump_wrapped_buffer[layer_id][nbuf] =
	    	ump_dd_handle_create_from_phys_blocks(&ump_memory_description, 1);
    }
	
    secure_id = ump_dd_secure_id_get(g_fbi-> ump_wrapped_buffer[layer_id][nbuf]);
	//printk("UMP: secure_id:%X, arg:%X",secure_id,arg);
    return put_user((unsigned int)secure_id, psecureid);
}
*/
static int __init disp_ump_module_init(void)
{
	int ret = 0;

	disp_get_ump_secure_id = _disp_get_ump_secure_id;

	return ret;
}

static void __exit disp_ump_module_exit(void)
{
	disp_get_ump_secure_id = NULL;
}

module_init(disp_ump_module_init);
module_exit(disp_ump_module_exit);

MODULE_AUTHOR("Henrik Nordstrom <henrik@henriknordstrom.net>");
MODULE_DESCRIPTION("sunxi display driver MALI UMP module glue");
MODULE_LICENSE("GPL");
