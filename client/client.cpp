#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

// for debug drawing
#include <imgui.h>
#include <box2d/b2_math.h>
#include <iplib/draw.h>
#include <GLFW/glfw3.h>

#include <iplib/Address.h>
#include <iplib/globals.h>
#include <iplib/GuiWindow.h>
#include <iplib/MathHelpers.h>
#include <iplib/Peer.h>
#include <iplib/Protocol.h>
#include <iplib/SocketUDP.h>

using namespace std;
using namespace iplib;

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

constexpr int PORT_SERVER = 30000;
constexpr int PORT_CLIENT = 30001;
net::Address ADDRESS_SERVER(127,0,0,1, PORT_SERVER);

struct dat_s {
    recursive_mutex rw_mtx;

    pid_s pids_theta;
    pid_s pids_x;

    struct {
        atomic<bool> enabled = true;
        float cart_x = 0;
        float maxControlVel = 0.4f;
    } control;

    struct {
        float pend_theta = 0;
        float pend_d_theta = 0;
        float cart_x = 0;
    } ground_truth;

    struct {
        bool reset_sim;
        float initial_impulse;
        bool stabilized;
        int ticks_stable;

        chrono::time_point<chrono::steady_clock> sim_start;
        chrono::duration<float, chrono::seconds::period> time_to_stable;
    } simulation;

    void Init() {
        pids_theta.setpoint = 0;
        pids_theta.prev_error = 0;
        pids_theta.integral = 0;
        pids_theta.kp = 2.0f;
        pids_theta.ki = 5.3f;
        pids_theta.kd = 0.00125f;

        pids_x.setpoint = 0;
        pids_x.prev_error = 0;
        pids_x.integral = 0;
        pids_x.kp = -0.08f;
        pids_x.ki = 0.0f;
        pids_x.kd = 0.0f;

        Reset();
    }

    void Reset() {
        lock_guard lock(rw_mtx);

        simulation.reset_sim = false;
        simulation.stabilized = false;
        simulation.ticks_stable = 0;
        simulation.time_to_stable = simulation.time_to_stable.zero();
        simulation.sim_start = chrono::steady_clock::now();

        ground_truth.pend_theta = 0;
        ground_truth.pend_d_theta = 0;
        ground_truth.cart_x = 0;
        control.cart_x = 0;

        pids_theta.setpoint = 0;
        pids_theta.integral = 0;
        pids_theta.prev_error = 0;

        pids_x.setpoint = 0;
        pids_x.integral = 0;
        pids_x.prev_error = 0;
    }
} dat;

void UpdateControl(dat_s *dat) {
    constexpr float timeStep = 1.0f / 60.0f;

    while (true) {
        {
            lock_guard lock(dat->rw_mtx);
            if (dat->control.enabled) {

                float theta = dat->ground_truth.pend_theta;
                while (theta > b2_pi) theta -= 2 * b2_pi;
                while (theta < -b2_pi) theta += 2 * b2_pi;

                if (abs(theta) < b2_pi * 0.3f && abs(dat->ground_truth.pend_d_theta) < dat->control.maxControlVel * timeStep) {
                    float x = dat->ground_truth.cart_x;
                    float valTheta = pid(dat->pids_theta, theta, timeStep);
                    float valDx = pid(dat->pids_x, x, timeStep);

                    dat->control.cart_x = x + valTheta + valDx;
                }
            }

            if (!dat->simulation.stabilized) {
                if (dat->ground_truth.pend_d_theta < 0.0001f)
                    dat->simulation.ticks_stable++;
                else
                    dat->simulation.ticks_stable = 0;
                
                // ~1 second
                if (dat->simulation.ticks_stable > 60) {
                    dat->simulation.stabilized = true;
                    dat->simulation.time_to_stable =  chrono::steady_clock::now() - dat->simulation.sim_start;
                }
                
            }
        }

        // just over 60 fps
        this_thread::sleep_for(chrono::milliseconds(16));
    }
}

void UpdateNet(dat_s *dat) {
    union rx_u {
        net::ipsrv_pos_t ipsrv_pos;
    } rx;

    union tx_u {
        net::sim_setup_t sim_setup;
        net::clisrv_cart_pos_t clisrv_cart_pos;
    } tx;

    net::Peer<net::SocketUDP> endpoint(IPLIB_MAX_PACKET_SIZE, IPLIB_PROTOCOL_ID, isLittleEndian());
    endpoint.GetConnection().Open(PORT_CLIENT);
    endpoint.GetConnection().SetBlocking(true);

    while (true) {
        while (endpoint.IsPacketReady()) {
            lock_guard lock(dat->rw_mtx);

            switch(endpoint.GetPacketType()) {
                case net::PacketType::IPSRV_POS:
                    endpoint.Receive(rx.ipsrv_pos);
                    dat->ground_truth.pend_theta = rx.ipsrv_pos.pend_theta;
                    dat->ground_truth.pend_d_theta = rx.ipsrv_pos.pend_d_theta;
                    dat->ground_truth.cart_x = rx.ipsrv_pos.cart_x;
                    break;
            }
        }

        {
            lock_guard lock(dat->rw_mtx);

            // transmit control values
            tx.clisrv_cart_pos.cart_x = dat->control.cart_x;
            endpoint.GetConnection().SetTransmitAddress(ADDRESS_SERVER);
            endpoint.Transmit(&tx.clisrv_cart_pos);

            if (dat->simulation.reset_sim) {
                tx.sim_setup.initial_impulse = dat->simulation.initial_impulse;
                dat->Reset();
                endpoint.GetConnection().SetTransmitAddress(ADDRESS_SERVER);
                endpoint.Transmit(&tx.sim_setup);
            }
        }

        this_thread::sleep_for(chrono::milliseconds(5));
    }

    endpoint.GetConnection().Close();
}

