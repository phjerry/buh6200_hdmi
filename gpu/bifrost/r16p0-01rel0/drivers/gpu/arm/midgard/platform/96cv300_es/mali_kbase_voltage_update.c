

#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/pm_opp.h>
#include <linux/ioport.h>
#include <linux/delay.h>

#include <mali_kbase.h>
#include <mali_kbase_defs.h>
#include <mali_kbase_config.h>

#include "mali_kbase_voltage_update.h"
#include "mali_kbase_dt.h"

#define LINE_BASE_ADDR 0x841800
#define LINE_BASE_SIZE 0x100

#define HPM_BASE_ADDR 0x00a15500
#define HPM_BASE_SIZE 0x100

#define OTP_BASE_ADDR 0xb00000
#define OTP_BASE_SIZE 0x400

typedef enum {
	LINE_REG_ADDR_800M = 0x68,    /* bit[31:16]: ratio; bit[15:0]: constant */
	LINE_REG_ADDR_675M = 0x6C,    /* bit[31:16]: ratio; bit[15:0]: constant */
	LINE_REG_ADDR_300M = 0x70,    /* bit[31:16]: ratio; bit[15:0]: constant */
}line_register_addr;

typedef enum {
	LINE_REG_MASK = 0xffff,
	LINE_REG_OFFSET_CONSTANT = 0,
	LINE_REG_OFFSET_RATIO = 16,
}line_register_mask;

typedef enum {
	HPM_REG_ADDR_INIT0 = 0x0,
	HPM_REG_ADDR_INIT1 = 0xc,
	HPM_REG_ADDR_RECORD_0_1 = 0x4,    /* bit[9:0]: record0; bit[21:12]: record1 */
	HPM_REG_ADDR_RECORD_2_3 = 0x8,    /* bit[9:0]: record2; bit[21:12]: record3 */
}hpm_register_addr;

typedef enum {
	HPM_REG_VALUE_INIT0 = 0x04000001,
	HPM_REG_VALUE_INIT1 = 0x03000000,
	HPM_REG_MASK_RECORD = 0x3ff,
	HPM_REG_OFFSET_RECORD_0 = 0,
	HPM_REG_OFFSET_RECORD_1 = 12,
	HPM_REG_OFFSET_RECORD_2 = 0,
	HPM_REG_OFFSET_RECORD_3 = 12,
}hpm_register_mask;

typedef enum {
	OTP_REG_ADDR_HPM = 0x314,       /* bit[30:20]: otp compensate for hpm */
	OTP_REG_ADDR_TEMP = 0x338,      /* bit[7:0]: GPU Tsensor temperature, limits: [-31.75C, 31.75C] */
	OTP_REG_ADDR_ENABLE = 0x340,    /* bit[2]: if OTP compensate or not */
	OTP_REG_ADDR_VOLT = 0x344,      /* bit[3:0]: 300M; bit[15:12]: 675M; bit[19:16]: 800M - otp compensate for voltage */
}otp_register_addr;

typedef enum {
	OTP_REG_MASK_ENABLE = 0x4,
	OTP_REG_MASK_HPM = 0x3ff,
	OTP_REG_OFFSET_HPM = 20,

	OTP_REG_MASK_TEMP = 0xf,
	OTP_REG_MASK_TEMP_SIGN = 0x8,            /* 1: negative; 0: positive */
	OTP_REG_MASK_TEMP_VALUE = 0x7,           /* limits: -31.75C ~ 31.75C */
	OTP_REG_OFFSET_TEMP = 8,

	OTP_REG_MASK_COMPENSATE = 0xf,
	OTP_REG_MASK_COMPENSATE_ENABLE = 0x8,    /* 1: compensate volt; 0: not compensate */
	OTP_REG_MASK_COMPENSATE_VOLT = 0x3,      /* 0x00: 15mv;  0x01: 30mv; 0x10: 45mv; 0x11: 60mv */
	OTP_REG_OFFSET_COMPENSATE_300M = 0,
	OTP_REG_OFFSET_COMPENSATE_675M = 12,
	OTP_REG_OFFSET_COMPENSATE_800M = 16,
}otp_register_mask;

