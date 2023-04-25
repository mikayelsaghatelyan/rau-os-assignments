#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <semaphore.h>

class Storage {
public:
    Storage(int size) : size(size), count(0) {
        sem_init(&mutex, 0, 1);
        sem_init(&empty, 0, size);
        sem_init(&full, 0, 0);
    }

    void insert_item(int item) {
        sem_wait(&empty);
        sem_wait(&mutex);

        buffer.push_back(item);
        count++;
        std::cout << "Producer produced: " << item << std::endl;

        sem_post(&mutex);
        sem_post(&full);
    }

    int remove_item() {
        int item;

        sem_wait(&full);
        sem_wait(&mutex);

        item = buffer.front();
        buffer.erase(buffer.begin());
        count--;
        std::cout << "Consumer consumed: " << item << std::endl;

        sem_post(&mutex);
        sem_post(&empty);

        return item;
    }

private:
    std::vector<int> buffer;
    int size;
    int count;
    sem_t mutex;
    sem_t empty;
    sem_t full;
};

void producer(Storage &storage, int producer_id) {
    for (int i = 0; i < 5; ++i) {
        int item = producer_id * 10 + i;
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 500));
        storage.insert_item(item);
    }
}

void consumer(Storage &storage) {
    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 500));
        storage.remove_item();
    }
}

int main() {
    int N = 10;
    int M = 2;
    int K = 2;

    Storage storage(N);

    std::vector<std::thread> producers(M);
    std::vector<std::thread> consumers(K);

    for (int i = 0; i < M; ++i) {
        producers[i] = std::thread(producer, std::ref(storage), i);
    }

    for (int i = 0; i < K; ++i) {
        consumers[i] = std::thread(consumer, std::ref(storage));
    }

    for (int i = 0; i < M; ++i) {
        producers[i].join();
    }

    for (int i = 0; i < K; ++i) {
        consumers[i].join();
    }

    return 0;
}