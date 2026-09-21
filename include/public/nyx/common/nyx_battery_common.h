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
 * @file nyx_battery_common.h
 *
 */


#ifndef _NYX_BATTERY_COMMON_H_
#define _NYX_BATTERY_COMMON_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
* @defgroup nyx_battery_public Battery
* @ingroup nyx_public
* @brief Nyx's public battery API.
* @{
*/

/**
 * The condition a battery's own driver reports it to be in.
 *
 * This is the driver's verdict rather than anything worked out from the
 * readings, and mirrors the kernel's POWER_SUPPLY_HEALTH_* values one for
 * one, so a module has only to map the string in the "health" sysfs
 * attribute. It is a different question from how worn out the pack is -
 * see @ref nyx_battery_status_t::capacity_full_design for that - and a pack
 * can be NYX_BATTERY_HEALTH_GOOD while holding half what it once did.
 *
 * NYX_BATTERY_HEALTH_UNKNOWN is zero, so a module that does not answer this
 * question, or a driver with no "health" attribute, leaves it unknown rather
 * than claiming the battery is fine.
 */
typedef enum
{
	NYX_BATTERY_HEALTH_UNKNOWN = 0,
	NYX_BATTERY_HEALTH_GOOD,
	NYX_BATTERY_HEALTH_OVERHEAT,
	NYX_BATTERY_HEALTH_DEAD,
	NYX_BATTERY_HEALTH_OVERVOLTAGE,
	NYX_BATTERY_HEALTH_UNSPEC_FAILURE,
	NYX_BATTERY_HEALTH_COLD,
	NYX_BATTERY_HEALTH_WATCHDOG_TIMER_EXPIRE,
	NYX_BATTERY_HEALTH_SAFETY_TIMER_EXPIRE,
	NYX_BATTERY_HEALTH_OVERCURRENT,
	NYX_BATTERY_HEALTH_CALIBRATION_REQUIRED,
	NYX_BATTERY_HEALTH_WARM,
	NYX_BATTERY_HEALTH_COOL,
	NYX_BATTERY_HEALTH_HOT,
	NYX_BATTERY_HEALTH_NO_BATTERY,
} nyx_battery_health_t;

/**
 * Struct to get current battery readings
 */
typedef struct
{
	bool present;       /** True if the battery is present */
	bool charging;      /** True if the battery is being charged currently */
	int32_t percentage; /** Battery capacity in percentage */
	int32_t temperature;    /** In celsius */
	int32_t current;    /** In mA */
	int32_t voltage;    /** In mV */
	float capacity;     /** In mAh*/
	int32_t avg_current;    /** In mA */
	float capacity_raw; /** In mAh*/
	float capacity_full40;  /** In mAh*/
	int32_t age;
	int32_t health;     /** One of @ref nyx_battery_health_t */

	/**
	 * What the pack held when it left the factory, in mAh, against which
	 * @ref capacity_full40 is what it holds now. The two together are the
	 * usual state-of-health figure; on their own neither says anything
	 * about wear, because a 3000 mAh reading means nothing without knowing
	 * whether the pack started at 3000 or at 4000.
	 *
	 * -1 where the driver does not report it, and equal to capacity_full40
	 * on a gauge that does no capacity learning and simply repeats the
	 * design figure - which is not the same as a pack in perfect health,
	 * and is worth telling apart before showing anyone a percentage.
	 */
	float capacity_full_design;
} nyx_battery_status_t;

/**
 * Maximum length, including the terminating NUL, of the identifying strings
 * carried by @ref nyx_battery_info_t.
 */
#define NYX_BATTERY_NAME_MAX 64

/**
 * Identity and readings for one battery out of possibly several.
 *
 * A device can carry more than one cell: the PinePhone (Pro) keyboard has its
 * own battery, which charges the phone over USB and appears alongside the
 * phone's own as a second power_supply of type "Battery". @ref
 * nyx_battery_query_battery_status() keeps reporting the primary battery, so
 * charging logic and low-battery shutdown are unaffected; the extra cells are
 * reached with @ref nyx_battery_query_battery_count() and @ref
 * nyx_battery_query_battery_info().
 */
typedef struct
{
	char name[NYX_BATTERY_NAME_MAX];    /** power_supply node name, e.g. "ip5xxx-battery" */
	char role[NYX_BATTERY_NAME_MAX];    /** what it powers: "main", "keyboard", ... */
	bool primary;                       /** True for the battery the charging logic follows */
	nyx_battery_status_t status;        /** Readings, as nyx_battery_query_battery_status() reports them */
} nyx_battery_info_t;

/**
 * Battery Charging Parameters
 */
typedef struct
{
	int32_t charge_min_temp_c;      /** Temperature below which charging is turned off */
	int32_t charge_max_temp_c;      /** Temperature above which charging is turned off */
	int32_t battery_crit_max_temp;      /** Temperature above which device is shut down */
	bool skip_battery_authentication;   /** Is battery authentication required (True/False) */
} nyx_battery_ctia_t;

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* _NYX_BATTERY_COMMON_H_ */
