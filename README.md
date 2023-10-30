# operenv-syspro
operating environments and system programming

## Lab1
#### Окно приложения нестандартной формы с нестандартной заливкой
Оконное приложение с нестандартным (непрямоугольным) главным окном, фон окна тоже более сложный, чем выполняемый автоматически. Например, окно овальной формы с градиентной заливкой фона. Набор элементов управления может быть минимальным – например, кнопка закрытия. (Потребуется соответствующий стиль для главного окна и «пустая» кисть для автоматической заливки, полностью «ручная» прорисовка изображения.)

## Lab2
#### Самовосстанавливающийся процесс
Возобновление работы процесса с сохранением (продолжением) текущих функций после завершения его сообщением WM_CLOSE. Игнорирование сообщения приводит, как правило, либо к объявлению его аварийным (автоматически), либо принудительному безусловному завершению посредством TaskManager. Решение: процесс завершается штатным образом, но перед этим порождает свою копию (аналогичный процесс из того же исполняемого файла). Копия продолжает выполняться вместо родительского процесса. Проблема: необходимость продолжить работу с теми же обрабатываемыми данными. Вариант решения: хранение текущих рабочих данных в структуре («прикладной контекст»), которая может выгружаться на диск или сохраняться в глобальной памяти (родителем) и загружаться оттуда (потомком-«наследником»).

## Lab3
#### Многопоточная сортировка
Разбиение массива на несколько частей (фрагментов), сортировка каждого  отдельным потоком, окончательная «сборка». Количество потоков (в т.ч. единственный поток), размер массива задаются пользователем,  количество потоков не слишком большое, чтобы оставалось удобным для  отображения. Отображение прогресса выполнения (достаточно готовности/неготовности  каждого фрагмента), время выполнения для сравнения и анализа  зависимости. 

## Lab4
#### Обработка содержимого файла, используя асинхронные операции 
Реализация обработки содержимого файла данных, используя  асинхронный ввод-вывод (чтение/запись очередных порций данных параллельно с выполнением обработки данных в памяти). Варьирование количества параллельно инициированных операций ввода вывода, влияние их на общею производительность. Поиск «узкого места» в программной реализации. Возможные варианты обработки: сортировка, статистическая обработка,  криптография и т.д. Объем данных следует выбирать достаточно  большим, чтобы длительность обработки была заметна и хорошо  поддавалась измерению. 

## Lab5
#### Интерактивное взаимодействие пользователей («чат»)
Упрощенный чат для нескольких пользователей. Процессы: клиенты и сервер. Процесс-сервер: создание необходимых для общения с клиентами IPC; прием и временное хранение сообщений; передача сообщений адресно одному или нескольким клиентам. Процесс-клиент: соединение с сервером; ввод пользовательских сообщений и передача их серверу; прием и отображение сообщений от сервера.

## Lab6
#### Модель «Писатели-читатели»
Реализация модели взаимодействия процессов (потоков) «писатели-читатели» с возможностью параметризации и наглядного представления результатов. Поскольку речь идет о модели, реальные данные не обязательны, можно ограничиться моделированием обращений к ним. Обеспечение корректного функционирования, т.е. избегание как «грязного» считывания данных и одновременно минимизация блокировок. Изменяемые параметры модели: количество «писателей» и «читателей»; интенсивность их обращений к ресурсам, длительность использования ресурса, размер блока данных и т.п.

## Lab7
#### Интерактивное взаимодействие пользователей («чат»)
Упрощенный чат для нескольких пользователей с использованием сетевых сокетов.
Архитектура: централизованная (выделенный процесс-сервер и процессы-клиенты) или децентрализованная (процессы-клиенты с «серверными» функциями).
Сервер: создание сокета для приема соединений или отдельных сообщений; прием и временное хранение сообщений; передача сообщений адресно одному или нескольким клиентам; поддержание списка актуальных клиентов.
Клиент: обнаружение сервера и соединение с ним; ввод пользовательских сообщений и передача их серверу либо напрямую соответствующему клиенту; прием и отображение сообщений.

## Lab8
#### Просмотр информации о системе
Утилита, обеспечивающая сбор информации о системе и отображение ее в удобном виде: аппаратное обеспечение, операционная система, количественные характеристики и т.д. Состав информации – на усмотрение разработчика, в качестве ориентира – стандартные «системные» сведения, можно расширить или специализировать на конкретном разделе. Источники информации: в основном реестр и/или специализированные системные функции, содержимое файловой системы, WMI, собственные измерения характеристик и т.д.
