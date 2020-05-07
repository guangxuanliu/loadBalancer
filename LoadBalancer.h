#ifndef LOADBALANCER_LOADBALANCER_H
#define LOADBALANCER_LOADBALANCER_H

#include <string>
#include <vector>

//服务器信息
struct ServerInfo{
    unsigned int connection;        //连接个数
    unsigned int cpuUsage;          //cpu使用率，例如50
    unsigned int ramUsage;          //内存使用率，例如50
    std::string ipAddr;             //ip地址
    unsigned short port;            //端口
};

class LoadBalancer {
public:
    LoadBalancer(unsigned int maxConnection, unsigned int maxCpuUsage, unsigned int maxRamUsage,
                 double connectionWeight, double cpuWeight, double ramWeight)
            : _maxConnection(maxConnection), _maxCpuUsage(maxCpuUsage), _maxRamUsage(maxRamUsage),
              _connectionWeight(connectionWeight), _cpuWeight(cpuWeight), _ramWeight(ramWeight) {

    }

    int getBestServer(const std::vector<ServerInfo> &infos);

public:
    unsigned int getMaxConnection() const;

    void setMaxConnection(unsigned int maxConnection);

    unsigned int getMaxCpuUsage() const;

    void setMaxCpuUsage(unsigned int maxCpuUsage);

    unsigned int getMaxRamUsage() const;

    void setMaxRamUsage(unsigned int maxRamUsage);

    double getConnectionWeight() const;

    void setConnectionWeight(double connectionWeight);

    double getCpuWeight() const;

    void setCpuWeight(double cpuWeight);

    double getRamWeight() const;

    void setRamWeight(double ramWeight);

private:
    unsigned int _maxConnection;
    unsigned int _maxCpuUsage;
    unsigned int _maxRamUsage;
    double _connectionWeight;
    double _cpuWeight;
    double _ramWeight;
};


#endif //LOADBALANCER_LOADBALANCER_H
