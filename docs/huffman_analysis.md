# Section 3.3: Huffman Coding - Algorithm Analysis

## 1. Introduction & Real-World Applications
**Huffman Coding** is a fundamental lossless data compression algorithm developed by David A. Huffman in 1952. It belongs to the class of **greedy algorithms** and constructs optimal **variable-length prefix codes** based on the frequencies of occurrence of each symbol in the input data.

* **Core Principle**: Symbols that appear more frequently are assigned shorter binary codewords, while rarer symbols receive longer codewords. Because it is a *prefix code* (no codeword is a prefix of any other codeword), the encoded bitstream can be uniquely and unambiguously decoded from left to right without special delimiter characters.
* **Real-World Applications**:
  * **JPEG Image Compression**: Huffman coding is used in the final entropy encoding stage after DCT transform and quantization.
  * **DEFLATE / GZIP / ZIP / PNG**: Used in combination with LZ77 to compress literal bytes and distance/length match tokens.
  * **MP3 Audio Format**: Encodes quantized spectral coefficients.

---

## 2. Language-Agnostic Pseudocode

### 2.1 Compression Phase
```text
Algorithm Huffman_Compress(InputText):
    Input:  A sequence of N characters/bytes InputText
    Output: Compressed binary file containing header + encoded bitstream

    1. Frequencies ← Count frequency of each distinct character in InputText
    2. MinHeap ← PriorityQueue()
    
    3. For each unique symbol sym in Frequencies:
           node ← CreateNode(symbol = sym, freq = Frequencies[sym])
           MinHeap.Push(node)
           
    4. While MinHeap.Size > 1:
           left ← MinHeap.PopMin()
           right ← MinHeap.PopMin()
           parent ← CreateNode(freq = left.freq + right.freq, leftChild = left, rightChild = right)
           MinHeap.Push(parent)
           
    5. Root ← MinHeap.PopMin()
    6. CodeTable ← EmptyMap()
    7. GenerateCodes(Root, currentBitString = "", CodeTable)
    
    8. WriteHeader(OutputBitStream, Frequencies, OriginalLength = N)
    9. For each character c in InputText:
           bits ← CodeTable[c]
           OutputBitStream.WriteBits(bits)
           
    10. OutputBitStream.Flush()
    11. Return OutputBitStream
```

### 2.2 Decompression Phase
```text
Algorithm Huffman_Decompress(InputBitStream):
    Input:  Compressed bitstream with serialized header
    Output: Reconstructed original uncompressed text

    1. ReadHeader(InputBitStream) → Frequencies, OriginalLength
    2. Reconstruct MinHeap from Frequencies
    3. Rebuild Huffman Tree (Root) using the exact same greedy combination steps
    
    4. CurrentNode ← Root
    5. DecodedCount ← 0
    6. OutputBuffer ← EmptyList()
    
    7. While DecodedCount < OriginalLength:
           bit ← InputBitStream.ReadBit()
           If bit == 0:
               CurrentNode ← CurrentNode.leftChild
           Else:
               CurrentNode ← CurrentNode.rightChild
               
           If CurrentNode is Leaf:
               OutputBuffer.Append(CurrentNode.symbol)
               DecodedCount ← DecodedCount + 1
               CurrentNode ← Root   // Reset back to root
               
    8. Return OutputBuffer
```

---

## 3. Step-by-Step Trace on Running Example

We use the standardized running example: **`"BABBACAC"`** (Length $N = 8$ characters).

### Step 3.1: Frequency Counting
| Symbol | Frequency | Probability ($P_i$) |
| :---: | :---: | :---: |
| **A** | 3 | $3 / 8 = 0.375$ |
| **B** | 3 | $3 / 8 = 0.375$ |
| **C** | 2 | $2 / 8 = 0.250$ |

### Step 3.2: Min-Heap Construction & Tree Merging
1. **Initial Min-Heap**:
   * Nodes: `[C: 2]`, `[A: 3]`, `[B: 3]`
