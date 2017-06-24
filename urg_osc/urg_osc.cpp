#include "stdafx.h"
#include "u_osc.h"
#include "urg.h"
#define RETRY 30
#define DELETEINTERVAL 30000; //30second
using namespace std;

map<int, pair<long, long>> retPos;

void mapCheck()
{
	DWORD interval = DELETEINTERVAL;
	map<int, pair<long, long>> checkmap;
	while (1)
	{
		//mapDeleteCheck
		for (auto mapPos :retPos)
		{
			auto itr = checkmap.find(mapPos.first);
			if (itr != checkmap.end())
			{
				if (itr->second != mapPos.second)
				{
					itr->second = mapPos.second;
				}
				else {
					retPos.erase(itr);
				}
			}
		}
		Sleep(interval);
	}
}

int main(int argc, char* argv[])
{
	if (argc != 7)
	{
		cout << "Arguments fails" << endl;
		cout << "1stArg:xMax" << endl
			<< "2ndArg:xMin" << endl
			<< "3rdArg:yMax" << endl
			<< "4thArg:yMin" << endl
			<< "5thArg:SensorIP" << endl
			<< "6thArg:SensorPort"<<endl
			<<endl;

		cout << "Press Any key" << endl;
		getchar();
		return false;
	}
	int port = atoi(argv[6]);
	shared_ptr<osc::u_osc> m_pOsc = make_shared<osc::u_osc>("localhost",port);
	shared_ptr<qrk::urg> m_pUrg = make_shared<qrk::urg>();

	pair<long, long> x;
	pair<long, long> y;
	x.first = atoi(argv[1]);
	x.second = atoi(argv[2]);
	y.first = atoi(argv[3]);
	y.second = atoi(argv[4]);

	m_pOsc->init();
	int retry = 0;
	while (retry < RETRY)
	{
		retry += 1;
		if (!m_pUrg->init(argv[5], port)) {
			cout << "Failed,ConnectUrgSencer,Retry: " << retry << endl;
		}
		else { break; }
	}if (retry >= RETRY)
	{
		cout << "ConnectRetry" << retry << "times, Failed Connect" << endl;
		return false;
	}

	retry = 0;
	while (retry < RETRY)
	{
		retry += 1;
		if (!m_pUrg->setPos(x, y)){
			cout << "Failed,SetUpUrgSencer,Retry: " << retry << endl;
		}
		else { break; }
	}if (retry >= RETRY)
	{
		cout << "ConnectRetry" << retry << "times, Failed Connect" << endl;
		return false;
	}

	std::thread th1(mapCheck);
	th1.join();

	long time=0;
	while (1)
	{
		m_pUrg->getPos(retPos, time);
		m_pOsc->send(retPos);
	}

    return true;
}

