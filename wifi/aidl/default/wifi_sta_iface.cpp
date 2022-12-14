/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "wifi_sta_iface.h"

#include <android-base/logging.h>

#include "aidl_return_util.h"
#include "aidl_struct_util.h"
#include "wifi_status_util.h"

namespace aidl {
namespace android {
namespace hardware {
namespace wifi {
using aidl_return_util::validateAndCall;

WifiStaIface::WifiStaIface(const std::string& ifname,
                           const std::weak_ptr<legacy_hal::WifiLegacyHal> legacy_hal,
                           const std::weak_ptr<iface_util::WifiIfaceUtil> iface_util)
    : ifname_(ifname), legacy_hal_(legacy_hal), iface_util_(iface_util), is_valid_(true) {
    // Turn on DFS channel usage for STA iface.
    legacy_hal::wifi_error legacy_status = legacy_hal_.lock()->setDfsFlag(ifname_, true);
    if (legacy_status != legacy_hal::WIFI_SUCCESS) {
        LOG(ERROR) << "Failed to set DFS flag; DFS channels may be unavailable.";
    }
}

std::shared_ptr<WifiStaIface> WifiStaIface::create(
        const std::string& ifname, const std::weak_ptr<legacy_hal::WifiLegacyHal> legacy_hal,
        const std::weak_ptr<iface_util::WifiIfaceUtil> iface_util) {
    std::shared_ptr<WifiStaIface> ptr =
            ndk::SharedRefBase::make<WifiStaIface>(ifname, legacy_hal, iface_util);
    std::weak_ptr<WifiStaIface> weak_ptr_this(ptr);
    ptr->setWeakPtr(weak_ptr_this);
    return ptr;
}

void WifiStaIface::invalidate() {
    legacy_hal_.reset();
    event_cb_handler_.invalidate();
    is_valid_ = false;
}

void WifiStaIface::setWeakPtr(std::weak_ptr<WifiStaIface> ptr) {
    weak_ptr_this_ = ptr;
}

bool WifiStaIface::isValid() {
    return is_valid_;
}

std::string WifiStaIface::getName() {
    return ifname_;
}

std::set<std::shared_ptr<IWifiStaIfaceEventCallback>> WifiStaIface::getEventCallbacks() {
    return event_cb_handler_.getCallbacks();
}

ndk::ScopedAStatus WifiStaIface::getName(std::string* _aidl_return) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::getNameInternal, _aidl_return);
}

ndk::ScopedAStatus WifiStaIface::registerEventCallback(
        const std::shared_ptr<IWifiStaIfaceEventCallback>& in_callback) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::registerEventCallbackInternal, in_callback);
}

ndk::ScopedAStatus WifiStaIface::getCapabilities(
        IWifiStaIface::StaIfaceCapabilityMask* _aidl_return) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::getCapabilitiesInternal, _aidl_return);
}

ndk::ScopedAStatus WifiStaIface::getApfPacketFilterCapabilities(
        StaApfPacketFilterCapabilities* _aidl_return) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::getApfPacketFilterCapabilitiesInternal, _aidl_return);
}

ndk::ScopedAStatus WifiStaIface::installApfPacketFilter(const std::vector<uint8_t>& in_program) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::installApfPacketFilterInternal, in_program);
}

ndk::ScopedAStatus WifiStaIface::readApfPacketFilterData(std::vector<uint8_t>* _aidl_return) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::readApfPacketFilterDataInternal, _aidl_return);
}

ndk::ScopedAStatus WifiStaIface::getBackgroundScanCapabilities(
        StaBackgroundScanCapabilities* _aidl_return) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::getBackgroundScanCapabilitiesInternal, _aidl_return);
}

ndk::ScopedAStatus WifiStaIface::getValidFrequenciesForBand(WifiBand in_band,
                                                            std::vector<int32_t>* _aidl_return) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::getValidFrequenciesForBandInternal, _aidl_return,
                           in_band);
}

