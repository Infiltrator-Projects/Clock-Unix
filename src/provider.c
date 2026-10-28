// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Independent fixed-unit clock resource. Compiled separately for five stable
 * time-system IDs; the implementation contains no Calendar or Common clock
 * formatting dependency. Common supplies the ABI and validates callbacks.
 */
#include <infiltratr/temporal_clock_resource.h>
#include <infiltratr/arithmetic.h>
#include <infiltratr/temporal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CLOCK_MICROSECONDS_PER_SECOND INT64_C(1000000)
#define CLOCK_MICROSECONDS_PER_DAY INT64_C(86400000000)

static uint64_t ticks_at(uint64_t phase, uint64_t per_day)
{
    return phase * per_day / (uint64_t)CLOCK_MICROSECONDS_PER_DAY;
}

static bool clock_render(const InfiltratrTemporalClockProvider *provider,
                         const char *locale, int64_t instant, int32_t offset,
                         bool seconds, bool vertical, double latitude, double longitude,
                         char *buffer, size_t capacity, size_t *length)
{
    char text[128] = {0};
    int written = -1;
    uint64_t tick;
    int64_t unix_seconds = 0;
    const char *sep = vertical ? "\n" : ":";

    (void)provider; (void)locale; (void)latitude; (void)longitude;
    (void)instant; (void)offset; (void)seconds; (void)vertical;
    (void)tick; (void)sep;
    if (!infiltratr_i64_floor_divmod(instant, CLOCK_MICROSECONDS_PER_SECOND,
                                    &unix_seconds, NULL)) return false;
    written = snprintf(text, sizeof(text), "%lld",
        (long long)unix_seconds);
    if (buffer == NULL || written <= 0 || (size_t)written >= sizeof(text) ||
        (size_t)written >= capacity) return false;
    memcpy(buffer, text, (size_t)written + 1U);
    if (length != NULL) *length = (size_t)written;
    return true;
}

static bool clock_next_tick(const InfiltratrTemporalClockProvider *provider,
                            int64_t instant, int32_t offset, bool seconds,
                            double latitude, double longitude, uint32_t *milliseconds)
{
    uint64_t phase, units, tick, boundary, remaining, result;
    (void)provider; (void)latitude; (void)longitude; (void)offset; (void)seconds;
    if (milliseconds == NULL) return false;
    units = UINT64_C(86400);
    phase = (uint64_t)infiltratr_temporal_local_microseconds_of_day(instant, 0);
    tick = ticks_at(phase, units);
    boundary = ((tick + 1U) * (uint64_t)CLOCK_MICROSECONDS_PER_DAY +
                 units - 1U) / units;
    remaining = boundary > phase ? boundary - phase : 1U;
    result = (remaining + UINT64_C(999)) / UINT64_C(1000);
    if (result == 0U || result > UINT32_MAX) return false;
    *milliseconds = (uint32_t)result;
    return true;
}

static const InfiltratrTemporalResourceMetadata metadata = {
    sizeof(InfiltratrTemporalResourceMetadata),
    INFILTRATR_TEMPORAL_RESOURCE_METADATA_ABI,
    INFILTRATR_TEMPORAL_RESOURCE_CLOCK,
    CLOCK_RESOURCE_ID, INFILTRATR_TEMPORAL_DISPLAY_NAME,
    CLOCK_RESOURCE_SECONDS, false, false
};
static const InfiltratrTemporalClockProvider provider = {
    sizeof(InfiltratrTemporalClockProvider),
    INFILTRATR_TEMPORAL_CLOCK_PROVIDER_ABI,
    CLOCK_RESOURCE_ID, clock_render, clock_next_tick
};
#if defined(_WIN32)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif
EXPORT const InfiltratrTemporalResourceMetadata *
infiltratr_temporal_resource_metadata_v1(void);
EXPORT const InfiltratrTemporalClockProvider *
infiltratr_temporal_clock_provider_v1(void);
EXPORT const InfiltratrTemporalResourceMetadata *
infiltratr_temporal_resource_metadata_v1(void) { return &metadata; }
EXPORT const InfiltratrTemporalClockProvider *
infiltratr_temporal_clock_provider_v1(void) { return &provider; }
