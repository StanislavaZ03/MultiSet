# MultiSet

## Description

The `MultiSet` class represents a multiset of numbers within a range from 0 to `n`, where `n` is specified in the constructor. Additionally, the constructor takes a parameter `k`, which defines how many bits are required to store the count of occurrences of a number (1 <= k <= 8). This means that each number can appear in the multiset at most `2^k - 1` times.

The class is optimized for memory usage and supports the following functionalities:

- **Add Number:** Insert a number into the multiset.
- **Count Occurrences:** Check how many times a number is present in the multiset.
- **Print Numbers:** Display all numbers contained in the multiset.
- **Print Memory Representation:** Show how the multiset is represented in memory.
- **Serialization/Deserialization:** Save and load the multiset to/from a binary file.
- **Intersection/Difference:** Compute the intersection or difference between two multisets.
- **Complement:** Compute the complement of the multiset (if a number `x` appears `p` times, in the complement it appears `2^k - 1 - p` times).
