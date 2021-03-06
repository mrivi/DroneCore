#pragma once

#include <memory>
#include <map>
#include "device_impl.h"
#include "mission.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"

namespace dronecore {

class MissionImpl : public PluginImplBase
{
public:
    MissionImpl();
    ~MissionImpl();

    void init() override;
    void deinit() override;

    void send_mission_async(const std::vector<std::shared_ptr<MissionItem>> &mission_items,
                            const Mission::result_callback_t &callback);

    void start_mission_async(const Mission::result_callback_t &callback);
    void pause_mission_async(const Mission::result_callback_t &callback);

    void set_current_mission_item_async(int current, Mission::result_callback_t &callback);

    bool is_mission_finished() const;

    int current_mission_item() const;
    int total_mission_items() const;

    void subscribe_progress(Mission::progress_callback_t callback);

    // Non-copyable
    MissionImpl(const MissionImpl &) = delete;
    const MissionImpl &operator=(const MissionImpl &) = delete;

private:
    void assemble_mavlink_messages();

    void process_mission_request(const mavlink_message_t &message);
    void process_mission_request_int(const mavlink_message_t &message);
    void process_mission_ack(const mavlink_message_t &message);
    void process_mission_current(const mavlink_message_t &message);
    void process_mission_item_reached(const mavlink_message_t &message);
    void send_mission_item(uint16_t seq);

    void copy_mission_item_vector(const std::vector<std::shared_ptr<MissionItem>> &mission_items);

    static void report_mission_result(const Mission::result_callback_t &callback,
                                      Mission::Result result);

    void report_progress();

    void receive_command_result(MavlinkCommands::Result result,
                                const Mission::result_callback_t &callback);

    Mission::result_callback_t _result_callback;

    enum class Activity {
        NONE,
        SET_CURRENT,
        SET_MISSION,
        SEND_COMMAND
    } _activity = Activity::NONE;

    int _last_current_mavlink_mission_item;
    int _last_reached_mavlink_mission_item;

    std::vector<std::shared_ptr<MissionItem>> _mission_items;
    std::vector<std::shared_ptr<mavlink_message_t>> _mavlink_mission_item_messages;

    std::map<int, int> _mavlink_mission_item_to_mission_item_indices;

    Mission::progress_callback_t _progress_callback;

    static constexpr uint8_t VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED = 1;

    // FIXME: these chould potentially change anytime
    // derived from:
    // https://github.com/PX4/Firmware/blob/master/src/modules/commander/px4_custom_mode.h
    static constexpr uint8_t PX4_CUSTOM_MAIN_MODE_AUTO = 4;
    static constexpr uint8_t PX4_CUSTOM_SUB_MODE_AUTO_LOITER = 3;
    static constexpr uint8_t PX4_CUSTOM_SUB_MODE_AUTO_MISSION = 4;

    void *_timeout_cookie = nullptr;
};

//static std::function<void(MavlinkCommands::Result)> empty_callback;

} // namespace dronecore
