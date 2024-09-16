import matplotlib.pyplot as plt

# 从文件读取数据
with open('/tmp/mem.txt', 'r') as file:
    data = [float(line.strip()) for line in file if line.strip()]

# 创建图形和轴对象
fig, ax = plt.subplots(figsize=(10, 6))

# 绘制折线图
ax.plot(range(1, len(data) + 1), data, marker='o')

# 设置标题和标签
ax.set_title('数据折线图', fontsize=16)
ax.set_xlabel('数据点', fontsize=12)
ax.set_ylabel('值', fontsize=12)

# 设置y轴的范围，使小的变化更明显
y_min = min(data) - 0.001
y_max = max(data) + 0.001
ax.set_ylim(y_min, y_max)

# 添加网格线
ax.grid(True, linestyle='--', alpha=0.7)

# 在每个数据点上标注值
for i, v in enumerate(data):
    ax.text(i + 1, v, f'{v:.3f}', ha='center', va='bottom', fontsize=8)

# 调整布局
plt.tight_layout()

# 保存图片为JPG格式
plt.savefig('data_line_chart.jpg', dpi=300, bbox_inches='tight')

print("图片已保存为 'data_line_chart.jpg'")