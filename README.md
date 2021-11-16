# 💻 NodeETA_Pulmao-pH
ESP-WROOM-32

##  Funcionalidades
### MODBUS 🔌
* Requisição valor do `pH`                     | tópico MQTT: `/eta/ph`.
* Requisição valor do `SetPoint1`              | tópico MQTT: `/eta/th_sp1`.
* Requisição escrita do `SetPoint1 pH`         | tópico MQTT: `/eta/envia_sp`.
* Requisição escrita do `SetPoint1 analógica`  | tópico MQTT: `/eta/envia_sp`.


## OTA

IP: 10.116.88.31  
Usuário: Esp-PH  
Senha: Esp-ph@nuplam  

## Esquemático


# 💻 NodeETA_OR
##  Funcionalidades

* Publica valor da `corrente` da bomba alta     | tópico MQTT: `/eta/bomba_alta`.
* Publica valor dos `estados` da Osmose Reversa | tópico MQTT: `/eta/estado`.

## OTA

IP: 10.116.88.30  
Usuário: Esp-OR  
Senha: Esp-or@nuplam  

## Esquemático

# 💻 NodeETA_Deio-bomba
##  Funcionalidades
* Publica valor da `corrente` da bomba          | tópico MQTT: `/eta/deio_bomba`.

## OTA

IP: 10.116.88.33  
Usuário: ESP_deio-bomba  
Senha: Esp_deio-bomba@nuplam  

## Esquemático
<img width=“964” alt=“java 8 and prio java 8  array review example” src=“https://github.com/gcavalheirof/nodeeta/blob/master/imagens/schem_bomba_deio.png”>

# 💻 NodeETA_OR-temp
##  Funcionalidades

* Publica valor da `temperatura`              | tópico MQTT: `/eta/or_temp`.

## OTA

IP: 10.116.88.32  
Usuário: Esp-OR-temp  
Senha: Esp-or-temp@nuplam  

## Esquemático
