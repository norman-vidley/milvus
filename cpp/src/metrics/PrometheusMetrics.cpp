/*******************************************************************************
 * Copyright 上海赜睿信息科技有限公司(Zilliz) - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 ******************************************************************************/

#include "PrometheusMetrics.h"
#include "utils/Log.h"
#include "SystemInfo.h"


namespace zilliz {
namespace vecwise {
namespace server {

ServerError
PrometheusMetrics::Init() {
    try {
        ConfigNode &configNode = ServerConfig::GetInstance().GetConfig(CONFIG_METRIC);
        startup_ = configNode.GetValue(CONFIG_METRIC_IS_STARTUP) == "true" ? true : false;
        // Following should be read from config file.
        const std::string bind_address = configNode.GetChild(CONFIG_PROMETHEUS).GetValue(CONFIG_METRIC_PROMETHEUS_PORT);
        const std::string uri = std::string("/metrics");
        const std::size_t num_threads = 2;

        // Init Exposer
        exposer_ptr_ = std::make_shared<prometheus::Exposer>(bind_address, uri, num_threads);

        // Exposer Registry
        exposer_ptr_->RegisterCollectable(registry_);
    } catch (std::exception& ex) {
        SERVER_LOG_ERROR << "Failed to connect prometheus server: " << std::string(ex.what());
        return SERVER_UNEXPECTED_ERROR;
    }

    return SERVER_SUCCESS;

}


void
PrometheusMetrics::CPUUsagePercentSet()  {
    if(!startup_) return ;
    double usage_percent = server::SystemInfo::GetInstance().CPUPercent();
    CPU_usage_percent_.Set(usage_percent);
}

void
PrometheusMetrics::RAMUsagePercentSet() {
    if(!startup_) return ;
    double usage_percent = server::SystemInfo::GetInstance().MemoryPercent();
    RAM_usage_percent_.Set(usage_percent);
}

void
PrometheusMetrics::GPUPercentGaugeSet() {
    if(!startup_) return;
    int numDevide = server::SystemInfo::GetInstance().num_device();
    std::vector<unsigned int> values = server::SystemInfo::GetInstance().GPUPercent();
//    for (int i = 0; i < numDevide; ++i) {
//        GPU_percent_gauges_[i].Set(static_cast<double>(values[i]));
//    }
    if(numDevide >= 1) GPU0_percent_gauge_.Set(static_cast<double>(values[0]));
    if(numDevide >= 2) GPU1_percent_gauge_.Set(static_cast<double>(values[1]));
    if(numDevide >= 3) GPU2_percent_gauge_.Set(static_cast<double>(values[2]));
    if(numDevide >= 4) GPU3_percent_gauge_.Set(static_cast<double>(values[3]));
    if(numDevide >= 5) GPU4_percent_gauge_.Set(static_cast<double>(values[4]));
    if(numDevide >= 6) GPU5_percent_gauge_.Set(static_cast<double>(values[5]));
    if(numDevide >= 7) GPU6_percent_gauge_.Set(static_cast<double>(values[6]));
    if(numDevide >= 8) GPU7_percent_gauge_.Set(static_cast<double>(values[7]));

    // to do
}

void PrometheusMetrics::GPUMemoryUsageGaugeSet() {
    if(!startup_) return;
    int numDevide = server::SystemInfo::GetInstance().num_device();
    std::vector<unsigned long long> values = server::SystemInfo::GetInstance().GPUMemoryUsed();
    constexpr unsigned long long MtoB = 1024*1024;
    int numDevice = values.size();
//    for (int i = 0; i < numDevice; ++i) {
//        GPU_memory_usage_gauges_[i].Set(values[i]/MtoB);
//    }
    if(numDevice >=1) GPU0_memory_usage_gauge_.Set(values[0]/MtoB);
    if(numDevice >=2) GPU1_memory_usage_gauge_.Set(values[1]/MtoB);
    if(numDevice >=3) GPU2_memory_usage_gauge_.Set(values[2]/MtoB);
    if(numDevice >=4) GPU3_memory_usage_gauge_.Set(values[3]/MtoB);
    if(numDevice >=5) GPU4_memory_usage_gauge_.Set(values[4]/MtoB);
    if(numDevice >=6) GPU5_memory_usage_gauge_.Set(values[5]/MtoB);
    if(numDevice >=7) GPU6_memory_usage_gauge_.Set(values[6]/MtoB);
    if(numDevice >=8) GPU7_memory_usage_gauge_.Set(values[7]/MtoB);

    // to do
}
void PrometheusMetrics::AddVectorsPerSecondGaugeSet(int num_vector, int dim, double time) {
    // MB/s
    if(!startup_) return;

    long long MtoB = 1024*1024;
    long long size = num_vector * dim * 4;
    add_vectors_per_second_gauge_.Set(size/time/MtoB);

}
void PrometheusMetrics::QueryIndexTypePerSecondSet(std::string type, double value) {
    if(!startup_) return;
    if(type == "IVF"){
        query_index_IVF_type_per_second_gauge_.Set(value);
    } else if(type == "IDMap"){
        query_index_IDMAP_type_per_second_gauge_.Set(value);
    }

}
void PrometheusMetrics::ConnectionGaugeIncrement() {
    if(!startup_) return;
    connection_gauge_.Increment();
}
void PrometheusMetrics::ConnectionGaugeDecrement() {
    if(!startup_) return;
    connection_gauge_.Decrement();
}

//void PrometheusMetrics::GpuPercentInit() {
//    int num_device = SystemInfo::GetInstance().num_device();
//    constexpr char device_number[] = "DeviceNum";
//    for(int i = 0; i < num_device; ++ i) {
//        GPU_percent_gauges_.emplace_back(GPU_percent_.Add({{device_number,std::to_string(i)}}));
//    }
//
//}
//void PrometheusMetrics::GpuMemoryInit() {
//    int num_device = SystemInfo::GetInstance().num_device();
//    constexpr char device_number[] = "DeviceNum";
//    for(int i = 0; i < num_device; ++ i) {
//        GPU_memory_usage_gauges_.emplace_back(GPU_memory_usage_.Add({{device_number,std::to_string(i)}}));
//    }
//}


}
}
}
