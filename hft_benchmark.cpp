#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <windows.h> // Essential for SetThreadAffinityMask

struct MarketPacket {
    uint64_t seq_num;
    double price;
};

// Global Atomics for the "Handshake"
std::atomic<bool> risk_fuse{true};
std::atomic<long long> start_time_ns{0};
std::atomic<long long> end_time_ns{0};

// --- CORE 2: THE GUARD ---
void run_risk_inspector(MarketPacket* p) {
    // Artificial delay to simulate market movement
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // START THE CLOCK
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    start_time_ns.store(now);
    
    // BLOW THE FUSE
    risk_fuse.store(false, std::memory_order_release);
}

// --- CORE 1: THE SNIPER ---
void run_execution_sniper(MarketPacket* p) {
    while (true) {
        if (!risk_fuse.load(std::memory_order_acquire)) {
            // STOP THE CLOCK
            auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            end_time_ns.store(now);
            break;
        }
    }
}

// 2. THE WARM_CACHE FUNCTION (Above main)
void warm_cache(MarketPacket* p) {
    for(int i = 0; i < 10000; i++) {
        p->seq_num += i;
        p->price += 0.01;
        volatile int dummy = p->seq_num; 
    }
}

int main() {
    MarketPacket my_packet{1, 155.50};

    // CALL THE WARM-UP HERE (Before the threads start)
    std::cout << "Warming up CPU cache..." << std::endl;
    warm_cache(&my_packet);

    const int iterations = 100;
    long long latencies[iterations];

    std::cout << "Running 100-iteration Benchmark..." << std::endl;

    for (int i = 0; i < iterations; i++) {
        risk_fuse.store(true); // Reset for next run
        
        std::thread t1(run_execution_sniper, &my_packet);
        std::thread t2(run_risk_inspector, &my_packet);

        SetThreadAffinityMask((HANDLE)t1.native_handle(), 1 << 0);
        SetThreadAffinityMask((HANDLE)t2.native_handle(), 1 << 1);

        t1.join();
        t2.join();

        latencies[i] = end_time_ns.load() - start_time_ns.load();
    }

    long long min_lat = latencies[1]; // Skip the 1st "cold" run
    long long total = 0;
    for (int i = 1; i < iterations; i++) {
        if (latencies[i] < min_lat) min_lat = latencies[i];
        total += latencies[i];
    }

    std::cout << "--------------------------------------" << std::endl;
    std::cout << "Average Latency: " << (total / (iterations - 1)) << " ns" << std::endl;
    std::cout << "Minimum (Best) Latency: " << min_lat << " ns" << std::endl;
    std::cout << "--------------------------------------" << std::endl;

    return 0;
}