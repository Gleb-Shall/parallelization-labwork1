# Optimization and CI Setup Report

## Completed Tasks

 1. CI/CD Pipeline Setup with GitHub Actions

 2. Parallel Computing Optimization

 3. Functionality Testing

 4. Automatic Performance Benchmarks

## Optimization Results

### Performance (measurements on 640×426 pixel image)

#### Full pipeline execution time:
- **Standard version**: 19 ms (0.434s total time)
- **Optimized version (sequential)**: 10 ms (0.185s total time)
- **Optimized version (4 threads)**: 11 ms (0.011s total time)
- **Overall speedup**: **1.73x** (standard → optimized with 4 threads)

#### Detailed operation measurements:

**Image rotation:**
- Standard version: ~3.3 ms
- Optimized (4 threads): ~2.2 ms (532+1624 μs)
- Best result: **1.76x** speedup on 3 threads

**Gaussian filter:**
- Standard version: ~7 ms (4+3 ms for two applications)
- Optimized (4 threads): ~1.8 ms (762+1032 μs)
- Best result: **3.39x** speedup on 1 thread

#### Thread scalability:
| Threads | Rotation | Filter | CPU Usage |
|---------|----------|--------|-----------|
| 1       | 1.29x    | 3.39x  | 6%        |
| 2       | 1.58x    | 3.10x  | ~85%      |
| 3       | 1.76x    | 3.07x  | ~120%     |
| 4       | 1.58x    | 2.17x  | 170%      |

**Optimal configuration**: 2-3 threads for maximum efficiency

### **Memory Usage:**
- Standard version: 818,024 bytes (0.78 MB)
- Optimized version: 818,040 bytes (0.78 MB)
- Memory increase: +16 bytes (0.002% increase)

### CI/CD
- Automatic build on Ubuntu
- Build with 2 compilers (GCC, Clang)
- Security checking
- Automatic benchmarks
- Memory checking with valgrind

## Usage Commands

### Build
```bash
# Standard version
make clean && make all

# Optimized version
make clean && make bmp_processor_optimized

# Both versions
make build-all
```

### Testing
```bash
# Memory check
make memcheck

# Benchmarks
make benchmark
make benchmark-optimized
```

### Usage
```bash
# Basic usage
./bmp_processor example.bmp
./bmp_processor_optimized example.bmp

# Parallel processing
./bmp_processor_optimized -p -t 4 example.bmp

# Advanced benchmarks
./bmp_processor_optimized --advanced
```


### Testing commands:
```bash
# Basic testing
./bmp_processor_optimized -p -t 2 example.bmp

# Advanced benchmark
./bmp_processor_optimized --advanced

# Version comparison
time ./bmp_processor example.bmp
time ./bmp_processor_optimized -p -t 2 example.bmp
```

---
**Author**: Gleb Shikunov  
**Contacts**: st128274@student.spbu.ru