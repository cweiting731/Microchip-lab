# 250kHz
OSCCON = 250000 # 時脈
reverse = False # Timer2 沒有反轉
PreScaler = 16
PostScaler = 16
timerMax = 256

preferTime = 0.25 # 秒

print("========== Timer2 Configuration =========")
print(f"OSCCON: {OSCCON}")
print(f"reverse: {reverse}")
print(f"PreScaler: {PreScaler}")
print(f"PostScaler: {PostScaler}")
print(f"timerMax: {timerMax}")
print(f"preferTime: {preferTime}")


# 時脈 / 4 / PreScaler / PostScaler * preferTime
target_counts = OSCCON / 4 / PreScaler / PostScaler * preferTime
print(f"Target counts: {target_counts}")
print("===========================================")