ndk::ScopedAStatus WifiStaIface::startBackgroundScan(int32_t in_cmdId,
                                                     const StaBackgroundScanParameters& in_params) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::startBackgroundScanInternal, in_cmdId, in_params);
}

ndk::ScopedAStatus WifiStaIface::stopBackgroundScan(int32_t in_cmdId) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::stopBackgroundScanInternal, in_cmdId);
}

ndk::ScopedAStatus WifiStaIface::enableLinkLayerStatsCollection(bool in_debug) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::enableLinkLayerStatsCollectionInternal, in_debug);
}

ndk::ScopedAStatus WifiStaIface::disableLinkLayerStatsCollection() {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::disableLinkLayerStatsCollectionInternal);
}

ndk::ScopedAStatus WifiStaIface::getLinkLayerStats(StaLinkLayerStats* _aidl_return) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::getLinkLayerStatsInternal, _aidl_return);
}

ndk::ScopedAStatus WifiStaIface::startRssiMonitoring(int32_t in_cmdId, int32_t in_maxRssi,
                                                     int32_t in_minRssi) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::startRssiMonitoringInternal, in_cmdId, in_maxRssi,
                           in_minRssi);
}

ndk::ScopedAStatus WifiStaIface::stopRssiMonitoring(int32_t in_cmdId) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::stopRssiMonitoringInternal, in_cmdId);
}

ndk::ScopedAStatus WifiStaIface::getRoamingCapabilities(StaRoamingCapabilities* _aidl_return) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::getRoamingCapabilitiesInternal, _aidl_return);
}

ndk::ScopedAStatus WifiStaIface::configureRoaming(const StaRoamingConfig& in_config) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::configureRoamingInternal, in_config);
}

ndk::ScopedAStatus WifiStaIface::setRoamingState(StaRoamingState in_state) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::setRoamingStateInternal, in_state);
}

ndk::ScopedAStatus WifiStaIface::enableNdOffload(bool in_enable) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::enableNdOffloadInternal, in_enable);
}

ndk::ScopedAStatus WifiStaIface::startSendingKeepAlivePackets(
        int32_t in_cmdId, const std::vector<uint8_t>& in_ipPacketData, char16_t in_etherType,
        const std::array<uint8_t, 6>& in_srcAddress, const std::array<uint8_t, 6>& in_dstAddress,
        int32_t in_periodInMs) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::startSendingKeepAlivePacketsInternal, in_cmdId,
                           in_ipPacketData, in_etherType, in_srcAddress, in_dstAddress,
                           in_periodInMs);
}

ndk::ScopedAStatus WifiStaIface::stopSendingKeepAlivePackets(int32_t in_cmdId) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::stopSendingKeepAlivePacketsInternal, in_cmdId);
}

ndk::ScopedAStatus WifiStaIface::startDebugPacketFateMonitoring() {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::startDebugPacketFateMonitoringInternal);
}

ndk::ScopedAStatus WifiStaIface::getDebugTxPacketFates(
        std::vector<WifiDebugTxPacketFateReport>* _aidl_return) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::getDebugTxPacketFatesInternal, _aidl_return);
}

ndk::ScopedAStatus WifiStaIface::getDebugRxPacketFates(
        std::vector<WifiDebugRxPacketFateReport>* _aidl_return) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::getDebugRxPacketFatesInternal, _aidl_return);
}

ndk::ScopedAStatus WifiStaIface::setMacAddress(const std::array<uint8_t, 6>& in_mac) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::setMacAddressInternal, in_mac);
}

ndk::ScopedAStatus WifiStaIface::getFactoryMacAddress(std::array<uint8_t, 6>* _aidl_return) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::getFactoryMacAddressInternal, _aidl_return);
}

ndk::ScopedAStatus WifiStaIface::setScanMode(bool in_enable) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::setScanModeInternal, in_enable);
}

ndk::ScopedAStatus WifiStaIface::setDtimMultiplier(int32_t in_multiplier) {
    return validateAndCall(this, WifiStatusCode::ERROR_WIFI_IFACE_INVALID,
                           &WifiStaIface::setDtimMultiplierInternal, in_multiplier);
}

