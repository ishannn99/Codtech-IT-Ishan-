#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <cassert>

// Thread-safe RLE compression for a chunk
std::string rleCompress(const std::string &data) {
    if (data.empty()) return "";
    
    std::string compressed;
    compressed.reserve(data.size() * 1.1); // Pre-allocate space
    
    char current = data[0];
    int count = 1;
    
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] == current && count < 255) {
            count++;
        } else {
            compressed += current;
            compressed += static_cast<char>(count);
            current = data[i];
            count = 1;
        }
    }
    compressed += current;
    compressed += static_cast<char>(count);
    
    return compressed;
}

// RLE decompression
std::string rleDecompress(const std::string &data) {
    if (data.empty()) return "";
    if (data.size() % 2 != 0) throw std::runtime_error("Invalid RLE data");
    
    std::string decompressed;
    size_t estimated_size = 0;
    for (size_t i = 1; i < data.size(); i += 2) {
        estimated_size += static_cast<unsigned char>(data[i]);
    }
    decompressed.reserve(estimated_size);
    
    for (size_t i = 0; i < data.size(); i += 2) {
        char ch = data[i];
        int count = static_cast<unsigned char>(data[i+1]);
        decompressed.append(count, ch);
    }
    
    return decompressed;
}

// Process file in chunks with thread-safe writing
void processFile(const std::string &inputPath, 
                const std::string &outputPath,
                bool compress,
                int threadCount) {
    
    // Read entire file
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile) throw std::runtime_error("Cannot open input file");
    
    std::string input((std::istreambuf_iterator<char>(inFile)), 
                     std::istreambuf_iterator<char>());
    
    // Prepare output file
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) throw std::runtime_error("Cannot open output file");
    
    // Split work
    size_t chunkSize = input.size() / threadCount;
    std::vector<std::thread> threads;
    std::vector<std::string> results(threadCount);
    std::mutex writeMutex;
    
    auto worker = [&](int threadIdx) {
        size_t start = threadIdx * chunkSize;
        size_t end = (threadIdx == threadCount - 1) ? input.size() : (threadIdx + 1) * chunkSize;
        
        // Adjust chunk boundaries to avoid splitting RLE sequences
        if (threadIdx > 0) {
            while (start < input.size() && input[start] == input[start-1]) {
                start++;
            }
        }
        
        std::string chunk(input.begin() + start, input.begin() + end);
        results[threadIdx] = compress ? rleCompress(chunk) : rleDecompress(chunk);
    };
    
    // Start threads
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(worker, i);
    }
    
    // Wait for completion
    for (auto &t : threads) {
        t.join();
    }
    
    // Write results
    for (auto &result : results) {
        outFile << result;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << (compress ? "Compression" : "Decompression") 
              << " with " << threadCount << " threads took: " 
              << duration.count() << " ms\n";
}

// Single-threaded version for comparison
void processFileSingleThread(const std::string &inputPath,
                           const std::string &outputPath,
                           bool compress) {
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile) throw std::runtime_error("Cannot open input file");
    
    std::string input((std::istreambuf_iterator<char>(inFile)), 
                     std::istreambuf_iterator<char>());
    
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) throw std::runtime_error("Cannot open output file");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    std::string result = compress ? rleCompress(input) : rleDecompress(input);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    outFile << result;
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    std::cout << (compress ? "Compression" : "Decompression") 
              << " (single-threaded) took: " 
              << duration.count() << " ms\n";
}

void validateFiles(const std::string &original, const std::string &decompressed) {
    std::ifstream origFile(original, std::ios::binary);
    std::ifstream decFile(decompressed, std::ios::binary);
    
    std::string origContent((std::istreambuf_iterator<char>(origFile)), 
                std::istreambuf_iterator<char>());
    std::string decContent((std::istreambuf_iterator<char>(decFile)), 
                std::istreambuf_iterator<char>());
    
    if (origContent == decContent) {
        std::cout << "Validation successful: files match\n";
    } else {
        std::cout << "Validation failed: files differ\n";
        std::cout << "Original size: " << origContent.size() << "\n";
        std::cout << "Decompressed size: " << decContent.size() << "\n";
    }
}

int main() {
    try {
        const std::string inputFile = "input.txt";
        const std::string compressedFile = "compressed.rle";
        const std::string decompressedFile = "decompressed.txt";
        const int threadCount = 4; // Adjust based on your CPU cores
        
        // Generate test file if needed
        {
            std::ofstream testFile(inputFile);
            for (int i = 0; i < 100000; i++) {
                testFile << "This is a test line with some repeated characters aaaaaaaand some more...\n";
            }
        }
        
        std::cout << "=== Single-threaded ===\n";
        processFileSingleThread(inputFile, compressedFile, true);
        processFileSingleThread(compressedFile, decompressedFile, false);
        validateFiles(inputFile, decompressedFile);
        
        std::cout << "\n=== Multi-threaded (" << threadCount << " threads) ===\n";
        processFile(inputFile, compressedFile, true, threadCount);
        processFile(compressedFile, decompressedFile, false, threadCount);
        validateFiles(inputFile, decompressedFile);
        
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}