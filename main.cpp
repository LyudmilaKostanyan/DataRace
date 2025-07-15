#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include <string>
#include <iomanip>

constexpr int NUM_INCREMENTS = 1'000'000;
constexpr int TIMEOUT_SECONDS = 5;

int counter_unsync = 0;
int counter_mutex = 0;
std::atomic<int> counter_atomic{0};
std::mutex mtx;

void thread_race() {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        ++counter_unsync; // race condition
    }
}

void thread_mutex() {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        ++counter_mutex;
    }
}

void thread_atomic() {
    for (int i = 0; i < NUM_INCREMENTS; ++i) {
        ++counter_atomic;
    }
}

struct Result {
    std::string name;
    int final_counter;
    double time_seconds;
};

Result run_test(const std::string& name, void(*func)()) {
    auto start = std::chrono::high_resolution_clock::now();
    std::thread t1(func);
    std::thread t2(func);
    t1.join();
    t2.join();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    int result = (name == "race") ? counter_unsync :
                 (name == "mutex") ? counter_mutex :
                 counter_atomic.load();

    return Result{name, result, duration.count()};
}

int main() {
    std::vector<Result> results;

    results.push_back(run_test("race", thread_race));
    results.push_back(run_test("mutex", thread_mutex));
    results.push_back(run_test("atomic", thread_atomic));

    std::cout << std::left << std::setw(12) << "Test"
              << std::setw(20) << "Final Counter"
              << std::setw(15) << "Time (s)"
              << "Comment" << "\n";
    std::cout << std::string(60, '-') << "\n";

    for (const auto& r : results) {
        std::string comment = (r.name == "race" && r.final_counter != 2 * NUM_INCREMENTS)
                                ? "Race Detected" : "OK";
        std::cout << std::left << std::setw(12) << r.name
                  << std::setw(20) << r.final_counter
                  << std::setw(15) << std::fixed << std::setprecision(5) << r.time_seconds
                  << comment << "\n";
    }

    return 0;
}