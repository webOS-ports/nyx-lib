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
 * @file nyx_battery.h
 *
 * @brief Nyx's public battery API.
 *
 */

#ifndef _NYX_BATTERY_H_
#define _NYX_BATTERY_H_

#include <nyx/common/nyx_battery_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @defgroup nyx_battery_public Battery
* @ingroup nyx_public
* @{
*/

/**
 * @brief Query battery status.
 *
 * @param[in]   handle - the event handle
 * @param[out]  status_out_ptr - current battery readings (needs to be allocated by caller)
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */
NYX_API_EXPORT nyx_error_t nyx_battery_query_battery_status(
    nyx_device_handle_t handle, nyx_battery_status_t *status_out_ptr);


/**
 * @brief Register callback function for any change in battery status.
 *
 * @param[in]  handle - the event handle
 * @param[in]  callback_func - the callback function to handle the event
 * @param[in]  context - for callback function
 *
 * @return error code (NYX_ERROR_NONE if operation is successful
 *
 */
NYX_API_EXPORT nyx_error_t nyx_battery_register_battery_status_callback(
    nyx_device_handle_t handle, nyx_device_callback_function_t callback_func,
    void *context);


/**
 * @brief Authenticate battery.
 *
 * @param[in]   handle - the device handle
 * @param[out]  result - authentication result (true/false).
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_battery_authenticate_battery(
    nyx_device_handle_t handle, bool *result);

/**
 * @brief Get CTIA parameters for the battery.
 *
 * @param[in]   handle - the device handle
 * @param[out]  param  - CTIA parameters
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_battery_get_ctia_parameters(
    nyx_device_handle_t handle, nyx_battery_ctia_t *param);

/**
 * @brief Set battery percentage at which to wake up the device if it suspends.
 *
 * @param[in]   handle - the device handle
 * @param[in]  percentage  - battery percentage at which to wake up
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_battery_set_wakeup_percentage(
    nyx_device_handle_t handle, int32_t percentage);

/**
 * @brief Set fake battery mode.
 *
 * @param[in]  handle - the device handle
 * @param[in]  enable - 1(enable), 0(disable)
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_battery_set_fake_mode(
    nyx_device_handle_t handle, bool enable);

/**
 * @brief Get fake battery mode.
 *
 * @param[in]  handle - the device handle
 * @param[in]  enable - Check sysfs. 1(enable), 0(disable)
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */

NYX_API_EXPORT nyx_error_t nyx_battery_get_fake_mode(
    nyx_device_handle_t handle, bool *enable);

/**
 * @brief Query how many batteries the module reports.
 *
 * Modules that only ever have one battery need not implement this, in which
 * case NYX_ERROR_NOT_IMPLEMENTED is returned and the caller should treat the
 * device as having exactly the one battery that @ref
 * nyx_battery_query_battery_status() describes.
 *
 * The count covers batteries the module knows about, present or not; a
 * detachable one keeps its slot with status.present false so that its
 * disappearance is reported rather than silently dropped.
 *
 * @param[in]   handle - the device handle
 * @param[out]  count_out_ptr - number of batteries (needs to be allocated by caller)
 *
 * @return error code (NYX_ERROR_NONE if operation is successful)
 *
 */
NYX_API_EXPORT nyx_error_t nyx_battery_query_battery_count(
    nyx_device_handle_t handle, int32_t *count_out_ptr);

/**
 * @brief Query identity and readings for one battery.
 *
 * Index 0 is the primary battery and reports what @ref
 * nyx_battery_query_battery_status() reports, so a caller that walks the list
 * need not query both.
 *
 * @param[in]   handle - the device handle
 * @param[in]   index - 0 .. count-1, as reported by @ref nyx_battery_query_battery_count()
 * @param[out]  info_out_ptr - identity and current readings (needs to be allocated by caller)
 *
 * @return error code (NYX_ERROR_NONE if operation is successful,
 *         NYX_ERROR_VALUE_OUT_OF_RANGE if index is past the last battery)
 *
 */
NYX_API_EXPORT nyx_error_t nyx_battery_query_battery_info(
    nyx_device_handle_t handle, int32_t index, nyx_battery_info_t *info_out_ptr);

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* _NYX_BATTERY_H_ */
