#pragma once
class UdpTransmitSocket;

namespace osc {
	class OutboundPacketStream;

	class u_osc
	{
	public:
		u_osc(const char* ip, const int port);
		~u_osc();
		void init();
		void send(std::map<int, std::pair<long, long>> &data);

	private:
		UdpTransmitSocket *socket;
		osc::OutboundPacketStream *p;
	};

}