# Zero-To-Architect-Cpp 🚀

> **"Talk is cheap. Show me the code."** — Linus Torvalds

![Language](https://img.shields.io/badge/language-C%2B%2B20-blue)
![Platform](https://img.shields.io/badge/platform-Linux%20Ubuntu%2022.04-orange)
![Hardware](https://img.shields.io/badge/GPU-Dual%20RTX%206000%20Ada-green)
![Build](https://img.shields.io/badge/build-CMake-brightgreen)
![License](https://img.shields.io/badge/license-MIT-lightgrey)

## 📖 项目背景 (Background)

**LeetCode 500+，无大型 C++ 工程落地经验。**

这是我的 **20 周（5 个月）** 强化特训记录。目标是从一名“传统算法做题家”转型为具备 **高性能后端与 AI 基础设施 (AI Infra)** 开发能力的 C++ 系统架构师。

本项目将摒弃 "C with Classes" 的陈旧思维，深度实践 Modern C++ (17/20) 特性，并最终落地两个工业级项目：
1.  **TinyRPC**: 一个基于 Reactor 模式的高性能 RPC 框架。
2.  **AI-Inference-Gateway**: 一个支持 TensorRT 加速的高并发 AI 推理服务（利用双路 RTX 6000 Ada）。

## 🛠️ 开发环境 (Dev Environment)

- **OS**: Ubuntu 22.04 LTS (Remote Server)
- **Hardware**: **Dual NVIDIA RTX 6000 Ada (192GB VRAM)** ⚡
- **Compiler**: GCC 12.3.0 / Clang 15 (C++20 Standard)
- **Build System**: CMake 3.22+
- **Tools**: GDB, Valgrind, Perf, FlameGraph
- **AI Stack**: CUDA 12.x, TensorRT 8.x

## 🗺️ 学习路线与进度 (Roadmap)

### 第一阶段：Modern C++ 觉醒与工程化 (Weeks 1-4)
> 目标：掌握资源管理 (RAII) 与现代构建系统，建立工程思维。

- [ ] **Week 01**: RAII 与智能指针本质 (手写 `UniquePtr`, `SharedPtr`)
- [ ] **Week 02**: 右值引用与移动语义 (`std::move`, `Rule of Five`, 完美转发)
- [ ] **Week 03**: 函数式编程与泛型 (`Lambda`, `std::function`, `Templates`)
- [ ] **Week 04**: 工具链与性能分析 (Modern CMake, `GDB` 调试, `Valgrind` 查漏)

### 第二阶段：Linux 系统编程与并发实战 (Weeks 5-8)
> 目标：深入 OS 内核，手写并发底座。

- [ ] **Week 05**: 系统调用与多线程基石 (手写线程安全队列 `BlockingQueue`)
- [ ] **Week 06**: 网络编程基础 (Socket, TCP 状态机, 粘包/拆包处理)
- [ ] **Week 07**: 高性能 IO 模型 (Select/Poll/**Epoll**, Reactor 模式实战)
- [ ] **Week 08**: **Muduo 源码剖析**与线程池设计 (学习陈硕大神的架构思想)

### 第三阶段：工业级项目 "TinyRPC" (Weeks 9-13) 🏆
> 目标：造轮子。实现一个高性能、支持 Protobuf 与服务发现的 RPC 框架。

- [ ] **Week 09**: 通信协议设计 (Protocol Buffers 封装与自定义协议头)
- [ ] **Week 10**: 高性能异步日志库 (双缓冲技术，避免磁盘 IO 阻塞 Worker 线程)
- [ ] **Week 11**: RPC 核心通信层 (基于 Epoll + Reactor 的网络库实现)
- [ ] **Week 12**: 分布式服务治理 (引入 **ZooKeeper/Etcd** 实现服务注册与发现)
- [ ] **Week 13**: 进阶特性 (协程化改造 / 负载均衡策略)

### 第四阶段：杀手锏项目 "AI Inference Gateway" (Weeks 14-18) ⚡
> 目标：差异化竞争。构建基于 TensorRT 的高性能 AI 推理网关。
> *此阶段充分利用 RTX 6000 Ada 硬件优势。*

- [ ] **Week 14**: CUDA 编程基础 & TensorRT C++ API 集成
- [ ] **Week 15**: 模型加载与显存管理 (ResNet/Llama 部署)
- [ ] **Week 16**: **Continuous Batching** (动态批处理) 调度算法实现
- [ ] **Week 17**: 性能压测与优化 (使用 `wrk` 压测，`FlameGraph` 火焰图分析热点)
- [ ] **Week 18**: 系统高可用设计 (熔断、降级、优雅退出)

### 第五阶段：面试冲刺 (Weeks 19-20)
- [ ] **Week 19**: 简历深度打磨 (STAR 法则，突出 AI Infra 亮点)
- [ ] **Week 20**: 模拟面试与系统设计 (System Design) 突击

## 📂 目录结构说明

```text
Zero-To-Architect-Cpp/
├── 01-modern-cpp-lab/     # Phase 1: 现代 C++ 核心特性练习 (SmartPtr, Move...)
├── 02-linux-sys-prog/     # Phase 2: Linux 系统编程 (ThreadPool, Epoll...)
├── 03-tiny-rpc/           # Phase 3: [核心项目] 高性能 RPC 框架实现
├── 04-ai-inference-gw/    # Phase 4: [杀手锏] AI 推理服务网关 (CUDA/TRT)
└── docs/                  # 学习笔记、架构图、性能分析火焰图