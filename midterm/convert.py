# 十進位轉十六進位
def decimal_to_hex(decimal_number):
    if not isinstance(decimal_number, int):
        raise ValueError("請輸入整數")
    return hex(decimal_number)

# 十六進位轉十進位
def hex_to_decimal(hex_string):
    if not isinstance(hex_string, str):
        raise ValueError("請輸入字串格式的十六進位")
    return int(hex_string, 16)

# 🧪 測試範例
print("Decimal 289564320 → Hex:", decimal_to_hex(289564320))
print("Hex '0x114f5a60' → Decimal:", hex_to_decimal("0x0AF242FA"))