int main(int argc, char *argv[]) {
    string prog_name = argv[0];

    if (argc > 2) {
        cout << "Usage: " << prog_name << " <server ip>\n";
        throw new runtime_error("invalid number of arguments - "s + to_string(argc));
    }

    if (argc == 2)
        ADDRESS_SERVER = net::Address::fromString(argv[1]);
    
    cout << "Server IP: " << ADDRESS_SERVER << '\n';
    dat.Init();

    thread net(UpdateNet, &dat);
    net.detach();
    thread ctrl(UpdateControl, &dat);
    ctrl.detach();

    gui::GuiWindow window(WINDOW_WIDTH, WINDOW_HEIGHT, "Inverted Pendulum Client");
    window.Open();

    glfwSwapInterval(1);    // vsync

    DebugDraw draw;
    draw.Create();
    g_camera.m_center.SetZero();
    g_camera.m_width = WINDOW_WIDTH;
    g_camera.m_height = WINDOW_HEIGHT;

    while (!window.GetShouldClose()) {
        window.BeginRender();

        {
            lock_guard lock(dat.rw_mtx);

            b2Vec2 cartPoints[4];
            cartPoints[0].Set(-2 + dat.ground_truth.cart_x, 1);
            cartPoints[1].Set(2 + dat.ground_truth.cart_x, 1);
            cartPoints[2].Set(2 + dat.ground_truth.cart_x, -1);
            cartPoints[3].Set(-2 + dat.ground_truth.cart_x, -1);

            b2Vec2 theta(cos(dat.ground_truth.pend_theta + b2_pi * 0.5f), sin(dat.ground_truth.pend_theta + b2_pi * 0.5f));
            auto pos = theta;
            pos.x = pos.x * 9 + dat.ground_truth.cart_x;
            pos.y *= 9;
            draw.DrawSegment(b2Vec2(-15, 0.5f), b2Vec2(-15, -1.0f), b2Color(1, 0, 0));
            draw.DrawSegment(b2Vec2(15, 0.5f), b2Vec2(15, -1.0f), b2Color(1, 0, 0));
            draw.DrawSegment(b2Vec2(-15, -1.0f), b2Vec2(15, -1.0f), b2Color(1, 0, 0));
            draw.DrawSolidPolygon(cartPoints, 4, b2Color(0.1f, 0.4f, 0.8f));
            draw.DrawSegment(b2Vec2(dat.ground_truth.cart_x, 0), pos, b2Color(0.1f, 0.8f, 0.4f));
            draw.DrawSolidCircle(pos, 1.0f, theta, b2Color(0.8f, 0.1f, 0.4f));
            draw.DrawPoint(b2Vec2(dat.control.cart_x, 0), 5.0f, b2Color(0.0f, 1.0f, 0.0f));

            auto port = ImGui::GetWindowViewport();
            g_camera.m_center.SetZero();
            g_camera.m_width = port->Size.x;
            g_camera.m_height = port->Size.y;
            draw.Flush();

            ImGui::Begin("Inverted Pendulum Client");

            if (ImGui::CollapsingHeader("Control")) {
                bool enableControl = dat.control.enabled;
                ImGui::Checkbox("Enable Control", &enableControl);
                dat.control.enabled = enableControl;

                ImGui::Text("Pendulum Theta: %f", dat.ground_truth.pend_theta);
                ImGui::SliderFloat("Cart X", &dat.control.cart_x, -15.0f, 15.0f);
                ImGui::SliderFloat("Maximum Controllable Vel", &dat.control.maxControlVel, 0, b2_pi);
                ImGui::DragFloat("Pendulum Theta Kp", &dat.pids_theta.kp, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("Pendulum Theta Ki", &dat.pids_theta.ki, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("Pendulum Theta Kd", &dat.pids_theta.kd, 0.01f, 0.0f, 10.0f);
                ImGui::DragFloat("Cart X Kp", &dat.pids_x.kp, 0.01f, -10.0f, 10.0f);
                ImGui::DragFloat("Cart X Ki", &dat.pids_x.ki, 0.01f, -10.0f, 10.0f);
                ImGui::DragFloat("Cart X Kd", &dat.pids_x.kd, 0.01f, -10.0f, 10.0f);
            }

            if (ImGui::CollapsingHeader("Simulation")) {
                // this is really a checkbox/toggle
                bool reset_sim = ImGui::Button("Reset Simulation");
                if (!dat.simulation.reset_sim)
                    dat.simulation.reset_sim = reset_sim;

                ImGui::DragFloat("Initial Impulse", &dat.simulation.initial_impulse, 1.0f, -200.0f, 200.0f);

                ImGui::Checkbox("Is Stabilized", &dat.simulation.stabilized);
                ImGui::Text("Time to Stable: %f", dat.simulation.time_to_stable.count());
            }

            ImGui::End();

        }   // lock

        window.EndRender();
    }

    draw.Destroy();
    window.Close();

    return 0;
}