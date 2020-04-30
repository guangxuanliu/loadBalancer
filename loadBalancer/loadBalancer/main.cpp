#include <iostream>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <memory>
#include <random>
#include <time.h>
#include <algorithm>
using namespace std;

const unsigned int hostNum = 10;				//服务器的数量
const unsigned int clientNum = 1000;			//客户端的数量
const unsigned int getHostInfoDuration = 2;		//模拟获取服务器信息的时间间隔，单位：秒
const unsigned int maxCpuUsage = 80;			//最大CPU使用率
const unsigned int maxRamUsage = 80;			//最大内存使用率
const unsigned int nextAllocateDuration = 1;	//若无可用服务器时，下次分配时间间隔

//客户端请求
struct Request
{
	int id;
	string content;
};

//主机信息
class HostInfo
{
public:
	void setServerID(unsigned int ID)
	{
		serverID = ID;
	}

	const unsigned int getServerID()
	{
		return serverID;
	}

	const unsigned int getCpuUsage()
	{
		return cpuUsage;
	}

	void setCpuUsage(unsigned int usage)
	{
		cpuUsage = usage;
	}

	const unsigned int getRamUsage()
	{
		return ramUsage;
	}

	void setRamUsage(unsigned int usage)
	{
		ramUsage = usage;
	}


	void increaseConnect()
	{
		cpuUsage += 5;
		ramUsage += 5;
		++connection;
	}

	void decreaseConnect()
	{
		if (connection > 0)
		{
			--connection;
			cpuUsage -= 5;
			ramUsage -= 5;
		}
	}

private:
	unsigned int serverID;
	unsigned int cpuUsage;
	unsigned int ramUsage;
	unsigned int connection = 0;
};

//权重
struct Weight
{
	double cpuWeight;
	double ramWeight;
};

//客户端请求
mutex mutex_request;
condition_variable cond_request;
queue<Request> data_request;

//服务器信息
mutex mutex_host;
condition_variable cond_host;
vector<HostInfo> data_host;

//权重
mutex mutex_weight;
condition_variable cond_weight;
Weight data_weight{0.5,0.5};

//模拟客户端请求
void simulationRequest()
{
	for (int i = 1;i <= clientNum;++i)
	{
		Request request;
		request.id = i;
		request.content = "simulation request";

		{
			lock_guard<mutex> lk(mutex_request);
			data_request.push(request);
			cond_request.notify_one();
		}
		this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

//模拟获取服务器信息
void simulationGetHostInfo()
{
	static bool first = true;
	default_random_engine engine(time(nullptr));
	uniform_int_distribution<int> dist(1, 99);

	while (true)
	{
		if (first)
		{
			data_host.reserve(hostNum);
			for (int i = 1;i <= hostNum;i++)
			{
				HostInfo info;
				info.setServerID(i);
				info.setCpuUsage(dist(engine));
				info.setRamUsage(dist(engine));
				{
					lock_guard<mutex> lk(mutex_host);
					data_host.push_back(info);
					cond_host.notify_one();
				}
			}
			first = false;
		}
		else
		{
			lock_guard<mutex> lk(mutex_host);
			for (auto &host : data_host)
			{
				host.decreaseConnect();
			}
			cond_host.notify_one();


		}
		this_thread::sleep_for(std::chrono::seconds(getHostInfoDuration));
	}
}

//处理请求，并分配服务器
void processRequest()
{
	while (true)
	{
		//获取客户端请求
		Request request;
		{
			unique_lock<mutex> lm(mutex_request);
			cond_request.wait(lm, [] {
				return !data_request.empty();
			});

			request = data_request.front();
		}

		//获取服务器信息和权重
		int choosedHost = -1;
		{
			double min = 0;

			unique_lock<mutex> lk1(mutex_host, defer_lock);
			unique_lock<mutex> lk2(mutex_weight, defer_lock);
			lock(lk1, lk2);
			
			for (size_t i = 0;i < data_host.size();++i)
			{
				//过滤
				if (data_host[i].getCpuUsage() > maxCpuUsage 
					|| data_host[i].getRamUsage() > maxRamUsage)
				{
					continue;
				}

				//打分
				double weigh = data_host[i].getCpuUsage() * data_weight.cpuWeight
					+ data_host[i].getRamUsage() * data_weight.ramWeight;
				
				if (min == 0)
				{
					min = weigh;
					choosedHost = i;
				}
				else if(weigh < min)
				{
					min = weigh;
					choosedHost = i;
				}
			}

			if (choosedHost != -1)
			{
				cout << "send request " << request.id << " to server No:" << data_host[choosedHost].getServerID() << endl;
				data_host[choosedHost].increaseConnect();
			}
			else
			{
				cout << "There are no enough host to allocate!!!" << endl;
			}
		}

		//移除客户端请求
		if(choosedHost != -1)
		{
			lock_guard<mutex> lm(mutex_request);
			if(!data_request.empty())
				data_request.pop();
		}
		else
		{
			this_thread::sleep_for(chrono::seconds(nextAllocateDuration));
		}

	}
}

int main() 
{
	thread t_request(simulationRequest);
	thread t_getHostInfo(simulationGetHostInfo);
	thread t_process(processRequest);

	t_request.join();
	t_getHostInfo.join();
	t_process.join();
	
	return 0;
}