typedef struct volt_update_config {
	int index;
	unsigned long freq;

	unsigned int line_constant;
	unsigned int line_ratio;
	unsigned int otp_volt;
}volt_update_config;

#define HPM_AVG 4
#define HPM_DELAY_MS 10
#define HPM_DIVISOR 1000

#define TEMP_H_BOUND 35
#define TEMP_L_BOUND 30
#define TEMP_COMPENSATE_VOLT 40
#define TEMP_PER_C 4

#define VOLT_MAX 1000
#define VOLT_PER_OTP 15
#define VOLT_DIVISOR 1000
#define VOLT_INIT (880 * VOLT_DIVISOR)

static volatile void __iomem *g_addr_line = NULL;
static volatile void __iomem *g_addr_hpm = NULL;
static volatile void __iomem *g_addr_otp = NULL;

static int g_hpm;

volt_update_config g_volt_config[] = {
	{ 0, 800000, 0, 0, 0 },
	{ 1, 675000, 0, 0, 0 },
	{ 2, 300000, 0, 0, 0 },
};

#define FREQ_NUM (sizeof(g_volt_config)/sizeof(g_volt_config[0]))

static int g_temp = 0;
static bool g_temp_status = false;

static void compute_hpm(void)
{
	int gpu_hpm = 0;
	int otp_hpm = 0;
	int opt_enable;
	struct kbase_device *kbdev = dev_get_drvdata(kbase_get_platform_device());

	/* init gpu hpm */
	regulator_set_voltage(kbdev->regulator, VOLT_INIT, VOLT_INIT);

	writel(HPM_REG_VALUE_INIT0, g_addr_hpm + HPM_REG_ADDR_INIT0);
	writel(HPM_REG_VALUE_INIT1, g_addr_hpm + HPM_REG_ADDR_INIT1);

	mdelay(HPM_DELAY_MS);

	/* read gpu hpm */
	gpu_hpm += (readl(g_addr_hpm + HPM_REG_ADDR_RECORD_0_1) >> HPM_REG_OFFSET_RECORD_0) & HPM_REG_MASK_RECORD;
	gpu_hpm += (readl(g_addr_hpm + HPM_REG_ADDR_RECORD_0_1) >> HPM_REG_OFFSET_RECORD_1) & HPM_REG_MASK_RECORD;
	gpu_hpm += (readl(g_addr_hpm + HPM_REG_ADDR_RECORD_2_3) >> HPM_REG_OFFSET_RECORD_2) & HPM_REG_MASK_RECORD;
	gpu_hpm += (readl(g_addr_hpm + HPM_REG_ADDR_RECORD_2_3) >> HPM_REG_OFFSET_RECORD_3) & HPM_REG_MASK_RECORD;
	gpu_hpm = gpu_hpm / HPM_AVG;

	/* read otp hpm */
	opt_enable = readl(g_addr_otp + OTP_REG_ADDR_ENABLE) & OTP_REG_MASK_ENABLE;

	if (opt_enable > 0) {
		otp_hpm = (readl(g_addr_otp + OTP_REG_ADDR_HPM) >> OTP_REG_OFFSET_HPM) & OTP_REG_MASK_HPM;
		g_hpm = min(otp_hpm, gpu_hpm);
	} else {
		g_hpm = gpu_hpm;
	}

	hisi_debug_info("HPM: final_hpm = %d, otp_hpm = %d, gpu_hpm = %d\n", g_hpm, otp_hpm, gpu_hpm);

	return;
}

static void compute_line(void)
{
	int i;
	unsigned int offset[FREQ_NUM] = {
		LINE_REG_ADDR_800M,
		LINE_REG_ADDR_675M,
		LINE_REG_ADDR_300M
	};

	for (i = 0; i < FREQ_NUM; i++) {
		g_volt_config[i].line_constant = (readl(g_addr_line + offset[i]) >> LINE_REG_OFFSET_CONSTANT) & LINE_REG_MASK;
		g_volt_config[i].line_ratio    = (readl(g_addr_line + offset[i]) >> LINE_REG_OFFSET_RATIO) & LINE_REG_MASK;

		hisi_debug_info("LINE: freq = %lu, constant = %d, ratio = %d\n",
			g_volt_config[i].freq, g_volt_config[i].line_constant, g_volt_config[i].line_ratio);
	}

	return;
}