std::pair<std::string, ndk::ScopedAStatus> WifiStaIface::getNameInternal() {
    return {ifname_, ndk::ScopedAStatus::ok()};
}

ndk::ScopedAStatus WifiStaIface::registerEventCallbackInternal(
        const std::shared_ptr<IWifiStaIfaceEventCallback>& callback) {
    if (!event_cb_handler_.addCallback(callback)) {
        return createWifiStatus(WifiStatusCode::ERROR_UNKNOWN);
    }
    return ndk::ScopedAStatus::ok();
}

std::pair<IWifiStaIface::StaIfaceCapabilityMask, ndk::ScopedAStatus>
WifiStaIface::getCapabilitiesInternal() {
    legacy_hal::wifi_error legacy_status;
    uint64_t legacy_feature_set;
    std::tie(legacy_status, legacy_feature_set) =
            legacy_hal_.lock()->getSupportedFeatureSet(ifname_);
    if (legacy_status != legacy_hal::WIFI_SUCCESS) {
        return {IWifiStaIface::StaIfaceCapabilityMask{},
                createWifiStatusFromLegacyError(legacy_status)};
    }
    uint32_t legacy_logger_feature_set;
    std::tie(legacy_status, legacy_logger_feature_set) =
            legacy_hal_.lock()->getLoggerSupportedFeatureSet(ifname_);
    if (legacy_status != legacy_hal::WIFI_SUCCESS) {
        // some devices don't support querying logger feature set
        legacy_logger_feature_set = 0;
    }
    uint32_t aidl_caps;
    if (!aidl_struct_util::convertLegacyFeaturesToAidlStaCapabilities(
                legacy_feature_set, legacy_logger_feature_set, &aidl_caps)) {
        return {IWifiStaIface::StaIfaceCapabilityMask{},
                createWifiStatus(WifiStatusCode::ERROR_UNKNOWN)};
    }
    return {static_cast<IWifiStaIface::StaIfaceCapabilityMask>(aidl_caps),
            ndk::ScopedAStatus::ok()};
}

std::pair<StaApfPacketFilterCapabilities, ndk::ScopedAStatus>
WifiStaIface::getApfPacketFilterCapabilitiesInternal() {
    legacy_hal::wifi_error legacy_status;
    legacy_hal::PacketFilterCapabilities legacy_caps;
    std::tie(legacy_status, legacy_caps) = legacy_hal_.lock()->getPacketFilterCapabilities(ifname_);
    if (legacy_status != legacy_hal::WIFI_SUCCESS) {
        return {StaApfPacketFilterCapabilities{}, createWifiStatusFromLegacyError(legacy_status)};
    }
    StaApfPacketFilterCapabilities aidl_caps;
    if (!aidl_struct_util::convertLegacyApfCapabilitiesToAidl(legacy_caps, &aidl_caps)) {
        return {StaApfPacketFilterCapabilities{}, createWifiStatus(WifiStatusCode::ERROR_UNKNOWN)};
    }
    return {aidl_caps, ndk::ScopedAStatus::ok()};
}

ndk::ScopedAStatus WifiStaIface::installApfPacketFilterInternal(
        const std::vector<uint8_t>& program) {
    legacy_hal::wifi_error legacy_status = legacy_hal_.lock()->setPacketFilter(ifname_, program);
    return createWifiStatusFromLegacyError(legacy_status);
}

std::pair<std::vector<uint8_t>, ndk::ScopedAStatus>
WifiStaIface::readApfPacketFilterDataInternal() {
    const std::pair<legacy_hal::wifi_error, std::vector<uint8_t>> legacy_status_and_data =
            legacy_hal_.lock()->readApfPacketFilterData(ifname_);
    return {std::move(legacy_status_and_data.second),
            createWifiStatusFromLegacyError(legacy_status_and_data.first)};
}

