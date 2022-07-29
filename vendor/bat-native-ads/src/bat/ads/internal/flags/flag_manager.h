/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_FLAGS_FLAG_MANAGER_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_FLAGS_FLAG_MANAGER_H_

#include "bat/ads/internal/flags/environment/environment_types.h"

namespace ads {

class FlagManager final {
 public:
  FlagManager();
  FlagManager(const FlagManager&) = delete;
  FlagManager& operator=(const FlagManager&) = delete;
  ~FlagManager();

  static FlagManager* GetInstance();

  static bool HasInstance();

  bool ShouldDebug() const { return should_debug_; }
  void SetShouldDebugForTesting(const bool should_debug) {
    should_debug_ = should_debug;
  }

  bool DidOverrideVariationsCommandLineSwitches() const {
    return did_override_variations_command_line_switches_;
  }
  void SetDidOverrideVariationsCommandLineSwitchesForTesting(
      const bool did_override_variations_command_line_switches) {
    did_override_variations_command_line_switches_ =
        did_override_variations_command_line_switches;
  }

  EnvironmentType GetEnvironmentType() const { return environment_type_; }
  void SetEnvironmentTypeForTesting(const EnvironmentType environment_type) {
    environment_type_ = environment_type;
  }

 private:
  void Initialize();

  EnvironmentType ChooseEnvironmentType() const;

  bool should_debug_ = false;

  bool did_override_variations_command_line_switches_ = false;

  EnvironmentType environment_type_ = EnvironmentType::kStaging;
};

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_FLAGS_FLAG_MANAGER_H_