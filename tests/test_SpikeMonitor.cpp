#include "SpikeMonitor.h"
#include <gtest/gtest.h>

TEST(SpikeMonitorTest, RecordSingleSpike) {
    SpikeMonitor monitor;
    monitor.on_spike(0.5, 1);

    ASSERT_EQ(monitor.spike_list.size(), 1);
    EXPECT_DOUBLE_EQ(monitor.spike_list[0].first, 0.5);
    EXPECT_EQ(monitor.spike_list[0].second, 1);
}

TEST(SpikeMonitorTest, RecordMultipleSpikesInOrder) {
    SpikeMonitor monitor;
    monitor.on_spike(0.1, 0);
    monitor.on_spike(0.3, 2);
    monitor.on_spike(0.2, 1);

    ASSERT_EQ(monitor.spike_list.size(), 3);
    EXPECT_DOUBLE_EQ(monitor.spike_list[0].first, 0.1);
    EXPECT_EQ(monitor.spike_list[0].second, 0);
    EXPECT_DOUBLE_EQ(monitor.spike_list[1].first, 0.3);
    EXPECT_EQ(monitor.spike_list[1].second, 2);
    EXPECT_DOUBLE_EQ(monitor.spike_list[2].first, 0.2);
    EXPECT_EQ(monitor.spike_list[2].second, 1);
}

TEST(SpikeMonitorTest, ResetSpikes) {
    SpikeMonitor monitor;
    monitor.on_spike(0.5, 1);
    monitor.on_spike(1.0, 2);

    ASSERT_EQ(monitor.spike_list.size(), 2);

    monitor.reset_spikes();
    EXPECT_TRUE(monitor.spike_list.empty());
}