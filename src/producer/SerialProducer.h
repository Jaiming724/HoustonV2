// Ensure you have defined ASIO_STANDALONE if your build system doesn't do it automatically
// #define ASIO_STANDALONE 
#include "asio.hpp"
#include <thread>
#include <mutex>
#include <deque>
#include <vector>
#include <string>
#include <iostream>
#include "../Dispatcher.h"
#include "DataProducer.h"


class SerialProducer : public DataProducer {

public:
    SerialProducer() : DataProducer("SerialProducer"), io_context_(), serial_(io_context_),
                       work_guard_(asio::make_work_guard(io_context_)) {

    }

    void setPort(const std::string &port_name) {
        this->portName = port_name;
    }

    bool start() override {
        if (portName.empty()) {
            std::cerr << "SerialProducer: port name is not set." << std::endl;
            return false;
        }
        std::error_code ec;
        serial_.open(portName, ec);

        if (ec) {
            std::cerr << "SerialProducer: failed to open " << portName << ": " << ec.message() << std::endl;
            return false;
        }

        using namespace asio;
        serial_.set_option(serial_port_base::baud_rate(115200));
        serial_.set_option(serial_port_base::character_size(8));
        serial_.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
        serial_.set_option(serial_port_base::parity(serial_port_base::parity::none));
        serial_.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));

        start_read();

        background_thread_ = std::thread([this]() {
            io_context_.run();
        });
        status = true;
        return true;
    }

    void stop() override {
        io_context_.stop();
        if (background_thread_.joinable()) {
            background_thread_.join();
        }
        status = false;
    }

    ~SerialProducer() {
        stop();
    }

    void produce(Dispatcher &dispatcher) {
        std::deque<std::vector<uint8_t>> batch;

        {
            std::lock_guard<std::mutex> lock(rx_mutex_);
            if (rx_queue_.empty()) return;
            batch.swap(rx_queue_);
        }

        for (const auto &packet: batch) {
            dispatcher.dispatchData(packet);
        }
    }

    void send_data(const std::vector<uint8_t> &data) {
        asio::post(io_context_, [this, data]() {
            bool write_in_progress = !tx_queue_.empty();
            tx_queue_.push_back(data);

            if (!write_in_progress) {
                start_write();
            }
        });
    }

private:
    asio::io_context io_context_;
    asio::executor_work_guard<asio::io_context::executor_type> work_guard_;
    asio::serial_port serial_;
    std::thread background_thread_;
    asio::streambuf read_buffer_;

    std::deque<std::vector<uint8_t>> rx_queue_;
    std::mutex rx_mutex_;
    const size_t RX_CAPACITY = 1024;

    std::deque<std::vector<uint8_t>> tx_queue_;
    std::string portName;

    void start_read() {
        // Read until 0x00 (null byte)
        asio::async_read_until(serial_, read_buffer_, '\0',
                               [this](const std::error_code &ec, std::size_t /*bytes_transferred*/) {
                                   if (!ec) {
                                       std::istream is(&read_buffer_);
                                       std::string s;

                                       // Extract up to the null terminator
                                       if (std::getline(is, s, '\0')) {
                                           if (!s.empty()) {
                                               std::vector<uint8_t> vec(s.begin(), s.end());

                                               std::lock_guard<std::mutex> lock(rx_mutex_);
                                               if (rx_queue_.size() < RX_CAPACITY) {
                                                   rx_queue_.push_back(std::move(vec));
                                               }
                                           }
                                       }
                                       start_read();
                                   } else {
                                       std::cerr << "SerialProducer: read error: " << ec.message() << std::endl;
                                   }
                               });
    }

    void start_write() {
        asio::async_write(serial_,
                          asio::buffer(tx_queue_.front()),
                          [this](const std::error_code &ec, std::size_t /*bytes*/) {
                              if (!ec) {
                                  tx_queue_.pop_front();
                                  if (!tx_queue_.empty()) {
                                      start_write();
                                  }
                              } else {
                                  std::cerr << "SerialProducer: write error: " << ec.message() << std::endl;
                              }
                          });
    }

};