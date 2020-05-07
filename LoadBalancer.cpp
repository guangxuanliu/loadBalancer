#include "LoadBalancer.h"

int LoadBalancer::getBestServer(const std::vector<ServerInfo> &infos) {

    int ret = -1;
    double score = 0;
    for(int i = 0; i < infos.size();++i)
    {
        //过滤
        if(infos[i].connection >= _maxConnection ||
        infos[i].cpuUsage >= _maxCpuUsage ||
        infos[i].ramUsage >= _maxRamUsage)
        {
            continue;
        }

        //打分
        double weigh = infos[i].connection * _connectionWeight +
                infos[i].cpuUsage * _cpuWeight +
                infos[i].ramUsage * _ramWeight;

        if(weigh < score)
        {
            score = weigh;
            ret = i;
        }
    }

    return ret;
}

unsigned int LoadBalancer::getMaxConnection() const {
    return _maxConnection;
}

void LoadBalancer::setMaxConnection(unsigned int maxConnection) {
    _maxConnection = maxConnection;
}

unsigned int LoadBalancer::getMaxCpuUsage() const {
    return _maxCpuUsage;
}

void LoadBalancer::setMaxCpuUsage(unsigned int maxCpuUsage) {
    _maxCpuUsage = maxCpuUsage;
}

unsigned int LoadBalancer::getMaxRamUsage() const {
    return _maxRamUsage;
}

void LoadBalancer::setMaxRamUsage(unsigned int maxRamUsage) {
    _maxRamUsage = maxRamUsage;
}

double LoadBalancer::getConnectionWeight() const {
    return _connectionWeight;
}

void LoadBalancer::setConnectionWeight(double connectionWeight) {
    _connectionWeight = connectionWeight;
}

double LoadBalancer::getCpuWeight() const {
    return _cpuWeight;
}

void LoadBalancer::setCpuWeight(double cpuWeight) {
    _cpuWeight = cpuWeight;
}

double LoadBalancer::getRamWeight() const {
    return _ramWeight;
}

void LoadBalancer::setRamWeight(double ramWeight) {
    _ramWeight = ramWeight;
}
