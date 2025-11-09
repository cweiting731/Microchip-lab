# (2 + (2 + 8 * L1 + 3) * L2) * 4 * 10^-6
# 給定 秒數, 計算 最接近的L1 L2 組合
def calculate_L1_L2(target_seconds):
    target_cycles = int(target_seconds * 0.25 * 10**6)

    closest_L1 = None
    closest_L2 = None
    closest_diff = float('inf')

    for L1 in range(0, 256):
        for L2 in range(0, 256):
            cycles = (2 + (2 + 8 * L1 + 3) * L2)
            diff = abs(cycles - target_cycles)

            if diff < closest_diff:
                closest_diff = diff
                closest_L1 = L1
                closest_L2 = L2

    return closest_L1, closest_L2

# 範例使用
target_seconds = 0.1  # 目標秒數
ans1, ans2 = calculate_L1_L2(target_seconds)
print("========== Calculation Result ==========")
print(f"Target seconds: {target_seconds}")
print(f"Closest L1: {ans1}, Closest L2: {ans2}")
print(f"Achieved seconds: {(2 + (2 + 8 * ans1 + 3) * ans2) * 4 * 10**-6}")
print("========================================")