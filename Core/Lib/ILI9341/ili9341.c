/*
 * ili9341.c
 *
 *  Created on: Dec 17, 2023
 *      Author: zh
 */

#include "stm32f4xx_hal.h"
#include "ili9341.h"

// Send command to display. Use ILI9341_SELECT() before
static inline void ILI9341_WriteCommand(ILI9341TypeDef *display, uint8_t command)
{
	ILI9341_COMMAND(display);
	HAL_SPI_Transmit(display->spi, &command, 1, 1);
}

// Send byte to display. Use ILI9341_SELECT() before
static inline void ILI9341_WriteByte(ILI9341TypeDef *display, uint8_t data)
{
	ILI9341_DATA(display);
	HAL_SPI_Transmit(display->spi, &data, 1, 1);
}

// Send buffer to display. Use ILI9341_SELECT() before
static inline void ILI9341_WriteBuffer(ILI9341TypeDef *display, uint8_t* buff, size_t buff_size)
{
	ILI9341_DATA(display);

    // split data in small chunks because HAL can't send more then 64K at once
    while (buff_size > 0) {
        uint16_t chunk_size = buff_size > 32768 ? 32768 : buff_size;
        HAL_SPI_Transmit(display->spi, buff, chunk_size, 10);
        buff += chunk_size;
        buff_size -= chunk_size;
    }
}

static inline void ILI9341_SetAddressWindow(ILI9341TypeDef *display, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // Column address set
	ILI9341_WriteCommand(display, 0x2A); // CASET

	uint8_t data[4];
	data[0] = x1 >> 8;
	data[1] = x1;
	data[2] = x2 >> 8;
	data[3] = x2;

	ILI9341_DATA(display);
	HAL_SPI_Transmit(display->spi, data, 4, 1);

    // Row address set
	ILI9341_WriteCommand(display, 0x2B); // RASET

	data[0] = y1 >> 8;
	data[1] = y1;
	data[2] = y2 >> 8;
	data[3] = y2;

	ILI9341_DATA(display);
	HAL_SPI_Transmit(display->spi, data, 4, 1);

    // Write to RAM
	ILI9341_WriteCommand(display, 0x2C); // RAMWR
}

static inline void ILI9341_Reset(ILI9341TypeDef *display)
{
    display->reset_gpio_port->BSRR = (uint32_t)(display->reset_pin) << 16U;
    HAL_Delay(5);
    display->reset_gpio_port->BSRR = display->reset_pin;
}

