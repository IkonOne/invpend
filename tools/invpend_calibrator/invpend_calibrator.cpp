#include <iostream>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <iplib/GuiWindow.h>
#include <iplib/PacketBuilder.h>
#include <iplib/Protocol.h>
#include <iplib/SerialConnection.h>
#include <iplib/Peer.h>
#include <iplib/MathHelpers.h>
#include <imgui.h>

using namespace std;
using namespace iplib;

string _port = "/dev/cu.usbserial14101";

/*********************************
 * Net/Serial
 *********************************/

net::Peer<net::SerialConnection> _peer(IPLIB_MAX_PACKET_SIZE, IPLIB_PROTOCOL_ID, isLittleEndian());

struct packets_s {
    shared_mutex mtx_rw;
    bool has_new;
    id_t rx_packet_type;

    net::IPSRV_POS ipsrv_pos;
} packets;

void listen() {
    while (true) {
        while (!_peer.IsPacketReady())
            this_thread::yield();

        packets.mtx_rw.lock();

        packets.has_new = true;
        packets.rx_packet_type = _peer.GetPacketType();
        switch(packets.rx_packet_type) {
            case net::PacketType::IPSRV_POS:
                _peer.Receive(packets.ipsrv_pos);
                break;
            
            default:
                packets.has_new = false;
        }

        packets.mtx_rw.unlock();
    }
}

/************************
 * GUI
 ************************/

constexpr int MAX_DATA_LEN = 50;

ImVector<float> _pend_thetas;

void update() {
    packets.mtx_rw.lock_shared();

    _pend_thetas.push_back(packets.ipsrv_pos.pend_theta);
    if (_pend_thetas.size() > MAX_DATA_LEN)
        _pend_thetas.erase(_pend_thetas.begin());

    packets.mtx_rw.unlock_shared();
}

void draw() {
    bool show_demo = true;
    ImGui::ShowDemoWindow(&show_demo);

    ImGui::Text("Simple data visualizaiton to aid with calibration");

    if (ImGui::CollapsingHeader("Pendulum")) {
        ImGui::Text("Pendulum Properties");
        ImGui::Separator();

        ImGui::Text("theta: ");
        ImGui::SameLine();
        ImGui::Text(to_string(_pend_thetas.back()).c_str());

        ImGui::PlotLines("theta", _pend_thetas.begin(), _pend_thetas.size());
        
        ImGui::Columns(1);
    }
}

int main(int argv, char *argc[]) {
    // _peer.GetConnection().SetDevice(argc[1]);
    // _peer.GetConnection().Open(net::Baud::_115200);

    {
        // this block scopes listen_thread so that it's 
        // destructor is called when completed, killing the thread.
        // All of this should probably go in a function.
        // thread listen_thread(listen);
        // listen_thread.detach();

        gui::GuiWindow window(800, 600, "Inverted Pendulum Calibrator");
        window.Open();

        while (!window.GetShouldClose()) {
            window.BeginRender();

            update();
            draw();

            window.EndRender();
        }

        window.Close();
    }

    // _peer.GetConnection().Close();

    return 0;
}