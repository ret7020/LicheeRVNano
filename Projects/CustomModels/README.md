# Custom models inference on NPU

This directory contains examples of running custom neural networks architectures on NPU. I recomend to start with DQN model, because it have simple architecture.

## Examples

- RL
    - [DQN](DQN/)


## General info

Pipeline to run model on board:
* Implement model architecture in PyTorch (you can use some other framework, that can convert models to ONNX format)
* Train model in PyTorch on your data (or RL training in simulation)
* Export trained model to ONNX format
* Convert ONNX model to MLIR via TPU MLIR
* (* Optional) If quantization needed: prepare calibration dataset, quantize model (int8)
* Convert model to CVI model
* Run model on NPU board with TDL SDK