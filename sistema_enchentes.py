import pandas as pd
import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report, accuracy_score
import matplotlib.pyplot as plt
import seaborn as sns
import joblib
import json
from datetime import datetime


# =================================================================
# 1. GERADOR DE DADOS SINTÉTICOS (BASEADO EM PADRÕES REAIS)
# =================================================================

def gerar_dataset_enchentes(n_amostras=1000):
    """
    Gera dataset sintético baseado em padrões reais de enchentes
    """
    print("Gerando dataset de enchentes...")

    np.random.seed(42)
    dados = []

    for i in range(n_amostras):
        # 30% chance de ser situação de enchente
        if np.random.random() < 0.3:
            # CENÁRIO DE ENCHENTE
            nivel_agua = np.random.normal(160, 40)  # Alto
            precipitacao_1h = np.random.normal(25, 8)  # Chuva intensa
            precipitacao_24h = np.random.normal(90, 25)  # Muita chuva acumulada
            taxa_subida = np.random.normal(12, 4)  # Subida rápida
            umidade_solo = np.random.normal(88, 8)  # Solo saturado
            temperatura = np.random.normal(23, 4)  # Temperatura
            enchente = 1  # SIM, há risco de enchente
        else:
            # CENÁRIO NORMAL
            nivel_agua = np.random.normal(75, 25)  # Normal
            precipitacao_1h = np.random.normal(5, 6)  # Pouca chuva
            precipitacao_24h = np.random.normal(18, 15)  # Pouca chuva acumulada
            taxa_subida = np.random.normal(1, 3)  # Subida lenta
            umidade_solo = np.random.normal(65, 15)  # Solo normal
            temperatura = np.random.normal(26, 5)  # Temperatura
            enchente = 0  # NÃO há risco

        # Garantir valores válidos
        dados.append([
            max(0, nivel_agua),
            max(0, precipitacao_1h),
            max(0, precipitacao_24h),
            taxa_subida,
            max(0, min(100, umidade_solo)),
            temperatura,
            enchente
        ])

    # Criar DataFrame
    colunas = [
        'nivel_agua_cm', 'precipitacao_1h_mm', 'precipitacao_24h_mm',
        'taxa_subida_cm_h', 'umidade_solo_pct', 'temperatura_c', 'enchente'
    ]

    df = pd.DataFrame(dados, columns=colunas)

    print(f"Dataset criado: {len(df)} amostras")
    print(f"Distribuição: {df['enchente'].value_counts().to_dict()}")

    return df


# =================================================================
# 2. MODELO DE MACHINE LEARNING
# =================================================================

def treinar_modelo_enchentes(df):
    """
    Treina modelo de ML para detectar enchentes
    """
    print("\n=== TREINANDO MODELO DE MACHINE LEARNING ===")

    # Separar features e target
    features = ['nivel_agua_cm', 'precipitacao_1h_mm', 'precipitacao_24h_mm',
                'taxa_subida_cm_h', 'umidade_solo_pct', 'temperatura_c']

    X = df[features]
    y = df['enchente']

    # Dividir dados
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=42, stratify=y
    )

    # Treinar modelo
    modelo = RandomForestClassifier(
        n_estimators=100,
        max_depth=10,
        random_state=42,
        class_weight='balanced'
    )

    modelo.fit(X_train, y_train)

    # Avaliar
    y_pred = modelo.predict(X_test)
    acuracia = accuracy_score(y_test, y_pred)

    print(f"Acurácia do modelo: {acuracia:.3f}")
    print("\nRelatório de classificação:")
    print(classification_report(y_test, y_pred))

    # Feature importance
    importance = pd.DataFrame({
        'feature': features,
        'importance': modelo.feature_importances_
    }).sort_values('importance', ascending=False)

    print("\nImportância das features:")
    for _, row in importance.iterrows():
        print(f"  {row['feature']}: {row['importance']:.3f}")

    # Salvar modelo
    joblib.dump(modelo, 'modelo_enchentes.pkl')
    print("\nModelo salvo como 'modelo_enchentes.pkl'")

    return modelo, acuracia


# =================================================================
# 3. SISTEMA DE PREDIÇÃO
# =================================================================

