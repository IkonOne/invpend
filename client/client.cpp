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
const net::Address ADDRESS_SERVER(127,0,0,1, PORT_SERVER);

struct dat_s {
    struct {
        atomic<float> cart_x = 0;
    } control;

    struct {
        atomic<float> pend_theta = 0;
        atomic<float> cart_x = 0;
    } ground_truth;
} dat;

void UpdateControl(dat_s *dat) {
    constexpr float timeStep = 1.0f / 60.0f;
    pid_s pidValues(0, 2.0f, 1.0f, 0.01f);

    while (true) {
        float theta = dat->ground_truth.pend_theta;
        while (theta > b2_pi) theta -= 2 * b2_pi;
        while (theta < -b2_pi) theta += 2 * b2_pi;

        if (abs(theta) < b2_pi * 0.4f) {
            float x = dat->ground_truth.cart_x;
            float val = pid(pidValues, theta, timeStep);
            dat->control.cart_x = x + val;
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

int main() {
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
        draw.DrawSolidPolygon(cartPoints, 4, b2Color(0.1f, 0.4f, 0.8f));
        draw.DrawSegment(b2Vec2(dat.ground_truth.cart_x, 0), pos, b2Color(0.1f, 0.8f, 0.4f));
        draw.DrawSolidCircle(pos, 1.0f, theta, b2Color(0.8f, 0.1f, 0.4f));
        draw.DrawPoint(b2Vec2(dat.control.cart_x, 0), 5.0f, b2Color(1.0f, 0.0f, 0.0f));

        auto port = ImGui::GetWindowViewport();
        g_camera.m_center.SetZero();
        g_camera.m_width = port->Size.x;
        g_camera.m_height = port->Size.y;
        draw.Flush();

        ImGui::Text("%f", (float)dat.ground_truth.pend_theta);

        window.EndRender();
    }

    draw.Destroy();
    window.Close();

    return 0;
}