std::pair<StaBackgroundScanCapabilities, ndk::ScopedAStatus>
WifiStaIface::getBackgroundScanCapabilitiesInternal() {
    legacy_hal::wifi_error legacy_status;
    legacy_hal::wifi_gscan_capabilities legacy_caps;
    std::tie(legacy_status, legacy_caps) = legacy_hal_.lock()->getGscanCapabilities(ifname_);
    if (legacy_status != legacy_hal::WIFI_SUCCESS) {
        return {StaBackgroundScanCapabilities{}, createWifiStatusFromLegacyError(legacy_status)};
    }
    StaBackgroundScanCapabilities aidl_caps;
    if (!aidl_struct_util::convertLegacyGscanCapabilitiesToAidl(legacy_caps, &aidl_caps)) {
        return {StaBackgroundScanCapabilities{}, createWifiStatus(WifiStatusCode::ERROR_UNKNOWN)};
    }
    return {aidl_caps, ndk::ScopedAStatus::ok()};
}

std::pair<std::vector<int32_t>, ndk::ScopedAStatus>
WifiStaIface::getValidFrequenciesForBandInternal(WifiBand band) {
    static_assert(sizeof(WifiChannelWidthInMhz) == sizeof(int32_t), "Size mismatch");
    legacy_hal::wifi_error legacy_status;
    std::vector<uint32_t> valid_frequencies;
    std::tie(legacy_status, valid_frequencies) = legacy_hal_.lock()->getValidFrequenciesForBand(
            ifname_, aidl_struct_util::convertAidlWifiBandToLegacy(band));
    return {std::vector<int32_t>(valid_frequencies.begin(), valid_frequencies.end()),
            createWifiStatusFromLegacyError(legacy_status)};
}

ndk::ScopedAStatus WifiStaIface::startBackgroundScanInternal(
        int32_t cmd_id, const StaBackgroundScanParameters& params) {
    legacy_hal::wifi_scan_cmd_params legacy_params;
    if (!aidl_struct_util::convertAidlGscanParamsToLegacy(params, &legacy_params)) {
        return createWifiStatus(WifiStatusCode::ERROR_INVALID_ARGS);
    }
    std::weak_ptr<WifiStaIface> weak_ptr_this = weak_ptr_this_;
    const auto& on_failure_callback = [weak_ptr_this](legacy_hal::wifi_request_id id) {
        const auto shared_ptr_this = weak_ptr_this.lock();
        if (!shared_ptr_this.get() || !shared_ptr_this->isValid()) {
            LOG(ERROR) << "Callback invoked on an invalid object";
            return;
        }
        for (const auto& callback : shared_ptr_this->getEventCallbacks()) {
            if (!callback->onBackgroundScanFailure(id).isOk()) {
                LOG(ERROR) << "Failed to invoke onBackgroundScanFailure callback";
            }
        }
    };
    const auto& on_results_callback =
            [weak_ptr_this](legacy_hal::wifi_request_id id,
                            const std::vector<legacy_hal::wifi_cached_scan_results>& results) {
                const auto shared_ptr_this = weak_ptr_this.lock();
                if (!shared_ptr_this.get() || !shared_ptr_this->isValid()) {
                    LOG(ERROR) << "Callback invoked on an invalid object";
                    return;
                }
                std::vector<StaScanData> aidl_scan_datas;
                if (!aidl_struct_util::convertLegacyVectorOfCachedGscanResultsToAidl(
                            results, &aidl_scan_datas)) {
                    LOG(ERROR) << "Failed to convert scan results to AIDL structs";
                    return;
                }
                for (const auto& callback : shared_ptr_this->getEventCallbacks()) {
                    if (!callback->onBackgroundScanResults(id, aidl_scan_datas).isOk()) {
                        LOG(ERROR) << "Failed to invoke onBackgroundScanResults callback";
                    }
                }
            };
    const auto& on_full_result_callback = [weak_ptr_this](
                                                  legacy_hal::wifi_request_id id,
                                                  const legacy_hal::wifi_scan_result* result,
                                                  uint32_t buckets_scanned) {
        const auto shared_ptr_this = weak_ptr_this.lock();
        if (!shared_ptr_this.get() || !shared_ptr_this->isValid()) {
            LOG(ERROR) << "Callback invoked on an invalid object";
            return;
        }
        StaScanResult aidl_scan_result;
        if (!aidl_struct_util::convertLegacyGscanResultToAidl(*result, true, &aidl_scan_result)) {
            LOG(ERROR) << "Failed to convert full scan results to AIDL structs";
            return;
        }
        for (const auto& callback : shared_ptr_this->getEventCallbacks()) {
            if (!callback->onBackgroundFullScanResult(id, buckets_scanned, aidl_scan_result)
                         .isOk()) {
                LOG(ERROR) << "Failed to invoke onBackgroundFullScanResult callback";
            }
        }
    };
    legacy_hal::wifi_error legacy_status =
            legacy_hal_.lock()->startGscan(ifname_, cmd_id, legacy_params, on_failure_callback,
                                           on_results_callback, on_full_result_callback);
    return createWifiStatusFromLegacyError(legacy_status);
}

