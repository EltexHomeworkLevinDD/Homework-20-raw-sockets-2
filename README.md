# Задание 
1. Написать эхо-udp сервер на обычных сокетах.
Сервер ждёт сообщения от клиента, содержащее строку. Изменяет строку и отправляет обратно клиенту.
2. Написать udp клиента на raw сокете, вручную заполняя только udp заголовок.
Клиент формирует буфер 8 байт udp заголовка + запас на отправляемую строку. Зполняет заголовок udp, отправляет серверу.
Ждёт получения сообщения от сервера, выводит строку. 