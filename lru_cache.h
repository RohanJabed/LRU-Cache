#pragma once
#include <unordered_map>
#include <chrono>
#include <stdexcept>
using namespace std;

struct Node {
    int key, value;
    Node* prev = nullptr;
    Node* next = nullptr;
    Node(int k, int v) : key(k), value(v) {}
};

class Cache {
public:
    explicit Cache(int capacity) : capacity(capacity) {
        if (capacity <= 0) throw invalid_argument("Capacity must be positive");
        head = new Node(0, 0);
        tail = new Node(0, 0);
        head->next = tail;
        tail->prev = head;
    }

    virtual ~Cache() {
        Node* cur = head;
        while (cur) { Node* nxt = cur->next; delete cur; cur = nxt; }
    }

    virtual int get(int key) {
        auto it = map.find(key);
        if (it == map.end()) return -1;
        _remove(it->second);
        _insertAtTail(it->second);
        return it->second->value;
    }

    virtual void put(int key, int value) {
        auto it = map.find(key);
        if (it != map.end()) { _remove(it->second); delete it->second; map.erase(it); }
        Node* node = new Node(key, value);
        _insertAtTail(node);
        map[key] = node;
        if ((int)map.size() > capacity) {
            Node* lru = head->next;
            _remove(lru);
            map.erase(lru->key);
            delete lru;
        }
    }

protected:
    void _remove(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void _insertAtTail(Node* node) {
        node->prev = tail->prev;
        node->next = tail;
        tail->prev->next = node;
        tail->prev = node;
    }

    int capacity;
    unordered_map<int, Node*> map;
    Node* head;
    Node* tail;
};

// ─── TTL Cache ────────────────────────────────────────────────────────────────
class TTLCache : public Cache {
    using ms = chrono::milliseconds;
    using clock = chrono::steady_clock;

    long long now() {
        return chrono::duration_cast<ms>(clock::now().time_since_epoch()).count();
    }

public:
    explicit TTLCache(int capacity) : Cache(capacity) {}

    void put(int key, int value, long long ttlMs = -1) {
        Cache::put(key, value);
        if (ttlMs >= 0) ttlMap[key] = now() + ttlMs;
        else            ttlMap.erase(key);
    }

    int get(int key) override {
        auto it = ttlMap.find(key);
        if (it != ttlMap.end() && now() > it->second) {
            auto node = map.find(key);
            if (node != map.end()) { _remove(node->second); delete node->second; map.erase(node); }
            ttlMap.erase(it);
            return -1;
        }
        return Cache::get(key);
    }

private:
    unordered_map<int, long long> ttlMap;
};
