# DataHub C++ SDK

Alibaba Cloud DataHub C++ SDK

## License

This project is licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.

## Requirements

To get started using this sdk, you will need these things:

### Core Dependencies

| Dependency | Description | Required |
|------------|-------------|----------|
| OpenSSL | Crypto and SSL library | Yes |
| libcurl | HTTP client library | Yes |
| protobuf | Protocol Buffers (runtime + compiler) | Yes |
| lz4 | LZ4 compression library | Yes |
| zlib | Compression library | Yes |
| log4cpp | Logging library | Yes (for client) |
| rapidjson | JSON library (header-only) | Yes |

### Build Tools

| Tool | Description | Required |
|------|-------------|----------|
| GCC | C++14 support required (GCC 4.9+ recommended) | Yes |
| CMake | Build system (3.5+) | Yes |
| protoc | Protocol Buffers compiler | Yes |

### Test Dependencies (Optional)

| Dependency | Description | Required |
|------------|-------------|----------|
| gtest | Google Test framework | For unit tests |
| gmock | Google Mock framework | For unit tests |

## Installation

### CentOS / RHEL

```bash
yum install -y openssl-devel libcurl-devel protobuf-devel lz4-devel zlib-devel log4cpp-devel rapidjson-devel gcc-c++ cmake
```

### Ubuntu / Debian

```bash
apt-get install -y libssl-dev libcurl4-openssl-dev libprotobuf-dev protobuf-compiler liblz4-dev zlib1g-dev liblog4cpp5v5 rapidjson-dev g++ cmake
```

### For Unit Tests

```bash
# CentOS / RHEL
yum install -y gtest-devel gmock-devel

# Ubuntu / Debian
apt-get install -y libgtest-dev libgmock-dev
```
