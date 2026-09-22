// Copyright (c) 2010-2018 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

/**
 * @file nyx_system.h
 *
 */

/**
 * @brief Nyx's public system API.
 *
 */

#ifndef _NYX_SYSTEM_H_
#define _NYX_SYSTEM_H_

#include <time.h>
#include <nyx/common/nyx_system_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @defgroup nyx_system_public System
* @ingroup nyx_public
* @{
*/

/**
 * @brief Set an RTC alarm.
 *
 * @param[in]  handle - the handle returned from nyx_device_open
 * @param[in]  time - time to set the alarm for (if 0, any existing RTC alarm present will be cleared)
 * @param[in]  callback_func - callback function to be called when any RTC alarm fires
 * @param[in]  context - context passed to the callback function
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_system_set_alarm(nyx_device_handle_t handle,
        time_t time, nyx_device_callback_function_t callback_func, void *context);

/**
 * @brief Query RTC time for next alarm.
 *
 * @param[in]  handle - the handle returned from nyx_device_open
 * @param[out] time - RTC time for next alarm
 *
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_system_query_next_alarm(nyx_device_handle_t
        handle, time_t *time);


/**
 * @brief Query current RTC time.
 *
 * @param[in]  handle - the handle returned from nyx_device_open
 * @param[out] time - RTC time returned by the RTC driver
 *
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_system_query_rtc_time(nyx_device_handle_t handle,
        time_t *time);



/**
 * @brief Suspend the device.
 *
 * @param[in]  handle - the handle returned from nyx_device_open
 * @param[out] success - true if the device suspended and has now resumed,
 *                       false if it never entered suspend (retry later)
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 * Contract (the same for nyx_system_suspend_async):
 *
 *  - The call blocks until the device has resumed again, or until the module
 *    determined that it could not enter suspend. Call it from a thread that
 *    may sleep for the whole suspend period, never from a main loop.
 *  - The module performs a single, one-shot suspend: it reads
 *    /sys/power/wakeup_count, writes the value back and then writes "mem" to
 *    /sys/power/state. It does not retry on its own and it never arms
 *    /sys/power/autosleep. The wait for the kernel's wakeup sources to go
 *    quiet (the wakeup_count read) is bounded to about a second: a source
 *    that stays active longer, a held kernel wakelock for instance, yields
 *    *success == false rather than an indefinitely parked caller, so the
 *    caller's policy runs again before the next attempt.
 *  - *success == false with NYX_ERROR_NONE means the kernel refused to enter
 *    suspend - typically a wakeup source (kernel wakelock, an interrupt that
 *    raced the attempt) - and that the caller should retry later, after its
 *    own idle policy has been satisfied again. It is not an error.
 *  - An error code is returned only for a bad handle or a module that does not
 *    implement suspend (NYX_ERROR_NOT_IMPLEMENTED).
 *
 */

NYX_API_EXPORT nyx_error_t nyx_system_suspend(nyx_device_handle_t handle,
        bool *success);


/**
 * @brief Suspend the device (asynchronous entry point, same contract).
 *
 * @param[in]  handle - the handle returned from nyx_device_open
 * @param[out] success - true if the device suspended and has now resumed,
 *                       false if it never entered suspend (retry later)
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 * Historically distinct from nyx_system_suspend so that a module could arm an
 * autosleep-style mechanism and return immediately, with nyx_system_resume
 * disarming it later. That model is no longer used: every LuneOS module
 * implements this call with exactly the blocking, one-shot body described for
 * nyx_system_suspend, so the call returns only once the device has resumed or
 * failed to enter suspend, and *success == false means "retry later".
 * sleepd calls this entry point; modules register both with the same body.
 *
 */

NYX_API_EXPORT nyx_error_t nyx_system_suspend_async(nyx_device_handle_t handle,
        bool *success);

/**
 * @brief Resume housekeeping after a suspend.
 *
 * @param[in] handle - the handle returned from nyx_device_open
 * @param[out] success - true if the module completed its resume housekeeping
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 * With the blocking suspend contract there is nothing to wake up: the device
 * has already resumed by the time nyx_system_suspend(_async) returns. This
 * call only disarms /sys/power/autosleep (writes "off"), and only where that
 * node exists, so that a device left with autosleep armed by something else
 * can stay awake. It is safe to call at any time.
 */

NYX_API_EXPORT nyx_error_t nyx_system_resume(nyx_device_handle_t handle,
        bool *success);

/**
 * @brief Shut down the device.
 *
 * @param[in] handle - the handle returned from nyx_device_open
 * @param[in] type  - normal or emergency shutdown
 * @param[in] reason - a (possibly NULL) string indicating the reason for the shutdown
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_system_shutdown(nyx_device_handle_t handle,
        nyx_system_shutdown_type_t type, const char *reason);


/**
 * @brief boost up the device for pre-configured duration.
 *
 * @param[in] handle - the handle returned from nyx_device_open
 * @param[in] reason - a (possibly NULL) string indicating the reason for the boost mode
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_system_cpu_boost_mode(nyx_device_handle_t handle,
        const char *reason);

/**
 * @brief return to normal mode.
 *
 * @param[in] handle - the handle returned from nyx_device_open
 * @param[in] reason - a (possibly NULL) string indicating the reason for the normal mode.
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_system_cpu_normal_mode(nyx_device_handle_t
        handle, const char *reason);

/**
 * @brief Reboot the device.
 *
 * @param[in] handle - the event handle
 * @param[in] type  -  normal or emergency reboot
 * @param[in] reason - a (possibly NULL) string indicating the reason for the reboot
 *
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_system_reboot(nyx_device_handle_t handle,
        nyx_system_shutdown_type_t type, const char *reason);

/**
 * @brief Erase a logical partition and restore its original contents upon the next reboot.
 *
 * The actual erasure might not occur until the reboot is performed.
 *
 * @param[in]  handle - the handle returned from nyx_device_open
 * @param[in]  type   - erase type
 *
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_system_erase_partition(nyx_device_handle_t
        handle, nyx_system_erase_type_t type);

/**
 * @brief Load a new QOS profile.
 *
 * @param[in] handle - the device handle
 * @param[in] profile - pointer to nyx_qos_profile_t structure populated with the requested QOS values
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_system_qos_request_profile(
    nyx_device_handle_t handle, nyx_qos_profile_t *profile);


/**
 * @brief Release the constraints in the specified qos profile.
 *
 * @param[in] handle - the device handle
 * @param[in] profile - the same nyx_qos_profile_t pointer passed in the request function
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_system_qos_release_profile(
    nyx_device_handle_t handle, nyx_qos_profile_t *profile);

/**
 * @brief Enable the system tickle mode (set system cpu frequency to max value) for
 * a specified time interval.
 *
 * @param[in] handle - the device handle
 * @param[in] duration - time in msec for tickle mode
 * (if this is set to 0, tickle mode will be set for a default duration)
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_system_qos_tickle(nyx_device_handle_t handle,
        int32_t duration);

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* _NYX_SYSTEM_H_ */
