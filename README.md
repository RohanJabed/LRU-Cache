# LRU Cache

A Least Recently Used (LRU) Cache implementation in C++ with O(1) `get` and `put` operations.

## Data Structures Used

- **HashMap** (`std::unordered_map`): Provides O(1) key lookup to find any node instantly.
- **Doubly Linked List**: Maintains access order. The head side holds the least recently used item; the tail side holds the most recently used item. Two dummy sentinel nodes (`head` and `tail`) eliminate edge-case checks on empty list boundaries.

## How LRU Ordering Is Maintained

Every `get(key)` and `put(key, value)` call moves the accessed/inserted node to the **tail** (most recently used position):

- `get`: removes the node from its current position → re-inserts at tail.
- `put` (new key): creates a new node → inserts at tail. If over capacity, removes `head->next` (the least recently used node) and deletes it from the map.
- `put` (existing key): removes the old node → inserts updated node at tail.

Both `_remove` and `_insertAtTail` are pointer operations on a doubly linked list, running in **O(1)** time.

## Time & Space Complexity

| Operation | Time | Space |
|-----------|------|-------|
| `get`     | O(1) | —     |
| `put`     | O(1) | —     |
| Overall   | —    | O(n) where n = capacity |

## How to Run

Requires a C++17 compiler (e.g. `g++`).

```bash
g++ -std=c++17 -o test_lru test.cpp && ./test_lru
```

## Example Output

```
=== Assignment Example ===
✅ cache.get(A): got 10, expected 10
✅ cache.get(B): got -1, expected -1
✅ cache.get(C): got 30, expected 30
✅ cache.get(A): got 10, expected 10

=== Additional Tests ===
✅ update existing key: got 99, expected 99
✅ capacity 1 - A evicted: got -1, expected -1
✅ capacity 1 - B present: got 2, expected 2
✅ get promotes - B evicted: got -1, expected -1
✅ get promotes - A still present: got 1, expected 1

=== TTL Bonus ===
✅ TTL - A present before expiry: got 100, expected 100
✅ TTL - B present: got 200, expected 200
✅ TTL - A expired after 60ms: got -1, expected -1
✅ TTL - B still present: got 200, expected 200
```

## Bonus: TTL / Expiration Support

`TTLCache` extends `Cache` and adds optional per-key TTL (time-to-live in milliseconds):

```cpp
TTLCache cache(3);
cache.put(1, 100, 3600000); // expires in 1 hour
cache.put(2, 200);          // no expiry
```

**Approach:** A separate `ttlMap` stores `key → expiry timestamp` using `std::chrono::steady_clock`. On `get`, if the current time exceeds the expiry, the entry is lazily evicted and `-1` is returned.

**Trade-offs:**
- Lazy eviction means expired keys occupy memory until accessed. For a production system, a background sweep or a min-heap of expiry times would be more memory-efficient.
- No TTL refresh on `get` — the expiry is fixed at `put` time, which is the most common expected behavior.
