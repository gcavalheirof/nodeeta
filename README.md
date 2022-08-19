# 💻 NodeETA_Pulmao-pH
ESP-WROOM-32

##  Funcionalidades
### MODBUS 🔌
* Requisição valor do `pH`                     | tópico MQTT: `/eta/ph`.
* Requisição valor do `SetPoint1`              | tópico MQTT: `/eta/th_sp1`.
* Requisição escrita do `SetPoint1`         | tópico MQTT: `/eta/envia_sp`.      (Motor agitador)
* Requisição escrita do `Saída analógica`  | tópico MQTT: `/eta/envia_sp`.       (Bomba de hidróxido)


## OTA

IP: *********  
Usuário: *********  
Senha: *********   

## Esquemático


# 💻 NodeETA_OR
##  Funcionalidades

* Publica valor da `corrente` da bomba alta     | tópico MQTT: `/eta/bomba_alta`.
* Publica valor dos `estados` da Osmose Reversa | tópico MQTT: `/eta/estado`.

## OTA

IP: *********  
Usuário: *********  
Senha: *********  

## Esquemático

# 💻 NodeETA_Deio-bomba
##  Funcionalidades
* Publica valor da `corrente` da bomba          | tópico MQTT: `/eta/deio_bomba`.

## OTA

IP: *********  
Usuário: *********    
Senha: *********   

## Esquemático
![schem](/imagens/schem_bomba_deio.png)

# 💻 NodeETA_OR-temp
##  Funcionalidades

* Publica valor da `temperatura`              | tópico MQTT: `/eta/or_temp`.

## OTA

IP: *********
Usuário: *********
Senha: ********* 

## Esquemático
![schem](/imagens/schem_temp.png)
