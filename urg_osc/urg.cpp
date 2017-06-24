#include "stdafx.h"
#include "urg.h"
#include "Urg_driver.h"
#include "math_utilities.h"

#define NEARLYPERCENT 0.1 // }10“
#define Port 10940
using namespace qrk;

urg::urg()
	:m_pUrg(new Urg_driver())
{
}


urg::~urg()
{
	stop();
	delete m_pUrg;
}

bool urg::init(const char* ip, const int port) {
	qrk::Lidar::connection_type_t connectType = qrk::Urg_driver::Ethernet;
	if (!m_pUrg->open(ip,Port,connectType))
	{
		std::cout << "urg_driver::open():" << ip << m_pUrg->what() << std::endl;
		return false;
	}
	return true;
}

void urg::stop() {
	m_pUrg->stop_measurement();
	m_pUrg->close();
}

bool urg::setPos(std::pair<long, long> xPos, std::pair<long, long>yPos) {

	if (!m_pUrg->set_scanning_parameter(m_pUrg->deg2step(-90), m_pUrg->deg2step(+90), 0))
	{
		std::cout << "Failed set Scanparams" << std::endl;
		return false;
	}
	if(!m_pUrg->start_measurement(qrk::Urg_driver::Distance_intensity, qrk::Urg_driver::Infinity_times, 0))
	{
		std::cout << "Failed start Measurement" << std::endl;
		return false;
	}
	x.first = xPos.first;
	x.second = xPos.second;

	y.first = yPos.first;
	y.second = yPos.second;

	return true;
}

void urg::getPos(std::map<int, std::pair<long, long> > &retPos, long &time) {
	std::vector<long> pData;
	std::map<int,std::pair<long, long>> m_pData;

	if (!m_pUrg->get_distance(pData,&time))
	{
		std::cout << "DataLost" << std::endl;
		return;
	}

	long min_distance = m_pUrg->min_distance();
	long max_distance = m_pUrg->max_distance();
	size_t data_n = pData.size();
	for (size_t i = 0; i < data_n; ++i) {
		long l = pData[i];
		if ((l <= min_distance) || (l >= max_distance)) {
			continue;
		}
		double radian = m_pUrg->index2rad(i);
		long _x = static_cast<long>(l * cos(radian));
		long _y = static_cast<long>(l * sin(radian));
		if (_x >= x.first && _x <= x.second)
		{
			if (_y >= y.first && _y <= y.second)
			{
				m_pData[i].first = _x;
				m_pData[i].second = _y;
			}
		}
	}
	if (retPos.size()!=0)
	{
		for (auto pos : retPos)
		{
			for (auto posData : m_pData)
			{
				if (posData.second.first >= pos.second.first*0.9 &&
					posData.second.first <= pos.second.first*1.1 &&
					posData.second.second >= pos.second.second*0.9 &&
					posData.second.second <= pos.second.second*1.1)
				{
					pos.second.first = posData.second.first;
					pos.second.second = posData.second.second;
				}
				else
				{
					auto max = retPos.size();
					retPos[max].first = posData.second.first;
					retPos[max].second = posData.second.second;
				}
			}
		}
	}
	else {
		int i = 0;
		for (auto posData : m_pData)
		{
			retPos[i].first = posData.second.first;
			retPos[i].second = posData.second.second;
			i++;
		}
	}
	return;
}
