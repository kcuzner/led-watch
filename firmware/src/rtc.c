/**
 * LED Wristwatch
 *
 * Kevin Cuzner
 */

#include "rtc.h"

#include "stm32l0xx.h"

#define ORIGIN_CENTURY 2000

static uint32_t time_buffer;
static uint32_t date_buffer;

void rtc_init(void)
{
    //enable PWR
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    //enable working with the LSE bits in the CSR
    PWR->CR |= PWR_CR_DBP;
    RCC->CSR |= RCC_CSR_LSEDRV_0 | RCC_CSR_LSEON; //low medium drive, LSE on
    while (!(RCC->CSR & RCC_CSR_LSERDY)) { }
    //enable RTC, select LSE as clock
    RCC->CSR |= RCC_CSR_RTCEN | RCC_CSR_RTCSEL_0;
}

bool rtc_is_set(void)
{
    return !!(RTC->ISR & RTC_ISR_INITS);
}

static uint8_t bin_to_bcd(uint8_t binary)
{
    uint8_t upper = binary / 10;
    uint8_t lower = binary % 10;
    return ((upper & 0xF) << 4) | (lower & 0xF);
}

static uint8_t bcd_to_bin(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0xF);
}

static uint8_t date_to_weekday(uint8_t year, uint8_t month, uint8_t day)
{
    uint16_t y = year + ORIGIN_CENTURY;
    uint8_t m = month;
    uint8_t d = day;

    //the following line of code costs around 400 bytes. Hopefully its worth it.
    //https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week#Implementation-dependent_methods
    uint8_t weekday = (d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)%7;

    return weekday + 1; //we require Monday = 1 format
}

uint8_t yr, mth, dy, hr, mn, sc;

void rtc_set(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    uint8_t year_bcd, month_bcd, day_bcd, hour_bcd, minute_bcd, second_bcd;
    uint8_t weekday;

    //Unprotect the RTC registers
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    //enter initialization mode
    RTC->ISR |= RTC_ISR_INIT;
    while (!(RTC->ISR & RTC_ISR_INITF)) { }

    //program the prescaler for the 32.768KHz clock
    RTC->PRER = 0x007F00FF;

    yr = year;
    mth = month;
    dy = day;
    hr = hour;
    mn = minute;
    sc = second;

    //Convert the values to BCD
    year_bcd = bin_to_bcd(year);
    month_bcd = bin_to_bcd(month);
    day_bcd = bin_to_bcd(day);
    hour_bcd = bin_to_bcd(hour);
    minute_bcd = bin_to_bcd(minute);
    second_bcd = bin_to_bcd(second);

    //Determine the weekday
    weekday = date_to_weekday(year, month, day);

    //Program the calendar
    RTC->TR = ((hour_bcd & 0x3F) << RTC_TR_HU_Pos) |
        ((minute_bcd & 0x7F) << RTC_TR_MNU_Pos) |
        ((second_bcd & 0x7F) << RTC_TR_SU_Pos);
    RTC->DR = (year_bcd << RTC_DR_YU_Pos) |
        ((weekday & 0x7) << RTC_DR_WDU_Pos) |
        ((month_bcd & 0x1F) << RTC_DR_MU_Pos) |
        ((day_bcd & 0x3F) << RTC_DR_DU_Pos);

    //Set hour format to 24 hours, use shadow registers
    RTC->CR = 0;

    //Exit initialization mode
    RTC->ISR &= ~RTC_ISR_INIT;
}

void rtc_refresh(void)
{
    //Wait for the sync flag
    while (!(RTC->ISR & RTC_ISR_RSF)) { }

    //Reset the sync flag
    RTC->ISR &= ~RTC_ISR_RSF;
}

uint8_t rtc_get_hours(void)
{
    return bcd_to_bin((RTC->TR >> RTC_TR_HU_Pos) & 0x3F);
}

uint8_t rtc_get_minutes(void)
{
    return bcd_to_bin((RTC->TR >> RTC_TR_MNU_Pos) & 0x7F);
}

uint8_t rtc_get_seconds(void)
{
    return bcd_to_bin((RTC->TR >> RTC_TR_SU_Pos) & 0x7F);
}

