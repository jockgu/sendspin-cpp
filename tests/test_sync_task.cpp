// Copyright 2026 Sendspin Contributors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "player_role_impl.h"

#include <gtest/gtest.h>

#include <vector>

namespace sendspin {
namespace {

class TestSyncTask final : public SyncTask {
public:
    using SyncTask::apply_stream_clear;
};

class RecordingListener final : public PlayerRoleListener {
public:
    size_t on_audio_write(uint8_t*, size_t length, uint32_t) override {
        events.push_back('W');
        return length;
    }

    void on_stream_clear() override {
        events.push_back('C');
    }

    std::vector<char> events;
};

TEST(SyncTask, StreamClearCallsListenerBetweenAudioWrites) {
    PlayerRole::Impl player(PlayerRoleConfig{}, nullptr, nullptr);
    RecordingListener listener;
    player.listener = &listener;

    TestSyncTask task;
    ASSERT_TRUE(task.init(&player, nullptr, 1024));
    SyncContext context;

    listener.on_audio_write(nullptr, 0, 0);
    task.apply_stream_clear(context);
    listener.on_audio_write(nullptr, 0, 0);

    EXPECT_EQ(listener.events, (std::vector<char>{'W', 'C', 'W'}));
}

}  // namespace
}  // namespace sendspin