void ILI9341_Init(ILI9341TypeDef *display)
{
	ILI9341_SELECT(display);
	ILI9341_Reset(display);

	// SOFTWARE RESET
	ILI9341_WriteCommand(display, 0x01);
	HAL_Delay(1000);

	// POWER CONTROL A
	ILI9341_WriteCommand(display, 0xCB);
	ILI9341_WriteByte(display, 0x39);
	ILI9341_WriteByte(display, 0x2C);
	ILI9341_WriteByte(display, 0x00);
	ILI9341_WriteByte(display, 0x34);
	ILI9341_WriteByte(display, 0x02);

	// POWER CONTROL B
	ILI9341_WriteCommand(display, 0xCF);
	ILI9341_WriteByte(display, 0x00);
	ILI9341_WriteByte(display, 0xC1);
	ILI9341_WriteByte(display, 0x30);

	// DRIVER TIMING CONTROL A
	ILI9341_WriteCommand(display, 0xE8);
	ILI9341_WriteByte(display, 0x85);
	ILI9341_WriteByte(display, 0x00);
	ILI9341_WriteByte(display, 0x78);

	// DRIVER TIMING CONTROL B
	ILI9341_WriteCommand(display, 0xEA);
	ILI9341_WriteByte(display, 0x00);
	ILI9341_WriteByte(display, 0x00);

	// POWER ON SEQUENCE CONTROL
	ILI9341_WriteCommand(display, 0xED);
	ILI9341_WriteByte(display, 0x64);
	ILI9341_WriteByte(display, 0x03);
	ILI9341_WriteByte(display, 0x12);
	ILI9341_WriteByte(display, 0x81);

	// PUMP RATIO CONTROL
	ILI9341_WriteCommand(display, 0xF7);
	ILI9341_WriteByte(display, 0x20);

	// POWER CONTROL,VRH[5:0]
	ILI9341_WriteCommand(display, 0xC0);
	ILI9341_WriteByte(display, 0x23);

	// POWER CONTROL,SAP[2:0];BT[3:0]
	ILI9341_WriteCommand(display, 0xC1);
	ILI9341_WriteByte(display, 0x10);

	// VCM CONTROL
	ILI9341_WriteCommand(display, 0xC5);
	ILI9341_WriteByte(display, 0x3E);
	ILI9341_WriteByte(display, 0x28);

	// VCM CONTROL 2
	ILI9341_WriteCommand(display, 0xC7);
	ILI9341_WriteByte(display, 0x86);

	// MEMORY ACCESS CONTROL
	ILI9341_WriteCommand(display, 0x36);
	ILI9341_WriteByte(display, 0x48);

	// PIXEL FORMAT
	ILI9341_WriteCommand(display, 0x3A);
	ILI9341_WriteByte(display, 0x55);

	// FRAME RATIO CONTROL, STANDARD RGB COLOR
	ILI9341_WriteCommand(display, 0xB1);
	ILI9341_WriteByte(display, 0x00);
	ILI9341_WriteByte(display, 0x18);

	// DISPLAY FUNCTION CONTROL
	ILI9341_WriteCommand(display, 0xB6);
	ILI9341_WriteByte(display, 0x08);
	ILI9341_WriteByte(display, 0x82);
	ILI9341_WriteByte(display, 0x27);

	// 3GAMMA FUNCTION DISABLE
	ILI9341_WriteCommand(display, 0xF2);
	ILI9341_WriteByte(display, 0x00);

	// GAMMA CURVE SELECTED
	ILI9341_WriteCommand(display, 0x26);
	ILI9341_WriteByte(display, 0x01);

	// POSITIVE GAMMA CORRECTION
	ILI9341_WriteCommand(display, 0xE0);
	ILI9341_WriteByte(display, 0x0F);
	ILI9341_WriteByte(display, 0x31);
	ILI9341_WriteByte(display, 0x2B);
	ILI9341_WriteByte(display, 0x0C);
	ILI9341_WriteByte(display, 0x0E);
	ILI9341_WriteByte(display, 0x08);
	ILI9341_WriteByte(display, 0x4E);
	ILI9341_WriteByte(display, 0xF1);
	ILI9341_WriteByte(display, 0x37);
	ILI9341_WriteByte(display, 0x07);
	ILI9341_WriteByte(display, 0x10);
	ILI9341_WriteByte(display, 0x03);
	ILI9341_WriteByte(display, 0x0E);
	ILI9341_WriteByte(display, 0x09);
	ILI9341_WriteByte(display, 0x00);

	// NEGATIVE GAMMA CORRECTION
	ILI9341_WriteCommand(display, 0xE1);
	ILI9341_WriteByte(display, 0x00);
	ILI9341_WriteByte(display, 0x0E);
	ILI9341_WriteByte(display, 0x14);
	ILI9341_WriteByte(display, 0x03);
	ILI9341_WriteByte(display, 0x11);
	ILI9341_WriteByte(display, 0x07);
	ILI9341_WriteByte(display, 0x31);
	ILI9341_WriteByte(display, 0xC1);
	ILI9341_WriteByte(display, 0x48);
	ILI9341_WriteByte(display, 0x08);
	ILI9341_WriteByte(display, 0x0F);
	ILI9341_WriteByte(display, 0x0C);
	ILI9341_WriteByte(display, 0x31);
	ILI9341_WriteByte(display, 0x36);
	ILI9341_WriteByte(display, 0x0F);

	// EXIT SLEEP
	ILI9341_WriteCommand(display, 0x11);
	HAL_Delay(120);

	// TURN ON DISPLAY
	ILI9341_WriteCommand(display, 0x29);

	// MADCTL
	ILI9341_WriteCommand(display, 0x36);
	ILI9341_WriteByte(display, display->orientation);

	ILI9341_UNSELECT(display);
}

