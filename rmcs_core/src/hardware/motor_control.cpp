#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <utility>

#include <rclcpp/logger.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp/node.hpp>
#include <rclcpp/node_options.hpp>

#include <librmcs/board/rmcs_board_lite.hpp>
#include <librmcs/data/datas.hpp>
#include <rmcs_executor/component.hpp>

#include "controller/pid/pid_calculator.hpp"
#include "hardware/device/can_packet.hpp"
#include "hardware/device/dji_motor.hpp"
#include "hardware/device/dr16.hpp"
#include "hardware/device/remote_control.hpp"

namespace rmcs_core::hardware{
    using controller::pid::PidCalculator;
    class Motorcontrol final
    : public rmcs_executor::Component
    , public rclcpp::Node
    , public librmcs::board::RmcsBoardLite::Callback{
   
public:
    class Command : public rmcs_executor::Component {
public: 
    explicit Command(Motorcontrol& o) : owner_(o) {}
    void update() override { owner_.command_update(); }
    private:
    Motorcontrol& owner_;
    };
    Motorcontrol()
        : Node(
        get_component_name(),
        rclcpp::NodeOptions{}.automatically_declare_parameters_from_overrides(true))
        , logger_(get_logger())
        , command_(create_partner_component<Command>(get_component_name() + "_command", *this))
        , motor_(*this, *command_, "/motor")
        , dr16_{} {
            motor_.configure(
            device::DjiMotor::Config{device::DjiMotor::Type::kGM6020, 1}
                .set_reduction_ratio(1.0));
        speed_pid_ = PidCalculator{
            get_parameter("speed_kp").as_double(),
            get_parameter("speed_ki").as_double(),
            get_parameter("speed_kd").as_double()};
        speed_pid_.output_min = -30000;
        speed_pid_.output_max = 30000;

        angle_pid_ = PidCalculator{
            get_parameter("angle_kp").as_double(),
            get_parameter("angle_ki").as_double(),
            get_parameter("angle_kd").as_double()};
        angle_pid_.output_min = -10;
        angle_pid_.output_max = 10;

        register_output("/motor/angle", motor_angle_output_);
        register_output("/motor/velocity", motor_velocity_output_);
        register_output("/motor/command", motor_command_output_);

        board_ = std::make_unique<librmcs::board::RmcsBoardLite>(
            *this, get_parameter("board_serial").as_string());
        remote_control_ = std::make_unique<device::RemoteControl>(*this);
        remote_control_->register_dr16(&dr16_);

        }
    
    private:
    rclcpp::Logger logger_;
    std::unique_ptr<librmcs::board::RmcsBoardLite> board_;
    std::shared_ptr<Command> command_;

    device::DjiMotor motor_;
    device::Dr16 dr16_;
    std::unique_ptr<device::RemoteControl> remote_control_;

    PidCalculator speed_pid_;
    PidCalculator angle_pid_;

    double filtered_velocity_ = 0.0;
    double motor_command_ = 0.0;

    OutputInterface<double> motor_angle_output_;
    OutputInterface<double> motor_velocity_output_;
    OutputInterface<double> motor_command_output_;
    };
}