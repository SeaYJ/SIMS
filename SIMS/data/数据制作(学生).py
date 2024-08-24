import pandas as pd
import random

# 定义一个函数用于解析数据
def parse_data(filenames):
    identity_ids = set()
    names = list()
    genders = list()
    ages = list()
    phone_numbers = list()

    # 读取并解析身份证号码、性别、年龄
    data = filenames[0]
    with open(data, 'r', encoding='utf-8') as file:
        content = file.read()
        lines = content.strip().split('\n')
        for line in lines:
            pairs = line.split(',')
            record = dict(pair.split(':') for pair in pairs)
            identity_ids.add(record['身份证号码'])
            genders.append(record['性别'])
            ages.append(record['年龄'])

    # 读取人名
    names_filename = filenames[1]
    with open(names_filename, 'r', encoding='utf-8') as file:
        names = file.read().strip().split('\n')

    # 读取电话号码
    phones_filename = filenames[2]
    with open(phones_filename, 'r', encoding='utf-8') as file:
        phone_numbers = file.read().strip().split('\n')

    # 将身份证号码集合转换为列表
    identity_ids = list(identity_ids)

    # 组合数据
    data_list = []
    for i in range(len(identity_ids)):
        data_list.append([
            identity_ids[i],
            names[i] if i < len(names) else '',
            genders[i],
            ages[i],
            phone_numbers[i] if i < len(phone_numbers) else ''
        ])

    # 创建DataFrame
    df = pd.DataFrame(data_list, columns=[
        '身份证号码', '姓名', '性别', '年龄', '电话号码'
    ])

    return df

def df_to_sql_insert(df, table_name='Student', limit=100):
    # 定义列名
    columns = 'id_number, name, gender, age, contact_info, student_id, university_id, college_id, major_id'
    
    # 初始化SQL命令的VALUES部分
    values = []
    
    # 初始化入学年份范围
    year_range = list(range(2001, 2025))
    
    # 遍历DataFrame的所有行，从100开始
    for index, row in df.iloc[100:].iterrows():
        sql_row = []
        
        # 生成university_id
        if 100 <= index < 280:
            university_id = 10001
        elif 280 <= index < 460:
            university_id = 10003
        elif 460 <= index < 640:
            university_id = 10213
        elif 640 <= index < 820:
            university_id = 10460
        else:
            university_id = 10486

        # 生成college_id
        college_map = {
            10001: [1312, 1313, 1314, 1315, 1316, 1317, 1318, 1319, 1320, 1321],
            10003: [1312, 1313, 1314, 1315, 1316, 1317, 1318, 1320, 1321],
            10213: [1312, 1313, 1315, 1316, 1317, 1320, 1321],
            10460: [1312, 1313, 1315, 1316, 1317, 1320, 1321],
            10486: [1312, 1313, 1315, 1316, 1317, 1320, 1321],
        }
        college_id = random.choice(college_map[university_id])
        
        # 生成major_id
        major_map = {
            1312: range(0x520, 0x529),
            1313: range(0x529, 0x532),
            1314: range(0x532, 0x53B),
            1315: range(0x53B, 0x544),
            1316: range(0x544, 0x54D),
            1317: range(0x54D, 0x556),
            1318: range(0x556, 0x55F),
            1319: range(0x55F, 0x568),
            1320: range(0x568, 0x571),
            1321: range(0x571, 0x57A),
        }
        major_id = random.choice(major_map[college_id])
        
        # 生成student_id
        year = year_range[(index - 100) % 20]
        student_id = f"{year}{(index - 99):04}"

        # 生成SQL行
        for i, value in enumerate(row):
            if i == df.columns.get_loc('年龄'):
                sql_row.append(str(value))
            else:
                sql_row.append(f"'{value}'")
        
        # 将生成的字段添加到SQL行
        sql_row.extend([f"'{student_id}'", f"'{university_id}'", f"'{college_id}'", f"'{major_id}'"])
        values.append(f"({', '.join(sql_row)})")
    
    # 将VALUES列表转换为字符串
    values_str = ",\n".join(values)
    
    # 构建完整的SQL插入命令
    sql = f"INSERT INTO {table_name} ({columns}) VALUES \n{values_str};"
    return sql

# 文件路径列表
filenames = ['1.txt', '2.txt', '3.txt']

# 解析数据并创建DataFrame
df = parse_data(filenames)

# 使用修正后的函数生成SQL插入命令
sql_insert = df_to_sql_insert(df)

# 输出SQL插入命令
print(sql_insert)
