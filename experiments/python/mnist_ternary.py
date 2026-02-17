import torch
import torch.nn as nn
import torch.optim as optim
from torchvision import datasets, transforms
from ternary_layers import TernaryLinear
from ternary_ops import quantize
import os

# Configuration
BATCH_SIZE = 64
EPOCHS = 5 # 5 Epochs should be enough for >90%
LR = 0.01

# 1. Data Loader
transform = transforms.Compose([
    transforms.ToTensor(),
    transforms.Normalize((0.1307,), (0.3081,))
])

# Check for data folder
data_path = './experiments/python/data'
if not os.path.exists(data_path):
    os.makedirs(data_path)

train_dataset = datasets.MNIST(data_path, train=True, download=True, transform=transform)
test_dataset = datasets.MNIST(data_path, train=False, transform=transform)

train_loader = torch.utils.data.DataLoader(train_dataset, batch_size=BATCH_SIZE, shuffle=True)
test_loader = torch.utils.data.DataLoader(test_dataset, batch_size=1000, shuffle=False)

# 2. Model: 784 -> 512 -> 10
class TernaryMNIST(nn.Module):
    def __init__(self):
        super(TernaryMNIST, self).__init__()
        # Enable Bias (Helix9 supports accumulator initialization)
        # Increase width to 512 for better capacity with ternary weights
        self.fc1 = TernaryLinear(784, 512, bias=True)
        self.fc2 = TernaryLinear(512, 10, bias=True)

    def forward(self, x):
        x = x.view(-1, 784)
        x = self.fc1(x)
        x = quantize(x)
        x = self.fc2(x)
        return x

model = TernaryMNIST()
optimizer = optim.Adam(model.parameters(), lr=LR)
scheduler = optim.lr_scheduler.StepLR(optimizer, step_size=2, gamma=0.1) # Decay LR
criterion = nn.CrossEntropyLoss()

print(f"Training Ternary MNIST (Weights & Activations in {{-1, 0, 1}})...")

def train(epoch):
    model.train()
    for batch_idx, (data, target) in enumerate(train_loader):
        optimizer.zero_grad()
        output = model(data)
        loss = criterion(output, target)
        loss.backward()
        optimizer.step()
        
        # Clamp weights
        for p in model.parameters():
            p.data.clamp_(-1.5, 1.5)
            
        if batch_idx % 100 == 0:
            print(f'Train Epoch: {epoch} [{batch_idx * len(data)}/{len(train_loader.dataset)} ({100. * batch_idx / len(train_loader):.0f}%)]\tLoss: {loss.item():.6f}')
    
    scheduler.step() # Update LR

def test():
    model.eval()
    test_loss = 0
    correct = 0
    with torch.no_grad():
        for data, target in test_loader:
            output = model(data)
            test_loss += criterion(output, target).item()
            pred = output.argmax(dim=1, keepdim=True)
            correct += pred.eq(target.view_as(pred)).sum().item()

    test_loss /= len(test_loader.dataset)
    acc = 100. * correct / len(test_loader.dataset)
    print(f'\nTest set: Average loss: {test_loss:.4f}, Accuracy: {correct}/{len(test_loader.dataset)} ({acc:.2f}%)\n')
    return acc

for epoch in range(1, EPOCHS + 1):
    train(epoch)
    acc = test()
    
# Save Model
torch.save(model.state_dict(), "experiments/python/mnist_ternary.pth")
print("Model saved to experiments/python/mnist_ternary.pth")
