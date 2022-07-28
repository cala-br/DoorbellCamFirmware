#include "rf.hpp"
#include <log/log.hpp>
#include <common.hpp>
#include <time/udl.hpp>
#include <math.h>

using namespace hal::rf;

constexpr static const wrapper::log::Module mod = wrapper::log::Module::RF;

TxPwm::TxPwm(gpio_num_t pin_num) {
  duty_resolution = LEDC_TIMER_10_BIT;

  ledc_timer_config_t timer_conf_local = {
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .duty_resolution = duty_resolution,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 1_kHz,
    .clk_cfg = LEDC_AUTO_CLK,
  };
  timer_conf = timer_conf_local;
  ledc_timer_config(&timer_conf);

  ledc_channel_config_t channel_conf_local = {
    .gpio_num = pin_num,
    .speed_mode = LEDC_HIGH_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .intr_type = LEDC_INTR_DISABLE,
    .timer_sel = LEDC_TIMER_0,
    .duty = 0,
    .hpoint = 0,
    .flags = { .output_invert = 0 },
  };
  channel_conf = channel_conf_local;
  ledc_channel_config(&channel_conf);
  
  static const wrapper::log::Logger logger = wrapper::log::Logger::getInstance();
  logger.log(mod, ESP_LOG_INFO, "Configured LEDC on gpio %d", pin_num);
}

void TxPwm::setDuty(unsigned long new_duty) const {
  ledc_set_duty(channel_conf.speed_mode, channel_conf.channel, new_duty);
  ledc_update_duty(channel_conf.speed_mode, channel_conf.channel);
}

void TxPwm::setDutyPercentage(float duty_cycle_percentage) {
  duty_cycle_percentage = duty_cycle_percentage > 1.f ? 1.f : duty_cycle_percentage;
  duty_cycle_percentage = duty_cycle_percentage < 0.f ? 0.f : duty_cycle_percentage;
  static const unsigned long max_duty = pow(2, static_cast<int>(duty_resolution)) - 1;
  setDuty(duty_cycle_percentage * max_duty);
}