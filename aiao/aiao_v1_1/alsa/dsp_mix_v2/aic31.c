/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao drv alsa reference.
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

#include "tlv320aic3x.h"
#include "alsa_i2c.h"

struct snd_soc_codec *codec_copy;

static const struct snd_soc_dapm_widget aic3x_dapm_widgets[] = {
    SND_SOC_DAPM_LINE("audio out", NULL),
    SND_SOC_DAPM_LINE("audio in", NULL),
};

static const struct snd_soc_dapm_route audio_map[] = {
    { "audio out", NULL, "HPLOUT" },
    { "audio out", NULL, "HPROUT" },

    { "LINE1L", NULL, "audio in" },
    { "LINE1R", NULL, "audio in" },
};

static void time_delay_us(int usec)
{
    int i, j;

    usec = usec;

    for (i = 0; i < usec * 5; i++) {
        for (j = 0; j < 47; j++) {
            ;
        }
    }
}

int hisi_alsa_codec_init(struct snd_soc_pcm_runtime *rtd)
{
    struct snd_soc_codec *codec = rtd->codec;
    struct snd_soc_dapm_context *dapm = &codec->dapm;

    codec_copy = codec;

    godbox_aic31_reset();

    snd_soc_dapm_new_controls(dapm, aic3x_dapm_widgets,
                              ARRAY_SIZE(aic3x_dapm_widgets));
    snd_soc_dapm_add_routes(dapm, audio_map, ARRAY_SIZE(audio_map));

    /* not connected on godbox */
    snd_soc_dapm_nc_pin(dapm, "HPLCOM");
    snd_soc_dapm_nc_pin(dapm, "HPRCOM");
    snd_soc_dapm_nc_pin(dapm, "LLOUT");
    snd_soc_dapm_nc_pin(dapm, "RLOUT");
    snd_soc_dapm_nc_pin(dapm, "MONO_LOUT");

    snd_soc_dapm_nc_pin(dapm, "LINE2L");
    snd_soc_dapm_nc_pin(dapm, "LINE2R");
    snd_soc_dapm_nc_pin(dapm, "MIC3L");
    snd_soc_dapm_nc_pin(dapm, "MIC3R");

    /* fixed route */
    snd_soc_dapm_enable_pin(dapm, "audio out");
    snd_soc_dapm_enable_pin(dapm, "audio in");

    snd_soc_dapm_sync(dapm);
#define tlv320aic31_write(chip_addr, reg_addr, value) \
    snd_soc_write(codec, reg_addr, value)
    codec->cache_only = 0;
    tlv320aic31_write(I2C_AIC31, 0x1, 0x80);

    time_delay_us(50);

    tlv320aic31_write(I2C_AIC31, 0x0, 0x0);

    tlv320aic31_write(I2C_AIC31, 12, 0);

    /* mclk = 12.288M,not PLL mode */
    /* tlv320aic31_write(I2C_AIC31, 102, 0x32); if pll,p=r=1,k=8 */
    tlv320aic31_write(I2C_AIC31, 102, 0xc2);
    tlv320aic31_write(I2C_AIC31, 101, 0x01);
    tlv320aic31_write(I2C_AIC31, 3, 0x91);
    tlv320aic31_write(I2C_AIC31, 4, 0x60);

    tlv320aic31_write(I2C_AIC31, 2, 0x44);
    tlv320aic31_write(I2C_AIC31, 8, 0x30);

    tlv320aic31_write(I2C_AIC31, 9, 0x07); /* 16bits */
    tlv320aic31_write(I2C_AIC31, 25, 0x80); /* enable micbias */

    tlv320aic31_write(I2C_AIC31, 17, 0xf);
    tlv320aic31_write(I2C_AIC31, 18, 0xf0);

    tlv320aic31_write(I2C_AIC31, 15, 0x1c);
    tlv320aic31_write(I2C_AIC31, 16, 0x1c);

    /* left adc single_end mode, 0dB volume; ena power */
    tlv320aic31_write(I2C_AIC31, 19, 0x04);

    /* right adc single_end mode, 0dB volume; ena power */
    tlv320aic31_write(I2C_AIC31, 22, 0x07);

    tlv320aic31_write(I2C_AIC31, 28, 0);
    tlv320aic31_write(I2C_AIC31, 31, 0);
    tlv320aic31_write(I2C_AIC31, 26, 0x00);
    tlv320aic31_write(I2C_AIC31, 29, 0x00);

    /* set fsref,dual rate mode or not;D1~D4 set I2S into dac,both dac open */
    tlv320aic31_write(I2C_AIC31, 7, 0xa);

    /* high-power outputs for ac-coupled */
    tlv320aic31_write(I2C_AIC31, 14, 0x80);

    /* left and right dac power */
    tlv320aic31_write(I2C_AIC31, 37, 0xc0);

    /* left and right dac power */
    tlv320aic31_write(I2C_AIC31, 38, 0x10);

    /* output common-mode voltage 1.65v, disable soft_step volume */
    tlv320aic31_write(I2C_AIC31, 40, 0x80);

    /* select DAC_L(R)1 to hi-power,left and right volume dependent controls */
    tlv320aic31_write(I2C_AIC31, 41, 0x00);

    /* output driver pop reduction(medium) */
    tlv320aic31_write(I2C_AIC31, 42, 0x88);

    /* left DAC channel not muted;DAC digital volume control(attenuation,adjust value level) */
    tlv320aic31_write(I2C_AIC31, 43, 0x99);

    /* right DAC channel not muted;DAC digital volume control(attenuation,adjust value level) */
    tlv320aic31_write(I2C_AIC31, 44, 0x99);

    /* change reg51 & reg65 from 0x9f to 0x9a, in order to remove the pop when initing */
    /* left output level control(enlarge,max);output mute, power */
    tlv320aic31_write(I2C_AIC31, 51, 0x9a);

    /* right output level control(enlarge,max);output mute,power */
    tlv320aic31_write(I2C_AIC31, 65, 0x9a);

    /* enable LEFT_LOP/M and RIGHT_LOP/M */
    /* 0: PGA_L is not routed to LEFT_LOP/M */
    tlv320aic31_write(I2C_AIC31, 81, 0x00);

    /* 0: DAC_L1 is routed to LEFT_LOP/M */
    tlv320aic31_write(I2C_AIC31, 82, 0x80);

    /* left LEFT_LOP level 0dB, un-mute, power up */
    tlv320aic31_write(I2C_AIC31, 86, 0x0b);

    /* 0: PGA_R is not routed to RIGHT_LOP/M */
    tlv320aic31_write(I2C_AIC31, 91, 0x00);

    /* 0: DAC_R1 is routed to RIGHT_LOP/M */
    tlv320aic31_write(I2C_AIC31, 92, 0x80);

    /* left LEFT_LOP level 0dB, un-mute, power up */
    tlv320aic31_write(I2C_AIC31, 93, 0x0b);

    /* DAC_L1 is routed to HPLOUT;output stage volume control(no attenuation) */
    tlv320aic31_write(I2C_AIC31, 47, 0x96);

    /* DAC_L1 is routed to HPLOUT;output stage volume control(no attenuation) */
    tlv320aic31_write(I2C_AIC31, 50, 0x00);

    /* DAC_R1 is routed to HPROUT;output stage volume control(no attenuation) */
    tlv320aic31_write(I2C_AIC31, 61, 0x00);

    /* DAC_R1 is routed to HPROUT;output stage volume control(no attenuation) */
    tlv320aic31_write(I2C_AIC31, 64, 0x96);

    snd_soc_dapm_sync(dapm);
    codec->cache_only = 1;
    return 0;
}