ndk::ScopedAStatus WifiStaIface::stopBackgroundScanInternal(int32_t cmd_id) {
    legacy_hal::wifi_error legacy_status = legacy_hal_.lock()->stopGscan(ifname_, cmd_id);
    return createWifiStatusFromLegacyError(legacy_status);
}

ndk::ScopedAStatus WifiStaIface::enableLinkLayerStatsCollectionInternal(bool debug) {
    legacy_hal::wifi_error legacy_status = legacy_hal_.lock()->enableLinkLayerStats(ifname_, debug);
    return createWifiStatusFromLegacyError(legacy_status);
}

ndk::ScopedAStatus WifiStaIface::disableLinkLayerStatsCollectionInternal() {
    legacy_hal::wifi_error legacy_status = legacy_hal_.lock()->disableLinkLayerStats(ifname_);
    return createWifiStatusFromLegacyError(legacy_status);
}

std::pair<StaLinkLayerStats, ndk::ScopedAStatus> WifiStaIface::getLinkLayerStatsInternal() {
    legacy_hal::wifi_error legacy_status;
    legacy_hal::LinkLayerStats legacy_stats;
    std::tie(legacy_status, legacy_stats) = legacy_hal_.lock()->getLinkLayerStats(ifname_);
    if (legacy_status != legacy_hal::WIFI_SUCCESS) {
        return {StaLinkLayerStats{}, createWifiStatusFromLegacyError(legacy_status)};
    }
    StaLinkLayerStats aidl_stats;
    if (!aidl_struct_util::convertLegacyLinkLayerStatsToAidl(legacy_stats, &aidl_stats)) {
        return {StaLinkLayerStats{}, createWifiStatus(WifiStatusCode::ERROR_UNKNOWN)};
    }
    return {aidl_stats, ndk::ScopedAStatus::ok()};
}

ndk::ScopedAStatus WifiStaIface::startRssiMonitoringInternal(int32_t cmd_id, int32_t max_rssi,
                                                             int32_t min_rssi) {
    std::weak_ptr<WifiStaIface> weak_ptr_this = weak_ptr_this_;
    const auto& on_threshold_breached_callback =
            [weak_ptr_this](legacy_hal::wifi_request_id id, std::array<uint8_t, ETH_ALEN> bssid,
                            int8_t rssi) {
                const auto shared_ptr_this = weak_ptr_this.lock();
                if (!shared_ptr_this.get() || !shared_ptr_this->isValid()) {
                    LOG(ERROR) << "Callback invoked on an invalid object";
                    return;
                }
                for (const auto& callback : shared_ptr_this->getEventCallbacks()) {
                    if (!callback->onRssiThresholdBreached(id, bssid, rssi).isOk()) {
                        LOG(ERROR) << "Failed to invoke onRssiThresholdBreached callback";
                    }
                }
            };
    legacy_hal::wifi_error legacy_status = legacy_hal_.lock()->startRssiMonitoring(
            ifname_, cmd_id, max_rssi, min_rssi, on_threshold_breached_callback);
    return createWifiStatusFromLegacyError(legacy_status);
}

