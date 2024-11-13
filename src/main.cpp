#include "cache.h"
#include "common.h"
#include <iostream>
#include <string>

int main(int argc, char **argv) {
    // Default values for policy and prefetcher
    std::string policy = "LRU";
    bool prefetcher_enabled = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.find("--policy=") == 0) {
            policy = arg.substr(9);
        } else if (arg == "--prefetcher=enabled") {
            prefetcher_enabled = true;
        } else if (arg == "--prefetcher=disabled") {
            prefetcher_enabled = false;
        }
    }

    // Initialize cache with the specified replacement policy and prefetcher setting
    initCache();
    if (policy == "LRU") {
        registerPolicy(CacheSimulation::Policy::LRU);
    } else if (policy == "RANDOM") {
        registerPolicy(CacheSimulation::Policy::RANDOM);
    } else if (policy == "TREELRU") {
        registerPolicy(CacheSimulation::Policy::TREELRU);
    } else {
        std::cerr << "Unknown policy: " << policy << std::endl;
        return 1;
    }

    if (prefetcher_enabled) {
        enablePrefetcher();
    }

    // Set up matrix multiplication and convolution arguments
    matrix_mult_args ma;
    convolution_args ca;
    initMatrixMultiplicationWithDimensions(256, ma);
    initConvolutionWithDimensions(256, ca);

    // Run base and optimized versions of convolution and matrix multiplication

    // Base Convolution
    baseConvolution(ca.dim, ca.src, ca.ker, ca.dst);
    copyBaseDst(ca);
    std::cout << "Base Convolution Stats:" << std::endl;
    printStats();
    std::cout << "=======================" << std::endl;
    resetCacheStats();

    // Optimized Convolution
    convolution(ca.dim, ca.src, ca.ker, ca.dst);
    checkBuffers(ca);
    std::cout << "Your Convolution Stats:" << std::endl;
    printStats();
    std::cout << "=======================" << std::endl;
    resetCacheStats();

    // Base Matrix Multiplication
    baseMatrixMultiplication(ma.dim, ma.src, ma.src2, ma.dst);
    copyBaseDst(ma);
    std::cout << "Base Matrix Multiplication Stats:" << std::endl;
    printStats();
    std::cout << "=================================" << std::endl;
    resetCacheStats();

    // Optimized Matrix Multiplication
    matrixMultiplication(ma.dim, ma.src, ma.src2, ma.dst);
    checkBuffers(ma);
    std::cout << "Your Matrix Multiplication Stats:" << std::endl;
    printStats();
    std::cout << "=================================" << std::endl;
    resetCacheStats();

    return 0;
}
