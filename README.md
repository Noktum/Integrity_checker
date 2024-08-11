# Intergrity checker

Утилита, осуществляющая контроль целостности файлов каталога посредством генерации хэш сумм SHA256. Написана на языке C с использованием OpenSSL и стандартной библиотеки.

## Описание работы

При старте утилиты выводится консольное меню с выбором 2х режимов работы:
1. Постановка каталога на контроль целостности.
2. Проведение контроля целостности каталога.

При вводе либого другого символа кроме "1" или "2" будет запрошен повторный ввод. Далее программа запрашивает путь до каталога, для которого будет осуществляться контроль, и списка контроля целостности (с расширением txt, иначе при выборе 2го режима в дальнейшем утилита не запустится).

### Постановка на контроль

Утилита создает указанный пользователем список контроля целостности (далее - список) и генерирует хэш для названия контролируемого каталога (далее - каталога), чтобы при проведении контроля целостности удостовериться в том, что список соответствует поданному каталогу. Данное значение записывается в 1ю строку.

Далее идет сканирование каталога на находящиеся в нем файлы. Найденные файлы записываются в список вместе со сгенерированной для них хэщ суммой через непечатный символ `file separator`.

### Проведение контроля целостности

В начале программа проверяет соотстветствие расширение списка и возможность его открытия на чтение. После этого происходит повторная генерация хэш суммы для названия каталога, которую сверяет с находящейся в файле. Если проверка проходит успешно, то утилита повторно просматривает каталог на наличие файлов в нем, параллельно генерируя для них хэш суммы. Далее происходит построчная сверка названий файлов и хэшей и в случае нахождения несоотвествия работа программы завершается с сообщением `Целостность каталога нарушена`. В противном случае, если несоответствий не будет найдено, отобразится сообщение `Целостность католога заверена`.

### Ведение журнала

Программа ведет одновременно 2 журнала: системный и локальный, с названием `journal.log`, куда дублируются все информационные и сообщения об ошибках. Сделано это для удобства наблюдения за процедурой контроля целостности.

## Сборка проекта

Для запуска утилиты нужно выполнить команду `make` в директории проекта. Должна быть установлена библиотека `libssl-dev` для осуществления генерации хэш ключей и сам `make`. Для этого можно выполнить команду `make install` с правами суперпользователя.