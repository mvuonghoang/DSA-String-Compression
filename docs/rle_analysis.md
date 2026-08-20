# Section 3.1: Run-Length Encoding (RLE) - Algorithm Analysis

## 1. Introduction & Real-World Applications

**Run-Length Encoding (RLE)** is one of the foundational and simplest forms of lossless data compression algorithms. It operates on the principle of **data redundancy reduction** by replacing consecutive repeated sequences of identical data elements (called a *run*) with a single data value and a count of its repetitions.

* **Core Principle**: Consecutive identical bytes are grouped into runs. In basic RLE, every run is stored as a `(count, value)` pair. However, standard byte-pair RLE suffers from severe **file expansion** (up to 100%) when applied to non-repeating data. To resolve this, our implementation adopts the **PackBits / Bit-Flag Standard**, which uses a control byte bit-flag (Bit 7) to dynamically distinguish between:
  1. **Repeat Runs**: Sequences of 3 to 128 identical bytes $\rightarrow$ encoded as 1 control byte + 1 symbol byte ($2\text{ bytes}$ total).
  2. **Literal Runs**: Sequences of 1 to 128 unrepeated bytes $\rightarrow$ encoded as 1 control byte + $L$ raw symbol bytes ($L + 1\text{ bytes}$ total).

* **Real-World Applications**:
  * **Image Formats (BMP, PCX, TGA, TIFF)**: RLE is extensively used to compress 8-bit paletted images, 1-bit line art, and computer-generated graphics with large areas of flat solid color.
  * **Fax Machines (CCITT Group 3/4)**: Encodes long horizontal runs of white and black pixels across scanned physical documents.
  * **Video & Image Codecs (JPEG, MPEG)**: Used as a post-processing step to encode runs of zero AC coefficients following Discrete Cosine Transform (DCT) quantization and Zig-Zag scanning.

---

## 2. Language-Agnostic Pseudocode

### 2.1 Compression Phase (PackBits RLE)

```text
Algorithm RLE_Compress(InputBytes):
    Input:  A sequence of N bytes InputBytes
    Output: Compressed binary file containing header + encoded RLE chunks

    1. WriteHeader(OutputBuffer, Magic = "RLE\1", OriginalLength = N)
    2. Pos ← 0

    3. While Pos < N:
           // Step A: Check for consecutive repeated bytes (Repeat Run)
           RunLen ← 1
           While (Pos + RunLen < N) and (RunLen < 128) and (InputBytes[Pos + RunLen] == InputBytes[Pos]):
               RunLen ← RunLen + 1

           If RunLen ≥ 3:
               ControlByte ← 0x80 | (RunLen - 1)   // Bit 7 = 1 indicates Repeat Run
               WriteByte(OutputBuffer, ControlByte)
               WriteByte(OutputBuffer, InputBytes[Pos])
               Pos ← Pos + RunLen
           Else:
               // Step B: Collect non-repeating bytes (Literal Run)
               LitLen ← 0
               While (Pos + LitLen < N) and (LitLen < 128):
                   // Peek ahead: stop literal run if a 3+ repeat run begins
                   AheadRun ← CountConsecutiveRepeats(InputBytes, Pos + LitLen)
                   If AheadRun ≥ 3:
                       Break
                   LitLen ← LitLen + 1

               ControlByte ← 0x00 | (LitLen - 1)   // Bit 7 = 0 indicates Literal Run
               WriteByte(OutputBuffer, ControlByte)
               WriteBytes(OutputBuffer, InputBytes[Pos ... Pos + LitLen - 1])
               Pos ← Pos + LitLen

    4. Flush(OutputBuffer)
    5. Return OutputBuffer
```

### 2.2 Decompression Phase (PackBits RLE)

```text
Algorithm RLE_Decompress(InputBitStream):
    Input:  Compressed binary file with serialized header
    Output: Reconstructed original bytes

    1. ReadHeader(InputBitStream) → Verify Magic ("RLE\1"), OriginalLength
    2. DecodedBytes ← 0
    3. OutputBuffer ← EmptyList()

    4. While DecodedBytes < OriginalLength:
           ControlByte ← ReadByte(InputBitStream)
           IsRepeat ← (ControlByte AND 0x80) != 0
           Count ← (ControlByte AND 0x7F) + 1

           If IsRepeat:
               ByteVal ← ReadByte(InputBitStream)
               For i from 1 to Count:
                   OutputBuffer.Append(ByteVal)
               DecodedBytes ← DecodedBytes + Count
           Else:
               For i from 1 to Count:
                   ByteVal ← ReadByte(InputBitStream)
                   OutputBuffer.Append(ByteVal)
               DecodedBytes ← DecodedBytes + Count

    5. Return OutputBuffer
```

---

## 3. Step-by-Step Trace on Running Example

We perform step-by-step traces using both the group standardized running example **`"BABBACAC"`** ($N = 8$ bytes) and a highly repetitive sequence **`"AAAAAAABBB"`** ($N = 10$ bytes).

### Case A: Standard Running Example `"BABBACAC"` ($N = 8$ bytes)

#### Step 3.1: Run Analysis & Scanning
Input string: `B - A - B - B - A - C - A - C`

