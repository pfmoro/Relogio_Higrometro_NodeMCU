

````markdown
# 🌡️ Relógio Higrometro NodeMCU + DHT11 + Display OLED

Este projeto implementa uma estação de monitoramento de **temperatura e umidade** interna e externa, exibindo os dados em um **display OLED I2C**.  
A comunicação externa é feita por **requisições HTTP** a um endpoint JSON em outros dispositivos da rede.

Para obter os dados externos, é necessário possuir sensores devidamente configurados, para referência, ver nodemcu1 do projeto https://github.com/pfmoro/Hestia_Sensores

## 🛠️ Hardware Utilizado
- **ESP8266 NodeMCU**
- **Sensor DHT11** (temperatura e umidade internos)
- **Display OLED 128x64** I2C
- Fonte 5V (micro-USB ou regulador)

## 📚 Bibliotecas Necessárias
Instale estas bibliotecas pela **IDE do Arduino**:
- `ESP8266WiFi`
- `ESP8266HTTPClient`
- `WiFiUdp`
- `NTPClient`
- `Adafruit GFX Library`
- `Adafruit SSD1306`
- `DHT sensor library`

> Para instalar:  
> **IDE do Arduino → Sketch → Incluir Biblioteca → Gerenciar Bibliotecas**  
> Pesquise pelo nome e clique em **Instalar**.

## ⚙️ Configuração
No início do código, edite as seguintes variáveis:
```cpp
const char* ssid     = "SUA_REDE_WIFI";
const char* password = "SUA_SENHA";

const char* ipList[] = {
  "http://192.168.0.200",
  "http://192.168.0.201"
};
````

## 🧠 Funcionamento

* Lê **temperatura e umidade internas** via DHT11
* Consulta **valores externos** em múltiplos IPs (falha → tenta próximo)
* Exibe no display em **modo alternado** (temperatura ↔ umidade)
* Caso alguma leitura falhe, o último valor válido é mostrado com um **indicador `#`**

Exemplo de exibição com valor defasado:

```
In: 24.1C#
Out: 28.3C
```

## 🔧 Pinos Utilizados

| Componente | Pino NodeMCU |
| ---------- | ------------ |
| DHT11      | D5           |
| OLED SDA   | D2           |
| OLED SCL   | D1           |

## 📦 Estrutura do Projeto

```
.
├── src/
│   └── main.ino       # Código principal do projeto
├── docs/
│   └── exemplo_display.jpg
└── README.md          # Documentação do projeto
```

## 🚀 Roadmap

* [x] Leitura separada de temperatura e umidade
* [x] Uso do último valor válido quando leitura falhar
* [x] Indicador de valor defasado
* [ ] Ícone visual em vez de `#` para alerta de falha
* [ ] Suporte a DHT22
* [ ] Envio para servidor MQTT

## 🤝 Contribuição

Sinta-se à vontade para abrir **Issues** e enviar **Pull Requests** com melhorias.

## 📄 Licença

Este projeto é distribuído sob a licença MIT - veja [LICENSE](LICENSE) para detalhes.

```


