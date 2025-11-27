# 計算伺服馬達所需設置參數的模組
# config
Fosc = 4000000 # 時脈頻率 (Hz)
Tosc = 1 / Fosc * 1_000_000 # 時脈週期 (us)
TMR2_prescale = 16 # TMR2 預分頻值

# 500 ~ 2400 us 對應 -90 ~ +90 度
angle = float(input("請輸入伺服馬達角度 (-90 到 90 度): "))
if angle < -90 or angle > 90:
    print("角度超出範圍")
else:
    duty_cycle = (angle + 90) * (2450 - 450) / 180 + 450
    print("所需脈衝寬度: {:.2f} us".format(duty_cycle))

    # 計算 TMR2 需要的計數值
    CCPR = duty_cycle / (Tosc * TMR2_prescale)
    print("所需 CCPR 計數值: {:.2f}".format(CCPR))
    # 轉成10bits的binary數值 前8bits用hex顯示 後2bits用binary顯示
    CCPR_int = int(CCPR)
    CCPR_high = CCPR_int >> 2
    CCPR_low = CCPR_int & 0b11
    print("設定 CCPR 寄存器值: {:0>2X} / {:02b}".format(CCPR_high, CCPR_low))