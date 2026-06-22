/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#include "ws2812.h"
#include <tim.h>
#include <math.h>

#define CODE_ONE_DUTY  140
#define CODE_ZERO_DUTY 70

#define RST_PERIOD_NUM 100

uint32_t ws2812_color[WS2812_NUM] = { 0 };
static uint32_t ws2812_color_current[WS2812_NUM];

/**
 * @brief Update the WS2812 LED data buffer and start DMA transfer.
 */
void ws2812_update(void)
{
    static uint16_t ws2812_data[RST_PERIOD_NUM + WS2812_NUM * 24];

    for (uint8_t led_id = 0; led_id < WS2812_NUM; led_id++) {
        ws2812_color_current[led_id] = ws2812_color[led_id];
        static uint8_t r, g, b;
        color_to_rgb(ws2812_color_current[led_id], &r, &g, &b);
        uint16_t *p = ws2812_data + RST_PERIOD_NUM + led_id * 24;
        for (uint8_t i = 0; i < 8; i++) {
            p[i] = (r << i) & 0x80 ? CODE_ONE_DUTY : CODE_ZERO_DUTY;
            p[i + 8] = (g << i) & 0x80 ? CODE_ONE_DUTY : CODE_ZERO_DUTY;
            p[i + 16] = (b << i) & 0x80 ? CODE_ONE_DUTY : CODE_ZERO_DUTY;
        }
    }
    tim3_ch2_pwm_start_dma((uint32_t *)ws2812_data, sizeof(ws2812_data) / sizeof(ws2812_data[0]));
}

/**
 * @brief Update LED colors using linear interpolation.
 * @param steps Number of gradient steps.
 * @param delay_ms Delay time between each step in milliseconds.
 */
void ws2812_gradient(uint8_t steps, uint16_t delay_ms)
{
    static uint8_t start_r[WS2812_NUM], start_g[WS2812_NUM], start_b[WS2812_NUM];
    static float r_step[WS2812_NUM], g_step[WS2812_NUM], b_step[WS2812_NUM];

    for (uint8_t i = 0; i < WS2812_NUM; i++) {
        color_to_rgb(ws2812_color_current[i], &start_r[i], &start_g[i], &start_b[i]);
        uint8_t target_r, target_g, target_b;
        color_to_rgb(ws2812_color[i], &target_r, &target_g, &target_b);

        r_step[i] = (float)(target_r - start_r[i]) / steps;
        g_step[i] = (float)(target_g - start_g[i]) / steps;
        b_step[i] = (float)(target_b - start_b[i]) / steps;
    }

    for (uint8_t step = 1; step <= steps; step++) {
        for (uint8_t led_id = 0; led_id < WS2812_NUM; led_id++) {
            uint8_t r = (uint8_t)(start_r[led_id] + r_step[led_id] * step);
            uint8_t g = (uint8_t)(start_g[led_id] + g_step[led_id] * step);
            uint8_t b = (uint8_t)(start_b[led_id] + b_step[led_id] * step);

            ws2812_set_rgb(led_id, r, g, b);
        }

        ws2812_update();
        HAL_Delay(delay_ms);
    }
}

/**
 * @brief Set the RGB color for a specific LED.
 * @param led_id LED index.
 * @param r Red intensity (0-255).
 * @param g Green intensity (0-255).
 * @param b Blue intensity (0-255).
 */
void ws2812_set_rgb(uint8_t led_id, uint8_t r, uint8_t g, uint8_t b)
{
    if (led_id >= WS2812_NUM) {
        return;
    }
    ws2812_color[led_id] = rgb_to_color(r, g, b);
}

/**
 * @brief Set the color for a specific LED using a 24-bit packed color.
 * @param led_id LED index.
 * @param color 24-bit packed RGB color.
 */
void ws2812_set(uint8_t led_id, uint32_t color)
{
    if (led_id >= WS2812_NUM) {
        return;
    }
    ws2812_color[led_id] = color;
}

/**
 * @brief Set the same 24-bit packed color for all LEDs.
 * @param color 24-bit packed RGB color.
 */
void ws2812_set_all(uint32_t color)
{
    for (uint8_t led_id = 0; led_id < WS2812_NUM; led_id++) {
        ws2812_color[led_id] = color;
    }
}

/**
 * @brief Convert RGB values to a 24-bit packed color.
 * @param r Red intensity (0-255).
 * @param g Green intensity (0-255).
 * @param b Blue intensity (0-255).
 * @return 24-bit packed RGB color.
 */
uint32_t rgb_to_color(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 8) | (g << 16) | b;
}

/**
 * @brief Convert a 24-bit packed color to RGB values.
 * @param color 24-bit packed RGB color.
 * @param r Pointer to receive red intensity.
 * @param g Pointer to receive green intensity.
 * @param b Pointer to receive blue intensity.
 */
void color_to_rgb(uint32_t color, uint8_t *r, uint8_t *g, uint8_t *b)
{
    *r = (color >> 16) & 0xFF;
    *g = (color >> 8) & 0xFF;
    *b = color & 0xFF;
}

/**
 * @brief Generate a rainbow color using sine-based interpolation.
 * @param frequency Color wave frequency.
 * @param phase Phase offset for the color cycle.
 * @param center Center value for the color output.
 * @param width Amplitude width for the color output.
 * @return 24-bit packed RGB rainbow color.
 */
uint32_t rainbow_color(float frequency, int phase, int center, int width)
{
    float r = sinf(frequency * phase + 0) * width + center;
    float g = sinf(frequency * phase + 2) * width + center;
    float b = sinf(frequency * phase + 4) * width + center;
    return rgb_to_color((uint8_t)r, (uint8_t)g, (uint8_t)b);
}

/**
 * @brief Play a moving rainbow effect on all LEDs.
 * @param steps Number of steps in the effect.
 * @param delay_ms Delay time between each step in milliseconds.
 */
void rainbow_effect(uint8_t steps, uint16_t delay_ms)
{
    float frequency = 0.1;
    int center = 128;
    int width = 127;

    for (int i = 0; i < steps; i++) {
        for (uint8_t led_id = 0; led_id < WS2812_NUM; led_id++) {
            uint32_t color = rainbow_color(frequency, i + led_id * 2, center, width);
            ws2812_set(led_id, color);
        }
        ws2812_update();
        HAL_Delay(delay_ms);
    }
}
