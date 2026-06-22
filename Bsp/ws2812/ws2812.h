/**
 * Copyright (c) 2026, Deadline039
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __WS2812_H
#define __WS2812_H

#include <stdint.h>

#define WS2812_NUM     1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void ws2812_update(void);
void ws2812_gradient(uint8_t steps, uint16_t delay_ms);
void ws2812_set(uint8_t led_id, uint32_t color);
void ws2812_set_rgb(uint8_t led_id, uint8_t r, uint8_t g, uint8_t b);
void ws2812_set_all(uint32_t color);
uint32_t rgb_to_color(uint8_t r, uint8_t g, uint8_t b);
void color_to_rgb(uint32_t color, uint8_t *r, uint8_t *g, uint8_t *b);

uint32_t rainbow_color(float frequency, int phase, int center, int width);
void rainbow_effect(uint8_t steps, uint16_t delay_ms);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __WS2812_H */