def prever_risco_enchente(modelo, nivel_agua, precipitacao_1h, precipitacao_24h,
                          taxa_subida, umidade_solo, temperatura):
    """
    Prediz risco de enchente baseado nos dados dos sensores
    """
    # Preparar dados
    dados = np.array([[nivel_agua, precipitacao_1h, precipitacao_24h,
                       taxa_subida, umidade_solo, temperatura]])

    # Predição
    predicao = modelo.predict(dados)[0]
    probabilidade = modelo.predict_proba(dados)[0][1]  # Prob. de enchente

    # Classificar risco
    if probabilidade < 0.3:
        risco = "BAIXO"
        cor = "🟢"
        acao = "Monitoramento normal"
    elif probabilidade < 0.7:
        risco = "MÉDIO"
        cor = "🟡"
        acao = "Atenção redobrada"
    else:
        risco = "ALTO"
        cor = "🔴"
        acao = "ALERTA! Preparar evacuação"

    return {
        'predicao_binaria': int(predicao),
        'probabilidade_enchente': round(probabilidade, 3),
        'nivel_risco': risco,
        'cor': cor,
        'acao_recomendada': acao
    }


# =================================================================
# 4. SIMULAÇÃO DE DADOS DO ESP32
# =================================================================

def simular_dados_esp32():
    """
    Simula dados vindos do ESP32/sensores
    """
    # Simular leituras realistas dos sensores
    dados = {
        'timestamp': datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
        'nivel_agua_cm': round(np.random.uniform(50, 180), 1),
        'precipitacao_1h_mm': round(max(0, np.random.normal(8, 12)), 1),
        'precipitacao_24h_mm': round(max(0, np.random.normal(25, 30)), 1),
        'taxa_subida_cm_h': round(np.random.normal(2, 6), 2),
        'umidade_solo_pct': round(np.random.uniform(40, 95), 1),
        'temperatura_c': round(np.random.uniform(18, 32), 1)
    }

    return dados


# =================================================================
# 5. SISTEMA PRINCIPAL
# =================================================================

def executar_sistema_completo():
    """
    Executa o sistema completo de detecção de enchentes
    """
    print("🌊 SISTEMA DE DETECÇÃO DE ENCHENTES 🌊")
    print("=" * 50)

    # 1. Gerar dados de treinamento
    df = gerar_dataset_enchentes(1000)

    # 2. Treinar modelo
    modelo, acuracia = treinar_modelo_enchentes(df)

    # 3. Demonstrar funcionamento
    print("\n" + "=" * 50)
    print("DEMONSTRAÇÃO DO SISTEMA EM TEMPO REAL")
    print("=" * 50)

    for i in range(5):
        print(f"\n--- LEITURA {i + 1} ---")

        # Simular dados do ESP32
        dados_sensor = simular_dados_esp32()

        # Fazer predição
        resultado = prever_risco_enchente(
            modelo,
            dados_sensor['nivel_agua_cm'],
            dados_sensor['precipitacao_1h_mm'],
            dados_sensor['precipitacao_24h_mm'],
            dados_sensor['taxa_subida_cm_h'],
            dados_sensor['umidade_solo_pct'],
            dados_sensor['temperatura_c']
        )

        # Exibir resultados
        print(f"Timestamp: {dados_sensor['timestamp']}")
        print(f"Dados do sensor: {dados_sensor}")
        print(f"RESULTADO: {resultado['cor']} Risco {resultado['nivel_risco']}")
        print(f"Probabilidade: {resultado['probabilidade_enchente']:.1%}")
        print(f"Ação: {resultado['acao_recomendada']}")

        # Salvar log
        log_entry = {**dados_sensor, **resultado}
        with open('log_sistema.json', 'a') as f:
            f.write(json.dumps(log_entry) + '\n')

    # 4. Estatísticas finais
    print("\n" + "=" * 50)
    print("RESUMO DO SISTEMA")
    print("=" * 50)
    print(f"✅ Modelo treinado com acurácia: {acuracia:.1%}")
    print(f"✅ Sistema funcionando e fazendo predições")
    print(f"✅ Logs salvos em 'log_sistema.json'")
    print(f"✅ Modelo salvo em 'modelo_enchentes.pkl'")

    return modelo, df


# =================================================================
# EXECUTAR SISTEMA
# =================================================================

if __name__ == "__main__":
    modelo, dataset = executar_sistema_completo()

    print("\n🎯 PROJETO PRONTO PARA ENTREGA!")
    print("Arquivos gerados:")
    print("- modelo_enchentes.pkl (modelo treinado)")
    print("- log_sistema.json (logs de funcionamento)")
    print("- Este script principal")

    print("\n📋 Próximo passo: Configurar ESP32 no Wokwi!")