void ILI9341_InvertColors(ILI9341TypeDef *display, uint8_t invert)
{
    ILI9341_SELECT(display);
    ILI9341_WriteCommand(display, (invert > 0) ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
    ILI9341_UNSELECT(display);
}

void ILI9341_DrawPixel(ILI9341TypeDef *display, uint16_t x, uint16_t y, uint16_t color)
{
	// Check out of bounds
	if((x >= display->width) || (y >= display->height))
		return;

	ILI9341_SELECT(display);
	ILI9341_SetAddressWindow(display, x, y, x + 1, y + 1);

	ILI9341_DATA(display);
	uint8_t data[2] = { color >> 8, color };
	HAL_SPI_Transmit(display->spi, data, 2, 1);

	ILI9341_UNSELECT(display);
}

static void ILI9341_Fill(ILI9341TypeDef *display, uint16_t color, uint32_t size)
{
	ILI9341_DATA(display);

	uint32_t buffer_size = 0;

	if ((size * 2) < ILI9341_BUFFER_MAX_SIZE)
		buffer_size = size;
	else
		buffer_size = ILI9341_BUFFER_MAX_SIZE;

	uint8_t color_shifted = color >> 8;
	uint8_t buffer[buffer_size];

	for (uint32_t i = 0; i < buffer_size; i += 2) {
		buffer[i]     = color_shifted;
		buffer[i + 1] = color;
	}

	uint32_t sending_size   = size * 2;
	uint32_t sending_blocks = sending_size / buffer_size;
	uint32_t sending_remain = sending_size % buffer_size;

	if (sending_blocks != 0) {
		for(uint32_t i = 0; i < sending_blocks; i++)
			HAL_SPI_Transmit(display->spi, (uint8_t*)buffer, buffer_size, 10);
	}

	HAL_SPI_Transmit(display->spi, (uint8_t*)buffer, sending_remain, 10);
}

void ILI9341_FillScreen(ILI9341TypeDef *display, uint16_t color)
{
	ILI9341_SELECT(display);
	ILI9341_SetAddressWindow(display, 0, 0, display->width - 1, display->height - 1);
	ILI9341_Fill(display, color, display->width * display->height);
	ILI9341_UNSELECT(display);
}

void ILI9341_FillRectangle(ILI9341TypeDef *display, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	if ((x >= display->width) || (y >= display->height))
		return;

	if ((x + w - 1) >= display->width)
		w = display->width - x;

	if ((y + h - 1) >= display->height)
		h = display->height - y;

	ILI9341_SELECT(display);
	ILI9341_SetAddressWindow(display, x, y, x + w - 1, y + h - 1);
	ILI9341_Fill(display, color, w * h);
	ILI9341_UNSELECT(display);
}

void ILI9341_Rectangle(ILI9341TypeDef *display, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	ILI9341_FillRectangle(display, x,     y,     w + 1, 1, color);
	ILI9341_FillRectangle(display, x, y + h,     w + 1, 1, color);
	ILI9341_FillRectangle(display, x,     y,     1,     h, color);
	ILI9341_FillRectangle(display, x + w, y,     1,     h, color);
}

static void ILI9341_WriteChar(ILI9341TypeDef *display, uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
    uint32_t i, b, j;

    ILI9341_SetAddressWindow(display, x, y, x + font.width - 1, y + font.height - 1);

    for (i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for (j = 0; j < font.width; j++) {
            if ((b << j) & 0x8000) {
                uint8_t data[] = { color >> 8, color & 0xFF };
                ILI9341_WriteBuffer(display, data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                ILI9341_WriteBuffer(display, data, sizeof(data));
            }
        }
    }
}

void ILI9341_WriteString(ILI9341TypeDef *display, uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor)
{
	ILI9341_SELECT(display);

    while (*str) {
        if (x + font.width >= display->width) {
            x = 0;
            y += font.height;

            if (y + font.height >= display->height)
                break;

            if (*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ILI9341_WriteChar(display, x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

	ILI9341_UNSELECT(display);
}