2. **Iteration 1**:
   * Pop two minimum nodes: `C (freq 2)` and `A (freq 3)`.
   * Create new internal node `N1 (freq = 2 + 3 = 5)` with left child `C` and right child `A`.
   * Push `N1: 5` back to heap.
   * Min-Heap state: `[B: 3]`, `[N1: 5]`.
3. **Iteration 2**:
   * Pop two minimum nodes: `B (freq 3)` and `N1 (freq 5)`.
   * Create new root node `Root (freq = 3 + 5 = 8)` with left child `B` and right child `N1`.
   * Push `Root: 8` back to heap.
   * Min-Heap size is 1 $\rightarrow$ **Done**.

### Step 3.3: Generated Binary Tree & Prefix Codebook
```text
            [Root: 8]
           /         \
       0  /           \  1
      [B: 3]        [N1: 5]
                   /       \
               0  /         \  1
                [C: 2]     [A: 3]
```

* **Codeword Mapping**:
  * `'B'` $\rightarrow$ `0` (1 bit)
  * `'C'` $\rightarrow$ `10` (2 bits)
  * `'A'` $\rightarrow$ `11` (2 bits)

### Step 3.4: Bitstream Encoding
Input string: `B - A - B - B - A - C - A - C`
* Encoded Bits: `0` + `11` + `0` + `0` + `11` + `10` + `11` + `10`
* Final Bitstream: `0110011101110` (Total: **13 bits**).
* **Comparison**:
  * Uncompressed ASCII (8 bits/char): $8 \times 8 = 64\text{ bits}$.
  * Huffman encoded data: $13\text{ bits} \approx 2\text{ bytes}$.
  * Space Savings on payload: $1 - \frac{13}{64} = 79.7\%$.

---

## 4. Complexity Analysis

Let $N$ be the length of the input data in bytes, and $K$ be the number of unique symbols ($K \le 256$ for standard ASCII/byte streams).

| Phase | Time Complexity | Space Complexity | Description |
| :--- | :--- | :--- | :--- |
| **Frequency Counting** | $O(N)$ | $O(K)$ | Single pass over input array. |
| **Tree Construction** | $O(K \log K)$ | $O(K)$ | Inserting $K$ elements and performing $K-1$ extractions on Min-Heap. |
| **Code Generation** | $O(K)$ | $O(K)$ | Recursive tree traversal (tree has $2K - 1$ total nodes). |
| **Data Encoding** | $O(N)$ | $O(N)$ | Looking up prefix codes and writing bits. |
| **Decompression** | $O(N)$ | $O(K)$ | Tree traversal with 1 bit per transition; total steps proportional to $N$. |
| **Overall** | **$O(N + K \log K)$** | **$O(K)$** | Highly efficient and linear $O(N)$ for fixed alphabet $K \le 256$. |

---

## 5. Critical Discussion & Performance Boundaries

1. **Optimality (Shannon Entropy)**:
   * Huffman coding produces an optimal prefix code that achieves the minimum average codeword length $L_{\text{avg}} = \sum P_i \cdot l_i$ for symbol-by-symbol encoding, bounded by:
     $$H(X) \le L_{\text{avg}} < H(X) + 1$$
     where $H(X) = -\sum P_i \log_2 P_i$ is the Shannon Entropy.
2. **Best-Case Scenario**:
   * Highly skewed frequency distributions (e.g. 95% `'A'`, 5% other chars). The dominant symbol gets a 1-bit code, yielding compression ratio $> 7\times$.
3. **Worst-Case Scenario (Negative Compression / File Expansion)**:
   * Uniformly distributed random data (high entropy, $H(X) \approx 8\text{ bits/byte}$). Each character receives approximately 8 bits, meaning the compressed data payload is $\approx N$ bytes, but the required **header overhead** (storing the frequency table / tree) causes the compressed file to be **larger** than the original file.
   * Very small files ($N < 100\text{ bytes}$): Header overhead outweighs payload savings.
