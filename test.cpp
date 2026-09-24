#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "lru_cache.h"
using namespace std;

void test(const string& label, int got, int expected) {
    bool pass = got == expected;
    cout << (pass ? "Done" : "Denied") << label
         << ": got " << got << ", expected " << expected << "\n";
}

int main() {
    // ─── Assignment Example ───────────────────────────────────────────────────
    cout << "=== Assignment Example ===\n";
    Cache cache(2);
    cache.put(1, 10);  // A=1, B=2, C=3 mapped to ints
    cache.put(2, 20);
    test("cache.get(A)", cache.get(1), 10);
    cache.put(3, 30);                          // evicts B
    test("cache.get(B)", cache.get(2), -1);
    test("cache.get(C)", cache.get(3), 30);
    test("cache.get(A)", cache.get(1), 10);

    // ─── Additional Tests ─────────────────────────────────────────────────────
    cout << "\n=== Additional Tests ===\n";

    Cache c2(2);
    c2.put(1, 1); c2.put(1, 99);
    test("update existing key", c2.get(1), 99);

    Cache c3(1);
    c3.put(1, 1); c3.put(2, 2);
    test("capacity 1 - A evicted", c3.get(1), -1);
    test("capacity 1 - B present", c3.get(2), 2);

    Cache c4(2);
    c4.put(1, 1); c4.put(2, 2);
    c4.get(1);         // A becomes MRU, B becomes LRU
    c4.put(3, 3);      // evicts B
    test("get promotes - B evicted",      c4.get(2), -1);
    test("get promotes - A still present", c4.get(1), 1);

    // ─── TTL Bonus ────────────────────────────────────────────────────────────
    cout << "\n=== TTL Bonus ===\n";
    TTLCache ttl(3);
    ttl.put(1, 100, 50);  // expires in 50ms
    ttl.put(2, 200);      // no expiry

    test("TTL - A present before expiry", ttl.get(1), 100);
    test("TTL - B present",               ttl.get(2), 200);

    this_thread::sleep_for(chrono::milliseconds(60));

    test("TTL - A expired after 60ms", ttl.get(1), -1);
    test("TTL - B still present",      ttl.get(2), 200);

    return 0;
}
