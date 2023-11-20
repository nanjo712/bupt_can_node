#ifndef BUPT_CAN_H
#define BUPT_CAN_H

#include <string>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <cstdint>
#include <array>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <memory>
#include <map>
#include <thread>

class Can
{
private:

    std::string can_name;
    struct ifreq ifr;
    struct sockaddr_can addr;

    int can_fd_read; // read can socket
    int can_fd_write; // write can socket

    std::mutex can_fd_write_mutex;

    std::queue<can_frame> send_que;
    std::mutex send_que_mutex;

    std::map< uint32_t, std::function<void(std::shared_ptr<can_frame>)> > recv_callback_map;
    std::mutex recv_callback_map_mutex;

    std::array<struct can_filter,512> filters;
    int filter_size;

    std::unique_ptr<std::thread> send_thread_;
    std::unique_ptr<std::thread> recv_thread_;

    bool isDestroyed;
    bool Started;

    void receive_thread();
    void send_thread();
public:
    enum CAN_ID_TYPE{
        CAN_ID_STD,
        CAN_ID_EXT,
        CAN_ID_ERR,
        CAN_ID_RTR
    };

    explicit Can(const std::string &can_name); // constructor
    ~Can(); // destructor
    /**
     * @brief Set the can id type
     * @param id_type The can id type
     * @param id The can id
     * @return The can id with type
    */
    uint32_t set_id_type(const CAN_ID_TYPE &id_type, const uint32_t &id);
    /**
     * @brief It will star to receive and send can frame
     * @param none
    */
    void can_start();
    /**
     * @brief Register a callback function to a can frame id
     * @param id The can frame id
     * @param callback The callback function
    */
    void register_msg(const uint32_t &id, const CAN_ID_TYPE &id_type, const std::function<void(const std::shared_ptr<can_frame>&)> callback);
    /**
     * @brief Send a can frame
     * @param id The can frame id
     * @param dlc The can frame dlc
     * @param data The can frame data
    */
    void send_can(const uint32_t &id, const CAN_ID_TYPE &id_type, const int &dlc, const std::array<uint8_t,8> &data);
    /**
     * @brief Send a can frame
     * @param frame The can frame
    */
    void send_can(const can_frame &frame);
    /**
     * @brief Send a can frame and wait for respond
     * @param id The can frame id
     * @param dlc The can frame dlc
     * @return true if success, false if failed
    */
    bool send_can_with_respond(const uint32_t &id, const CAN_ID_TYPE &id_type, const int &dlc, const std::array<uint8_t,8> &data);
    /**
     * @brief Send a can frame and wait for respond
     * @param frame The can frame
     * @return true if success, false if failed
    */
    bool send_can_with_respond(const can_frame &frame);
    /**
     * @brief Set the recv filter
     * @param none
     * @return 0 if success, -1 if failed
    */
    int set_recv_filter();
    /**
     * @brief Check if the can is started
     * @param none
     * @return true if started, false if not started
     */
    bool isStarted() const;
};

#endif // BUPT_CAN_H