ndk::ScopedAStatus WifiStaIface::stopRssiMonitoringInternal(int32_t cmd_id) {
    legacy_hal::wifi_error legacy_status = legacy_hal_.lock()->stopRssiMonitoring(ifname_, cmd_id);
    return createWifiStatusFromLegacyError(legacy_status);
}

std::pair<StaRoamingCapabilities, ndk::ScopedAStatus>
WifiStaIface::getRoamingCapabilitiesInternal() {
    legacy_hal::wifi_error legacy_status;
    legacy_hal::wifi_roaming_capabilities legacy_caps;
    std::tie(legacy_status, legacy_caps) = legacy_hal_.lock()->getRoamingCapabilities(ifname_);
    if (legacy_status != legacy_hal::WIFI_SUCCESS) {
        return {StaRoamingCapabilities{}, createWifiStatusFromLegacyError(legacy_status)};
    }
    StaRoamingCapabilities aidl_caps;
    if (!aidl_struct_util::convertLegacyRoamingCapabilitiesToAidl(legacy_caps, &aidl_caps)) {
        return {StaRoamingCapabilities{}, createWifiStatus(WifiStatusCode::ERROR_UNKNOWN)};
    }
    return {aidl_caps, ndk::ScopedAStatus::ok()};
}

ndk::ScopedAStatus WifiStaIface::configureRoamingInternal(const StaRoamingConfig& config) {
    legacy_hal::wifi_roaming_config legacy_config;
    if (!aidl_struct_util::convertAidlRoamingConfigToLegacy(config, &legacy_config)) {
        return createWifiStatus(WifiStatusCode::ERROR_INVALID_ARGS);
    }
    legacy_hal::wifi_error legacy_status =
            legacy_hal_.lock()->configureRoaming(ifname_, legacy_config);
    return createWifiStatusFromLegacyError(legacy_status);
}

ndk::ScopedAStatus WifiStaIface::setRoamingStateInternal(StaRoamingState state) {
    legacy_hal::wifi_error legacy_status = legacy_hal_.lock()->enableFirmwareRoaming(
            ifname_, aidl_struct_util::convertAidlRoamingStateToLegacy(state));
    return createWifiStatusFromLegacyError(legacy_status);
}

ndk::ScopedAStatus WifiStaIface::enableNdOffloadInternal(bool enable) {
    legacy_hal::wifi_error legacy_status = legacy_hal_.lock()->configureNdOffload(ifname_, enable);
    return createWifiStatusFromLegacyError(legacy_status);
}

ndk::ScopedAStatus WifiStaIface::startSendingKeepAlivePacketsInternal(
        int32_t cmd_id, const std::vector<uint8_t>& ip_packet_data, char16_t ether_type,
        const std::array<uint8_t, 6>& src_address, const std::array<uint8_t, 6>& dst_address,
        int32_t period_in_ms) {
    legacy_hal::wifi_error legacy_status = legacy_hal_.lock()->startSendingOffloadedPacket(
            ifname_, cmd_id, ether_type, ip_packet_data, src_address, dst_address, period_in_ms);
    return createWifiStatusFromLegacyError(legacy_status);
}

ndk::ScopedAStatus WifiStaIface::stopSendingKeepAlivePacketsInternal(int32_t cmd_id) {
    legacy_hal::wifi_error legacy_status =
            legacy_hal_.lock()->stopSendingOffloadedPacket(ifname_, cmd_id);
    return createWifiStatusFromLegacyError(legacy_status);
}

ndk::ScopedAStatus WifiStaIface::startDebugPacketFateMonitoringInternal() {
    legacy_hal::wifi_error legacy_status = legacy_hal_.lock()->startPktFateMonitoring(ifname_);
    return createWifiStatusFromLegacyError(legacy_status);
}

