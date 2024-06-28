import matplotlib.pyplot as plt

# Values of K
K_values = [1, 2, 4, 8, 16]

# Corresponding execution times
execution_times = [6.524, 5.637, 4.452, 6.469, 11.912]

# Plotting the graph
plt.figure(figsize=(8, 6))
plt.plot(K_values, execution_times, marker='o', linestyle='-')
plt.title('Execution Time vs K')
plt.xlabel('K (Number of Processes)')
plt.ylabel('Execution Time (milliseconds)')
plt.grid(True)
plt.tight_layout()

# Show plot
plt.show()



