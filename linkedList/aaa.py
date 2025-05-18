import pandas as pd

# Ler o arquivo Excel
df = pd.read_excel('teste.xlsx')

# Salvar como CSV corretamente
df.to_csv('dados_formatados.csv', index=False, sep=',', encoding='utf-8')