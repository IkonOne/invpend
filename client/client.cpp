#include <atomic>
#include <chrono>
#include <iostream>
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
    struct {
        atomic<bool> enabled = true;
        atomic<float> cart_x = 0;
        atomic<float> maxControlVel = 0.4f;
    } control;

    struct {
        atomic<float> pend_theta = 0;
        atomic<float> pend_d_theta = 0;
        atomic<float> cart_x = 0;
    } ground_truth;

    struct {
        atomic<float> kp;
        atomic<float> ki;
        atomic<float> kd;
    } pids_theta;

    struct {
        atomic<float> kp;
        atomic<float> ki;
        atomic<float> kd;
    } pids_x;
} dat;

void UpdateControl(dat_s *dat) {
    constexpr float timeStep = 1.0f / 60.0f;
    pid_s pidsTheta(0, 2.0f, 5.3f, 0.00125f);
    dat->pids_theta.kp = pidsTheta.kp;
    dat->pids_theta.ki = pidsTheta.ki;
    dat->pids_theta.kd = pidsTheta.kd;

    pid_s pidsX(0, -0.08f, 0.0f, 0.0f);
    dat->pids_x.kp = pidsX.kp;
    dat->pids_x.ki = pidsX.ki;
    dat->pids_x.kd = pidsX.kd;

    while (true) {
        if (dat->control.enabled) {
            float theta = dat->ground_truth.pend_theta;
            while (theta > b2_pi) theta -= 2 * b2_pi;
            while (theta < -b2_pi) theta += 2 * b2_pi;

            if (abs(theta) < b2_pi * 0.3f && abs(dat->ground_truth.pend_d_theta) < dat->control.maxControlVel * timeStep) {
                float x = dat->ground_truth.cart_x;

                pidsTheta.kp = dat->pids_theta.kp;
                pidsTheta.ki = dat->pids_theta.ki;
                pidsTheta.kd = dat->pids_theta.kd;
                float valTheta = pid(pidsTheta, theta, timeStep);

                pidsX.setpoint = 0;
                pidsX.kp = dat->pids_x.kp;
                pidsX.ki = dat->pids_x.ki;
                pidsX.kd = dat->pids_x.kd;
                float valDx = pid(pidsX, x, timeStep);

                dat->control.cart_x = x + valTheta + valDx;
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
        net::clisrv_cart_pos_t clisrv_cart_pos;
    } tx;

    net::Peer<net::SocketUDP> endpoint(IPLIB_MAX_PACKET_SIZE, IPLIB_PROTOCOL_ID, isLittleEndian());
    endpoint.GetConnection().Open(PORT_CLIENT);
    endpoint.GetConnection().SetBlocking(true);

    while (true) {
        while (endpoint.IsPacketReady()) {
            switch(endpoint.GetPacketType()) {
                case net::PacketType::IPSRV_POS:
                    endpoint.Receive(rx.ipsrv_pos);
                    dat->ground_truth.pend_theta = rx.ipsrv_pos.pend_theta;
                    dat->ground_truth.pend_d_theta = rx.ipsrv_pos.pend_d_theta;
                    dat->ground_truth.cart_x = rx.ipsrv_pos.cart_x;
                    break;
            }
        }

        // transmit control values
        tx.clisrv_cart_pos.cart_x = dat->control.cart_x;
        endpoint.GetConnection().SetTransmitAddress(ADDRESS_SERVER);
        endpoint.Transmit(&tx.clisrv_cart_pos);

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
        ADDRESS_SERVER.fromString(argv[1]);
    
    cout << "Server IP: " << ADDRESS_SERVER << '\n';

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

        bool enableControl = dat.control.enabled;
        ImGui::Checkbox("Enable Control", &enableControl);
        dat.control.enabled = enableControl;

        ImGui::Text("Pendulum Theta: %f", (float)dat.ground_truth.pend_theta);

        float cartX = dat.control.cart_x;
        ImGui::SliderFloat("Cart X", &cartX, -15.0f, 15.0f);
        dat.control.cart_x = cartX;

        float maxControlVel = dat.control.maxControlVel;
        ImGui::SliderFloat("Maximum Controllable Vel", &maxControlVel, 0, b2_pi);
        dat.control.maxControlVel = maxControlVel;

        float kp = dat.pids_theta.kp;
        ImGui::DragFloat("Pendulum Theta Kp", &kp, 0.01f, 0.0f, 10.0f);
        dat.pids_theta.kp = kp;

        float ki = dat.pids_theta.ki;
        ImGui::DragFloat("Pendulum Theta Ki", &ki, 0.01f, 0.0f, 10.0f);
        dat.pids_theta.ki = ki;

        float kd = dat.pids_theta.kd;
        ImGui::DragFloat("Pendulum Theta Kd", &kd, 0.01f, 0.0f, 10.0f);
        dat.pids_theta.kd = kd;

        float dkp = dat.pids_x.kp;
        ImGui::DragFloat("Cart X Kp", &dkp, 0.01f, -10.0f, 10.0f);
        dat.pids_x.kp = dkp;

        float dki = dat.pids_x.ki;
        ImGui::DragFloat("Cart X Ki", &dki, 0.01f, -10.0f, 10.0f);
        dat.pids_x.ki = dki;

        float dkd = dat.pids_x.kd;
        ImGui::DragFloat("Cart X Kd", &dkd, 0.01f, -10.0f, 10.0f);
        dat.pids_x.kd = dkd;


        window.EndRender();
    }

    draw.Destroy();
    window.Close();

    return 0;
}