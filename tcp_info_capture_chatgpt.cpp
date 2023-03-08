/*
This version uses a `TcpInfoCapture` class to encapsulate the packet capture and TCP information extraction functionality. The class has a constructor that takes the interface name as a parameter and initializes the member variables, and a destructor that closes the device and deallocates it. The `startCapture()` method finds and opens the device, sets the packet capture filter to capture only TCP traffic, and returns `true` if successful. The `stopCapture()` method stops the packet capture. The `printTcpInfo()` method starts the packet capture using the `startCapture()` method and iterates through the captured packets to extract the TCP information. The `packetHandler()` static method is used as the packet capture callback function to store the captured packets in a member variable vector.
*/

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include "pcapplusplus/PcapLiveDeviceList.h"
#include "pcapplusplus/TcpReassembly.h"

using namespace std;
using namespace pcpp;

class TcpInfoCapture
{
public:
    TcpInfoCapture(const string &interfaceName) : m_interfaceName(interfaceName), m_dev(nullptr) {}

    ~TcpInfoCapture()
    {
        if (m_dev != nullptr)
        {
            m_dev->close();
            delete m_dev;
        }
    }

    bool startCapture()
    {
        // Find the network interface
        PcapLiveDeviceList devList;
        m_dev = devList.getPcapLiveDeviceByName(m_interfaceName);
        if (m_dev == nullptr)
        {
            cerr << "Could not find interface '" << m_interfaceName << "'" << endl;
            return false;
        }

        // Open the device
        if (!m_dev->open())
        {
            cerr << "Could not open device '" << m_interfaceName << "'" << endl;
            return false;
        }

        // Set the packet capture filter to only capture TCP traffic
        string filter = "tcp";
        if (!m_dev->setFilter(filter))
        {
            cerr << "Could not set filter '" << filter << "'" << endl;
            return false;
        }

        return true;
    }

    void stopCapture()
    {
        if (m_dev != nullptr)
        {
            m_dev->stopCapture();
        }
    }

    void printTcpInfo()
    {
        // Start the packet capture
        m_dev->startCapture(this->packetHandler, this);

        // Print TCP info
        for (auto packet : m_packets)
        {
            // IP and TCP layers
            auto ipLayer = packet.getLayer<IPv4Layer>();
            auto tcpLayer = packet.getLayer<TcpLayer>();

            if (ipLayer != nullptr && tcpLayer != nullptr)
            {
                // TCP throughput
                uint32_t packetLength = packet.getRawPacket()->getRawDataLen();
                double throughput = (double)packetLength / tcpLayer->getTcpHeader()->syn ? 1 : tcpLayer->getTcpHeader()->ack;

                // TCP RTT
                uint32_t tcpTimestampValue = tcpLayer->getTcpHeader()->timestampValue;
                uint32_t tcpTimestampEchoReply = tcpLayer->getTcpHeader()->timestampEchoReply;
                if (tcpTimestampValue != 0 && tcpTimestampEchoReply != 0)
                {
                    uint32_t rtt = tcpTimestampEchoReply - tcpTimestampValue;
                    cout << "RTT: " << rtt << endl;
                }

                // TCP inflight, CWND, RWND, and ssthresh
                TcpReassembly::TcpSessionKey tcpKey(
                    ipLayer->getSrcIpAddress(), ipLayer->getDstIpAddress(), tcpLayer->getSrcPort(), tcpLayer->getDstPort(), tcpLayer->getTcpHeader()->ackNumber);
                TcpReassembly::TcpSession *tcpSession = TcpReassembly::TcpReassembly::getInstance().getSession(tcpKey);
                if (tcpSession != nullptr)
                {
                    uint32_t inflight = tcpSession->getInflightData();
                    uint32_t cwnd = tcpSession->getCwndSize();
                    uint32_t rwnd = tcpSession->getRwndSize();
                    uint32_t ssthresh = tcpSession->getSsthreshSize();
                    cout << "Inflight: " << inflight << endl;
                    cout << "CWND: " << cwnd << endl;
                    cout << "RWND: " << rwnd << endl;
                    cout << "ssthresh: " << ssthresh << endl;
                }
            }
        }
    }

private:
    static void packetHandler(Packet &packet, void *userData)
    {
        auto tcpInfoCapture = reinterpret_cast<TcpInfoCapture *>(userData);
        tcpInfoCapture->m_packets.push_back(packet);
    }
    string m_interfaceName;
    PcapLiveDevice *m_dev;
    vector<Packet> m_packets;
};

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <interface>" << endl;
        return 1;
    }
    string interfaceName = argv[1];
    TcpInfoCapture tcpCapture(interfaceName);

    if (!tcpCapture.startCapture())
    {
        return 1;
    }

    tcpCapture.printTcpInfo();
    tcpCapture.stopCapture();

    return 0;
}
