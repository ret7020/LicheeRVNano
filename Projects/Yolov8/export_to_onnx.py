from ultralytics import YOLO
import types
import sys

input_size = (int(sys.argv[2]), int(sys.argv[3]))

def forward2(self, x):
  x_reg = [self.cv2[i](x[i]) for i in range(self.nl)]
  x_cls = [self.cv3[i](x[i]) for i in range(self.nl)]
  return x_reg + x_cls

model_path = sys.argv[1]
model = YOLO(model_path)
model.model.model[-1].forward = types.MethodType(forward2, model.model.model[-1])
model.export(format='onnx', opset=11, imgsz=input_size)

