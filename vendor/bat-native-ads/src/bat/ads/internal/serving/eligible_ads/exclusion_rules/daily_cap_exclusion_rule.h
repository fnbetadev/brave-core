/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_SERVING_ELIGIBLE_ADS_EXCLUSION_RULES_DAILY_CAP_EXCLUSION_RULE_H_
#define BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_SERVING_ELIGIBLE_ADS_EXCLUSION_RULES_DAILY_CAP_EXCLUSION_RULE_H_

#include <string>

#include "bat/ads/internal/ad_events/ad_event_info_aliases.h"
#include "bat/ads/internal/creatives/creative_ad_info.h"
#include "bat/ads/internal/serving/eligible_ads/exclusion_rules/exclusion_rule_interface.h"

namespace ads {

class DailyCapExclusionRule final
    : public ExclusionRuleInterface<CreativeAdInfo> {
 public:
  explicit DailyCapExclusionRule(const AdEventList& ad_events);
  ~DailyCapExclusionRule() override;

  DailyCapExclusionRule(const DailyCapExclusionRule&) = delete;
  DailyCapExclusionRule& operator=(const DailyCapExclusionRule&) = delete;

  std::string GetUuid(const CreativeAdInfo& creative_ad) const override;

  bool ShouldExclude(const CreativeAdInfo& creative_ad) override;

  std::string GetLastMessage() const override;

 private:
  bool DoesRespectCap(const AdEventList& ad_events,
                      const CreativeAdInfo& creative_ad);

  AdEventList ad_events_;

  std::string last_message_;
};

}  // namespace ads

#endif  // BRAVE_VENDOR_BAT_NATIVE_ADS_SRC_BAT_ADS_INTERNAL_SERVING_ELIGIBLE_ADS_EXCLUSION_RULES_DAILY_CAP_EXCLUSION_RULE_H_