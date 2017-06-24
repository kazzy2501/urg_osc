#pragma once


namespace qrk {
	class Urg_driver;
	class Lidar;

	class urg
	{
	public:
		urg();
		~urg();

		bool init(const char* ip, const int port);
		void stop();

		bool setPos(std::pair<long, long> xPos, std::pair<long, long>yPos);
		void getPos(std::map<int, std::pair<long, long> > &retPos, long &time);

	private:
		Urg_driver *m_pUrg;
		std::pair<long, long> x;
		std::pair<long, long> y;
	};

}