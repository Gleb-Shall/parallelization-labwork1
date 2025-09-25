## Parallelization LabWork1

Shikunov Gleb Aleksandrovich, 24.B81-mm

# Contacts
st128274@student.spbu.ru

## Description
Parallelization LabWork1

## Build

### Standard Version
```bash
make clean
make all
```

### Optimized Version
```bash
make clean
make bmp_processor_optimized
```

### Both Versions
```bash
make build-all
```

## Usage

### Basic Usage
```bash
# Standard version
./bmp_processor example.bmp

# Optimized version
./bmp_processor_optimized example.bmp
```

### Parallel Processing
```bash
# Automatic thread count detection
./bmp_processor_optimized -p example.bmp

# Specify exact thread count
./bmp_processor_optimized -p -t 8 example.bmp
```

### Benchmarks
```bash
# Basic benchmark
./bmp_processor_optimized --benchmark

# Advanced scalability benchmark
./bmp_processor_optimized --advanced
```

## Testing

### Memory Check
```bash
make memcheck
```

