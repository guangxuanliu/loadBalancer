#include <iostream>

using namespace std;

#include <pthread.h>
#include <unistd.h>
#include <queue>

#include <sys/sysinfo.h>
#include <fstream>


pthread_mutex_t g_mutex;
queue<int> g_queue;
int i = 0;


// ��ȡ�ڴ�ʹ����
// ����һ
int getMemUsage() {
    struct sysinfo tmp;
    int ret = sysinfo(&tmp);
    unsigned long free = 0, total = 0;
    if (ret == 0) {
        free = (unsigned long) tmp.freeram / (1024 * 1024);
        total = (unsigned long) tmp.totalram / (1024 * 1024);
    }
    return ((double) (total - free)) * 100 / total;
}

// ��ȡ�ڴ�ʹ����
// ������
// ������ͬ����һ
int getMemUsage2() {
    ifstream mem_stream("/proc/meminfo", ios_base::in);
    string tmp;
    unsigned long memTotal, memFree;

    if (mem_stream.is_open())
    {
        mem_stream >> tmp >> memTotal >> tmp >> tmp >> memFree;
        mem_stream.close();
    }

    if(memTotal != 0)
    {
        return ((double) (memTotal - memFree)) * 100 / memTotal;
    } else{
        return 0;
    }
}

//��ȡCPU��ʹ����
typedef struct CPU_PACKED {
    char name[20];
    unsigned int user;
    unsigned int nice;
    unsigned int system;
    unsigned int idle;
    unsigned int iowait;
    unsigned int irq;
    unsigned int softirq;
} CpuUsage;

double calCpuUsage(CpuUsage *o, CpuUsage *n) {
    double od, nd;
    double id, sd;
    double cpu_use;

    od = (double) (o->user + o->nice + o->system + o->idle + o->softirq + o->iowait +
                   o->irq);//��һ��(�û�+���ȼ�+ϵͳ+����)��ʱ���ٸ���od
    nd = (double) (n->user + n->nice + n->system + n->idle + n->softirq + n->iowait +
                   n->irq);//�ڶ���(�û�+���ȼ�+ϵͳ+����)��ʱ���ٸ���od

    id = (double) (n->idle);        //�û���һ�κ͵ڶ��ε�ʱ��֮���ٸ���id
    sd = (double) (o->idle);       //ϵͳ��һ�κ͵ڶ��ε�ʱ��֮���ٸ���sd

    if ((nd - od) != 0)
        cpu_use = 100.0 - ((id - sd)) / (nd - od) * 100.00;
    else cpu_use = 0;
    return cpu_use;
}

void getCpuStat(CpuUsage *cpust) {

    CpuUsage *cpu_occupy;
    cpu_occupy = cpust;

    ifstream in("/proc/stat",ios::in);
    if(in.is_open())
    {
        in >> cpu_occupy->name >> cpu_occupy->user
        >> cpu_occupy->nice >> cpu_occupy->system
        >> cpu_occupy->idle >> cpu_occupy->iowait
        >> cpu_occupy->irq >> cpu_occupy->softirq;

        in.close();
    }
}

double getCpuRate(int microseconds) {
    CpuUsage cpuStat1;
    CpuUsage cpuStat2;
    double usage;

    //��һ�λ�ȡcpuʹ�����
    getCpuStat((CpuUsage *) &cpuStat1);

    usleep(microseconds);

    //�ڶ��λ�ȡcpuʹ�����
    getCpuStat((CpuUsage *) &cpuStat2);

    //����cpuʹ����
    usage = calCpuUsage(&cpuStat1, &cpuStat2);

    return usage;
}

void *producer(void *arg) {
    while (true) {
        pthread_mutex_lock(&g_mutex);

        g_queue.push(++i);

        pthread_mutex_unlock(&g_mutex);

        usleep(1);
    }
}

void* consumer(void *arg)
{
    while (true)
    {
        pthread_mutex_lock(&g_mutex);

        while (!g_queue.empty())
        {
//            cout << g_queue.front() << endl;
            g_queue.pop();
        }
        pthread_mutex_unlock(&g_mutex);

        usleep(1);
    }
}


int main() {

    pthread_mutex_init(&g_mutex, NULL);

    pthread_t tid1,tid2;
    pthread_create(&tid1,NULL,producer,NULL);
    pthread_create(&tid2,NULL,consumer,NULL);

    while (true)
    {
        std::cout << "RAM:" << getMemUsage() << "\t\tCPU:" << getCpuRate(1*1000*1000) << std::endl;
    }


    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);

    pthread_mutex_destroy(&g_mutex);
    return 0;
}