* At `Pos = 0` (`'B'`): Next byte is `'A'` (No repeat run $\ge 3$).
* At `Pos = 2` (`'B'`): Next byte is `'B'` (Repeat run length = 2, below threshold 3).
* **Conclusion**: Entire string `"BABBACAC"` contains no consecutive repeated runs of length $\ge 3$.

#### Step 3.2: PackBits Encoding
* The algorithm groups all 8 characters into a single **Literal Run**:
  * `LitLen = 8`
  * `ControlByte = 0x00 | (8 - 1) = 0x07` (Binary: `00000111`).
* **Output Payload Stream**: `0x07` followed by raw characters `'B', 'A', 'B', 'B', 'A', 'C', 'A', 'C'`.
* Total Payload Bytes: **9 bytes** (1 control byte + 8 literal data bytes).

#### Step 3.3: Comparison Between Basic RLE vs PackBits RLE
| Algorithm Variant | Output Byte Stream Representation | Total Size | Expansion / Savings |
| :--- | :--- | :---: | :---: |
| **Uncompressed Raw** | `'B', 'A', 'B', 'B', 'A', 'C', 'A', 'C'` | **8 bytes** | Baseline |
| **Basic Byte-Pair RLE** | `[1,'B'], [1,'A'], [2,'B'], [1,'A'], [1,'C'], [1,'A'], [1,'C']` | **14 bytes** | **+75.0% Expansion** ⚠️ |
| **PackBits Flag RLE** | `[0x07], 'B', 'A', 'B', 'B', 'A', 'C', 'A', 'C'` | **9 bytes** | **+12.5% Overhead** (1 byte) |

---

### Case B: Repetitive Sequence `"AAAAAAABBB"` ($N = 10$ bytes)

#### Step 3.1: Run Analysis & Scanning
1. **Run 1**: Symbol `'A'` repeats **7 times** at `Pos = 0..6`.
   * `RunLen = 7` ($\ge 3$).
   * `ControlByte = 0x80 | (7 - 1) = 0x86` (Binary: `10000110`).
   * Output: `0x86`, `'A'`.
2. **Run 2**: Symbol `'B'` repeats **3 times** at `Pos = 7..9`.
   * `RunLen = 3` ($\ge 3$).
   * `ControlByte = 0x80 | (3 - 1) = 0x82` (Binary: `10000010`).
   * Output: `0x82`, `'B'`.

#### Step 3.2: Encoded Payload Stream
* **Compressed Bytes**: `[0x86, 'A', 0x82, 'B']` (Total: **4 bytes**).
* **Space Savings**:
  $$\text{Space Savings} = \left(1 - \frac{4}{10}\right) \times 100\% = 60.0\%$$

---

## 4. Complexity Analysis

Let $N$ be the length of the input data in bytes.

| Phase | Time Complexity | Space Complexity | Description |
| :--- | :--- | :--- | :--- |
| **Run-Length Scanning** | $O(N)$ | $O(1)$ | Single linear scan over input buffer to identify repeat and literal boundaries. |
| **Data Encoding** | $O(N)$ | $O(N)$ | Writing control bytes and literal/repeat byte payloads to disk/memory. |
| **Decompression** | $O(N)$ | $O(N)$ | Direct byte copies into output buffer based on control byte length counts. |
| **Overall** | **$O(N)$** | **$O(1)$ Auxiliary** | Strictly linear time $O(N)$ with minimal $O(1)$ extra memory overhead during execution. |

---

## 5. Critical Discussion & Performance Boundaries

1. **Best-Case Scenario**:
   - Highly repetitive contiguous byte sequences (e.g. solid-color image buffers or uninitialized memory dumps, e.g., 10MB of consecutive `'A'` bytes).
   - In PackBits RLE, a run of 128 identical bytes is compressed into **2 bytes** (1 control byte `0x8F` + 1 value byte `'A'`).
   - For $N = 10\text{ MB}$, compressed size is:
     $$\text{Compressed Size} \approx \frac{10,485,760}{128} \times 2 \approx 163,840 \text{ bytes} \approx 160 \text{ KB}$$
   - Compression Ratio: $\approx \mathbf{64.0\times}$ (Space Savings: $\mathbf{98.4\%}$).

2. **Worst-Case Scenario (Negative Compression / File Expansion Boundary)**:
   - High-entropy, non-repeating data (e.g. encrypted files, compressed archives, or uniform random binary data `0x00`-`0xFF`).
   - Under basic byte-pair RLE `[count][symbol]`, every unrepeated byte becomes 2 bytes, causing a catastrophic **100% file expansion** ($N \rightarrow 2N$).
   - Under PackBits Flag RLE, non-repeating sequences are packed into Literal Runs of up to 128 bytes with only 1 control byte overhead per 128 bytes. The worst-case file expansion is strictly bounded by:
     $$\text{Expansion}_{\text{max}} = \frac{N}{128} \text{ bytes} \approx +\mathbf{0.78\%}$$
   - This mathematical guarantee ensures that PackBits RLE will never blow up file sizes, even on worst-case random binary inputs.

3. **Algorithm Limitations & Trade-offs**:
   - **No Multi-Byte Pattern Matching**: RLE can only compress single-character consecutive runs. Sub-string patterns such as `"ABCABCABCABC"` contain no single-character repeats, resulting in 0% compression for RLE, whereas dictionary-based algorithms (**LZW** or **Deflate / LZ77**) compress such multi-byte patterns easily.