std::pair<std::vector<WifiDebugTxPacketFateReport>, ndk::ScopedAStatus>
WifiStaIface::getDebugTxPacketFatesInternal() {
    legacy_hal::wifi_error legacy_status;
    std::vector<legacy_hal::wifi_tx_report> legacy_fates;
    std::tie(legacy_status, legacy_fates) = legacy_hal_.lock()->getTxPktFates(ifname_);
    if (legacy_status != legacy_hal::WIFI_SUCCESS) {
        return {std::vector<WifiDebugTxPacketFateReport>(),
                createWifiStatusFromLegacyError(legacy_status)};
    }
    std::vector<WifiDebugTxPacketFateReport> aidl_fates;
    if (!aidl_struct_util::convertLegacyVectorOfDebugTxPacketFateToAidl(legacy_fates,
                                                                        &aidl_fates)) {
        return {std::vector<WifiDebugTxPacketFateReport>(),
                createWifiStatus(WifiStatusCode::ERROR_UNKNOWN)};
    }
    return {aidl_fates, ndk::ScopedAStatus::ok()};
}

std::pair<std::vector<WifiDebugRxPacketFateReport>, ndk::ScopedAStatus>
WifiStaIface::getDebugRxPacketFatesInternal() {
    legacy_hal::wifi_error legacy_status;
    std::vector<legacy_hal::wifi_rx_report> legacy_fates;
    std::tie(legacy_status, legacy_fates) = legacy_hal_.lock()->getRxPktFates(ifname_);
    if (legacy_status != legacy_hal::WIFI_SUCCESS) {
        return {std::vector<WifiDebugRxPacketFateReport>(),
                createWifiStatusFromLegacyError(legacy_status)};
    }
    std::vector<WifiDebugRxPacketFateReport> aidl_fates;
    if (!aidl_struct_util::convertLegacyVectorOfDebugRxPacketFateToAidl(legacy_fates,
                                                                        &aidl_fates)) {
        return {std::vector<WifiDebugRxPacketFateReport>(),
                createWifiStatus(WifiStatusCode::ERROR_UNKNOWN)};
    }
    return {aidl_fates, ndk::ScopedAStatus::ok()};
}

ndk::ScopedAStatus WifiStaIface::setMacAddressInternal(const std::array<uint8_t, 6>& mac) {
    bool status = iface_util_.lock()->setMacAddress(ifname_, mac);
    if (!status) {
        return createWifiStatus(WifiStatusCode::ERROR_UNKNOWN);
    }
    return ndk::ScopedAStatus::ok();
}

std::pair<std::array<uint8_t, 6>, ndk::ScopedAStatus> WifiStaIface::getFactoryMacAddressInternal() {
    std::array<uint8_t, 6> mac = iface_util_.lock()->getFactoryMacAddress(ifname_);
    if (mac[0] == 0 && mac[1] == 0 && mac[2] == 0 && mac[3] == 0 && mac[4] == 0 && mac[5] == 0) {
        return {mac, createWifiStatus(WifiStatusCode::ERROR_UNKNOWN)};
    }
    return {mac, ndk::ScopedAStatus::ok()};
}

ndk::ScopedAStatus WifiStaIface::setScanModeInternal(bool enable) {
    // OEM's need to implement this on their devices if needed.
    LOG(WARNING) << "setScanModeInternal(" << enable << ") not supported";
    return createWifiStatus(WifiStatusCode::ERROR_NOT_SUPPORTED);
}

ndk::ScopedAStatus WifiStaIface::setDtimMultiplierInternal(const int multiplier) {
    legacy_hal::wifi_error legacy_status = legacy_hal_.lock()->setDtimConfig(ifname_, multiplier);
    return createWifiStatusFromLegacyError(legacy_status);
}

}  // namespace wifi
}  // namespace hardware
}  // namespace android
}  // namespace aidl
