import torch
from diffusers import StableDiffusionPipeline
from diffusers import StableDiffusionImg2ImgPipeline
from torch.utils.data import DataLoader, Dataset
from torchvision import transforms
from PIL import Image
import os
# 自定义数据集类
class ImageToImageDataset(Dataset):
    def __init__(self, source_dir, target_dir, transform=None):
        self.source_dir = source_dir
        self.target_dir = target_dir
        self.transform = transform
        self.source_images = os.listdir(source_dir)

    def __len__(self):
        return len(self.source_images)

    def __getitem__(self, idx):
        source_path = os.path.join(self.source_dir, self.source_images[idx])
        target_path = os.path.join(self.target_dir, self.source_images[idx])
        
        source_image = Image.open(source_path).convert("RGB")
        target_image = Image.open(target_path).convert("RGB")

        if self.transform:
            source_image = self.transform(source_image)
            target_image = self.transform(target_image)

        return source_image, target_image

# 数据预处理
transform = transforms.Compose([
    transforms.Resize((512, 512)),
    transforms.ToTensor(),
])

# 加载数据集
from torch.utils.data import DataLoader
source_dir = "img1"  # 替换为您的源图像路径
target_dir = "label1"  # 替换为您的目标图像路径

dataset = ImageToImageDataset(source_dir, target_dir, transform=transform)
dataloader = DataLoader(dataset, batch_size=1, shuffle=True)

# 加载预训练模型
model_id = "CompVis/stable-diffusion-v1-4"
# pipe = StableDiffusionPipeline.from_pretrained(model_id).to("cuda")
pipe = StableDiffusionImg2ImgPipeline.from_pretrained(model_id).to("cuda")
# 提取模型组件
# unet = pipe.unet
optimizer = torch.optim.Adam(pipe.unet.parameters(), lr=1e-5)
# 训练参数
num_epochs = 10
to_tensor = transforms.ToTensor()  
# 训练循环
for epoch in range(num_epochs):
    for source_images, target_images in dataloader:
        optimizer.zero_grad()
        source_images = source_images.to("cuda")
        target_images = target_images.to("cuda")
        
        # 进行图像到图像的转换
        # with torch.no_grad():
        generated_images = pipe(prompt="",image=source_images, num_inference_steps=50).images
        
        generated_image = generated_images[0]
        generated_image_tensor = to_tensor(generated_image).unsqueeze(0).to("cuda").requires_grad_(True) 
        print(type(generated_image))
        print(type(target_images))
        # 计算损失（可以根据具体任务定义损失函数）
        # 这里假设使用 L1 损失
        # print(generated_images.size())
        # print(target_images.size())
        loss = torch.nn.functional.l1_loss(generated_image_tensor, target_images)

        # 反向传播
        loss.backward()
        optimizer.step()

        print(f"Epoch [{epoch}/{num_epochs}], Loss: {loss.item()}")

# 保存微调后的模型
pipe.save_pretrained("fine_tuned_model")



# from diffusers import StableDiffusionImg2ImgPipeline
# import torch
# from torchvision import transforms
# from torch.utils.data import Dataset, DataLoader
# import os
# from PIL import Image
# import torch.nn as nn
# import numpy as np
# # 自定义数据集类
# # 自定义数据集类
# class CustomImageDataset(Dataset):
#     def __init__(self, input_images, target_images, transform=None):
#         self.input_images = input_images
#         self.target_images = target_images
#         self.transform = transform

#     def __len__(self):
#         return len(self.input_images)

#     def __getitem__(self, idx):
#         input_image = Image.open(self.input_images[idx])  # 加载输入图像
#         target_image = Image.open(self.target_images[idx])  # 加载目标图像
        
#         if self.transform:
#             input_image = self.transform(input_image)
#             target_image = self.transform(target_image)

#         return {'input_images': input_image, 'target_images': target_image}
# # 数据预处理
# transform = transforms.Compose([
#     transforms.Resize((256, 256)),
#     transforms.ToTensor(),
# ])

# # 加载数据集
# input_dir = 'img1'
# target_dir = 'label1'

# input_images = [os.path.join(input_dir, f) for f in os.listdir(input_dir) if f.endswith(('png', 'jpg', 'jpeg'))]  
# target_images = [os.path.join(target_dir, f) for f in os.listdir(target_dir) if f.endswith(('png', 'jpg', 'jpeg'))]  

# # Check the collected image paths  
# print("Input images:", input_images)  
# print("Target images:", target_images)  

# # Ensure both lists have the same length  
# if len(input_images) != len(target_images):  
#     raise ValueError("The number of input images and target images must match.") 

# dataset = CustomImageDataset(input_dir, target_dir, transform=transform)
# dataloader = DataLoader(dataset, batch_size=8, shuffle=True)

# # 加载模型
# model_id = "CompVis/stable-diffusion-v1-4"
# pipe = StableDiffusionImg2ImgPipeline.from_pretrained(model_id)
# pipe = pipe.to("cuda" if torch.cuda.is_available() else "cpu")

# # 设置学习率
# learning_rate = 1e-5
# model = pipe.unet  # 获取 UNet 模型
# optimizer = torch.optim.Adam(model.parameters(), lr=learning_rate)

# # 训练参数
# num_epochs = 5
# loss_function = nn.MSELoss()  # 使用均方误差损失

# # 训练循环
# for epoch in range(num_epochs):
#     model.train()
#     total_loss = 0
#     for batch in dataloader:
#         input_images = batch['input_images'].to("cuda" if torch.cuda.is_available() else "cpu")
#         target_images = batch['target_images'].to("cuda" if torch.cuda.is_available() else "cpu")

#         optimizer.zero_grad()  # 清除梯度

#         # 生成图像
#         generated_images = pipe(input_images).images

#         # 计算损失
#         loss = loss_function(generated_images, target_images)
#         total_loss += loss.item()

#         # 反向传播和优化
#         loss.backward()
#         optimizer.step()

#     avg_loss = total_loss / len(dataloader)
#     print(f"Epoch [{epoch+1}/{num_epochs}], Loss: {avg_loss:.4f}")

# # 测试阶段
# model.eval()
# test_loss = 0
# with torch.no_grad():
#     for batch in dataloader:
#         input_images = batch['input_images'].to("cuda" if torch.cuda.is_available() else "cpu")
#         target_images = batch['target_images'].to("cuda" if torch.cuda.is_available() else "cpu")

#         generated_images = pipe(input_images).images
#         loss = loss_function(generated_images, target_images)
#         test_loss += loss.item()

# avg_test_loss = test_loss / len(dataloader)
# print(f"Test Loss: {avg_test_loss:.4f}")

