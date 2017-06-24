#include "stdafx.h"
#include "u_osc.h"
#include "OscOutboundPacketStream.h"
#include "UdpSocket.h"

#define OUTPUT_BUFFER_SIZE 1024
using namespace osc;

u_osc::u_osc(const char* ip, const int port)
	:socket(new UdpTransmitSocket(IpEndpointName(ip,port)))
{
}


u_osc::~u_osc()
{
}
void u_osc::init() {
	char buffer[OUTPUT_BUFFER_SIZE];
	p = new OutboundPacketStream(buffer, OUTPUT_BUFFER_SIZE);
}

void u_osc::send(std::map<int, std::pair<long, long>> &data) {
	
	for (auto id : data)
	{
		char buffer[OUTPUT_BUFFER_SIZE];
		OutboundPacketStream out(buffer, OUTPUT_BUFFER_SIZE);
		 out << BeginBundleImmediate <<
			BeginMessage(std::to_string(id.first).c_str()) <<
			"x: "<<	id.second.first << 
			"y: "<< id.second.second <<
			EndMessage;
		 p = &out;

		 socket->Send(p->Data(), p->Size());
		 std::cout << "id: " << id.first <<
			 "x: " << id.second.first <<
			 "y: " << id.second.second << std::endl;
	}
}