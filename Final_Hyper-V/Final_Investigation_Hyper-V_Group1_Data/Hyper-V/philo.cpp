#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <memory> // Necesario para std::unique_ptr

const int NUM_PHILOSOPHERS = 5;

class Fork {
public:
    std::mutex mtx;
};

class Philosopher {
private:
    int id;
    Fork& left_fork;
    Fork& right_fork;
    std::thread th;

    void dine() {
        while (true) {
            think();
            eat();
        }
    }

    void think() {
        std::cout << "Philosopher " << id << " is thinking.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    void eat() {
        std::unique_lock<std::mutex> left_lock(left_fork.mtx, std::defer_lock);
        std::unique_lock<std::mutex> right_lock(right_fork.mtx, std::defer_lock);

        std::lock(left_lock, right_lock);

        std::cout << "Philosopher " << id << " is eating.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

public:
    Philosopher(int id, Fork& left_fork, Fork& right_fork)
        : id(id), left_fork(left_fork), right_fork(right_fork),
          th(&Philosopher::dine, this) {}

    ~Philosopher() {
        if (th.joinable()) {
            th.join();
        }
    }

    bool isJoinable() {
        return th.joinable();
    }

    void join() {
        if (th.joinable()) {
            th.join();
        }
    }
};

int main() {
    std::vector<Fork> forks(NUM_PHILOSOPHERS);
    std::vector<std::unique_ptr<Philosopher>> philosophers;

    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers.push_back(std::unique_ptr<Philosopher>(
            new Philosopher(i, forks[i], forks[(i + 1) % NUM_PHILOSOPHERS])));
    }

    for (auto& philosopher : philosophers) {
        if (philosopher->isJoinable()) {
            philosopher->join();
        }
    }

    return 0;
}