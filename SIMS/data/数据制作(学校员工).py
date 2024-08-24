import pandas as pd

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

# 文件路径列表
filenames = ['1.txt', '2.txt', '3.txt']

# 解析数据并创建DataFrame
df = parse_data(filenames)

def df_to_sql_insert(df, table_name='SchoolStaff', limit=100):
    # 定义列名，包括年龄列
    columns = 'id_number, name, gender, age, contact_info'
    
    # 初始化SQL命令的VALUES部分
    values = []
    
    # 遍历DataFrame的前limit行
    for index, row in df.iloc[:limit].iterrows():
        # 将除年龄外的其他列值用单引号包围
        sql_row = []
        for i, value in enumerate(row):
            if i == df.columns.get_loc('年龄'):  # 假设'年龄'是DataFrame中的列名
                sql_row.append(str(value))  # 年龄列不使用单引号
            else:
                sql_row.append(f"'{value}'")  # 其他列使用单引号
        values.append(f"({', '.join(sql_row)})")  # 将行数据添加到VALUES列表
    
    # 将VALUES列表转换为字符串
    values_str = ",\n".join(values)
    
    # 构建完整的SQL插入命令
    sql = f"INSERT INTO {table_name} ({columns}) VALUES \n{values_str};"
    return sql

# 使用修正后的函数生成SQL插入命令
sql_insert = df_to_sql_insert(df)

# 输出SQL插入命令
print(sql_insert)