static void compute_otp(void)
{
	int i, value;
	unsigned int offset[FREQ_NUM] = {
		OTP_REG_OFFSET_COMPENSATE_800M,
		OTP_REG_OFFSET_COMPENSATE_675M,
		OTP_REG_OFFSET_COMPENSATE_300M
	};

	for (i = 0; i < FREQ_NUM; i++) {
		value = (readl(g_addr_otp + OTP_REG_ADDR_VOLT) >> offset[i]) & OTP_REG_MASK_COMPENSATE;

		if (value & OTP_REG_MASK_COMPENSATE_ENABLE) {
			value &= OTP_REG_MASK_COMPENSATE_VOLT;
			g_volt_config[i].otp_volt = (value + 1) * VOLT_PER_OTP;
		} else {
			g_volt_config[i].otp_volt = 0;
		}

		hisi_debug_info("OTP: freq = %lu, otp_volt = %d\n", g_volt_config[i].freq, g_volt_config[i].otp_volt);
	}

	return;
}

static void compute_temp(void)
{
	unsigned int value;
	unsigned int sign, temp;

	value = (readl(g_addr_otp + OTP_REG_ADDR_TEMP) >> OTP_REG_OFFSET_TEMP) & OTP_REG_MASK_TEMP;

	sign = (value & OTP_REG_MASK_TEMP_SIGN) ? -1 : 1;
	temp = (value & OTP_REG_MASK_TEMP_VALUE) / TEMP_PER_C;

	g_temp = sign * temp;

	hisi_debug_info("TEMP: temprature = %d\n", (int)g_temp);

	return;
}

int kbase_voltage_update(unsigned long freq, unsigned long *volt)
{
	int index;

	for (index = 0; index < FREQ_NUM; index++) {
		if (g_volt_config[index].freq == freq) {
			break;
		}
	}

	if (index >= FREQ_NUM) {
		hisi_error_info();
		return -1;
	}

	/* Line -> Volt */
	*volt = g_volt_config[index].line_constant - g_volt_config[index].line_ratio * g_hpm / HPM_DIVISOR;

	/* OTP -> Volt */
	*volt += g_volt_config[index].otp_volt;

	/* Temp -> Volt */
	compute_temp();

	if (g_temp != 0) {
		if ((g_temp < TEMP_L_BOUND) && !g_temp_status) {
			volt += TEMP_COMPENSATE_VOLT;
			g_temp_status = true;
		} else if ((g_temp > TEMP_H_BOUND) && g_temp_status) {
			volt -= TEMP_COMPENSATE_VOLT;
			g_temp_status = false;
		}
	}

	if (*volt > VOLT_MAX) {
		*volt = VOLT_MAX;
	}

	hisi_debug_info("Final Volt = %lu, OTP Volt = %d, Temp Volt = %d\n",
		*volt, g_volt_config[index].otp_volt, g_temp_status ? TEMP_COMPENSATE_VOLT : 0);

	*volt = *volt * VOLT_DIVISOR;

	return 0;
}

int kbase_voltage_update_init(void)
{
	g_addr_line = (void __iomem*)ioremap_nocache(LINE_BASE_ADDR, LINE_BASE_SIZE);
	if (g_addr_line == NULL) {
		hisi_error_info();
		return -1;
	}

	g_addr_hpm = (void __iomem*)ioremap_nocache(HPM_BASE_ADDR, HPM_BASE_SIZE);
	if (g_addr_hpm == NULL) {
		hisi_error_info();
		return -1;
	}

	g_addr_otp = (void __iomem*)ioremap_nocache(OTP_BASE_ADDR, OTP_BASE_SIZE);
	if (g_addr_otp == NULL) {
		hisi_error_info();
		return -1;
	}

	compute_hpm();

	compute_line();

	compute_otp();

	compute_temp();

	return 0;
}

int kbase_voltage_update_deinit(void)
{
	iounmap(g_addr_line);

	iounmap(g_addr_hpm);

	iounmap(g_addr_otp);

	g_addr_line = NULL;

	g_addr_hpm = NULL;

	g_addr_otp = NULL;

	return 0;
}





