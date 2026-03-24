# Low-Latency C++ Inter-Core Signaling Benchmark
**Performance: 300ns Minimum Latency**

This project demonstrates a high-performance "Sniper/Guard" architecture designed for High-Frequency Trading (HFT) environments. It focuses on minimizing inter-thread communication latency using hardware-level optimizations.

### Key Features:
* **Lock-Free Synchronization:** Uses `std::atomic` with `memory_order_acquire` and `memory_order_release` to eliminate mutex overhead.
* **Thread Affinity (CPU Pinning):** Implements `SetThreadAffinityMask` to isolate execution threads to specific physical cores, reducing OS context-switching.
* **Cache Warming:** Includes a pre-execution warm-up sequence to ensure the CPU remains in a high-performance "Turbo" state and data resides in the L1 cache.
* **Thermal Management:** Logic accounts for thermal throttling constraints on fanless hardware.

### Benchmark Results:
| Metric | Result |
| :--- | :--- |
| **Minimum Latency** | **300 ns** |
| **Architecture** | Lock-Free Atomic Fuse |
| **Hardware** | Lenovo IdeaPad (Fanless) |

> *Note: While Windows OS jitter and thermal limits affect average latency, the 300ns minimum reflects the raw efficiency of the underlying C++ implementation.*
