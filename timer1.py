# 250kHz
OSCCON = 250000 # 時脈
reverse = True # Timer1 有反轉
PreScaler = 8
timerMax = 65535

preferTime = 0.04 # 秒

print("========== Timer1 Configuration =========")
print(f"OSCCON: {OSCCON}")
print(f"reverse: {reverse}")
print(f"PreScaler: {PreScaler}")
print(f"timerMax: {timerMax}")
print(f"preferTime: {preferTime}")


# timerMax - (時脈 / 4 / PreScaler * preferTime)
target_counts = timerMax - (OSCCON / 4 / PreScaler * preferTime)
print(f"Target counts: {target_counts} | Hex: {int(target_counts):04X}")
print("===========================================")